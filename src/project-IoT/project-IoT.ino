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
float desired_temperature = 0;
float water_temperature = 0;
float electric_cost = 0;
float gas_cost = 0;
float moisture_sensor = 0;
float moisture_level = 0;
bool remote_button;
bool local_button;

char state = 'O';

float getTempSensorValue();
int getHumiSensorValue();

void ServoHandle(int val);
void RelayHandle(int val);

Servo myservo;

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

  parse_command_from_serial();
  
  target_temperature = getTempSensorValue();
  moisture_level = getHumiSensorValue();

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
      if(desired_temperature >= target_temperature){
        state = 'O';
        RelayHandle(1);
      }
      else {
        RelayHandle(0);

        if(electric_cost < gas_cost){
          state = 'E';
        }

        if(gas_cost < electric_cost){          
          state = 'G';
          ServoHandle(200);
          moisture_sensor = (gas_cost * (1 - (moisture_level/1000) * 8));

          if(moisture_sensor >= 0 && moisture_sensor <= 950)
          {
            Serial.println("gas heater efficient");
          }
          else{
            Serial.print("gas heater inefficient");
          }
        }
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
      gas_cost = atof(value);
      DEBUG_VAR(gas_cost);
    break;
    case 'e':
      electric_cost = atof(value);
      DEBUG_VAR(electric_cost);
    break;
    case 't':
      desired_temperature = atof(value);
      DEBUG_VAR(desired_temperature);
    break;
    case 'r':
      remote_button = atof(value);
      DEBUG_VAR(desired_temperature);
    break;
    case 'l':
      local_button = atof(value);
      DEBUG_VAR(local_button);
    break;
    case 'w':
      water_temperature = atof(value);
      DEBUG_VAR(water_temperature);
    break;
    case 'm':
      moisture_level = atof(value);
      DEBUG_VAR(moisture_level);
    break;
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
