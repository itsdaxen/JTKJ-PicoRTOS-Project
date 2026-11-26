#include "input/input.h"
#include "tkjhat/sdk.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>

#define DEFAULT_STACK_SIZE 2048

int main() {

  stdio_init_all();

  init_hat_sdk();
  sleep_ms(300); // Wait some time so initialization of USB and hat is done.

  init_button1();
  init_button2();
  init_display();

  TaskHandle_t hInputTask = NULL;

  // Create the tasks with xTaskCreate
  BaseType_t result = xTaskCreate(input_task, "input", DEFAULT_STACK_SIZE, NULL,
                                  2, &hInputTask);

  if (result != pdPASS) {
    printf("Input task creation failed\n");
    return 0;
  }

  // Start the scheduler (never returns)
  vTaskStartScheduler();

  // Never reach this line.
  return 0;
}
