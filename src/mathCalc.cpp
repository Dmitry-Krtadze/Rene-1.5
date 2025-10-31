#include "config.h"
#include "mathCalc.h"
#include <math.h>

static inline float rad2deg(float r) { return r * 180.0f / M_PI; }
static inline float clamp1(float v) { return v < -1.0f ? -1.0f : (v > 1.0f ? 1.0f : v); }
static inline float norm360(float a) { while(a < 0) a += 360.0f; while(a >= 360.0f) a -= 360.0f; return a; }


int calculateBaseAngle(float x, float z) {
   
}

void calculateAngles(float x, float y, float z) {
  const float L0  = BASE_HEIGHT;
  const float L1  = L1_LENGTH;
  const float L2  = L2_LENGTH;
  const float div = 1.0f;

  float Xpos    = x;
  float YposWL0 = y;

  float Ypos = L0 - YposWL0;  // Высота, как было
  float d = sqrtf(Xpos * Xpos + Ypos * Ypos) / div;  // Оставляем без z, как вы просили

  if (!isfinite(d) || d < 1e-6f) {
    OPI_UART.println("[ERROR] skip: d≈0");
    return;
  }

  float q1 = atan2f(Ypos, Xpos);  // Угол в плоскости XY

  auto clamp1 = [](float v) {
    return (v < -1.0f) ? -1.0f : (v > 1.0f ? 1.0f : v);
  };

  float arg2 = clamp1((d*d + L1*L1 - L2*L2) / (2.0f * L1 * d));
  float arg3 = clamp1((L1*L1 + L2*L2 - d*d) / (2.0f * L1 * L2));
  float q2 = acosf(arg2);
  float q3 = acosf(arg3);
  if (!isfinite(q2) || !isfinite(q3)) {
    OPI_UART.println("[ERROR] skip: acos NaN");
    return;
  }

  float a1_deg = ((q1 - q2) * 180.0f / M_PI) * -1;
  float a2_deg = 180.0f - (q3 * 180.0f / M_PI);

  if (fabsf(a1_deg) < 1e-5f && fabsf(a2_deg) < 1e-5f) {
    OPI_UART.println("[ERROR] skip: angles≈0");
    return;
  }

  servoAngles[0] = calculateBaseAngle(x, z);  // Z влияет на поворот базы
  servoAngles[1] = a1_deg * 2;
  servoAngles[2] = a2_deg * 2;

  OPI_UART.print("[INFO] J0=");
  OPI_UART.print(servoAngles[0]);
  OPI_UART.print("  J1=");    
  OPI_UART.print(servoAngles[1]);
  OPI_UART.print("  J2=");    
  OPI_UART.println(servoAngles[2]);
}

bool areCoorsInvalid(Coors c) {
  return isnan(c.x) || isnan(c.y);
}

bool areServoAnglesCanBeCalculated(Coors c) {
  calculateAngles(c.x, c.y, c.z);
  for (int i = 1; i <= 2; ++i) {    
    if (isnan(servoAngles[i])) return false;
  }
  return true;
}