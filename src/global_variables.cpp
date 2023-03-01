#include "global_variables.h"

const uint32_t interval = 100; // Display update interval

// Pin definitions
// Row select and enable
const int RA0_PIN = D3;
const int RA1_PIN = D6;
const int RA2_PIN = D12;
const int REN_PIN = A5;

// Matrix input and output
const int C0_PIN = A2;
const int C1_PIN = D9;
const int C2_PIN = A6;
const int C3_PIN = D1;
const int OUT_PIN = D11;

// Audio analogue out
const int OUTL_PIN = A4;
const int OUTR_PIN = A3;

// Joystick analogue in
const int JOYY_PIN = A0;
const int JOYX_PIN = A1;

// Output multiplexer bits
const int DEN_BIT = 3;
const int DRST_BIT = 4;
const int HKOW_BIT = 5;
const int HKOE_BIT = 6;

// Constants for the global
const int32_t stepSizes[12] = {
    51076056,
    54113197,
    57330935,
    60740009,
    64351798,
    68178356,
    72232452,
    76527617,
    81078186,
    85899345,
    91007186,
    96418755};

const char * Key_set[13] = {"Not Pressed", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

const uint8_t octave = 4;

volatile int32_t currentStepSize = 0;
volatile int32_t currentKey = 0;
volatile int8_t knob3Rotation = 0;
volatile uint8_t keyArray[7] = {0};

volatile uint8_t globalTX_Message[8]={0};
volatile uint8_t globalRX_Message[8]={0};

volatile QueueHandle_t msgInQ;
SemaphoreHandle_t RX_MessageMutex;