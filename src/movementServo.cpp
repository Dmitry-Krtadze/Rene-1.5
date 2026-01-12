#include "config.h"
#include "movementServo.h"



void moveServos() {
  servo3.setGoalAngle(servoAngles[2]);
  delay(250);
  servo2.setGoalAngle(servoAngles[1]);
  delay(250);
  servo1.setGoalAngle(servoAngles[0]);
  
  
}


void changeServo(int servoNum) {
  static const float dir[4] = { SERVO1_DIR, SERVO2_DIR, SERVO3_DIR, SERVO4_DIR };

  if (currentAngles[servoNum] > servoAngles[servoNum]) {
    int spd = (currentAngles[servoNum] - servoAngles[servoNum]) / 20;
    if (spd == 0) spd = 1;
    currentAngles[servoNum] -= spd;
    servos[servoNum]->setGoalAngle(currentAngles[servoNum] * dir[servoNum]);
    change = true;
  }
  else if (currentAngles[servoNum] < servoAngles[servoNum]) {
    int spd = (servoAngles[servoNum] - currentAngles[servoNum]) / 20;
    if (spd == 0) spd = 1;
    currentAngles[servoNum] += spd;
    servos[servoNum]->setGoalAngle(currentAngles[servoNum] * dir[servoNum]);
    change = true;
  }
}
