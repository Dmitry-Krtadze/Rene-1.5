#include <Arduino.h>
#include <OCServo.h>



byte id1 = 0x05;
byte id2 = 0x02;
byte id3 = 0x03;
byte id4 = 0x04;
OCServo servo1(id1, &Serial1);
OCServo servo2(id2, &Serial1);
OCServo servo3(id3, &Serial1);
OCServo servo4(id4, &Serial1);



HardwareSerial OPI_UART(PA3, PA2); // RX, TX
#define OPI_BAUD 115200

float posX = 90, posY = 90, posZ = 90;
int grip = 0;

String inputBuffer = "";

void parseCoordinates(const String &data) {
  int c1 = data.indexOf(',');
  int c2 = data.indexOf(',', c1 + 1);
  int c3 = data.indexOf(',', c2 + 1);

  if (c1 == -1 || c2 == -1 || c3 == -1) {
    OPI_UART.println("ERR_FORMAT");
    return;
  }

  String sx = data.substring(0, c1);
  String sy = data.substring(c1 + 1, c2);
  String sz = data.substring(c2 + 1, c3);
  String sg = data.substring(c3 + 1);

  posX = sx.toFloat();
  posY = sy.toFloat();
  posZ = sz.toFloat();
  grip = sg.toInt();

  // Отправляем подтверждение обратно в OrangePi
  String reply = "OK:";
  reply += posX;
  reply += ",";
  reply += posY;
  reply += ",";
  reply += posZ;
  reply += ",";
  reply += grip;

  OPI_UART.println(reply);
}

void setup() {
  Serial.begin(115200);      // Можно оставить для диагностики (монитор)
  OPI_UART.begin(OPI_BAUD);  // UART OrangePi

  servo1.begin();
  servo1.setBaudRate(38400);
  servo1.setMode(OCS_SERVO);
  servo1.setMaxTorque(200);
  delay(200);
  servo2.begin();
  servo2.setBaudRate(38400);
  servo2.setMode(OCS_SERVO);
  servo2.setMaxTorque(200);
  delay(200);
  servo2.begin();
  servo3.setBaudRate(38400);
  servo3.setMode(OCS_SERVO);
  servo3.setMaxTorque(200);
  delay(200);
  servo4.begin();
  servo4.setBaudRate(38400);
  servo4.setMode(OCS_SERVO);
  servo4.setMaxTorque(200);
  delay(200);

}

void loop() {
  while (OPI_UART.available()) {
    char ch = OPI_UART.read();

    if (ch == '\n') {
      parseCoordinates(inputBuffer);
      inputBuffer = "";
    } else {
      inputBuffer += ch;
    }
  }
  servo1.setGoalAngle(posX);
  delay(200);
  servo2.setGoalAngle(posY);
  delay(200);
  servo3.setGoalAngle(posZ);
  delay(200);
  if(grip == 1){
    servo4.setGoalAngle(200);
    delay(200);
  }else{
    servo4.setGoalAngle(0);
    delay(200);
  }
  
}
