#include "frtos_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "tkjhat/sdk.h"
#include "string.h"
#include <stdio.h>
#include "com.h"
static QueueHandle_t message_queue;
static QueueHandle_t led_queue;
static QueueHandle_t oled_queue; 
typedef enum {
    RECEIVE,
    SEND
} communicationstates;
static communicationstates current = RECEIVE;
void test_task(void *pvParameters) {
    while (1)
    {
        // printf("Test task is running!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }  
}

void led_task(void *pvParameters) {
    char received[MESSAGE_SIZE];
    init_led();
    while(1) {
        if (xQueueReceive(led_queue, received, portMAX_DELAY) == pdPASS) {
            printf("__LED received: %s__\n", received);
            hello_blink(received);
        }
    }
}

void hello_blink (const char *message) {
    int i = 0;
    while (message[i] != '\0')
    {
        if (message[i] == '.') {
            //For dots, the led will be 1s on
            set_led_status(true);
            vTaskDelay(pdMS_TO_TICKS(1000));
            set_led_status(false);
            if (message[i + 1] == '-' || message[i + 1] == '.') {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
        else if (message[i] == '-') {
            //For dash, the led will be 3s on
            set_led_status(true);
            vTaskDelay(pdMS_TO_TICKS(3000));
            set_led_status(false);
            if (message[i + 1] == '-' || message[i + 1] == '.') {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
        else if (message[i] == ' ') {
            if (message[i + 1] == ' ')  {
                if (message[i + 2] == '\0')  {
                    set_led_status(false);
                    break;
                }
                else {
                    set_led_status(false);
                    vTaskDelay(pdMS_TO_TICKS(7000));
                    i += 2;
                    continue;
                }
            }
            else {
                set_led_status(false);
                vTaskDelay(pdMS_TO_TICKS(3000));
            }
        }
        i++;
    }
    
}

void oled_task (void *pvParameters) {
    char display[MESSAGE_SIZE];
    init_display();
    printf("Initializing OLED display\n");
    while (1) {
        if (xQueueReceive(oled_queue, display, portMAX_DELAY) == pdPASS) {
            clear_display();
            write_text_xy(0, 0, display);
            write_text_xy(0, 16, "HELLO");
        }
    }
}
void receive_task(void *pvParameters) {
    receive_message((QueueHandle_t) pvParameters);
}

void process_task(void *pvParameters) {
    char received_mes[MESSAGE_SIZE];
    while(1) {
        if (xQueueReceive(message_queue, received_mes, portMAX_DELAY) == pdPASS) {
            // printf("__Message from Queue:__ %s\n", received_mes);
            send_message(received_mes);
        }
        if (xQueueReceive(led_queue, received_mes, portMAX_DELAY) == pdPASS) {
            
        }
    }
}

void com_state_machine(void *pvParameters) {
    char received_message[MESSAGE_SIZE];
    while (1) {
        switch (current)
        {
        case RECEIVE:
            if(receive_message(message_queue) == pdPASS) {
                // printf("__[TRANSITION] RECEIVE -> SEND__\n");
                current = SEND;
            }
            break;
        
        case SEND:
            if (xQueueReceive(message_queue, received_message, 0) == pdPASS) {
                // printf("__[STATE] Sending message:__ %s \n", received_message);
                xQueueSend(led_queue, received_message, 0);
                xQueueSend(oled_queue, received_message, 0);
                send_message(received_message);
                current = RECEIVE;
                // printf("__[TRANSITION] RECEIVE -> SEND__\n", received_message);
            }
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
void initialize_task(void) {
    message_queue = xQueueCreate(5, MESSAGE_SIZE);
    led_queue = xQueueCreate(5, MESSAGE_SIZE);
    oled_queue = xQueueCreate(5, MESSAGE_SIZE);
    if (message_queue == NULL) {
        printf("Message queue initialization failed!\n");
        return;
    }
    if (led_queue == NULL) {
        printf("LED queue initialization failed!\n");
        return;
    }
    if (oled_queue == NULL) {
        printf("OLED queue initialization failed!\n");
        return;
    }
    // if (xTaskCreate(test_task, "Test task", 255, NULL, 2, NULL) != pdPASS) {
        // printf("Test task initialization failed!\n");
    // }
    if (xTaskCreate(led_task, "Led task", 255, NULL, 1, NULL) != pdPASS) {
        printf("LED task initialization failed!\n");
    }
    if (xTaskCreate(oled_task, "OLed task", 1024, NULL, 1, NULL) != pdPASS) {
        printf("OLED task initialization failed!\n");
    }
    if (xTaskCreate(com_state_machine, "Communication 2 state machine", 255, NULL, 1, NULL) != pdPASS) {
        printf("Com task initialization failed!\n");
    }
}
