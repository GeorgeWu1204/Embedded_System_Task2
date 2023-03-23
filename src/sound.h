#include "global_variables.h"
#include "sine.cpp"

#ifndef SOUND_H
#define SOUND_H

void write_to_double_buffer(void *pvParameters);
void initialize_table();
int16_t calculate_vout();

#endif