#include <Wire.h>
#include "Adafruit_MPRLS.h"
#include <Servo.h>

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

//definitions for mechanical components
int actuator_RPWM = 11;  //connect Arduino pin 7 to IBT-2 pin RPWM FOR ACTUATOR ORANGE
int actuator_LPWM = 10;  //connect Arduino pin 6 to IBT-2 pin LPWM FOR ACTUATOR WHITE

int pinion_RPWM = 13;  //connect Arduino pin 13 to IBT-2 pin RPWM FOR PINION BLUE
int pinion_LPWM = 12;  //connect Arduino pin 12 to IBT-2 pin LPWM FOR PINION GREEN

int spin_RPWM = 7; //connect Arduino pin 9 to IBT-2 pin RPWM FOR SPIN MOTOR YELLOW
int spin_LPWM = 6; //connect Arduino pin 8 to IBT-2 pin LPWM FOR SPIN MOTOR BROWN

int solenoid_RPWM = 9; //connect Arduino pin 11 to IBT-2 pin RPWM FOR SOLENOID PURPLE
int solenoid_LPWM = 8; //connect Arduino pin 10 to IBT-2 pin LPWM FOR SOLENOID GREY

int i = 0;

void setup() {
  Serial.begin(2000000);
  if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");
  delay(100);
  
  pinMode(actuator_RPWM, OUTPUT); //Configure pin 13 as an Output
  pinMode(actuator_LPWM, OUTPUT); //Configure pin 12 as an Output
  pinMode(pinion_RPWM, OUTPUT); //Configure pin 11 as an Output
  pinMode(pinion_LPWM, OUTPUT); //Configure pin 10 as an Output
  pinMode(spin_RPWM, OUTPUT); //Configure pin 9 as an Output
  pinMode(spin_LPWM, OUTPUT); //Configure pin 8 as an Output
  pinMode(solenoid_RPWM, OUTPUT); //Configure pin 7 as an Output
  pinMode(solenoid_LPWM, OUTPUT); //Configure pin 6 as an Output


  pinMode(A0, INPUT); //x-value
  pinMode(A2, INPUT); //y-value

  analogWrite(actuator_RPWM, 400);
  analogWrite(actuator_LPWM, 0);
  delay(10000);
}


void loop() {
  Serial.print("Controlling Actuator with Joystick\n");
  while(1) {
    int yVal = analogRead(A2); //sets the Y value
    if (yVal < 375) { //Joystick is tilted left
      analogWrite(actuator_RPWM, 400);
      analogWrite(actuator_LPWM, 0);
      i -= 1;
      Serial.print("i value is: ");
      Serial.print(i);
    }
    else if (yVal > 675) { //Joystick is tilted right
        analogWrite(actuator_RPWM, 0);
        analogWrite(actuator_LPWM, 400);
        i += 1;
        Serial.print("i value is: ");
        Serial.print(i);
        Serial.print("\n");
    }
    else { //Joystick is not tilted either direction
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 0);
    }
  }
}