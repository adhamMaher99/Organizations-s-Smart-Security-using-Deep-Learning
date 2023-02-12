#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <cvzone.h>
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//The pins for each component
int motor1=9;
int motor2=6;
String readString;
int pos;
int DHTPIN=10;
int pirPin = 11;                 // PIR Out pin 
int pirStat = 0;                   // PIR status
int PIRled = 13;                // LED 
int SensorLedR1= 0;
int SensorLedG1 = 1;
int SensorLedB1 = 2;
int smokeA0 = A3;
int sensorThreshold = 300;
int flameD = 7 ;
int flame_detected ;
Servo doorServo;
Servo garServo;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  doorServo.attach(motor1);
  doorServo.write(170);
  garServo.attach(motor2);
  garServo.write(0);
  pinMode(PIRled, OUTPUT);     
  pinMode(pirPin, INPUT);
  pinMode(SensorLedR1, OUTPUT);
  pinMode(SensorLedB1, OUTPUT);
  pinMode(SensorLedG1, OUTPUT);
  pinMode(smokeA0, INPUT);
  pinMode(flameD, INPUT) ;     
  RGB_color(0,0,0);
}

void flame_sensor(){
  flame_detected = digitalRead(flameD) ;
  if (flame_detected == 1)
  {
    Serial.println("Flame detected...! take action immediately.");
    RGB_color(0, 255, 0); // Blue
    delay(2000);
  }
}

void PIR_sensor(){
  pirStat = digitalRead(pirPin); 
  if (pirStat == HIGH) {            // if motion detected
     digitalWrite(PIRled, HIGH);  // turn LED ON
    Serial.println("Hey I got you!!!");
  } 
  else {
     digitalWrite(PIRled, LOW); // turn LED OFF if we have no motion
  }
  delay(100);
}


void LPG_sensor(){
  int analogSensor = analogRead(smokeA0);
  //Serial.print("Pin A0: ");
  //Serial.println(analogSensor);
  if (analogSensor > sensorThreshold)
  {
    RGB_color(0, 255, 0); // Blue
    Serial.println("Gas !!!!!!!!!");
    delay(1000);
  }  
}


void updateDisplay() { 
  /*Function to update the displayed on the LCD*/
   lcd.setCursor(0,0);
   lcd.print("Hello, Mr/Ms: ");
   lcd.setCursor(0,1);
   lcd.print(readString);
   dht.begin();
}



void open_door(){
    for (pos = 0; pos <=130; pos += 1) { 
      garServo.write(pos);              
      delay(10);                 
    }
  }

void close_door(){
    for (pos = 130; pos >= 0; pos -= 1) { 
      garServo.write(pos);              
      delay(10);
    }
  }

void doorControl(){
  /*Function controlling the door by opening it when recognizing a family member*/
       for (pos = 170; pos >= 60; pos -= 1) { 
        doorServo.write(pos);              
        delay(10);
       }
      delay(3000);
      for (pos = 60; pos <=170; pos += 1) { 
        doorServo.write(pos);              
        delay(10);                 
      }
      //delay(5000);
  }






void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(SensorLedR1, red_light_value);
  analogWrite(SensorLedG1, green_light_value);
  analogWrite(SensorLedvoid dht_sensor(){
  float h = dht.readHumidity();        // read humidity
  float t = dht.readTemperature();     // read temperature
  if (isnan(h) || isnan(t)) { //Check if any reads failed 
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(2500);
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F(" Humidity: "));
  Serial.print(h);
  Serial.print(F("%\nTemperature: "));
  Serial.print(t);
  Serial.print(F("°C\n"));
  //Serial.print(F("\nHeat index: "));
  //Serial.print(hic);
  //Serial.print(F("°C "));
  delay(1000);
}  B1, blue_light_value);
}

void garage_door(){
  if(readString[0]='1'){
      open_door();
  }
    if(readString[0]=='0'){
      close_door();
  }
}


void loop() {
  
  while (Serial.available())
    {
      readString="";
      readString=Serial.readString();
      if(readString[0]=='1'){
        open_door();
      }
      else if(readString[0]=='0') close_door();
      
      else{
       updateDisplay();
       doorControl();
      }
      }
    flame_sensor();
    LPG_sensor();
    PIR_sensor();
    dht_sensor();
  }
