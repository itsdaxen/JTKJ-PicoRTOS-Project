#include "ui.h"
#include "../display.h"

bool handle_ui(Ui *ui, bool btn1_pressed, bool btn2_pressed,
               bool btn1_was_pressed, bool btn2_was_pressed) {
  if (ui->state == UI_OUTSIDE_MENU && btn1_pressed && btn2_pressed) {
    ui->state = UI_MENU_REQUESTED;
  }

  switch (ui->state) {
  case UI_OUTSIDE_MENU:
    return true;

  case UI_MENU_REQUESTED:
    if (!btn1_pressed && !btn2_pressed) {
      ui->state = UI_MENU_ACTIVE;
      display_menu(ui->menu_selection);
    }
    break;

  case UI_MENU_ACTIVE:
    if (!(btn1_pressed && btn2_pressed)) {
      if (btn1_pressed && !btn1_was_pressed) {
        ui->menu_selection++;
        if (ui->menu_selection > LAST_MESSAGE) {
          ui->menu_selection = TILT;
        }
        display_menu(ui->menu_selection);
      }

      if (btn2_pressed && !btn2_was_pressed) {
        ui->state = UI_MENU_SELECTION_CONFIRMED;
        ui->input_mode = ui->menu_selection == TAP    ? TAP_MODE
                         : ui->menu_selection == TILT ? TILT_MODE
                                                      : INBOX_MODE;
      }
    }
    break;

  case UI_MENU_SELECTION_CONFIRMED:
    if (!btn1_pressed && !btn2_pressed) {
      ui->state = UI_OUTSIDE_MENU;
    }
    break;
  }

  return false;
}
