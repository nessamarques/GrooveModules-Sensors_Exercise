#include <SPI.h>
#include "rgb_lcd.h"
#include <stdlib.h>
#include <string.h>
#include <Servo.h>
#include <TH02_dev.h>

rgb_lcd lcd;

const int pinButton =0;
const int pinServo = 6;
const int pinRelay = 5;

Servo myservo;

int getTempSensorValue();
int getHumiSensorValue();

void ServoHandle(int val);
void RelayHandle(int val);

void setup() {
  Serial.begin(9600);
  pinMode(pinButton,INPUT);
  myservo.attach(pinServo);
  myservo.write(0);
  lcd.begin(16, 2);

}

void loop() {
  parse_command_from_serial();
  delay(100);
  isButtonPressed();

  int temp = getTempSensorValue();
  int humi = getHumiSensorValue();

  lcd.clear();
  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print("C|Humi:");
  lcd.print(humi);
}

int isButtonPressed(){
  int state = 0;
  if(digitalRead(pinButton)){
    while(digitalRead(pinButton));
    state = 1;
    
    //ServoHandle(200);
    //RelayHandle(0);  
    Serial.println("Button pressed!");
  }

  return state;
}

int getTempSensorValue(){
  int temper =(int) TH02.ReadTemperature();
  Serial.print("\nTemperature: ");
  Serial.println(temper);  
  return temper;
}

int getHumiSensorValue(){
  int humidity =(int)TH02.ReadHumidity();
  Serial.print("\nHumidity: ");
  Serial.println(humidity);  
  return humidity;
}

void ServoHandle(int val){
  static unsigned long curt = millis();
  if(millis()-curt > 1000){
    myservo.write(val);

    Serial.print("\nServoHandle: ");
    Serial.println(val);
    
    curt = millis();
  }
}

void RelayHandle(int val){

  Serial.print("\RelayHandle: ");
  Serial.println(val);
  
  if(val==1) {
    digitalWrite(pinRelay,HIGH);
  }
  else if(val==0){
    digitalWrite(pinRelay,LOW);
  }
}


/*** PARSE ***/
#define DEBUG_VAR(X) Serial.print(#X); Serial.print(": "); Serial.println(X);

void apply_command(char command, char value[]) {
  DEBUG_VAR(command);
  switch (tolower(command)) {
    case 'g':
      float gas_cost = atof(value);
      DEBUG_VAR(gas_cost);
    break;
    // (...)
  }
}

void parse_command_from_serial() {
  
  while (Serial.available()) {
  
    char command = Serial.read();
    int index;
    char value[10] = "\0";
    
    for (index = 0; Serial.available(); index++) {
      value[index] = Serial.read();
      
      if (value[index] == '\n' || value[index] == ',') {
        value[index] = '\0';
        break;  // command termination character
      }
    }
    
    apply_command(command, value);
  }
}
