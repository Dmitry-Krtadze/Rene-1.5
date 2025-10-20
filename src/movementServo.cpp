#include "config.h"
#include "movementServo.h"

void moveServos() {
  change = true;

  for (int j = 0; j < 180; j++) {
    changeServo(0);
    changeServo(1);
    changeServo(2);
    changeServo(3);

    if (!change)
      break;

    change = false;
    delay(15);
  }
}

void changeServo(int servoNum) {
  if (currentAngles[servoNum] > servoAngles[servoNum]) {
    int spd = (currentAngles[servoNum] - servoAngles[servoNum]) / 20;
    if (spd == 0) spd = 1;
    currentAngles[servoNum] -= spd;
    servos[servoNum]->setGoalAngle(currentAngles[servoNum]);
    change = true;
  }
  else if (currentAngles[servoNum] < servoAngles[servoNum]) {
    int spd = (servoAngles[servoNum] - currentAngles[servoNum]) / 20;
    if (spd == 0) spd = 1;
    currentAngles[servoNum] += spd;
    servos[servoNum]->setGoalAngle(currentAngles[servoNum]);
    change = true;
  }
}