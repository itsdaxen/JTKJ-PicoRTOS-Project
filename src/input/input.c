#include "input.h"
#include "../display.h"
#include "buttons.h"
#include "editor.h"
#include "imu.h"
#include "taps.h"
#include "tilt.h"
#include "tkjhat/sdk.h"
#include "ui.h"
#include <FreeRTOS.h>
#include <math.h>
#include <stdio.h>
#include <task.h>

#define INPUT_POLL_MS 10

void input_task(void *arg) {
  (void)arg;

  ImuSample sample;

  Editor editor = {0};
  TapDetector taps = {.first_run = true};

  if (!initialize_imu()) {
    vTaskDelete(NULL);
    return;
  }

  ButtonState button1 = {0};
  ButtonState button2 = {0};
  Ui ui = {.state = UI_OUTSIDE_MENU,
           .input_mode = TILT_MODE,
           .menu_selection = TILT};

  while (1) {
    char tilt_symbol = '\0';

    bool btn1_pressed = gpio_get(BUTTON1);
    bool btn2_pressed = gpio_get(BUTTON2);

    UiState previous_ui_state = ui.state;
    if (!handle_ui(&ui, btn1_pressed, btn2_pressed, button1.was_pressed,
                   button2.was_pressed)) {
      taps.count = 0;
      taps.was_tapped = false;
      taps.first_run = true;
      button1.hold_handled = false;
      button2.hold_handled = false;
    } else {
      if (read_imu(&sample)) {
        if (ui.input_mode == TILT_MODE) {
          tilt_symbol = detect_tilt_symbol(sample.ax, sample.ay, sample.az);
        }

        if (ui.input_mode == TAP_MODE) {
          float magnitude =
              sqrtf(sample.ax * sample.ax + sample.ay * sample.ay +
                    sample.az * sample.az);
          char symbol =
              detect_tap_symbol(&taps, magnitude, xTaskGetTickCount());
          if (symbol) {
            if (!editor_append(&editor, symbol)) {
              printf("__Message length limit reached__\n");
            } else {
              display_text(editor.text);
            }
          }
        }

      } else {
        printf("__Failed to read imu data__\n");
      }

      if (ui.input_mode != INBOX_MODE) {
        ButtonEvent event1 =
            detect_button_event(&button1, btn1_pressed, xTaskGetTickCount());
        if (event1 == BUTTON_HOLD) {
          if (editor_backspace(&editor)) {
            display_text(editor.text);
          }
        } else if (event1 == BUTTON_SHORT_PRESS && tilt_symbol) {
          if (!editor_append(&editor, tilt_symbol)) {
            printf("__Message length limit reached__\n");
          } else {
            display_text(editor.text);
          }
        }

        ButtonEvent event2 =
            detect_button_event(&button2, btn2_pressed, xTaskGetTickCount());
        if (event2 == BUTTON_HOLD) {
          printf("%s  \n", editor.text);
          editor_clear(&editor);
          taps.count = 0;
          display_text("Message sent");
        } else if (event2 == BUTTON_SHORT_PRESS) {
          if (!editor_append(&editor, ' ')) {
            printf("__Message length limit reached__\n");
          } else {
            display_text(editor.text);
          }
        }
      }
    }

    if (previous_ui_state == UI_MENU_SELECTION_CONFIRMED &&
        ui.state == UI_OUTSIDE_MENU) {
      if (ui.input_mode == INBOX_MODE) {
        display_text("Inbox not connected");
      } else {
        display_text(editor.text);
      }
    }

    button1.was_pressed = btn1_pressed;
    button2.was_pressed = btn2_pressed;

    vTaskDelay(pdMS_TO_TICKS(INPUT_POLL_MS));
  }
}
