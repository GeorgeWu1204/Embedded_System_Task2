#include <algorithm>
#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>



#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

// // Constants

extern const uint32_t interval;

// // Pin definitions
// // Row select and enable
extern const int RA0_PIN;
extern const int RA1_PIN;
extern const int RA2_PIN;
extern const int REN_PIN;

// Matrix input and output
extern const int C0_PIN;
extern const int C1_PIN;
extern const int C2_PIN;
extern const int C3_PIN;
extern const int OUT_PIN;

// Audio analogue out
extern const int OUTL_PIN;
extern const int OUTR_PIN;

// Joystick analogue in
extern const int JOYY_PIN;
extern const int JOYX_PIN;

// Output multiplexer bits
extern const int DEN_BIT;
extern const int DRST_BIT;
extern const int HKOW_BIT;
extern const int HKOE_BIT;

// Global variables
extern const int32_t stepSizes[12];
extern const char *Key_set[13];
extern const uint8_t octave;
extern volatile int32_t currentKey;
extern volatile int8_t knob3Rotation;
extern volatile uint8_t keyArray[7];
extern volatile uint8_t globalTX_Message[8];
extern volatile uint8_t globalRX_Message[8];
extern volatile QueueHandle_t msgInQ;
extern volatile QueueHandle_t msgOutQ;
extern SemaphoreHandle_t keyArrayMutex;
extern SemaphoreHandle_t RX_MessageMutex;
extern SemaphoreHandle_t CAN_TX_Semaphore;

//setup function
void setPinDirections();


#endif