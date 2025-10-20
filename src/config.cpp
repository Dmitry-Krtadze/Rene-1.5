#include "config.h"

byte id1 = 0x05;
byte id2 = 0x02;
byte id3 = 0x03;
byte id4 = 0x04;

OCServo servo1(id1, &Serial1);
OCServo servo2(id2, &Serial1);
OCServo servo3(id3, &Serial1);
OCServo servo4(id4, &Serial1);
OCServo* servos[4] = { &servo1, &servo2, &servo3, &servo4 };

HardwareSerial OPI_UART(PA3, PA2);

float posX = 90, posY = 90, posZ = 90;
int grip = 0;
String inputBuffer = "";
Coors prevCoors = {0, 0, 0, 0};

int servoAngles[4] = { BASE_ANGLE_1, BASE_ANGLE_2, BASE_ANGLE_3, BASE_ANGLE_4 };
int currentAngles[4] = { BASE_ANGLE_1, BASE_ANGLE_2, BASE_ANGLE_3, BASE_ANGLE_4 };
int servs[4] = { 5, 2, 3, 4 };
bool change = false;
