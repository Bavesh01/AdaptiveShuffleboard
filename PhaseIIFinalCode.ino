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

int pinion_RPWM = 7;  //connect Arduino pin 32 to IBT-2 pin RPWM FOR PINION BLUE
int pinion_LPWM = 6;  //connect Arduino pin 33 to IBT-2 pin LPWM FOR PINION GREEN

int spin_RPWM = 9; //connect Arduino pin 9 to IBT-2 pin RPWM FOR SPIN MOTOR YELLOW
int spin_LPWM = 8; //connect Arduino pin 8 to IBT-2 pin LPWM FOR SPIN MOTOR BROWN

int solenoid_RPWM = 5; //connect Arduino pin 11 to IBT-2 pin RPWM FOR SOLENOID PURPLE
int solenoid_LPWM = 4; //connect Arduino pin 10 to IBT-2 pin LPWM FOR SOLENOID GREY

int xPosition = 0; //initialize the global x position value
int *p_xPosition; //create a pointer to this global variable so that it can be updated in functions

int readPuff() { //measure pressure and convert to PSI
  float pressure_hPa = mpr.readPressure();
  int p = pressure_hPa / 68.947572932;
  delay(100);
  return p;
}

void controlAim() { //control the linear actuator motor with the joystick
  p_xPosition = &xPosition; //use the global x-position pointer
  Serial.print("Controlling Actuator with Joystick\n");
  Serial.println("IMAGE100");
  while (1) {
    int yVal = analogRead(A2); //sets the Y value to joystick position
    int puff = readPuff();
    if (puff>14) { //If the user sufficiently puffs to confirm
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 0);
      break; //stop the motor and return to confirm function
    }
    else if (yVal < 425 && xPosition > -34) { //Joystick is tilted left and not at the left boundary
      analogWrite(actuator_RPWM, 400);
      analogWrite(actuator_LPWM, 0);
      *p_xPosition -= 1;
      Serial.println("IMAGE102"); //run the motor left, decrease x position value, and show the corresponding image
    }
    else if (yVal > 625 && xPosition < 106) { //Joystick is tilted right and not at the right boundary
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 400);
      *p_xPosition += 1;
      Serial.println("IMAGE101"); //run the motor right, increase x position value, and show the corresponding image
    }
    else { //Joystick is not tilted either direction
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 0);
      Serial.println("IMAGE100"); //stop the motor and show the corresponding image
    }
  }
}

void setSpin(int spinVal) { //sets the spin motor values, if positive spin value set the RPWM and if negative set the LPWM
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
  Serial.println("IMAGE200");
  Serial.print("Controlling Spin with Joystick\n");
  while (1) {
    int yVal = analogRead(A2);
    int puff = readPuff();
    if (puff>14) { //If the user suffieciently puffs to confirm
      break; //break to confirm loop
    }
    else if (yVal < 425 && spinVal != -100) { //If joystick is tilted left
      spinVal -= 20; //spin value increases by 20
      delay(100);
      int k = 200 + spinVal/20;
      String m = "IMAGE" + String(k); //displays the corresponding spin image
      Serial.println(m);
      setSpin(spinVal); //sets the spin value
      delay(1000);
    }
    else if (yVal > 625 && spinVal != 100) {
      spinVal += 20; //spin value decreases by 20
      delay(100);
      int k = 200 + spinVal/20;
      String m = "IMAGE" + String(k); //displays the corresponding spin image
      Serial.println(m);
      setSpin(spinVal); //sets the spin value
      delay(1000);
    }
  }
}

void controlAngle() {
  int angleDeg = 87; //set the launch angle to 90 degrees
  Serial.println("IMAGE300");
  Serial.print("Controlling Launch Angle with Joystick");
  Serial.print("\n");
  while (1) {
    int yVal = analogRead(A2); //sets the Y value
    int puff = readPuff();
    if (puff>14) { //If the user sufficiently puffs to confirm
      break;
    }
    else if (yVal < 425 && angleDeg != 57) { //Joystick is tilted left
      angleDeg -= 3;
      Serial.print("Current Angle: ");
      Serial.print(angleDeg);
      Serial.print("\n");
      myservo.write(angleDeg);
      Serial.println("IMAGE302");
      delay(1000);
    }
    else if (yVal > 625 && angleDeg != 117) { //Joystick is tilted left
      angleDeg += 3;
      Serial.print("Current Angle: ");
      Serial.print(angleDeg);
      Serial.print("\n");
      myservo.write(angleDeg);
      Serial.println("IMAGE301");
      delay(1000);
    }
  }
}

void controlShoot() {
  int powerLevel = 0; //initialize the power level variable
  int loop = 1; //initialize the confirm variable to 1
  delay(100);
  Serial.println("IMAGE400"); //show the initial power level image
  Serial.print("Setting Power and Shooting!\n");
  Serial.print("Sip to set speed\n");
  Serial.print("Current Power Level: ");
  Serial.print(powerLevel);
  Serial.print("\n");
  while(loop) {
    int puff = readPuff(); //update the current pressure value
    int yVal = analogRead(A2); //update the current y-value from the joystick
    if (yVal < 425 && powerLevel != 0) { //if the joystick is tilted left and power level isn't 0
      powerLevel -= 1; //decrease the power level by 1
      Serial.print("Current Power Level: ");
      Serial.print(powerLevel);
      Serial.print("\n");
      delay(250);
      int k = 400 + powerLevel;
      String m = "IMAGE" + String(k); //display the current power level image
      Serial.println(m);
    }
    else if (yVal > 625 && powerLevel != 16) { //if the joystick is tilted right and power level isn't 16
      powerLevel += 1; //increase the power level by 1
      Serial.print("Current Power Level: ");
      Serial.print(powerLevel);
      Serial.print("\n");
      delay(250);
      int k = 400 + powerLevel;
      String m = "IMAGE" + String(k); //display the current power level image
      Serial.println(m);
    }
    else if (puff>14) { //if the user puffs to confirm the power level
      Serial.println("IMAGE888");
      delay(1000);
      while(1) { //loop which asks the user to confirm that theyre ready to shoot
        int confirmPuff = readPuff();
        int confirmxVal = analogRead(A0);
        int confirmyVal = analogRead(A2);
        if (confirmPuff>14) { //if they puff a second time, continue with shooting
          loop = 0;
          break;
        }
        else if (confirmxVal < 425 || confirmxVal > 625 || confirmyVal < 425 || confirmyVal > 625) { //if they move the joystick in any direction, return to power selecting
          break;
        }
      }
    }
  }
  delay(100);
  takeShot(powerLevel);
}

void takeShot(int powerLevel) {
  Serial.println("IMAGE999"); //display shooting image
  Serial.print("Shooting!!\n");
  int shotSpeed = calcSpeed(powerLevel); //SPEED EQUATION
  analogWrite(pinion_RPWM, shotSpeed); //Pinion moves rack forward
  analogWrite(pinion_LPWM, 0);
  delay(18000/shotSpeed); //For a certain amount of time depending on the shot speed
  analogWrite(solenoid_RPWM, 255); //Solenoid opens, launching puck
  analogWrite(solenoid_LPWM, 0);
  delay(12000/shotSpeed);
  analogWrite(pinion_RPWM, 0); //Pinion stops, stopping rack
  analogWrite(pinion_LPWM, 0);
  analogWrite(spin_RPWM, 0); //Spin stops
  analogWrite(spin_LPWM, 0);
  delay(2000); //Waits 2 seconds
  analogWrite(solenoid_RPWM, 0); //Closes solenoid
  analogWrite(solenoid_LPWM, 0);
  delay(2000);
  reHome(shotSpeed); //reHome the system 
}

int calcSpeed(int powerLevel) {
  if (powerLevel < 14) {
    int shotSpeed = (powerLevel * 3) + 60;
    return shotSpeed;
  }
  else if (powerLevel >= 14) {
    int shotSpeed = 155 + 50*(powerLevel - 14);
    return shotSpeed;
  }
}

void reHome(int shotSpeed) {
  delay(100);
  Serial.println("IMAGE001"); //show the rehoming image
  p_xPosition = &xPosition;
  Serial.print("System Rehoming\n");
  analogWrite(pinion_RPWM, 0);
  analogWrite(pinion_LPWM, .5*shotSpeed); 
  delay(26000/shotSpeed);
  analogWrite(pinion_RPWM, 0);
  analogWrite(pinion_LPWM, 0); //retract the claw using the pinion in reverse
  delay(1000);
  myservo.write(87); //set the launch angle back to 90 degrees
  int homingDirection = 0;
  while(1){ //if the system is not at x=0 (home) return to x=0
    delay(100);
    if (*p_xPosition > 0) { //if its greater than 0 (right) move left
      analogWrite(actuator_RPWM, 400);
      analogWrite(actuator_LPWM, 0);
      *p_xPosition -= 1;
    }
    else if (*p_xPosition < 0){ //if its less than 0 (left) move right
      analogWrite(actuator_RPWM, 0);
      analogWrite(actuator_LPWM, 400);
      *p_xPosition += 1;
    }
    else if (*p_xPosition == 0){
      break;
    }
  }
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 0);
  moveMode(); //enter move mode loop
}

void moveMode() {
  delay(100);
  Serial.println("IMAGE002");
  Serial.print("System entering Move Mode\n");
  Serial.print("Puff to confirm system in place for next shot with puck reloaded\n"); //display the move mode image
  while(1) { //wait for the user to puff to confirm system is ready to shoot again
    int puff = readPuff();
    if (puff>14) {
      Serial.print("Puff confirmed, returning to home screen\n");
      delay(1500);
      break; //break to reHome
    }
  } 
}

void precisionConfirmImage(int choice) {
  if (choice == 1) { //if they selected launch angle, display launch angle confirm image
    delay(300);
    Serial.print("Selected Launch Angle\n");
    Serial.println("IMAGE030");
  }
  else if (choice == 2) { //if they selected aim, display aim confirm image
    delay(300);
    Serial.print("Selected Aim\n");
    Serial.println("IMAGE010");
  }
  else if (choice == 3) { //if they selected spin, display spin confirm image
    delay(300);
    Serial.print("Selected Spin\n");
    Serial.println("IMAGE020");
  }
  else if (choice == 4) { //if they selected power and shoot, display power and shoot confirm image
    delay(300); 
    Serial.print("Selected Power and Shoot\n");
    Serial.println("IMAGE040");
  }
  Serial.print("CONFIRM YOUR CHOICE?\n");
}

void precisionConfirm(int choice) {
  precisionConfirmImage(choice);
  while(1) {
    int confirmXVal = analogRead(A0); //refresh the x-value with the current joystick position
    int confirmYVal = analogRead(A2); //refresh the y-value with the current joystick position
    int puff = readPuff(); //read the current pressure

    if (choice == 1) { //If the user selected LAUNCH ANGLE CONTROL

      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(500);
        controlAngle(); //control launch angle
        break; //break to homescreen
      }
      else if (confirmYVal > 625 || confirmXVal > 625 || confirmXVal < 425) { // If the joystick is moved RIGHT, UP, OR DOWN
        delay(500);
        break; //break to homescreen
      }
    }

    else if (choice == 2) { //If the user selected AIM CONTROL
      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(500);
        controlAim(); //control aim
        break; //break to homescreen
      }
      else if (confirmYVal > 625 || confirmYVal < 425 || confirmXVal < 425) { // If the joystick is moved RIGHT, LEFT, OR DOWN)
        delay(500);
        break; //break to homescreen
      }
    }

    else if (choice == 3) { //If the user selected SPIN CONTROL
      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(500);
        controlSpin(); //control spin
        break; //break to homescreen
      }
      else if (confirmXVal < 425 || confirmXVal > 625 || confirmYVal < 425) { // If the joystick is moved DOWN, UP, OR LEFT
        delay(500);
        break; //break to homescreen
      }
    }

    else if (choice == 4) { //If the user selected POWER AND SHOOT
      if (puff>14) { //If the user sufficiently puffs to confirm
        delay(500);
        controlShoot(); //control power and shoot
        break; //return to homescreen
      }
      else if (confirmYVal < 425 || confirmXVal > 625 || confirmYVal > 625) { // If the joystick is moved LEFT, UP, OR RIGHT
        delay(500);
        break; //return to homescreen
      }
    }
  }
}

void precisionHomescreen() {
  int run = 1;
  int *p_run;
  p_run = &run;
  while(run) {
    Serial.print("ON HOME SCREEN \n"); //start on the homescreen and display homescreen image
    Serial.println("IMAGE000");
    while(1) {
      int homeXVal = analogRead(A0); //refresh the x-value with the current joystick position
      int homeYVal = analogRead(A2); //refresh the y-value with the current joystick position
      int choice = 0; //initialize the choice variable
      if (homeYVal < 425) { //Joystick tilted LEFT, run LAUNCH ANGLE CONTROL
        delay(1000);
        precisionConfirm(1); //wait a second, then ask the user for confirmation on their choice
        break; //break and return to homescreen
      }
      else if (homeYVal > 625) { //Joystick tilted RIGHT, run SPIN CONTROL
        delay(1000);
        precisionConfirm(3); //wait a second, then ask the user for confirmation on their choice
        break; //break and return to homescreen
      }
      else if (homeXVal > 625) { //Joystick tilted UP, run AIM CONTROL
        delay(1000);
        precisionConfirm(2); //wait a second, then ask the user for confirmation on their choice
        break; //break and return to homescreen
      }
      else if (homeXVal < 425) { //Joystick tilted DOWN, run POWER AND SHOOT
        delay(1000);
        precisionConfirm(4); //wait a second, then ask the user for confirmation on their choice
        *p_run = 0;
        break; //break and return to homescreen
      }
    }
  }
}

void pickArcadeSpin() {
  Serial.println("IMAGE720");
  int spinChoice = 1;
  int *p_spinChoice;
  p_spinChoice = &spinChoice;
  while(1) {
    int yVal = analogRead(A2);
    int puff = readPuff();
    //display the correct image function (takes in choice)
    if (puff>14) {
      delay(1500);
      setArcadeSpin(*p_spinChoice);
      break;
    }
    else if (yVal < 375 && *p_spinChoice != 0) {
      *p_spinChoice -= 1;
      delay(500);
    }
    else if (yVal > 675 && *p_spinChoice != 2) {
      *p_spinChoice += 1;
      delay(500);
    }
    if (*p_spinChoice == 1){
      Serial.println("IMAGE720");
    }
    else if (*p_spinChoice == 0) {
      Serial.println("IMAGE719");
    }
    else if (*p_spinChoice == 2) {
      Serial.println("IMAGE721");
    }
  }
}

void setArcadeSpin(int choice) {
  if (choice == 0) {
    analogWrite(spin_RPWM, 0);
    analogWrite(spin_LPWM, 75);
  }
  else if (choice == 2) {
    analogWrite(spin_RPWM, 75);
    analogWrite(spin_LPWM, 0);
  }
}

void controlArcadeAngle() {
  Serial.println("IMAGE730");
  int angleDeg = 90; //set the initial launch angle to 90 degrees
  myservo.write(angleDeg);
  int direction = 1; //set the initial direction to 1 (CW)
  while(1) {
    int puff = readPuff(); //update the pressure sensor value
    if (puff>14) {
      delay(1500);
      break; //if user puffs, break from the angle control loop
    }
    else if (direction == 1) { //if direction is CW, increase angle by 1 every 15ms until 110 deg, then switch direction
      if (angleDeg < 110) {
        angleDeg += 4;
        myservo.write(angleDeg);
        delay(100);
      }
      else if (angleDeg == 110) {
        direction = 0;
      }
    }
    else if (direction == 0) { //if direction is CCW, decrease angle by 1 every 15ms until 70 deg, then switch direction
      if (angleDeg > 70) {
        angleDeg -= 4;
        myservo.write(angleDeg);
        delay(100);
      }
      else if (angleDeg == 70) {
        direction = 1;
      }
    }
    int k = ((angleDeg - 90)/4) + 730;
    String m = "IMAGE" + String(k);
    Serial.println(m);
  }
  delay(2000);
}

void controlArcadePowerAndShoot() {
  int powerLevel = 0;
  int direction = 1;
  Serial.println("IMAGE400");
  while(1) {
    int puff = readPuff();
    if (puff>14) {
      delay(500);
      break;
    }
    else if (direction == 1) {
      if (powerLevel < 16) {
        powerLevel += 1;
        int k = 400 + powerLevel;
        String m = "IMAGE" + String(k); //display the current power level image
        Serial.println(m);
        delay(100);
      }
      else if (powerLevel == 16) {
        direction = 0;
      }
    }
    else if (direction == 0) {
      if (powerLevel > 0) {
        powerLevel -= 1;
        int k = 400 + powerLevel;
        String m = "IMAGE" + String(k); //display the current power level image
        Serial.println(m);
        delay(100);
      }
      else if (powerLevel == 0) {
        direction = 1;
      }
    }
  }
  takeShot(powerLevel);
}

void runArcade() {
    pickArcadeSpin();
    controlAim();
    delay(750);
    controlArcadeAngle();
    controlArcadePowerAndShoot();
}

void setup() {
  Serial.begin(2000000); //start serial communication with the raspberry pi
  if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  } 
  Serial.println("Found MPRLS sensor");
  delay(100); //check to make sure the pressure sensor is connected
  
  pinMode(actuator_RPWM, OUTPUT); //Configure pin 13 as an Output
  pinMode(actuator_LPWM, OUTPUT); //Configure pin 12 as an Output
  pinMode(pinion_RPWM, OUTPUT); //Configure pin 11 as an Output
  pinMode(pinion_LPWM, OUTPUT); //Configure pin 10 as an Output
  pinMode(spin_RPWM, OUTPUT); //Configure pin 9 as an Output
  pinMode(spin_LPWM, OUTPUT); //Configure pin 8 as an Output
  pinMode(solenoid_RPWM, OUTPUT); //Configure pin 7 as an Output
  pinMode(solenoid_LPWM, OUTPUT); //Configure pin 6 as an Output
  myservo.attach(3, 500, 2500);  // attaches the servo on pin 34 to Arduino


  pinMode(A0, INPUT); //x-value input
  pinMode(A2, INPUT); //y-value input

  myservo.write(87); //set launch angle to 90 degrees
  delay(100);

  Serial.print("Doing initial Homing\n");
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 400);
  delay(14000);
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 0); //move linear actuator as far right as possible

  analogWrite(actuator_RPWM, 400);
  analogWrite(actuator_LPWM, 0);
  delay(11000);
  analogWrite(actuator_RPWM, 0);
  analogWrite(actuator_LPWM, 0); //move linear acuator 11 seconds left, setting this spot as your home position x=0
}

void loop() {
  Serial.println("IMAGE777");
  int selected = 0;
  int *p_selected;
  p_selected = &selected;
  while(1) {
    int YVal = analogRead(A2);
    int puff = readPuff();
    if (puff > 14 && *p_selected == 0) {
      delay(2000);
      precisionHomescreen();
    }
    else if (puff > 14 && *p_selected == 1) {
      delay(2000);
      runArcade();
    }
    else if (YVal < 375 && *p_selected != 0) {
      *p_selected -= 1;
      delay(500);
    }
    else if (YVal > 675 && *p_selected != 1) {
      *p_selected += 1;
      delay(500);
    }
    if (*p_selected == 0) {
      Serial.println("IMAGE777");
    }
    else if (*p_selected == 1) {
      Serial.println("IMAGE778");
    }
  }
}