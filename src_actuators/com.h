#ifndef COM_H
#define COM_H
#include "FreeRTOS.h"
#include "queue.h"

#define MESSAGE_SIZE 255

BaseType_t receive_message(QueueHandle_t message_queue);
void send_message(const char *message);
#endif
