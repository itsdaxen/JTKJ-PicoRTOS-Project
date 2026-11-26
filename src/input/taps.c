#include "taps.h"
#include <task.h>

#define TAP_COOLDOWN_MS 400
#define TAP_GROUP_TIMEOUT_MS 1210

typedef enum { NO_TAP, DOUBLE_TAP, TRIPLE_TAP } TapType;

char detect_tap_symbol(TapDetector *detector, float magnitude, TickType_t now) {
  TickType_t elapsed = now - detector->tapped_at;
  char symbol = '\0';

  // Wait 1210ms to see if no other tap is coming
  if (elapsed >= pdMS_TO_TICKS(TAP_GROUP_TIMEOUT_MS) && detector->count > 0) {
    TapType type = detector->count == 2   ? DOUBLE_TAP
                   : detector->count == 3 ? TRIPLE_TAP
                                          : NO_TAP;
    symbol = type == DOUBLE_TAP ? '-' : type == TRIPLE_TAP ? '.' : '\0';
    detector->count = 0;
  }

  if (elapsed >= pdMS_TO_TICKS(TAP_COOLDOWN_MS)) {
    detector->was_tapped = false;
  }
  if (magnitude >= detector->saved_magnitude * 1.08 && !detector->was_tapped &&
      !detector->first_run) {
    detector->tapped_at = now;
    detector->was_tapped = true;
    detector->count++;
  }

  detector->saved_magnitude = magnitude;
  detector->first_run = false;
  return symbol;
}
