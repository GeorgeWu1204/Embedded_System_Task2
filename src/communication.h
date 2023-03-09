#include <U8g2lib.h>
#include <ES_CAN.h>
#include "global_variables.h"
#include "global_functions.h"
#include "reorganize.h"

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

void decodeTask(void *pvParameters);
void sendMessage(int8_t index, bool press);
void initializeCAN();
void CAN_TX_Task (void * pvParameters);

#endif
