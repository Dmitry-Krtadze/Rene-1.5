#include <Arduino.h>
#include <OCServo.h>
#include <ArduinoJson.h>
#include "config.h"
#include "comun.h"
#include "mathCalc.h"
#include "movementServo.h"
int aboba = 0, biba = 0;
void setZeroPositionAsPos(OCServo *servo, int pos)
{
  // uint16_t cur = servo->getCurrentPosition();
  // uint16_t zeroShift = (cur > 2047) ? ((4096 + cur - pos) % 4095) : (cur - pos);
  // servo->setZeroCorrection(zeroShift);

  uint16_t currentPos = servo->getCurrentPosition();
  delay(100);
  uint16_t zeroShift = 0;

  if (pos > 2047)
  {
    zeroShift = (4096 + currentPos - pos) % 4096;
  }
  else
  {
    zeroShift = (currentPos - pos);
  }
  aboba = zeroShift;
  biba = currentPos;

  servo->setZeroCorrection(zeroShift);
}

void calibrationRene()
{
  if (!digitalRead(CALIB_BUTTON))
  {
    digitalWrite(RedLED, HIGH);
    digitalWrite(GreenLED, HIGH);
    digitalWrite(BlueLED, HIGH);

    // servo1.setLockSign(0x00);
    // delay(100);
    // servo2.setLockSign(0x00);
    // delay(100);
    // servo3.setLockSign(0x00);
    // delay(100);
    // servo4.setLockSign(0x00);
    // delay(100);

    setZeroPositionAsPos(&servo1, 0);
    delay(100);
    // setZeroPositionAsPos(&servo2, 0);
    // delay(100);
    // setZeroPositionAsPos(&servo3, 4090);
    // delay(100);
    // setZeroPositionAsPos(&servo4, 0);
    // delay(100);

    for (int i = 0; i < 2; i++)
    {
      digitalWrite(RedLED, LOW);
      digitalWrite(GreenLED, LOW);
      digitalWrite(BlueLED, LOW);
      delay(120);
      digitalWrite(RedLED, HIGH);
      digitalWrite(GreenLED, HIGH);
      digitalWrite(BlueLED, HIGH);
      delay(120);
    }

    digitalWrite(RedLED, LOW);
    digitalWrite(GreenLED, LOW);
    digitalWrite(BlueLED, LOW);
    delay(120);

    // servo1.setLockSign(0x01);
    // delay(60);
    // servo2.setLockSign(0x01);
    // delay(60);
    // servo3.setLockSign(0x01);
    // delay(60);
    // servo4.setLockSign(0x01);
    // delay(60);
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(CALIB_BUTTON, INPUT);
  pinMode(RedLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);

  OPI_UART.begin(OPI_BAUD);

  servo1.begin();
  servo1.setBaudRate(38400);
  servo1.setMode(OCS_SERVO);
  servo1.setMaxTorque(300);
  delay(200);
  servo2.begin();
  servo2.setBaudRate(38400);
  servo2.setMode(OCS_SERVO);
  servo2.setMaxTorque(300);
  delay(200);
  servo3.begin();
  servo3.setBaudRate(38400);
  servo3.setMode(OCS_SERVO);
  servo3.setMaxTorque(300);
  delay(200);
  servo4.begin();
  servo4.setBaudRate(38400);
  servo4.setMode(OCS_SERVO);
  servo4.setMaxTorque(300);
  delay(200);

  // calibrationRene();
  OPI_UART.println("[INFO]  Start complete");
}

void analyzeCors_MoveServo()
{
  if (OPI_UART.available() == 0)
  {
    return;
  }
  delay(100);

  // Получаем координаты с UART
  Coors coors = readUART();
  if (areCoorsInvalid(coors))
    return;

  if (!areCoorsFixed(coors, prevCoors))
  {
    prevCoors = coors;
    return;
  }

  prevCoors = coors;
  if (!areServoAnglesCanBeCalculated(coors))
    return;

  calculateAngles(coors.x, coors.y, CUBE_HEIGHT);
  servos[3]->setGoalAngle(BASE_ANGLE_4);
  moveServos();
  delay(150);
  servos[3]->setGoalAngle(BASE_ANGLE_4 - 45);

  delay(350);

  servoAngles[0] = BASE_ANGLE_1;
  servoAngles[1] = BASE_ANGLE_2;
  servoAngles[2] = BASE_ANGLE_3;
  moveServos();

  calculateAngles(coors.finishX, coors.finishY, CUP_HEIGHT);
  moveServos();
  delay(100);
  servos[3]->setGoalAngle(BASE_ANGLE_4);

  delay(100);

  servoAngles[0] = BASE_ANGLE_1;
  servoAngles[1] = BASE_ANGLE_2;
  servoAngles[2] = BASE_ANGLE_3;
  moveServos();
}

void TestMoveOnCors()
{
  if (OPI_UART.available() == 0)
    return;
  delay(50);
  Coors coors = readUART();
  if (areCoorsInvalid(coors))
    return;
  calculateAngles(coors.x, coors.y, coors.z);
  moveServos();
}

void TestMoveServo()
{
  servo1.setGoalPosition(300);
  servoAngles[1] = 0;
  servoAngles[2] = 0;
  servoAngles[3] = 0;
  moveServos();
  OPI_UART.print(" Move on 1 side");
  OPI_UART.print(servo1.getCurrentPosition());

  // delay(3000);

  // servoAngles[0] = 120;
  // servoAngles[1] = 0;
  // servoAngles[2] = 0;
  // servoAngles[3] = 0;
  // moveServos();
  // OPI_UART.print(" Move on 2 side");
  // OPI_UART.print(servo1.getCurrentPosition());

  // delay(3000);
}

void changeGrip(bool grip)
{
  if (!grip)
  {
    servo4.setGoalPosition(4000);
  }
  else
  {
    servo4.setGoalPosition(3200);
  }
}

enum RobotState : uint8_t
{
  ST_READY = 0,
  ST_ACCEPTED,
  ST_MOVE_TO_CUBE,
  ST_GRAB_CUBE,
  ST_MOVE_TO_MID,
  ST_MOVE_TO_CUP,
  ST_RELEASE_CUBE,
  ST_DONE
};

static RobotState st = ST_READY;
static VisionCommand cmd;
static String rxLine;

static void sendRobotStatus(const char *s)
{
  OPI_UART.print("{\"robot_status\":\"");
  OPI_UART.print(s);
  OPI_UART.println("\"}");
}


static void gripClose() { servo4.setGoalPosition(3200); } 
static void gripOpen() { servo4.setGoalPosition(4000); } 

static bool waitArmSettled(uint32_t timeoutMs = 4000, uint16_t eps = 2, uint8_t stableNeed = 10)
{
  uint32_t t0 = millis();

  uint16_t p1_prev = servo1.getCurrentPosition();
  uint16_t p2_prev = servo2.getCurrentPosition();
  uint16_t p3_prev = servo3.getCurrentPosition();

  uint8_t stable = 0;

  while (millis() - t0 < timeoutMs)
  {
    delay(20);

    uint16_t p1 = servo1.getCurrentPosition();
    uint16_t p2 = servo2.getCurrentPosition();
    uint16_t p3 = servo3.getCurrentPosition();

    bool ok =
        (abs((int)p1 - (int)p1_prev) <= eps) &&
        (abs((int)p2 - (int)p2_prev) <= eps) &&
        (abs((int)p3 - (int)p3_prev) <= eps);

    if (ok)
      stable++;
    else
      stable = 0;

    p1_prev = p1;
    p2_prev = p2;
    p3_prev = p3;

    if (stable >= stableNeed)
      return true;
  }
  return false; 
}

static bool goToXYZ(float x, float y, float z)
{
  calculateAngles(x, y, z);
  moveServos();
  return waitArmSettled();
}

int target = 0;

void loop()
{


  if (st == ST_READY)
  {
    if (readJsonLine(rxLine))
    {
      VisionCommand tmp;
      if (parseVisionJson(rxLine, tmp))
      {
        cmd = tmp;
        sendRobotStatus("accepted_command"); 
        st = ST_ACCEPTED;
      }
      rxLine = "";
    }
  }

  switch (st)
  {

  case ST_ACCEPTED:
    if (cmd.status == 200 && strcmp(cmd.target, "dual") == 0)
    {
      sendRobotStatus("moving_to_cube");
      st = ST_MOVE_TO_CUBE;
    }
    else if (cmd.status == 204)
    {
      sendRobotStatus("no_pair");
      st = ST_DONE;
    }
    else if (cmd.status == 404)
    {
      sendRobotStatus("no_aruco_board");
      st = ST_DONE;
    }
    else if (cmd.status == 500)
    {
      sendRobotStatus("server_error");
      st = ST_DONE;
    }
    else
    {
      sendRobotStatus("unknown_status");
      st = ST_DONE;
    }
    break;

  case ST_MOVE_TO_CUBE:
  {
    bool ok = goToXYZ(cmd.block.x, cmd.block.y, cmd.block.z);
    if (ok)
    {
      sendRobotStatus("cube_reached");
      st = ST_GRAB_CUBE;
    }
    else
    {
      sendRobotStatus("move_timeout_cube");
      st = ST_DONE;
    }
  }
  break;

  case ST_GRAB_CUBE:
    gripClose();
    delay(600); 
    sendRobotStatus("cube_grabbed");
    st = ST_MOVE_TO_MID;
    break;

  case ST_MOVE_TO_MID:
  {
    sendRobotStatus("moving_to_mid");
    bool ok = goToXYZ(150.0f, 200.0f, cmd.block.z);
    if (ok)
    {
      sendRobotStatus("mid_reached");
      st = ST_MOVE_TO_CUP;
    }
    else
    {
      sendRobotStatus("move_timeout_mid");
      st = ST_DONE;
    }
  }
  break;

  case ST_MOVE_TO_CUP:
  {
    sendRobotStatus("moving_to_cup");
    bool ok = goToXYZ(cmd.cup.x, cmd.cup.y, cmd.cup.z);
    if (ok)
    {
      sendRobotStatus("cup_reached");
      st = ST_RELEASE_CUBE;
    }
    else
    {
      sendRobotStatus("move_timeout_cup");
      st = ST_DONE;
    }
  }
  break;

  case ST_RELEASE_CUBE:
    gripOpen();
    delay(600);
    sendRobotStatus("cube_released");
    st = ST_DONE;
    break;

  case ST_DONE:
    sendRobotStatus("ready"); 
    st = ST_READY;
    break;

  default:
    st = ST_READY;
    break;
  }
}

// TestMoveOnCors();
// changeGrip(grip);

// OPI_UART.print("   L0:");
// OPI_UART.print(servo1.getCurrentPosition());
// OPI_UART.print("   L1:");
// OPI_UART.print(servo2.getCurrentPosition());
// OPI_UART.print("   L3:");
// OPI_UART.print(servo3.getCurrentPosition());

// OPI_UART.print("   BI:");
// OPI_UART.print(biba);//servo1.getTotalTurns());
// OPI_UART.print("   AB:");
// OPI_UART.println(aboba);

// if (target == 0) target = 500;
// else target = 0;

// servo2.setGoalPosition(target);

// servo4.setGoalPosition(4000);
// delay(2000);
// servo4.setGoalPosition(3200);
// delay(2000);
// servo3.setGoalAngle(360);
// servo2.setGoalAngle(180);
// servo3.setGoalAngle(180);

// OPI_UART.print("L0:");

// delay(100);
// OPI_UART.print("L0:");
// OPI_UART.print(servo1.getCurrentPosition());
// OPI_UART.print("   L1:");
// OPI_UART.print(servo2.getCurrentPosition());
// OPI_UART.print("   L3:");
// OPI_UART.println(servo3.getCurrentPosition());
// int pos = servo1.getCurrentPosition();
// if (pos == -1) {digitalWrite(RedLED, HIGH); digitalWrite(BlueLED, LOW);}
// else {digitalWrite(RedLED, LOW); digitalWrite(BlueLED, HIGH);}

// ######## Тест серво + комуникация ########
//  servo1.setGoalAngle(posX);
//  delay(200);
//  servo2.setGoalAngle(posY);
//  delay(200);
//  servo3.setGoalAngle(posZ);
//  delay(200);
//  if(grip == 1){
//    servo4.setGoalAngle(200);
//    delay(200);
//  }else{
//    servo4.setGoalAngle(0);
//    delay(200);
//  }

// OPI_UART.print("[INFO] Base="); OPI_UART.print(servoAngles[0]);
// OPI_UART.print("  Joint1="); OPI_UART.print(servoAngles[1]);
// OPI_UART.print("  Joint2="); OPI_UART.println(servoAngles[2]);