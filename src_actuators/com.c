#include "com.h"
#include <stdio.h>
#include <pico/stdlib.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "frtos_task.h"
static char message_buffer[MESSAGE_SIZE];
static int buffer_index = 0;
static bool overflow = false;
BaseType_t receive_message(QueueHandle_t message_queue) {
    while(1) {
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT)  {
        if (ch == '\n') {
            if (overflow) {
                // printf("__Message out of range!__\n");
                buffer_index = 0;
                overflow = false;
            }
            else {
                if (buffer_index <= 2) {
                    // printf("__The message is too short.Please try again!__\n");
                    buffer_index = 0;
                    overflow = false;
                }
                else if (message_buffer[buffer_index - 1] != ' ' || message_buffer[buffer_index - 2] != ' ') {
                    // printf("__Incorrect message protocol!__\n");
                    buffer_index = 0;
                    overflow = false;
                }
                else {
                buffer_index = buffer_index - 2;
                message_buffer[buffer_index] = '\0';
                // printf("__Message received:__ %s\n", message_buffer);
                // Send to queue so the receiver can pick it up from the queue
                if (xQueueSend(message_queue, message_buffer, 0) != pdPASS) {
                    // printf("__Queue is full! Message discarded.__\n");
                    buffer_index = 0;
                    overflow = false;
                }
                else {
                    // printf("__Sending message to queue:__ %s \n", message_buffer);
                    buffer_index = 0;
                    overflow = false;
                    return pdPASS;
                    }
                }
            }
        }
        else if (overflow) {

        }
        else if (buffer_index < MESSAGE_SIZE - 1) {
            message_buffer[buffer_index] = (char)ch;
            buffer_index++;
        }
        else {
            overflow = true;
        }
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    }

void send_message (const char *message) {
    printf("%s  \n", message);
}
    
