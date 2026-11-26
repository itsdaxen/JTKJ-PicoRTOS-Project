#include "buttons.h"
#include <task.h>

#define BUTTON_HOLD_MS 200
ButtonEvent detect_button_event(ButtonState *button, bool pressed,
                                TickType_t now) {
  if (pressed) {
    // On a new press record time and reset the hold flag.
    if (!button->was_pressed) {
      button->pressed_at = now;
      button->hold_handled = false;
    }
    if (!button->hold_handled &&
        now - button->pressed_at >= pdMS_TO_TICKS(BUTTON_HOLD_MS)) {
      button->hold_handled = true;
      return BUTTON_HOLD;
    }
  } else if (button->was_pressed) {
    bool was_held = button->hold_handled;
    button->hold_handled = false;
    if (!was_held) {
      return BUTTON_SHORT_PRESS;
    }
  }

  return BUTTON_NONE;
}
