#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <OCServo.h>

// -------- STRUCTS (перенесено вверх!) ----------
typedef struct {
  float x;
  float y;
  float finishX;
  float finishY;
} Coors;

// -------- SERVO IDS ----------
extern byte id1;
extern byte id2;
extern byte id3;
extern byte id4;

// -------- SERVO OBJECTS ----------
extern OCServo servo1;
extern OCServo servo2;
extern OCServo servo3;
extern OCServo servo4;
extern OCServo* servos[4];

// -------- UART ----------
extern HardwareSerial OPI_UART;
#define OPI_BAUD 115200

// -------- VARIABLES ----------
extern float posX, posY, posZ;
extern int grip;
extern String inputBuffer;
extern Coors prevCoors;        // теперь тип уже известен
extern int servoAngles[4];
extern int currentAngles[4];
extern int servs[4];
extern bool change;

// -------- CONSTANTS ----------
#define CALIB_BUTTON PB5
#define RedLED PB6
#define GreenLED PB7
#define BlueLED PB4

#define BASE_ANGLE_1 180
#define BASE_ANGLE_2 20
#define BASE_ANGLE_3 20
#define BASE_ANGLE_4 20

#define ANGLE_2_SHIFT -23
#define ANGLE_3_SHIFT 0

#define CUBE_HEIGHT 20
#define CUP_HEIGHT 120
#define BASE_HEIGHT 83
#define L1_LENGTH 125
#define L2_LENGTH 140

#define CAMERA_X_KOEF 1.23
#define CAMERA_Y_KOEF 1.23
#define Y_COOR_SHIFT 280
#define X_COOR_SHIFT 330

#endif
