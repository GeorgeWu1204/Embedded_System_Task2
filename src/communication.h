#include <U8g2lib.h>
#include <ES_CAN.h>
#include "global_variables.h"

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

void decodeTask(void *pvParameters);
void sendMessage(uint8_t previous_array [], uint8_t current_array []);
void initializeCAN();

#endif
