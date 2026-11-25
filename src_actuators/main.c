
#include <stdio.h>
#include <pico/stdlib.h>
#include "com.h"
#include "FreeRTOS.h"
#include "task.h"
#include "frtos_task.h"
#include "tkjhat/sdk.h"
int main() {
    stdio_init_all();
    init_led();
    sleep_ms(2000);
    // printf("Hello World\n");
    // printf("Task is starting!");
    init_hat_sdk();
    sleep_ms(400);
    initialize_task();
    vTaskStartScheduler();
    printf("Cannot start the scheduler!");
    
    // while (1) {
        // sleep_ms(200);
    // }
    return 0;

}

