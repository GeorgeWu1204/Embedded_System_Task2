#include "global_variables.h"

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
volatile int32_t currentIndex = 0;
volatile int8_t knob3Rotation = 0;
volatile uint8_t keyArray[7] = {0};

volatile uint8_t globalTX_Message[8]={0};
volatile uint8_t globalRX_Message[8]={0};