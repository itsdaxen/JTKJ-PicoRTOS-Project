#ifndef BUTTONS_H
#define BUTTONS_H

#include <FreeRTOS.h>
#include <stdbool.h>

typedef enum { BUTTON_NONE, BUTTON_SHORT_PRESS, BUTTON_HOLD } ButtonEvent;

typedef struct {
  bool was_pressed;
  bool hold_handled;
  TickType_t pressed_at;
} ButtonState;

ButtonEvent detect_button_event(ButtonState *button, bool pressed,
                                TickType_t now);

#endif
