#include "imu.h"
#include "tkjhat/sdk.h"
#include <stdio.h>

bool initialize_imu(void) {
  if (init_ICM42670() == 0) {
    printf("ICM-42670P initialized successfully!\n");
    if (ICM42670_start_with_default_values() != 0) {
      printf("ICM-42670P could not initialize accelerometer or gyroscope\n");
      return false;
    }
  } else {
    printf("Failed to initialize ICM-42670P.\n");
    return false;
  }

  return true;
}

bool read_imu(ImuSample *sample) {
  return ICM42670_read_sensor_data(&sample->ax, &sample->ay, &sample->az,
                                   &sample->gx, &sample->gy, &sample->gz,
                                   &sample->temperature) == 0;
}
