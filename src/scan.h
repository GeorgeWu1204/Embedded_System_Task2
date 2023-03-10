#include "global_variables.h"
#include "global_functions.h"
#include "communication.h"
#include "knob.cpp"
#include "reorganize.h"

#ifndef SCAN_H
#define SCAN_H
void setRow(uint8_t rowIdx);
void scanKeysTask(void *pvParameters);
uint8_t readCols();
// void *pvParameters
#endif