#ifndef MATHCALC_H
#define MATHCALC_H

#include "config.h"
#include "mathCalc.h"

void calculateAngles(float x, float y, float z);
int calculateBaseAnlge(float x, float y);
int calculateFirstShoulderAngle(float z, float d, float rd, float l1, float l2, bool underTheBase);
int calculateSecondShoulderAngle(float rd, float l1, float l2);
bool areCoorsInvalid(Coors coors);
bool areCoorsFixed(Coors coors, Coors prevCoors);
bool areServoAnglesCanBeCalculated(Coors coors);

#endif
