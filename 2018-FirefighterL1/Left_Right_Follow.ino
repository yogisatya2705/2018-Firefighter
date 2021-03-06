int followDirection = 0; //left follow or right follow
int lowerleftroomdistance = 10; //distance to activate Rick's Code
double diagonalMove = 5; //how far the robot should move for its diagonal path
double lateralMove = 5; //how far the robot should move to exit the lower right room

void upperRightRoom()
{
  if(!dog1){
    rollForward();
    while(frontRightUltrasonic.getDistance() > closeToWall && rightUltrasonic.getDistance() < closeToWall){
      delay(50);
    }
    stopRobot();

    flashLED(MIC_LED, 500, 3);

    moveForward(2);
    turn(-90);    // if there is no right wall turn right
    moveForward(4);
    turn(30);
    turn(-210);

    flashLED(MIC_LED, 500, 4);

    moveForward(4);
    turn(-90);
    rollForward();
    while(frontRightUltrasonic.getDistance() > closeToWall){
      delay(50);
    }
    stopRobot();
    turn(-90);
    stopRobot();

    flashLED(MIC_LED, 500, 5);
  }else{
    rollForward();
    while(frontRightUltrasonic.getDistance() > closeToWall && leftUltrasonic.getDistance() < closeToWall){
      delay(50);
    }
    stopRobot();
    moveForward(2);
    turn(90);
    moveForward(5);
    rollForward();
    while(frontRightUltrasonic.getDistance() > closeToWall && leftUltrasonic.getDistance() < closeToWall){
      delay(50);
    }
    stopRobot();
    moveForward(3);
    turn(-90);
  }
}

void secondDogCheck() { //robot is facing downward where the second dog could potentially be
  double dogDistance = 3;
  if(frontRightUltrasonic.getDistance() < dogDistance){
    dog2 = true;
  }
  delay(50);
}

void levelOneNav() {

  upperRightRoom();
  if(!dog1){
    secondDogCheck();
  }

  if(dog2) {
    turn(-90); //turn towards start
    moveForward(60); //move back to start
    turn(90); //turn downward
    moveForward(36);  //move down past upperRightRoom
    turn(90); //turn left
    moveForward(60); //move back into position for lowerRightRoom
    turn(-90);
    rollForward();
    while(rightUltrasonic.getDistance() < closeToWall) {
      delay(50);
    }
    stopRobot();
    lowerRightRoom();
  } else {
    rollForward();
    while (rightUltrasonic.getDistance() < closeToWall) {
      delay(50);
    }
    stopRobot();
    lowerRightRoom();
  }
  while(followDirection >= 7)
  {
    if (frontRightUltrasonic.getDistance() > closeToWall && leftUltrasonic.getDistance() < closeToWall)
    {
      moveSlightRight();
      delay(500);    //move left diagnal
      while(leftUltrasonic.getDistance() < closeToWall){
        moveSlightLeft();
      }
      delay(500);
    }

    else if (leftUltrasonic.getDistance() > closeToWall)
    {
      turn(90);    // if there is no left wall turn left
      delay(500);
      followDirection = followDirection + 1;
    }
    else if (frontRightUltrasonic.getDistance() < lowerleftroomdistance)
    {
      followDirection = followDirection + 1;
    }
  }

  //previously, this was where the lowerRightRoom code tripped

  while(followDirection >= 9)
  {
    if (frontRightUltrasonic.getDistance() > closeToWall && leftUltrasonic.getDistance() < closeToWall)
    {
      moveSlightRight();
      delay(500);    //move left diagnal
      while(leftUltrasonic.getDistance() < closeToWall){
        moveSlightLeft();
      }
      delay(500);
    }

    else if (leftUltrasonic.getDistance() > closeToWall && frontRightUltrasonic.getDistance() > closeToWall)
    {
      turn(90);
      delay(500);
      followDirection = followDirection + 1;
    }
    else if (leftUltrasonic.getDistance() < closeToWall && frontRightUltrasonic.getDistance() < closeToWall)
    {
      turn(-90);
      delay(500);
      followDirection = followDirection + 1;
    }
  }
}

void lowerRightRoom(){
  turn(-45);

  moveForward(diagonalMove);

  turn(-90);
  delay(1000);  //see if there's fire in the upper corner
  turn(180);
  delay(1000);  //see if there's fire in the lower corner
  turn(-45);

  while(frontRightUltrasonic.getDistance() > closeToWall){ //roll forward until the robot gets close to the lower wall
    rollForward();
    delay(50);
  }
  stopRobot();

  turn(-90);  //turn right towards the exit of the room
  moveForward(lateralMove);
  turn(0);
}
