#include <Arduino.h>
#include <OCServo.h>
#include "config.h"
#include "comun.h"
#include "mathCalc.h"
#include "movementServo.h"



void setZeroPositionAsPos(OCServo *servo, int pos) {
  uint16_t cur = servo->getCurrentPosition();
  uint16_t zeroShift = (cur > 2047) ? ((4096 + cur - pos) % 4095) : (cur - pos);
  servo->setZeroCorrection(zeroShift);
}


void calibrationRene() {
  if (!digitalRead(CALIB_BUTTON)) {
    digitalWrite(RedLED, HIGH); digitalWrite(GreenLED, HIGH); digitalWrite(BlueLED, HIGH);
    servo1.setLockSign(0x00);   delay(100);
    servo2.setLockSign(0x00);   delay(100);
    servo3.setLockSign(0x00);   delay(100);
    servo4.setLockSign(0x00);   delay(100);

    setZeroPositionAsPos(&servo1, 2048); delay(100);
    setZeroPositionAsPos(&servo2, 0);    delay(100);
    setZeroPositionAsPos(&servo3, 0);    delay(100);
    setZeroPositionAsPos(&servo4, 0);    delay(100);

    servo2.setGoalPosition(2048, 1000);  delay(700);
    servo3.setGoalPosition(2048, 1000);  delay(700);
    servo4.setGoalPosition(200, 1000);   delay(700);

    for (int i = 0; i < 2; i++) {
      digitalWrite(RedLED, LOW); digitalWrite(GreenLED, LOW); digitalWrite(BlueLED, LOW); delay(120);
      digitalWrite(RedLED, HIGH); digitalWrite(GreenLED, HIGH); digitalWrite(BlueLED, HIGH); delay(120);
    }

    servo2.setGoalPosition(200, 1000);   delay(700);
    servo3.setGoalPosition(900, 1000);   delay(700);
    servo4.setGoalPosition(0, 1000);     delay(700);

    digitalWrite(RedLED, LOW); digitalWrite(GreenLED, LOW); digitalWrite(BlueLED, LOW); delay(120);

    servo1.setLockSign(0x01);   delay(60);
    servo2.setLockSign(0x01);   delay(60);
    servo3.setLockSign(0x01);   delay(60);
    servo4.setLockSign(0x01);   delay(60);
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(CALIB_BUTTON, INPUT);
  pinMode(RedLED, OUTPUT);  
  pinMode(BlueLED, OUTPUT);  
  pinMode(GreenLED, OUTPUT);
  
  OPI_UART.begin(OPI_BAUD);  

  servo1.begin(); servo1.setBaudRate(38400); servo1.setMode(OCS_SERVO); servo1.setMaxTorque(300);
  delay(200);
  servo2.begin(); servo2.setBaudRate(38400); servo2.setMode(OCS_SERVO); servo2.setMaxTorque(300);
  delay(200);
  servo3.begin(); servo3.setBaudRate(38400); servo3.setMode(OCS_SERVO); servo3.setMaxTorque(300);
  delay(200);
  servo4.begin(); servo4.setBaudRate(38400); servo4.setMode(OCS_SERVO); servo4.setMaxTorque(300);
  delay(200);


  // calibrationRene();
  OPI_UART.println("[INFO]  Start complete");
  servo1.setGoalAngle(180);
}

void analyzeCors_MoveServo(){
if (OPI_UART.available() == 0) {
    return;
  }
  delay(100);

  // Получаем координаты с UART
  Coors coors = readUART();
  if (areCoorsInvalid(coors)) return;

  if (!areCoorsFixed(coors, prevCoors)) {
    prevCoors = coors;
    return;
  }

  prevCoors = coors;
  if (!areServoAnglesCanBeCalculated(coors)) return;

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

void TestMoveOnCors(){
  if (OPI_UART.available() == 0) return;
  delay(50);

  // читаем координаты
  Coors coors = readUART();
  if (areCoorsInvalid(coors)) return;

  // считаем углы для X, Y, Z
  calculateAngles(coors.x, coors.y, coors.z);

  // применяем рассчитанные углы
  moveServos();


}

void TestMoveServo(){
  servoAngles[0] = 150;
  servoAngles[1] = 40;
  servoAngles[2] = 100;
  servoAngles[3] = 20;
  moveServos();

  delay(2000);

  servoAngles[0] = 90;
  servoAngles[1] = 100;
  servoAngles[2] = 60;
  servoAngles[3] = 120;
  moveServos();

  
  delay(2000);
}

void loop() {
  TestMoveOnCors();

  //servo3.setGoalAngle(360);
  // servo2.setGoalAngle(180);
  // servo3.setGoalAngle(180);

  // OPI_UART.print("L0:");
  // OPI_UART.print(servo1.getCurrentPosition());
  // OPI_UART.print("   L1:");
  // OPI_UART.print(servo2.getCurrentPosition());
  // OPI_UART.print("   L3:");
  // OPI_UART.println(servo3.getCurrentPosition());


  delay(100);
  // OPI_UART.print("L0:");
  // OPI_UART.print(servo1.getCurrentPosition());
  // OPI_UART.print("   L1:");
  // OPI_UART.print(servo2.getCurrentPosition());
  // OPI_UART.print("   L3:");
  // OPI_UART.println(servo3.getCurrentPosition());
  // int pos = servo1.getCurrentPosition();
  // if (pos == -1) {digitalWrite(RedLED, HIGH); digitalWrite(BlueLED, LOW);}
  // else {digitalWrite(RedLED, LOW); digitalWrite(BlueLED, HIGH);}

}

  //######## Тест серво + комуникация ########
  // servo1.setGoalAngle(posX);
  // delay(200);
  // servo2.setGoalAngle(posY);
  // delay(200);
  // servo3.setGoalAngle(posZ);
  // delay(200);
  // if(grip == 1){
  //   servo4.setGoalAngle(200);
  //   delay(200);
  // }else{
  //   servo4.setGoalAngle(0);
  //   delay(200);
  // }
  

  // OPI_UART.print("[INFO] Base="); OPI_UART.print(servoAngles[0]);
  // OPI_UART.print("  Joint1="); OPI_UART.print(servoAngles[1]);
  // OPI_UART.print("  Joint2="); OPI_UART.println(servoAngles[2]);