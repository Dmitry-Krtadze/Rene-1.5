#pragma once
#include <Arduino.h>

void calculateAngles(float x, float y, float z);

int  calculateBaseAnlge(float x, float z);
int  calculateFirstShoulderAngle(float x, float z, float l1, float l2);
int  calculateSecondShoulderAngle(float x, float z, float l1, float l2);

bool areCoorsInvalid(Coors c);
bool areCoorsFixed(Coors c, Coors p);
bool areServoAnglesCanBeCalculated(Coors c);
