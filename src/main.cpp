#include "global_variables.h"
#include "display.h"
#include "scan.h"
#include "sineLookUpTable.cpp"

// sine lookup table
CentralOctaveLookUpTable centralOctaveLookUpTable;

void sampleISR()
{
  //static int32_t phaseAcc = 0;
  static int32_t sampleNumber = 0;
  sampleNumber += 1;
  if(sampleNumber>tableSizes[currentKey]){
    sampleNumber = 0;
  };
  int32_t Vout = centralOctaveLookUpTable.accessTable(currentKey,sampleNumber);
  // int32_t Vout = (phaseAcc >> 24) -128;
  Vout = Vout >> (8 - knob3Rotation);

  if(currentKey!=100){
    analogWrite(OUTR_PIN, Vout + 128);
  }
  else{
    analogWrite(OUTR_PIN,0);
  }
}

void setup()
{
  // Set pin directions
  setPinDirections();

  // Initialise display
  initialiseDisplay();

  // Initialise UART
  Serial.begin(9600);
  Serial.println("Hello World");

  // Initialise mutex
  keyArrayMutex = xSemaphoreCreateMutex();
  RX_MessageMutex = xSemaphoreCreateMutex();
  CAN_TX_Semaphore = xSemaphoreCreateCounting(3,3);
  
  // Initialise SampleISR
  TIM_TypeDef *Instance = TIM1;
  HardwareTimer *sampleTimer = new HardwareTimer(Instance);
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  
  // initialise lookuptable
  centralOctaveLookUpTable.initializeTable();
  // initialise communication
  initializeCAN();
  
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

  // Initialize threading decodeTask
  TaskHandle_t decodeHandle = NULL;
  xTaskCreate(
      decodeTask, /* Function that implements the task */
      "decode",         /* Text name for the task */
      256,               /* Stack size in words, not bytes */
      NULL,              /* Parameter passed into the task */
      3,                 /* Task priority */
      &decodeHandle);   /* Pointer to store the task handle */

      // Initialize threading decodeTask
  TaskHandle_t transmitHandle = NULL;
  xTaskCreate(
      CAN_TX_Task , /* Function that implements the task */
      "transmit",         /* Text name for the task */
      256,               /* Stack size in words, not bytes */
      NULL,              /* Parameter passed into the task */
      4,                 /* Task priority */
      &transmitHandle);   /* Pointer to store the task handle */

  // Start RTOS scheduler
  vTaskStartScheduler();
} 

void loop()
{}