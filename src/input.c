#include "input.h"
#include "tkjhat/sdk.h"
#include <FreeRTOS.h>
#include <math.h>
#include <stdio.h>
#include <task.h>

typedef enum { ORIENT_UNKNOWN, ORIENT_DOT, ORIENT_DASH } Orientation;
typedef enum { NO_TAP, DOUBLE_TAP, TRIPLE_TAP } TapType;
typedef enum { TILT_MODE, TAP_MODE, INACTIVE } InputMode;
typedef enum { TILT, TAP, LAST_MESSAGE } MenuSelection;

void input_task(void *arg) {
  (void)arg;

  float ax, ay, az;
  float gx, gy, gz, t;

  float saved_magnitude = 0;

  int tap = 0;
  bool was_tapped = false;
  bool first_run = true;

  char message[10] = "";
  size_t message_len = 0;

  // Setting up the sensor.
  if (init_ICM42670() == 0) {
    printf("ICM-42670P initialized successfully!\n");
    if (ICM42670_start_with_default_values() != 0) {
      printf("ICM-42670P could not initialize accelerometer or gyroscope\n");
      vTaskDelete(NULL);
    }
  } else {
    printf("Failed to initialize ICM-42670P.\n");
    vTaskDelete(NULL);
  }

  bool btn1_was_pressed = false;
  bool btn2_was_pressed = false;
  bool backspace_done = false;
  bool message_confirmed = false;
  bool menu_requested = false;
  bool menu_open = false;
  bool wait_for_button_release = false;

  TickType_t btn1_pressed_at = 0;
  TickType_t btn2_pressed_at = 0;
  TickType_t tapped_at = 0;

  InputMode input_mode = TILT_MODE;
  MenuSelection menu_selection = TILT;

  // Start collection data here. Infinite loop.
  while (1) {
    Orientation orientation = ORIENT_UNKNOWN;

    bool btn1_pressed = gpio_get(BUTTON1);
    bool btn2_pressed = gpio_get(BUTTON2);

    // Menu buttons preparation
    if (wait_for_button_release) {
      if (!btn1_pressed && !btn2_pressed) {
        wait_for_button_release = false;
        backspace_done = false;
        message_confirmed = false;
      }
      btn1_was_pressed = btn1_pressed;
      btn2_was_pressed = btn2_pressed;
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    if (btn1_pressed && btn2_pressed && !menu_open) {
      menu_requested = true;
    }
    if (menu_requested || menu_open) {
      tap = 0;
      was_tapped = false;
      first_run = true;
    }

    // Menu selection
    if (menu_requested && !menu_open && !btn1_pressed && !btn2_pressed) {
      menu_requested = false;
      menu_open = true;

      printf("Menu: (Press button1 to navigate or button2 to confirm)\n");
      printf(menu_selection == TILT ? "--> Tilt mode\n" : "    Tilt mode\n");
      printf(menu_selection == TAP ? "--> Tap mode\n" : "    Tap mode\n");
      printf(menu_selection == LAST_MESSAGE ? "--> Last message\n"
                                            : "    Last message\n");

    } else if (menu_open && !(btn1_pressed && btn2_pressed)) {
      if (btn1_pressed && !btn1_was_pressed) {
        menu_selection++;
        if (menu_selection >= 3) {
          menu_selection = 0;
        }

        printf("Menu: (Press button1 to navigate or button2 to confirm)\n");
        printf(menu_selection == TILT ? "--> Tilt mode\n" : "    Tilt mode\n");
        printf(menu_selection == TAP ? "--> Tap mode\n" : "    Tap mode\n");
        printf(menu_selection == LAST_MESSAGE ? "--> Last message\n"
                                              : "    Last message\n");
      }

      if (btn2_pressed && !btn2_was_pressed) {
        menu_requested = false;
        menu_open = false;
        wait_for_button_release = true;

        input_mode = menu_selection == TAP    ? TAP_MODE
                     : menu_selection == TILT ? TILT_MODE
                                              : INACTIVE;

        printf("Menu closed. You have selected: \n");
        printf(menu_selection == TILT           ? "Tilt mode\n"
               : menu_selection == TAP          ? "Tap mode\n"
               : menu_selection == LAST_MESSAGE ? "Last message\n"
                                                : "Invalid");
      }
    }

    if (!menu_requested && !menu_open && !wait_for_button_release) {
      if (ICM42670_read_sensor_data(&ax, &ay, &az, &gx, &gy, &gz, &t) == 0) {
        // Calculate total acceleration magnitude
        float magnitude = sqrtf(ax * ax + ay * ay + az * az);

        if (input_mode == TILT_MODE) {
          // In a stable state g is close to 1
          if (magnitude >= 0.75f && magnitude <= 1.25f) {
            float z_share = fabsf(az) / magnitude;

            if (z_share >= 0.75f) {
              orientation = ORIENT_DOT;
            } else if (z_share <= 0.65f) {
              orientation = ORIENT_DASH;
            }
          }
        }

        if (input_mode == TAP_MODE && !menu_requested && !menu_open) {
          TapType tapType = NO_TAP;

          // Read taps:
          TickType_t now = xTaskGetTickCount();
          TickType_t elapsed = now - tapped_at;

          // Wait 1210ms to see if no other tap is coming
          if ((elapsed >= pdMS_TO_TICKS(1210)) && tap > 0) {
            tapType = tap == 2 ? DOUBLE_TAP : tap == 3 ? TRIPLE_TAP : NO_TAP;

            char symbol = tapType == DOUBLE_TAP   ? '-'
                          : tapType == TRIPLE_TAP ? '.'
                                                  : '\0';
            if (tapType) {
              if (message_len >=
                  sizeof message - 1) // check we don't exceed the cap
              {
                printf("Message: %s | Max length reached!\n", message);
              } else {
                message[message_len] = symbol;
                message_len++;
                message[message_len] = '\0';
                printf("Message: %s\n", message);
              }
            }

            tap = 0;
          }
          // Allow news taps only if interval between them is more than 400ms
          if (elapsed >= pdMS_TO_TICKS(400)) {
            was_tapped = false;
          }
          if ((magnitude >= saved_magnitude * 1.08) && !was_tapped &&
              !first_run) {
            tapped_at = now;
            was_tapped = true;
            tap++;
          }

          saved_magnitude = magnitude;
          first_run = false;
        }

      } else {
        printf("Failed to read imu data\n");
      }

      // Symbol handling here
      char symbol = orientation == ORIENT_DOT    ? '.'
                    : orientation == ORIENT_DASH ? '-'
                                                 : '\0';

      if (input_mode != INACTIVE) {
        if (btn1_pressed) {
          // On a new press record time
          if (!btn1_was_pressed) {
            btn1_pressed_at = xTaskGetTickCount();
          }
          // After 200ms hold backspace
          if ((xTaskGetTickCount() - btn1_pressed_at >= pdMS_TO_TICKS(200)) &&
              !backspace_done) {
            backspace_done = true;

            if (message_len > 0) {
              message[message_len - 1] = '\0';
              message_len--;
              printf("Message: %s\n", message);
            }
          }
        } // On release, confirm symbol if hold didn't fire
        else if (btn1_was_pressed) {
          if (!backspace_done && symbol) {
            if (message_len >=
                sizeof message - 1) // check we don't exceed the cap
            {
              printf("Message: %s | Max length reached!\n", message);

            } else {
              message[message_len] = symbol;
              message_len++;
              message[message_len] = '\0';
              printf("Message: %s\n", message);
            }
          }
          backspace_done = false;
        }

        if (btn2_pressed) {
          // On a new press record time and reset confirmed flag
          if (!btn2_was_pressed) {
            btn2_pressed_at = xTaskGetTickCount();
            message_confirmed = false;
          }
          // After 200ms hold confirm and clear draft
          if (!message_confirmed &&
              (xTaskGetTickCount() - btn2_pressed_at >= pdMS_TO_TICKS(200))) {
            printf("%s  \n", message);
            printf("Message sent \n");
            message_confirmed = true;
            message[0] = '\0';
            message_len = 0;
          }
        } // On release, add a space if hold didn't fire
        else if (btn2_was_pressed && !message_confirmed) {
          if (message_len >=
              sizeof message - 1) // check we don't exceed the cap
          {
            printf("Message: %s | Max length reached!\n", message);
          } else {
            message[message_len] = ' ';
            message_len++;
            message[message_len] = '\0';
            printf("Message: %s\n", message);
          }
        }
      }
    }

    btn1_was_pressed = btn1_pressed;
    btn2_was_pressed = btn2_pressed;

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
