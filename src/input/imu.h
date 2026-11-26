#ifndef IMU_H
#define IMU_H

#include <stdbool.h>

typedef struct {
  float ax, ay, az;
  float gx, gy, gz;
  float temperature;
} ImuSample;

bool initialize_imu(void);
bool read_imu(ImuSample *sample);

#endif
