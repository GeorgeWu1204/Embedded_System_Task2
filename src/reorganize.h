#include "global_variables.h"

#ifndef REORGANIZE_H
#define REORGANIZE_H

// void reorganizePositions();
// void sendS(uint8_t msg0, uint8_t msg1, uint8_t msg2);
// void sendHandshakeMessage(uint8_t msg0, uint8_t msg1, uint8_t msg2);
uint8_t getHashedID();
void configTask(void *pvParameters);
#endif