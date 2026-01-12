
// #include "config.h"
// #include "mathCalc.h"
// #include <math.h>

// static inline float rad2deg(float r) { return r * 180.0f / M_PI; }
// static inline float clamp1(float v) { return v < -1.0f ? -1.0f : (v > 1.0f ? 1.0f : v); }
// static inline float norm360(float a) { while(a < 0) a += 360.0f; while(a >= 360.0f) a -= 360.0f; return a; }





// int calculateBaseAngle(float x, float z) {
//   return BASE_ANGLE_1 + round(atan(x / z) * 180 / M_PI) * 1.35 ;
// }

// void calculateAngles(float x, float y, float z) {
//   const float L0  = BASE_HEIGHT;
//   const float L1  = L1_LENGTH;
//   const float L2  = L2_LENGTH;
//   const float div = 1.0f;

//   float Xpos    = sqrt(x * x + z * z);
//   float YposWL0 = y;

//   float Ypos = L0 - YposWL0; 
//   float d = sqrtf(Xpos * Xpos + Ypos * Ypos) / div;  

//   if (!isfinite(d) || d < 1e-6f) {
//     OPI_UART.println("[ERROR] skip: d≈0");
//     return;
//   }

//   float q1 = atan2f(Ypos, Xpos);  

//   auto clamp1 = [](float v) {
//     return (v < -1.0f) ? -1.0f : (v > 1.0f ? 1.0f : v);
//   };

//   float arg2 = clamp1((d*d + L1*L1 - L2*L2) / (2.0f * L1 * d));
//   float arg3 = clamp1((L1*L1 + L2*L2 - d*d) / (2.0f * L1 * L2));
//   float q2 = acosf(arg2);
//   float q3 = acosf(arg3);
//   if (!isfinite(q2) || !isfinite(q3)) {
//     OPI_UART.println("[ERROR] skip: acos NaN");
//     return;
//   }

//   float a1_deg = ((q1 - q2) * 180.0f / M_PI) * -1;
//   float a2_deg = 180.0f - (q3 * 180.0f / M_PI);

//   if (fabsf(a1_deg) < 1e-5f && fabsf(a2_deg) < 1e-5f) {
//     OPI_UART.println("[ERROR] skip: angles≈0");
//     return;
//   }

//   servoAngles[0] = calculateBaseAngle(x, z) - 40;  
//   servoAngles[1] = a1_deg * 2;
//   servoAngles[2] = a2_deg * 2;

  

//   OPI_UART.print("[INFO] J0=");
//   OPI_UART.print(servoAngles[0]);
//   OPI_UART.print("  J1=");    
//   OPI_UART.print(servoAngles[1]);
//   OPI_UART.print("  J2=");    
//   OPI_UART.print(servoAngles[2]);
//   OPI_UART.print("  GRIP=");    
//   OPI_UART.println(grip);

// }

// bool areCoorsInvalid(Coors c) {
//   return isnan(c.x) || isnan(c.y);
// }

// bool areServoAnglesCanBeCalculated(Coors c) {
//   calculateAngles(c.x, c.y, c.z);
//   for (int i = 1; i <= 2; ++i) {    
//     if (isnan(servoAngles[i])) return false;
//   }
//   return true;
// }


#include "config.h"
#include "mathCalc.h"
#include <math.h>

static inline float rad2deg(float r) { return r * 180.0f / M_PI; }

static inline float norm360(float a) {
  a = fmodf(a, 360.0f);
  if (a < 0) a += 360.0f;
  return a;
}

static inline int toServo360(float baseDeg, float zeroDeg, int dir) {
  float out = norm360(zeroDeg + dir * baseDeg);
  return (int)lroundf(out);
}

static inline float clamp1f(float v) {
  return (v < -1.0f) ? -1.0f : (v > 1.0f) ? 1.0f : v;
}

static inline float applyXCompByZ(float x, float z) {
#if XZ_COMP_ENABLE
  float az = fabsf(z);

  if (az <= (float)XZ_COMP_Z_DEADZONE) return x;

  float dz = az - (float)XZ_COMP_Z_DEADZONE;
  float reduction = dz * (float)XZ_COMP_K;

  if (reduction > (float)XZ_COMP_MAX_REDUCTION) reduction = (float)XZ_COMP_MAX_REDUCTION;

  float ax = fabsf(x);
  float ax_new = ax - reduction;
  if (ax_new < 0.0f) ax_new = 0.0f;

  return (x < 0.0f) ? -ax_new : ax_new;
#else
  (void)z;
  return x;
#endif
}


void calculateAngles(float x, float y, float z) {
  const float L0  = BASE_HEIGHT;
  const float L1  = L1_LENGTH;
  const float L2  = L2_LENGTH;
  const float div = 1.0f;

  // X COMP: уменьшаем X в зависимости от |Z|
  float x_comp = applyXCompByZ(x, z);

  const float baseDeg = rad2deg(atan2f(x_comp, z));

  servoAngles[0] = toServo360((baseDeg * 1.35 ) - 60, (float)BASE_ANGLE_1, 1);

  // X COMP: используем уже компенсированный X
  float Xpos    = sqrtf(x_comp * x_comp + z * z);
  float YposWL0 = y;

  float Ypos = L0 - YposWL0;
  float d = sqrtf(Xpos * Xpos + Ypos * Ypos) / div;

  if (!isfinite(d) || d < 1e-6f) {
    OPI_UART.println("[ERROR] skip: d≈0");
    return;
  }

  float q1 = atan2f(Ypos, Xpos);

  float arg2 = clamp1f((d*d + L1*L1 - L2*L2) / (2.0f * L1 * d));
  float arg3 = clamp1f((L1*L1 + L2*L2 - d*d) / (2.0f * L1 * L2));
  float q2 = acosf(arg2);
  float q3 = acosf(arg3);

  if (!isfinite(q2) || !isfinite(q3)) {
    OPI_UART.println("[ERROR] skip: acos NaN");
    return;
  }

  float a1_deg = ((q1 - q2) * 180.0f / M_PI) * -1.0f;
  float a2_deg = 180.0f - (q3 * 180.0f / M_PI);

  if (fabsf(a1_deg) < 1e-5f && fabsf(a2_deg) < 1e-5f) {
    OPI_UART.println("[ERROR] skip: angles≈0");
    return;
  }

  servoAngles[1] = (int)lroundf(a1_deg * 2.0f);
  servoAngles[2] = (int)lroundf(a2_deg * 2.0f);

  OPI_UART.print("[INFO] J0=");
  OPI_UART.print(servoAngles[0]);
  OPI_UART.print("  J1=");
  OPI_UART.print(servoAngles[1]);
  OPI_UART.print("  J2=");
  OPI_UART.print(servoAngles[2]);
  OPI_UART.print("  GRIP=");
  OPI_UART.println(grip);
}


bool areCoorsInvalid(Coors c) {
  return isnan(c.x) || isnan(c.y) || isnan(c.z);
}

bool areServoAnglesCanBeCalculated(Coors c) {
  calculateAngles(c.x, c.y, c.z);
  return true;
}
