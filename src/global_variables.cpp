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


// Test
#ifdef TEST_OVERALL
  uint8_t joystick_counter = 0;
  uint8_t write_to_double_buffer_counter = 0;
#endif


// Constants for the global
volatile uint16_t global_keyArray_concated;

//Key 
const char * Key_set[13] = {"Not Pressed", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const uint32_t stepSizes [12] = { 
  51076056,
  54113197,
  57330935,
  60740010,
  64351798,
  68178356,
  72232452,
  76527617,
  81078186,
  85899345,
  91007186,
  96418755
 };

//Communication
volatile QueueHandle_t msgInQ;
volatile QueueHandle_t msgOutQ;
SemaphoreHandle_t CAN_TX_Semaphore;

// Rotation Knob
volatile int8_t knob3Rotation = 0;

Knob knob1;
volatile int8_t knob1Rotation = 0;
volatile int8_t prev_knob1Rotation = 0;

// Sound 
bool sawTooth_selected = false;
volatile uint8_t octave;
SemaphoreHandle_t sound_tableMutex;
SemaphoreHandle_t sampleBufferSemaphore;


uint8_t sampleBuffer0[SAMPLE_BUFFER_SIZE];
uint8_t sampleBuffer1[SAMPLE_BUFFER_SIZE];
volatile bool writeBuffer1 = false;

float local_timestep [12] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

Key sound_table [12];
volatile bool outBits[8] = {false,false,false,true,true,true,true};

// config
uint8_t ownID;
volatile uint8_t previous_west;
volatile uint8_t previous_east;

volatile bool configFlag = true;
volatile bool endConfigFlag = false;

std::map<uint8_t, uint8_t> positionTable;
SemaphoreHandle_t westeastArrayMutex;
volatile uint8_t westeastArray[2];
bool single_board;
// set receiver
bool main_speaker = false;

// mute
bool mute = false;

//critical section
SemaphoreHandle_t critical_section_mutex;

// set pin
void setPinDirections(){
  pinMode(RA0_PIN, OUTPUT);
  pinMode(RA1_PIN, OUTPUT);
  pinMode(RA2_PIN, OUTPUT);
  pinMode(REN_PIN, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(OUTL_PIN, OUTPUT);
  pinMode(OUTR_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(C0_PIN, INPUT);
  pinMode(C1_PIN, INPUT);
  pinMode(C2_PIN, INPUT);
  pinMode(C3_PIN, INPUT);
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);
}
