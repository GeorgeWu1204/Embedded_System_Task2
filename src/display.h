#include "global_variables.h"
#ifndef DISPLAY_H
#define DISPLAY_H

void setOutMuxBit(const uint8_t bitIdx, const bool value);
void displayUpdateTask(void *pvParameters);
void initialiseDisplay();
#endif