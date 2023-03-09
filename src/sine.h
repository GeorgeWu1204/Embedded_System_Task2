#include "global_variables.h"
// #include "communication.h"
#include "sineLookUpTable.cpp"

#ifndef SINE_H
#define SINE_H

void scanKeysTask(void *pvParameters);
void write_to_double_buffer(void *pvParameters);
void initialize_table();
int16_t calculate_vout();

#endif