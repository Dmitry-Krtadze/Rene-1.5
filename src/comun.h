#ifndef COMUN_H
#define COMUN_H

#include "config.h"
#include <Arduino.h>

struct Pose {
  float x;
  float y;
  float z;
  float ry;
};

struct VisionCommand {
  int status;
  char target[8];
  char msg[48];
  Pose block;
  Pose cup;
  bool valid;
};

bool readJsonLine(String &line);                 // читает строку до '\n'
bool parseVisionJson(const String &line, VisionCommand &out);  // парсит JSON

void parseCoordinates(const String &data);
Coors readUART();

#endif
