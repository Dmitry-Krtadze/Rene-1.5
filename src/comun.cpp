#include "config.h"

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


Coors readUART() {
  // читаем строку до '\n'
  while (OPI_UART.available()) {
    char c = OPI_UART.read();
    if (c == '\n') {
      parseCoordinates(inputBuffer); // обновит posX, posY, posZ и grip
      inputBuffer = "";
      break;
    } else {
      inputBuffer += c;
    }
  }

  // конвертируем принятые координаты в структуру Coors
  return Coors{
    posX,
    posY,
    posX,  // можно заменить на finishX при реальной команде
    posY   // можно заменить на finishY
  };
}