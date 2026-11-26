#ifndef UI_H
#define UI_H

#include "menu.h"
#include <stdbool.h>

typedef enum { TILT_MODE, TAP_MODE, INBOX_MODE } InputMode;
typedef enum {
  UI_OUTSIDE_MENU,
  UI_MENU_REQUESTED,
  UI_MENU_ACTIVE,
  UI_MENU_SELECTION_CONFIRMED
} UiState;

typedef struct {
  UiState state;
  InputMode input_mode;
  MenuSelection menu_selection;
} Ui;

bool handle_ui(Ui *ui, bool btn1_pressed, bool btn2_pressed,
               bool btn1_was_pressed, bool btn2_was_pressed);

#endif
