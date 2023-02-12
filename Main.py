import cv2
import numpy as np
import face_recognition
import serial
import os
from cvzone.SerialModule import SerialObject
from time import sleep

cond = 0  # Condition for sending a name
port = "COM6"  # Port for the arduino
ard = serial.Serial(port, 9600, timeout=5)  # Initializing the arduino
def encodes(images):
    encoded = [] #Array for
    for img in images:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  #Because in opencv the channels for img is BGR
        encode = face_recognition.face_encodings(img)[0]
        encoded.append(encode)
    return encoded


path = 'images'
images = []
classNames = []
myList = os.listdir(path)

for i in myList:   # List of images stored in data
    cur = cv2.imread(f'{path}/{i}')
    images.append(cur)
    classNames.append(os.path.splitext(i)[0])  # Getting only the name without the extension

encodeKnown = encodes(images)  # Do the encodings for all the images
# print(len(encodeKnown))

cap = cv2.VideoCapture(1)  # Import from the camera
cap.set(10, 100)  # To increase the quality of brightness
while True:  # To capture the image from camera and saving it into img
    success, img = cap.read()
    imgS = cv2.resize(img, (0, 0), None, 0.25, 0.25)   # Reducing the size just for increasing the speed as it is not a recorded video
    imgS = cv2.cvtColor(imgS, cv2.COLOR_BGR2RGB)

    facesloc = face_recognition.face_locations(imgS)  # To find the face in the images
    encode = face_recognition.face_encodings(imgS, facesloc)  # Finding the encoding of the face

    for en, loc in zip(encode, facesloc):  # Zip just for work with the same loop
        matches = face_recognition.compare_faces(encodeKnown, en)  # Compare the face on the camera with the stored ones
        faceDis = face_recognition.face_distance(encodeKnown, en)  # Percentage of similarity between the faces

        matchInd = np.argmin(faceDis) # Finding best match
        if matches[matchInd]:
            name = classNames[matchInd].upper()
            # print(faceDis[matchInd])

        else:
            name = 'Unkown'
        print(name)
        y1, x1, y2, x2 = loc  # To draw a rectangle surrounding the face and containing the name
        y1, x1, y2, x2 = 4 * y1, 4 * x1, 4 * y2, 4 * x2
        cv2.rectangle(img, (x1, y1), (x2, y2), (0, 255, 255), 2)
        cv2.rectangle(img, (x1, y2 - 35), (x2, y2), (0, 255, 0), cv2.FILLED)
        cv2.putText(img, name, (x1 - 120, y2 - 6), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2)
        if name != 'Unkown':  # Sending the order for the arduino to control the door
            if cond == 0:
                ard.write(name.encode())
                cond = 1
                sleep(3)

    cv2.imshow('My Cam', img)  # Showing the image screen
    cv2.waitKey(1)
