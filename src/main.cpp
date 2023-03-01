#include <algorithm>
#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>
#include "knob.cpp"
// #include "sin.cpp"
#include "communication.cpp"
// #include "global_variables.h"

// Constants

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

// Display driver object
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);

// note freq

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

volatile int32_t currentStepSize;
volatile int32_t currentIndex;
volatile int8_t knob3Rotation;
volatile uint8_t keyArray[7];

volatile uint8_t globalTX_Message[8]={0};
volatile uint8_t globalRX_Message[8]={0};



SemaphoreHandle_t keyArrayMutex;
// Knob knob1;
// Knob knob2;
Knob knob3;
Communication communication(0x123);
//CentralOctaveLookUpTable centralOctaveLookUpTable;


// Function to set outputs using key matrix
void setOutMuxBit(const uint8_t bitIdx, const bool value)
{
  digitalWrite(REN_PIN, LOW);
  digitalWrite(RA0_PIN, bitIdx & 0x01);
  digitalWrite(RA1_PIN, bitIdx & 0x02);
  digitalWrite(RA2_PIN, bitIdx & 0x04);
  digitalWrite(OUT_PIN, value);
  digitalWrite(REN_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(REN_PIN, LOW);
}

uint8_t readCols()
{
  uint8_t C0 = digitalRead(C0_PIN);
  uint8_t C1 = digitalRead(C1_PIN);
  uint8_t C2 = digitalRead(C2_PIN);
  uint8_t C3 = digitalRead(C3_PIN);
  uint8_t concate_result = (C3 << 3) | (C2 << 2) | (C1 << 1) | C0;
  return concate_result;
}

void setRow(uint8_t rowIdx)
{
  digitalWrite(REN_PIN, LOW);
  digitalWrite(RA0_PIN, rowIdx & 1);
  digitalWrite(RA1_PIN, (rowIdx >> 1) & 1);
  digitalWrite(RA2_PIN, (rowIdx >> 2) & 1);
  digitalWrite(REN_PIN, HIGH);
}

void sampleISR()
{
  static int32_t phaseAcc = 0;
  static int32_t sampleNumber = 0;
  phaseAcc += currentStepSize;
  sampleNumber += 1;
  int32_t Vout = (phaseAcc >> 24) -128;
  // int32_t Vout = (centralOctaveLookUpTable.accessTable(currentIndex,sampleNumber)>> 24) - 128;
  Vout = Vout >> (8 - knob3Rotation);
  analogWrite(OUTR_PIN, Vout + 128);
}
    

void scanKeysTask(void *pvParameters)
{
  const TickType_t xFrequency = 50 / portTICK_PERIOD_MS;
  // xFrequency initiation interval of task set to 50ms
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // xLastWakeTime will store the time (tick count) of the last initiation.
  uint32_t localCurrentStepSize;
  uint8_t knob3_current_val;
  uint8_t localkeyArray[7];
  uint8_t previouslocalkeyArray[7];

  while (1)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    // vTaskDelayUntil blocks execution of the thread until xFrequency ticks have happened since the last execution of the loop.
    for (uint8_t i = 0; i < 4; i++)
    {
      setRow(i);
      delayMicroseconds(3);
      localkeyArray[i] = readCols();
    }
    xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
    std::copy(localkeyArray,localkeyArray+7,keyArray);
    xSemaphoreGive(keyArrayMutex);

    // send message if any key changed
    communication.sendMessage(previouslocalkeyArray, localkeyArray);
    std::copy(communication.getTXMessageValue(), communication.getTXMessageValue() + 8, globalTX_Message); 
 
    // for sound output
    uint32_t keys = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
    localCurrentStepSize = 0;
    uint8_t currentIndex = 0;
    for (int g = 0; g < 12; g++)
    {
      if (((keys >> g) & 1) == 0)
      {
        localCurrentStepSize = stepSizes[g];
        currentIndex = g;
        // Serial.print("GGGGGGGG");
        // Serial.print(centralOctaveLookUpTable.accessTable(currentIndex, 100));
      }
    }
    __atomic_store_n(&currentStepSize, localCurrentStepSize, __ATOMIC_RELAXED);
    __atomic_store_n(&currentIndex, currentIndex, __ATOMIC_RELAXED); // This is 

    // detect knob rotation
    knob3_current_val = localkeyArray[3] & 3 ;
    knob3.updateRotationValue(knob3_current_val);
    knob3Rotation = knob3.getRotationValue();
    std::copy(localkeyArray, localkeyArray + 7, previouslocalkeyArray); 
  }
}
 
void displayUpdateTask(void *pvParameters)
{
  const TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint32_t output;
  uint8_t key_index;
  const char *output_key;
  uint8_t localkeyArray[7];
  // Update display
  while (1)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    u8g2.clearBuffer();                 // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
    std::copy(keyArray,keyArray+7,localkeyArray);
    xSemaphoreGive(keyArrayMutex);
    output = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
    key_index = 0;
    for (int g = 0; g < 12; g++){
      if (((output >> g) & 1) == 0)
      {
        key_index = g + 1;
      }
    }

    // receive message
    // std::copy(communication.getRXMessageValue(), communication.getRXMessageValue() + 8, globalRX_Message);  
    
    // u8g2.setCursor(2, 10);
    // u8g2.print(output, HEX);
    // u8g2.setCursor(2, 20);
    // u8g2.print(Key_set[key_index]);
    // u8g2.sendBuffer(); // transfer internal memory to the display
    // // Toggle LED
    // digitalToggle(LED_BUILTIN);

    // // Display the latest transmit message
    // u8g2.setCursor(66,30);
    // u8g2.print((char) TX_Message[0]);
    // u8g2.print(TX_Message[1]);
    // u8g2.print(TX_Message[2]);
    u8g2.setCursor(2,30);
    u8g2.print((char) globalTX_Message[0]);
    u8g2.print(globalTX_Message[1]);
    u8g2.print(globalTX_Message[2]);
    u8g2.sendBuffer();

    u8g2.setCursor(60,30);
    u8g2.print((char) globalRX_Message[0]);
    u8g2.print(globalRX_Message[1]);
    u8g2.print(globalRX_Message[2]);
    u8g2.sendBuffer();
  }
}





void setup()
{
  // put your setup code here, to run once:

  // Set pin directions
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

  // Initialise display
  setOutMuxBit(DRST_BIT, LOW); // Assert display logic reset
  delayMicroseconds(2);
  setOutMuxBit(DRST_BIT, HIGH); // Release display logic reset
  u8g2.begin();
  setOutMuxBit(DEN_BIT, HIGH); // Enable display power supply

  // Initialise UART
  Serial.begin(9600);
  Serial.println("Hello World");

  // Initialise mutex
  keyArrayMutex = xSemaphoreCreateMutex();

  // Initialise SampleISR
  TIM_TypeDef *Instance = TIM1;
  HardwareTimer *sampleTimer = new HardwareTimer(Instance);
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  
  // initialise communication
  communication.Initialize_CAN();

  
  //centralOctaveLookUpTable.initializeTable();
  
  // Initialize threading scanKeysTask
  TaskHandle_t scanKeysHandle = NULL;
  xTaskCreate(
      scanKeysTask,     /* Function that implements the task */
      "scanKeys",       /* Text name for the task */
      64,               /* Stack size in words, not bytes */
      NULL,             /* Parameter passed into the task */
      2,                /* Task priority */
      &scanKeysHandle); /* Pointer to store the task handle */

  // Initialize threading displayUpdateTask
  TaskHandle_t displayHandle = NULL;
  xTaskCreate(
      displayUpdateTask, /* Function that implements the task */
      "display",         /* Text name for the task */
      256,               /* Stack size in words, not bytes */
      NULL,              /* Parameter passed into the task */
      1,                 /* Task priority */
      &displayHandle);   /* Pointer to store the task handle */

  // // Initialize threading decodeTask
  // TaskHandle_t decodeHandle = NULL;
  // xTaskCreate(
  //     decodeTask, /* Function that implements the task */
  //     "decode",         /* Text name for the task */
  //     256,               /* Stack size in words, not bytes */
  //     NULL,              /* Parameter passed into the task */
  //     3,                 /* Task priority */
  //     &decodeHandle);   /* Pointer to store the task handle */

  // Start RTOS scheduler
  vTaskStartScheduler();
} 

void loop()
{
}