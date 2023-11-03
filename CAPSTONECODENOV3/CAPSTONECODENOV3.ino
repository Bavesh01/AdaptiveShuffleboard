#include <Wire.h>
#include "Adafruit_MPRLS.h"
#include <Servo.h>

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);
Servo myservo;  // create servo object to control a servo

//definitions for mechanical components
int actuator_RPWM = 11;  //connect Arduino pin 7 to IBT-2 pin RPWM FOR ACTUATOR ORANGE
int actuator_LPWM = 10;  //connect Arduino pin 6 to IBT-2 pin LPWM FOR ACTUATOR WHITE

int pinion_RPWM = 33;  //connect Arduino pin 32 to IBT-2 pin RPWM FOR PINION BLUE
int pinion_LPWM = 32;  //connect Arduino pin 33 to IBT-2 pin LPWM FOR PINION GREEN

int spin_RPWM = 7; //connect Arduino pin 9 to IBT-2 pin RPWM FOR SPIN MOTOR YELLOW
int spin_LPWM = 6; //connect Arduino pin 8 to IBT-2 pin LPWM FOR SPIN MOTOR BROWN

int solenoid_RPWM = 9; //connect Arduino pin 11 to IBT-2 pin RPWM FOR SOLENOID PURPLE
int solenoid_LPWM = 8; //connect Arduino pin 10 to IBT-2 pin LPWM FOR SOLENOID GREY

int xPosition = 0;


int readPuff() {
  //measure pressure and convert to PSI
  float pressure_hPa = mpr.readPressure();
  //Serial.print("Pressure (hPa): "); 
  //Serial.println(pressure_hPa);
  //Serial.print("Pressure (PSI): "); 
  int p = pressure_hPa / 68.947572932;
  //Serial.println(p);
  delay(100);
  return p;
}

void controlAim() {
  while (1) { //loops through aim control until a 
    Serial.print("Controlling Actuator with Joystick");
    Serial.print("\n");
    int yVal = analogRead(A2); //sets the Y value
    int puff = readPuff();
    if (puff>14) { //If the user sufficiently puffs to confirm
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 0);
      break;
    }
    else if (yVal < 375 && xPosition > -34) { //Joystick is tilted left
      analogWrite(actuator_RPWM, 400);
      analogWrite(actuator_LPWM, 0);
      xPosition -= 1;
      Serial.print("X position is: ");
      Serial.print(xPosition);
      Serial.print("\n");
    }
    else if (yVal > 675 && xPosition < 106) { //Joystick is tilted right
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 400);
      xPosition += 1;
      Serial.print("X position is: ");
      Serial.print(xPosition);
      Serial.print("\n");
    }
    else { //Joystick is not tilted either direction
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 0);
    }
  }
}

void setSpin(int spinVal) {
  if (spinVal > 0) {
    analogWrite(spin_RPWM, spinVal);
    analogWrite(spin_LPWM, 0);
  }
  else if (spinVal < 0) {
    analogWrite(spin_RPWM, 0);
    analogWrite(spin_LPWM, (-1*spinVal));
  }
  else {
    analogWrite(spin_RPWM, 0);
    analogWrite(spin_LPWM, 0);
  }
}

void controlSpin() {
  int spinVal = 0;
  Serial.print("Controlling Spin with Joystick\n");
  while (1) {
    int yVal = analogRead(A2);
    int puff = readPuff();
    if (puff>14) { //If the user suffieciently puffs to confirm
      break;
    }
    else if (puff<14) { //If the user sufficiently sips to cancel out spin
      spinVal = 0;
    }
    else if (yVal < 375 && spinVal != -100) { //If joystick is tilted left
      spinVal -= 20;
    }
    else if (yVal > 675 && spinVal != 100) {
      spinVal += 20;
    }
    setSpin(spinVal);
    delay(1500);
  }
}

void controlAngle() {
  int angleDeg = 90; //
  Serial.print("Controlling Launch Angle with Joystick");
  Serial.print("\n");
  while (1) {
    int yVal = analogRead(A2); //sets the Y value
    int puff = readPuff();
    if (puff>14) { //If the user sufficiently puffs to confirm
      break;
    }
    else if (yVal < 375 && angleDeg != 60) { //Joystick is tilted left
      angleDeg -= 5;
      Serial.print("Current Angle: ");
      Serial.print(angleDeg);
      Serial.print("\n");
    }
    else if (yVal > 675 && angleDeg != 120) { //Joystick is tilted left
      angleDeg += 5;
      Serial.print("Current Angle: ");
      Serial.print(angleDeg);
      Serial.print("\n");
    }
    myservo.write(angleDeg);
    delay(1000);
  }
}

void controlShoot() {
  int powerLevel = 0;
  Serial.print("Setting Power and Shooting!\n");
  Serial.print("Sip to set speed\n");
  Serial.print("Current Power Level: ");
  Serial.print(powerLevel);
  Serial.print("\n");
  while(1) {
    int puff = readPuff();
    if (puff<14 && powerLevel != 16) {
      powerLevel += 1;
      Serial.print("Current Power Level: ");
      Serial.print(powerLevel);
      Serial.print("\n");
      delay(250);
    }
    else if (puff<14 && powerLevel == 16) {
      powerLevel = 0;
      Serial.print("Resetting Power Level to 0");
      Serial.print("\n");
      delay(250);
    }
    else if (puff>14) {
      break;
    }
  }
  Serial.print("Shooting!!\n");
  int shotSpeed = (powerLevel *4) + 15;
  int shotTime = (5*60*75)/shotSpeed;
  analogWrite(pinion_RPWM, shotSpeed); //Pinion moves rack forward
  analogWrite(pinion_LPWM, 0);
  delay(shotTime); //For a certain amount of time
  analogWrite(solenoid_RPWM, 1200); //Solenoid opens, launching puck
  analogWrite(solenoid_LPWM, 0);
  delay(100);
  analogWrite(pinion_RPWM, 0); //Pinion stops, stopping rack
  analogWrite(pinion_LPWM, 0);
  analogWrite(spin_RPWM, 0); //Spin stops
  analogWrite(spin_LPWM, 0);
  delay(2000); //Waits 2 seconds
  analogWrite(solenoid_RPWM, 0); //Closes solenoid
  analogWrite(solenoid_LPWM, 0);
  delay(2000);
  reHome();  
}

void reHome() {
  Serial.print("System Rehoming\n");
  analogWrite(pinion_RPWM, 0);
  analogWrite(pinion_LPWM, 30);
  delay(600);
  analogWrite(pinion_RPWM, 0);
  analogWrite(pinion_LPWM, 0);
  delay(200);
  Serial.println("Current X Position is: ");
  Serial.println(xPosition);
  while(1) {
    if (xPosition > 0) {
      analogWrite(actuator_RPWM, 400);
      analogWrite(actuator_LPWM, 0);
      xPosition -= 1;
    }
    else if(xPosition < 0) {
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 400);
      xPosition += 1;
    }
    else if (xPosition == 0){
      break;
    } 
  }
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 0);
  moveMode();
}

void moveMode() {
  Serial.print("System entering Move Mode\n");
  Serial.print("Puff to confirm system in place for next shot with puck reloaded\n");
  while(1) {
    int puff = readPuff();
    if (puff>14) {
      Serial.print("Puff confirmed, returning to home screen\n");
      break;
    }
  } 
}

void confirm(int choice) {
  if (choice == 1) {
    Serial.print("Selected Launch Angle\n");
  }
  else if (choice == 2) {
    Serial.print("Selected Aim\n");
  }
  else if (choice == 3) {
    Serial.print("Selected Spin\n");
  }
  else if (choice == 4) {
    Serial.print("Selected Power and Shoot\n");
  }
  Serial.print("CONFIRM YOUR CHOICE?\n");
  while(1) {
    int confirmXVal = analogRead(A0);
    int confirmYVal = analogRead(A2);
    int puff = readPuff();

    if (choice == 1) { //If the user selected LAUNCH ANGLE CONTROL

      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(2000);
        controlAngle();
      }
      else if (confirmXVal > 675 || confirmYVal > 675 || confirmXVal < 375) { // If the joystick is moved RIGHT, UP, OR DOWN
        delay(2000);
        break;
      }
    }

    else if (choice == 2) { //If the user selected AIM CONTROL
      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(2000);
        controlAim();
        break;
      }
      else if (confirmXVal > 675 || confirmXVal < 375 || confirmYVal < 375) { // If the joystick is moved RIGHT, LEFT, OR DOWN)
        delay(2000);
        break;
      }
    }

    else if (choice == 3) { //If the user selected SPIN CONTROL
      if (puff>14) { //If the user sufficiently puffs to 
        delay(2000);
        controlSpin();
        break;
      }
      else if (confirmXVal < 375 || confirmYVal > 675 || confirmYVal < 375) { // If the joystick is moved LEFT, UP, OR DOWN
        delay(2000);
        break;
      }
    }

    else if (choice == 4) { //If the user selected POWER AND SHOOT
      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(2000);
        controlShoot();
        break;
      }
      else if (confirmYVal < 375 || confirmXVal > 675 || confirmYVal > 675) { // If the joystick is moved LEFT, RIGHT, OR UP
        delay(2000);
        break;
      }
    }
  }
}

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
  myservo.attach(34, 500, 2500);  // attaches the servo on pin 34 to Arduino


  pinMode(A0, INPUT); //x-value
  pinMode(A2, INPUT); //y-value

  Serial.print("Doing initial Homing\n");
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 400);
  delay(14000);
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 0);

  analogWrite(actuator_RPWM, 400);
  analogWrite(actuator_LPWM, 0);
  delay(11000);
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 0);
}

void loop() {
  Serial.print("ON HOME SCREEN \n");
  while(1) {
    int homeXVal = analogRead(A0);
    int homeYVal = analogRead(A2);
    int choice = 0;

    if (homeYVal < 375) { //Joystick tilted LEFT, run LAUNCH ANGLE CONTROL
      Serial.print("XVal is: ");
      Serial.print(homeXVal);
      Serial.print("\n");
      Serial.print("YVal is: ");
      Serial.print(homeYVal);
      Serial.print("\n");
      delay(2000);
      confirm(1);
      break;
    }
    else if (homeYVal > 675) { //Joystick tilted RIGHT, run SPIN CONTROL
      Serial.print("XVal is: ");
      Serial.print(homeXVal);
      Serial.print("\n");
      Serial.print("YVal is: ");
      Serial.print(homeYVal);
      Serial.print("\n");
      delay(2000);
      confirm(3);
      break;
    }
    else if (homeXVal > 675) { //Joystick tilted UP, run AIM CONTROL
      Serial.print("XVal is: ");
      Serial.print(homeXVal);
      Serial.print("\n");
      Serial.print("YVal is: ");
      Serial.print(homeYVal);
      Serial.print("\n");
      delay(2000);
      confirm(2);
      break;
    }
    else if (homeXVal < 375) { //Joystick tilted DOWN, run POWER AND SHOOT
      Serial.print("XVal is: ");
      Serial.print(homeXVal);
      Serial.print("\n");
      Serial.print("YVal is: ");
      Serial.print(homeYVal);
      Serial.print("\n");
      delay(2000);
      confirm(4);
      break;
    }
  }
}
