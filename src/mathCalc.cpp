#include "config.h"
#include "mathCalc.h"


void calculateAngles(float x, float y, float z) {
  if (x > 40) { x += 10; y += 10; }
  if (x < -40) { x += 10;}
  if (x < 0 && y < 120 && y > 100) {x += 5; y -= 5; }
  int angle1 = calculateBaseAnlge(x, y);

  float h = BASE_HEIGHT;
  float l1 = L1_LENGTH;
  float l2 = L2_LENGTH;

  bool underTheBase = z < h;
  z = abs(h - z);
  float d = sqrt(x * x + y * y);
  
  float rd = sqrt(d * d + z * z);

  int angle2 = calculateFirstShoulderAngle(z, d, rd, l1, l2, underTheBase);
  int angle3 = calculateSecondShoulderAngle(rd, l1, l2);

  if (y > 255) { angle2 -= 5; angle3 += 13; }
  else if (y > 215) angle3 += 6;
  else if (y > 160) angle3 += 2;
  else if (y < 90) angle3 -= 5;

  if (y < 112 && x > -60 && x < 60) {angle3 += 3;}


  servoAngles[0] = angle1;
  servoAngles[1] = angle2 + ANGLE_2_SHIFT;
  servoAngles[2] = angle3 + ANGLE_3_SHIFT;
}

int calculateBaseAnlge(float x, float y) {
  return BASE_ANGLE_1 + round(atan(x / y) * 180 / M_PI);
}

int calculateFirstShoulderAngle(float z, float d, float rd, float l1, float l2, bool underTheBase) {
  float heightAngle = atan(z / d) * 180 / M_PI;
  float result = acos((l1 * l1 + rd * rd - l2 * l2) / (2 * l1 * rd)) * 180 / M_PI;
  return underTheBase ? result - heightAngle : result + heightAngle;
}

int calculateSecondShoulderAngle(float rd, float l1, float l2) {
  return acos((l1 * l1 + l2 * l2 - rd * rd) / (2 * l1 * l2)) * 180 / M_PI;
}

bool areCoorsInvalid(Coors coors) {
  return (coors.x == 0 && coors.y == 0) || (coors.y < 30 || coors.finishY < 30);
}

bool areCoorsFixed(Coors coors, Coors prevCoors) {
  return (coors.finishX <= prevCoors.finishX + 1 && coors.finishX >= prevCoors.finishX - 1) && (coors.finishY <= prevCoors.finishY + 1 && coors.finishY >= prevCoors.finishY - 1) && (coors.x <= prevCoors.x + 1 && coors.x >= prevCoors.x - 1) && (coors.y <= prevCoors.y + 1 && coors.y >= prevCoors.y - 1);
}

bool areServoAnglesCanBeCalculated(Coors coors) {
  bool valid = true;
  calculateAngles(coors.x, coors.y, CUBE_HEIGHT);
  for (int i = 0; i < 4; i++) {
    if (isnan(servoAngles[i]) || servoAngles[i] == 0) {
      valid = false;
    }
  }
  calculateAngles(coors.finishX, coors.finishY, CUP_HEIGHT);
  for (int i = 0; i < 4; i++) {
    if (isnan(servoAngles[i]) || servoAngles[i] == 0) {
      valid = false;
    }
  }
  return valid;
}
