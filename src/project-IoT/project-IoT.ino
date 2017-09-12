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

float target_temperature = 0;
float desired_temperature = 30;

//int target_humidity = 0;
//int water_temperature = 0;
//int electric_cost = 0;
//int gas_cost = 0;
//bool remote_button;

char state = 'O';

Servo myservo;

float getTempSensorValue();
int getHumiSensorValue();

void ServoHandle(int val);
void RelayHandle(int val);

void setup() {
  Serial.begin(9600);
  pinMode(pinButton,INPUT);
  myservo.attach(pinServo);
  //myservo.write(0);
  lcd.begin(16, 2);
}

void loop() {

  delay(100);
  isButtonPressed();

  //parse_command_from_serial();
  target_temperature = getTempSensorValue();
  
  //target_humidity = getHumiSensorValue();
  //lcd.print("C|Humi:");
  //lcd.print(humi);

  lcd.clear();
  lcd.print(state);
  lcd.print("   ");
  lcd.print(target_temperature);
  lcd.print(" C ");
  lcd.setCursor(0,1);
  lcd.print("    ");
  lcd.print(desired_temperature);
  lcd.print(" C ");
}



int isButtonPressed(){
  if(digitalRead(pinButton)){
    
    while(digitalRead(pinButton))
    {
      Serial.println("Button pressed!");
    
      if(desired_temperature >= target_temperature){
        state = 'O';
      }
      else {
        // TODO: Validate E x G if button pressed
        state = 'E';
  
        RelayHandle(1);
        ServoHandle(200);
      } 
    }  
  }
  return state;
}

float getTempSensorValue(){
  float temper =(float) TH02.ReadTemperature(); 
  return temper;
}

int getHumiSensorValue(){
  int humidity =(int)TH02.ReadHumidity();
  return humidity;
}

void ServoHandle(int val){
  static unsigned long curt = millis();
  if(millis()-curt > 1000){
    myservo.write(val);
    curt = millis();
  }
}

void RelayHandle(int val){
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
