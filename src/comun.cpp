#include "comun.h"
#include <ArduinoJson.h>

static bool copyStr(const char* src, char* dst, size_t dstSize) {
  if (!dst || dstSize == 0) return false;
  if (!src) { dst[0] = '\0'; return true; }
  strncpy(dst, src, dstSize - 1);
  dst[dstSize - 1] = '\0';
  return true;
}

bool readJsonLine(String &line) {
  while (OPI_UART.available()) {
    char c = (char)OPI_UART.read();
    if (c == '\n') return true;
    line += c;
    if (line.length() > 700) { line = ""; return false; } // защита от мусора
  }
  return false;
}

bool parseVisionJson(const String &line, VisionCommand &out) {
  out = {};
  out.valid = false;

  StaticJsonDocument<768> doc;
  DeserializationError err = deserializeJson(doc, line);
  if (err) return false;

  out.status = doc["status"] | 0;
  copyStr(doc["target"] | "", out.target, sizeof(out.target));
  copyStr(doc["msg"] | "", out.msg, sizeof(out.msg));

  // 200 + dual => есть координаты
  if (out.status == 200 && strcmp(out.target, "dual") == 0) {
    JsonObject b = doc["block_position"];
    JsonObject c = doc["cup_position"];

    out.block.x  = b["y"]  | NAN;
    out.block.y  = b["z"]  | NAN;
    out.block.z  = b["x"]  | NAN;
    out.block.ry = b["Ry"] | NAN;

    out.cup.x  = c["y"]  | NAN;
    out.cup.y  = c["z"]  | NAN;
    out.cup.z  = c["x"]  | NAN;
    out.cup.ry = c["Ry"] | NAN;

    out.valid = isfinite(out.block.x) && isfinite(out.block.y) && isfinite(out.block.z) &&
                isfinite(out.cup.x)   && isfinite(out.cup.y)   && isfinite(out.cup.z);
    return out.valid;
  }

  // 204/404/500 — координат нет, но команда валидна как статус
  out.valid = (out.status == 204 || out.status == 404 || out.status == 500);
  return out.valid;
}
