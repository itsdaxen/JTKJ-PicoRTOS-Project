#ifndef TAPS_H
#define TAPS_H

#include <FreeRTOS.h>
#include <stdbool.h>
typedef struct {
  float saved_magnitude;
  int count;
  bool was_tapped;
  bool first_run;
  TickType_t tapped_at;
} TapDetector;

char detect_tap_symbol(TapDetector *detector, float magnitude, TickType_t now);

#endif
