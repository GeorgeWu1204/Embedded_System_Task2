#include "global_variables.h"
#include "display.h"
#include "scan.h"
#include "sine.h"
#include "reorganize.h"
// #include "sineLookUpTable.cpp"

// sine lookup table
// CentralOctaveLookUpTable centralOctaveLookUpTable;

// void sampleISR()
// {
//   //static int32_t phaseAcc = 0;
//   static int32_t sampleNumber = 0;
//   sampleNumber += 1;
//   if(sampleNumber>tableSizes[currentKey]){
//     sampleNumber = 0;
//   };
//   int32_t Vout = centralOctaveLookUpTable.accessTable(currentKey,sampleNumber);
//   // int32_t Vout = (phaseAcc >> 24) -128;
//   Vout = Vout >> (8 - knob3Rotation);

//   if((__atomic_load_n(&octave,__ATOMIC_RELAXED) == 4) && __atomic_load_n(&pressed,__ATOMIC_RELAXED)){
//     analogWrite(OUTR_PIN, Vout + 128);
//   }
//   else{
//     analogWrite(OUTR_PIN,0);
//   }
// }

void sampleISR() {
  static uint32_t readCtr = 0;
  if (readCtr == SAMPLE_BUFFER_SIZE) {
    // xSemaphoreTakeFromISR(sampleBufferSemaphore, NULL);
    readCtr = 0;
    writeBuffer1 = !writeBuffer1;
    xSemaphoreGiveFromISR(sampleBufferSemaphore, NULL);
  }

  if (writeBuffer1 && (__atomic_load_n(&octave,__ATOMIC_RELAXED) == 4) ){
    analogWrite(OUTR_PIN, sampleBuffer0[readCtr++]);
  }
  else{
    analogWrite(OUTR_PIN, sampleBuffer1[readCtr++]);}
}




void setup()
{
  // Set pin directions
  setPinDirections();

  // Initialise display
  initialiseDisplay();

  // Initialise UART
  Serial.begin(9600);

  // set own ID (used for ordering)
  ownID = getHashedID();

  // Initialise mutex
  keyArrayMutex = xSemaphoreCreateMutex();
  sound_tableMutex = xSemaphoreCreateMutex();
  RX_MessageMutex = xSemaphoreCreateMutex();
  CAN_TX_Semaphore = xSemaphoreCreateCounting(3,3);
  sampleBufferSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(sampleBufferSemaphore);
  
  # ifndef DISABLE_THREADS
  // Initialise SampleISR
  TIM_TypeDef *Instance = TIM1;
  HardwareTimer *sampleTimer = new HardwareTimer(Instance);
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  # endif
  
  // initialise lookuptable
  // centralOctaveLookUpTable.initializeTable();
  // initialise communication
  initializeCAN();
  initialize_table();
  // Initialize threading scanKeysTask

  # ifndef DISABLE_THREADS
  TaskHandle_t scanKeysHandle = NULL;
  TaskHandle_t displayHandle = NULL;
  TaskHandle_t write_to_double = NULL;
  xTaskCreate(
      scanKeysTask,     /* Function that implements the task */
      "scanKeys",       /* Text name for the task */
      64,               /* Stack size in words, not bytes */
      NULL,             /* Parameter passed into the task */
      5,                /* Task priority */
      &scanKeysHandle); /* Pointer to store the task handle */
  
  // Initialize threading displayUpdateTask
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
      2,                 /* Task priority */
      &decodeHandle);   /* Pointer to store the task handle */

  // Initialize threading transmitTask
  TaskHandle_t transmitHandle = NULL;
  xTaskCreate(
      CAN_TX_Task , /* Function that implements the task */
      "transmit",         /* Text name for the task */
      256,               /* Stack size in words, not bytes */
      NULL,              /* Parameter passed into the task */
      3,                 /* Task priority */
      &transmitHandle);   /* Pointer to store the task handle */
  xTaskCreate(
    write_to_double_buffer,		    /* Function that implements the task */
    "write_to_buffer",		      /* Text name for the task */
    64,      		        /* Stack size in words, not bytes */
    NULL,			          /* Parameter passed into the task */
    5,			            /* Task priority */
    &write_to_double);  /* Pointer to store the task handle */

  // Start RTOS scheduler
  vTaskStartScheduler();
  #endif

  
  #ifdef TEST_SCANKEYS
    // void *pvParameters;
    uint32_t startTime = micros();
    for (int iter = 0; iter < 32; iter++) {
      scanKeysTask(NULL);
    }
    Serial.print("Time to run Scan keys task 32 times: ");
    Serial.print((micros()-startTime)/32000);
    Serial.println(" ms");
  
  #endif
  
  #ifdef TEST_DISPLAY
    uint32_t startTime = micros();
    for (int iter = 0; iter < 100; iter++) {
      displayUpdateTask(NULL);
    }
    Serial.print("Average time to run display task 1 time: ");
    Serial.print((micros()-startTime)/100000);
    Serial.println(" ms");
    while(1);
  #endif

  #ifdef TEST_DECODE
    Serial.println("Start");
    uint8_t msg[8] = {'P',4,9,0,0,0,0,0};
    for (int iter = 0; iter < 384; iter++){
      xQueueSend(msgInQ, msg, NULL);
    }
    uint32_t startTime = micros();
    for (int iter = 0; iter < 384; iter++) {
      decodeTask(NULL);
    }
    Serial.print("Time to run 384 decode tasks: ");
    Serial.print((micros()-startTime));
    Serial.println("us");
    while(1);
  #endif

  #ifdef TEST_TRANSMIT
    Serial.println("Start");
    uint8_t msg[8] = {'P',4,9,0,0,0,0,0};
    for (int iter = 0; iter < 384; iter++){
      xQueueSend(msgOutQ, msg, NULL);
    }
    uint32_t startTime = micros();
    for (int iter = 0; iter < 384; iter++) {
      CAN_TX_Task(NULL);
    }
    Serial.print("Time to run 384 transmit tasks: ");
    Serial.print((micros()-startTime));
    Serial.println("us");
    while(1);
  #endif
} 

void loop()
{}