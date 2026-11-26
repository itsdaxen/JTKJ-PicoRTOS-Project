#include "tilt.h"
#include <math.h>

typedef enum { ORIENT_UNKNOWN, ORIENT_DOT, ORIENT_DASH } Orientation;

char detect_tilt_symbol(float ax, float ay, float az) {
  float magnitude = sqrtf(ax * ax + ay * ay + az * az);
  Orientation orientation = ORIENT_UNKNOWN;

  // In a stable state g is close to 1.
  if (magnitude >= 0.75f && magnitude <= 1.25f) {
    float z_share = fabsf(az) / magnitude;
    if (z_share >= 0.75f) {
      orientation = ORIENT_DOT;
    } else if (z_share <= 0.65f) {
      orientation = ORIENT_DASH;
    }
  }

  return orientation == ORIENT_DOT    ? '.'
         : orientation == ORIENT_DASH ? '-'
                                      : '\0';
}
