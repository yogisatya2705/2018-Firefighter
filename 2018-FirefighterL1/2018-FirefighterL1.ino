#include <Servo.h>
#include <Motor.h>

#include <Ultrasonic.h>

#include "FreqCount.h"

#define LEFT_MOTOR_1 6
#define LEFT_MOTOR_2 7

#define RIGHT_MOTOR_1 8
#define RIGHT_MOTOR_2 9

#define IR_PIN_LEFT 6
#define IR_PIN_RIGHT 20

#define EXTINGUISHER_PIN 7
#define CAMERA 15

//LED Pins
#define FLAME_LED 30
#define MIC_LED 31

//Ultrasonic Trigger and Echo pins (30-39)
#define F_R_ECHO  45  //Front-right Ultrasonic
#define F_R_TRIG  44

#define R_ECHO  53  //Right Ultrasonic
#define R_TRIG  52

#define B_ECHO  51  //Back Ultrasonic
#define B_TRIG  50

#define L_ECHO  46  //Left Ultrasonic
#define L_TRIG  49

// Bounds for start sound frequency:
#define LOW_START 3306
#define HIGH_START 4200

Motor leftMotor(LEFT_MOTOR_1, LEFT_MOTOR_2);
Motor rightMotor(RIGHT_MOTOR_1,RIGHT_MOTOR_2);
Servo extinguisher;

//Motor top speeds
double rightSpeed = 0.5;
double leftSpeed = 0.5;

//dog flags
bool dog1 = false;
bool dog2 = false;

Ultrasonic frontRightUltrasonic(F_R_ECHO, F_R_TRIG, true);
Ultrasonic leftUltrasonic(L_ECHO, L_TRIG, true);
Ultrasonic rightUltrasonic(R_ECHO, R_TRIG, true);
Ultrasonic backUltrasonic(B_ECHO, B_TRIG, true);

//Move controls
float v_max = 24;
float t_acceleration = 0;

float getRotation();

//Gyroscope controls
float gyroStartAngle;
float gyroTargetAngle = 0;

float angleThreshold = 3;
float driftAngle = 30;

unsigned long freqCount;
double closeToWall = 2;   //defines how close the robot should be (inches) to the wall to register as being "too close"

long getFreqCount() {
  return freqCount;
}

void rollForward() {
  leftMotor.set(leftSpeed);
  rightMotor.set(rightSpeed);
}

void rollBackward() {
  leftMotor.set(-leftSpeed);
  rightMotor.set(-rightSpeed);
}

void moveForward(float distance){
  float time_travel = (distance/v_max) - t_acceleration;
  leftMotor.set(leftSpeed);
  rightMotor.set(rightSpeed);
  delay(time_travel*1000);
  leftMotor.set(0);
  rightMotor.set(0);
}

void moveSlightLeft() {
  leftMotor.set(leftSpeed/2);
  rightMotor.set(rightSpeed*.75);
  delay(100);
  stopRobot();
}

void moveSlightRight() {
  leftMotor.set(leftSpeed*.75); //Half power plus additional 25
  rightMotor.set(rightSpeed/2);
  delay(100);
  stopRobot();
}

void stopRobot() {
  leftMotor.set(0);
  rightMotor.set(0);
}

boolean detectFire(){
  if(digitalRead(IR_PIN_RIGHT || IR_PIN_RIGHT)){
    return true;
  } else {
    return false;
  }
}

void startExtinguisher(){
  extinguisher.write(0);
}

void stopExtinguisher(){
  extinguisher.write(70);
  while(true){
    digitalWrite(FLAME_LED, HIGH);
  }
}

//Gyro functions
//Resets the gyro so that the current positioning is angle "0"
void resetGyro(){
  for(int x=0; x<250; x++){
    getRotation();
    delay(25);
  }
  gyroStartAngle = getRotation();
  gyroTargetAngle = 0;
}

//Returns the current angle of the robot relative to its starting angle
float getGyroAngle(){
  float angle = getRotation() - gyroStartAngle;
  if(angle >= 360){
    angle = angle - 360;
  }
  else if(angle < 0){
    angle = 360 + angle;
  }
  return angle;
}

//adjusts the target angle based on how much we want to turn and turns the robot until that target is reached
//use turn(0) to simply get the gyro back on track if it's off target
void turn(int angle){
  if(angle > driftAngle){
    angle -= driftAngle;
  }else if(angle < -driftAngle){
    angle += driftAngle;
  }
  
  gyroTargetAngle += angle;
  if(gyroTargetAngle >= 360){
    gyroTargetAngle = gyroTargetAngle - 360;
  }
  else if(gyroTargetAngle < 0){
    gyroTargetAngle = 360 + gyroTargetAngle;
  }

  if(angle > 0){
    leftMotor.set(-leftSpeed);
    rightMotor.set(rightSpeed);
  }else{
    leftMotor.set(leftSpeed);
    rightMotor.set(-rightSpeed);
  }

  while(abs(getGyroAngle() - gyroTargetAngle) > angleThreshold){
    Serial.println(String(getGyroAngle()));
    delay(5);
  }

  stopRobot();
}

void extinguishFire(){
  while (detectFire()) {
     if (detectFire()){
      digitalWrite(FLAME_LED, HIGH);
     } else {
     digitalWrite(FLAME_LED, LOW);
     }

    if (frontRightUltrasonic.getDistance() > 3) {
      rollForward();
    } else {
      stopRobot();
      delay(500);

      startExtinguisher();
      delay(500);

      if (!detectFire()) {
        stopExtinguisher();
      }
    }
    delay(100);
  }
}

void checkMicrophone() {
  // Measure sound:
  while (true) {
    if (FreqCount.available()) {
      freqCount = FreqCount.read() * 5;
      Serial.println(String(getFreqCount()));
    } else {
      freqCount = 0;
    }
    // If the sound frequency is within start sound bounds, turn on robot:
    if (LOW_START < freqCount && freqCount < HIGH_START) {
      break;
    }
  }
  
  flashLED(MIC_LED, 1000, 1);
}

void flashLED(int pin, int milliseconds, int numFlashes){
  for(int x=0; x<numFlashes; x++){
    digitalWrite(pin, HIGH);
    delay(milliseconds);
    digitalWrite(pin, LOW);
    if(x < numFlashes-1)
      delay(milliseconds);
  }
}


float calcAvg(float x,float y){
  float avg=(x+y)/2;
  return avg;
}

void startUp(){
  //ping ultrasonics twice and average the values to get (hopefully) accurate values
  float right=calcAvg(rightUltrasonic.getDistance(),rightUltrasonic.getDistance());
  float left=calcAvg(leftUltrasonic.getDistance(),leftUltrasonic.getDistance());
  float front=calcAvg(frontRightUltrasonic.getDistance(),frontRightUltrasonic.getDistance());
  float back=calcAvg(backUltrasonic.getDistance(),backUltrasonic.getDistance());

  if(right < closeToWall && back < closeToWall && left < closeToWall) {  //robot is facing downward WITH DOG
    turn(0);
    dog1 = true;
  } else if(right < closeToWall && front < closeToWall && back < closeToWall){  //robot is facing backward WITH DOG
    turn(90);
    dog1 = true;
  } else if(front < closeToWall && left < closeToWall && right < closeToWall){ //robot is facing upward WITH DOG
    turn(180);
    dog1 = true;
  } else if(front < closeToWall && left < closeToWall && back < closeToWall){   //robot is facing the correct direction WITH DOG
    turn(-90);
    dog1 = true;
  } else if(right < closeToWall && back < closeToWall) { //downward, no dog
    turn(90);
  } else if(right < closeToWall && front < closeToWall) { //backward, no dog
    turn(180);
  } else if(front < closeToWall && left < closeToWall) { //upward, no dog
    turn(-90);
  } else {  //forward, no dog
    turn(0);
  }

  flashLED(MIC_LED, 500, 1);

  if(!dog1){
    rollForward();
    while(rightUltrasonic.getDistance() > closeToWall){   //roll forward until the first room is encountered, then kick out of function
      delay(50);
    }
    stopRobot();
  }else{
    rollForward();
    while(leftUltrasonic.getDistance() > closeToWall){   //roll forward until the first room is encountered, then kick out of function
      delay(50);
    }
    stopRobot();
  }

  flashLED(MIC_LED, 500, 2);
}

void setup() {
  stopRobot();
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Setting up extinguisher");
  extinguisher.attach(EXTINGUISHER_PIN);
  extinguisher.write(70);

  Serial.println("Setting up IR pins and interrupts");
  pinMode(IR_PIN_LEFT, INPUT);
  pinMode(IR_PIN_RIGHT, INPUT);
  //attachInterrupt(digitalPinToInterrupt(IR_PIN_LEFT), extinguishFire, RISING);
  //attachInterrupt(digitalPinToInterrupt(IR_PIN_RIGHT), extinguishFire, RISING);

  Serial.println("Setting up flame, mic leds");
  pinMode(FLAME_LED, OUTPUT);
  pinMode(MIC_LED, OUTPUT);

  Serial.println("Setting up gyro");
  setup_gyro();
  resetGyro();

  stopRobot();  //for the love of god stop

  while(true){
    Serial.println("Listening for microphone");
    FreqCount.begin(200); // Begin measuring sound
    checkMicrophone();  //robot stays in setup until frequency is heard

    turn(90);
  }
}

void loop() {

  //startUp();
  //levelOneNav();
  //getRotation();
  delay(100);
}
