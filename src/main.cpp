#include <Arduino.h>
#include <U8g2lib.h>
#include <vector>
#include <STM32FreeRTOS.h>

#include "global_variables.h"
#include "global_functions.h"
// #include "communication.h"
#include "sound.h"
#include "display.h"
#include "scan.h"
#include "config.h"
#include "joystick.h"

// Display Sound and scan key TODO
  // volatile uint8_t element;
  // DAC_HandleTypeDef hdac;

void sampleISR() {
  static uint32_t readCtr = 0;
  if (readCtr == SAMPLE_BUFFER_SIZE) {
    // xSemaphoreTakeFromISR(sampleBufferSemaphore, NULL);
    readCtr = 0;
    writeBuffer1 = !writeBuffer1;
    xSemaphoreGiveFromISR(sampleBufferSemaphore, NULL);
  }
  if (writeBuffer1){
    analogWrite(OUTR_PIN, sampleBuffer0[readCtr++]);
  }
  else{
    analogWrite(OUTR_PIN, sampleBuffer1[readCtr++]);}
}

TIM_TypeDef *Instance = TIM1;
HardwareTimer *sampleTimer = new HardwareTimer(Instance);



void setup() {

  Serial.begin(9600);
  setPinDirections();
  initialiseDisplay();
  ownID = getHashedID();

  // Declare Semaphore
  sound_tableMutex = xSemaphoreCreateMutex();
  westeastArrayMutex = xSemaphoreCreateMutex();
  sampleBufferSemaphore = xSemaphoreCreateBinary();
  critical_section_mutex = xSemaphoreCreateMutex();
  CAN_TX_Semaphore = xSemaphoreCreateCounting(3,3);

  initializeCAN();
  xSemaphoreGive(sampleBufferSemaphore);

  # ifndef DISABLE_THREADS

  // Timer Setting
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();
  Serial.print("Inity");
  initialize_table();


  // Initialize run thread
  TaskHandle_t scanKeysHandle = NULL;
  TaskHandle_t displayKeysHandle = NULL;
  TaskHandle_t write_to_double = NULL;
  TaskHandle_t transmitHandle = NULL;
  TaskHandle_t decodeHandle = NULL;
  TaskHandle_t configHandle = NULL;
  TaskHandle_t joystickHandle = NULL;

  xTaskCreate(decodeTask, "decode", 256, NULL, 4, &decodeHandle);   

  xTaskCreate(CAN_TX_Task, "transmit", 256, NULL, 3, &transmitHandle); 

  xTaskCreate(scanKeysTask,"scanKeys", 64, NULL,5, &scanKeysHandle);  

  xTaskCreate(displayUpdateTask,"displayKeys",256,NULL,1,&displayKeysHandle);

  xTaskCreate(write_to_double_buffer, "write_to_buffer",256,NULL, 6, &write_to_double);  
  
  xTaskCreate(configTask, "config",64,NULL,0 ,&configHandle);  

  xTaskCreate(joystickTask, "joystick", 64, NULL, 2, & joystickHandle);

  vTaskStartScheduler();
  #endif

  #ifdef TEST_SCANKEYS
    // void *pvParameters;
    uint32_t startTime = micros();
    Serial.print("Time");
    for (int iter = 0; iter < 32; iter++) {
      scanKeysTask(NULL);
    }
    Serial.print("Time to run Scan keys task 1 times: ");
    Serial.print((micros()-startTime)/32000.0);    
    Serial.println(" ms");
  #endif

  #ifdef TEST_DISPLAY
    #define MAIN_SPEAKER_ON
    uint32_t startTime = micros();
    for (int iter = 0; iter < 100; iter++) {
      displayUpdateTask(NULL);
    }
    Serial.print("Average time to run display task 1 time: ");
    Serial.print((micros()-startTime)/100000.0);
    Serial.println(" ms");
    while(1);
  #endif

  #ifdef TEST_DECODE
    Serial.println("Start");
    uint8_t msg[8] = {'R',4,9,0,0,0,0,0};
    for (int iter = 0; iter < 384; iter++){
      xQueueSend(msgInQ, msg, NULL);
    }
    uint32_t startTime = micros();
    for (int iter = 0; iter < 384; iter++) {
      decodeTask(NULL);
    }
    Serial.print("Time to run 1 decode tasks: ");
    Serial.print((micros()-startTime) / 384.0);
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
    Serial.print((micros()-startTime)/ 384.0);
    Serial.println("us");
    while(1);
  #endif

  #ifdef TEST_CONFIG
    uint32_t startTime = micros();
    for (int iter = 0; iter < 10; iter++) {
      configTask(NULL);
    }
    Serial.print("Average time to run config task 1 time: ");
    Serial.print((micros()-startTime)/10000.0);
    Serial.println(" ms");
    while(1);
  #endif

  #ifdef TEST_JOYSTICK
    uint32_t startTime = micros();
    for (int iter = 0; iter < 100; iter++) {
      joystickTask(NULL);
    }
    Serial.print("Average time to run joystick task 1 time: ");
    Serial.print((micros()-startTime)/100000.0);
    Serial.println(" ms");
    while(1);
  #endif

  #ifdef TEST_SAMPLEISR
    uint32_t startTime = micros();
    for (int iter = 0; iter < 100; iter++) {
      sampleISR();
    }
    Serial.print("Average time to run sample ISR interrupt 1 time: ");
    Serial.print((micros()-startTime)/100.0);
    Serial.println(" us");
    while(1);
  #endif

  #ifdef TEST_BUFFER
    #define SAWTooth_selected
    uint32_t startTime = micros();
    for (int iter = 0; iter < 100; iter++) {
      write_to_double_buffer(NULL);
      xSemaphoreGive(sampleBufferSemaphore);
    }
    Serial.print("Average time to run double buffer task 1 time: ");
    Serial.print((micros()-startTime)/100.0);
    Serial.println(" us");
    while(1);
  #endif
}

void loop() {
  #ifdef TEST_OVERALL
    char run_time_stats [1000];
    vTaskGetRunTimeStats(run_time_stats);
    Serial.print("Joy: ");
    Serial.print(joystick_counter);
    Serial.print("Write: ");
    Serial.println(write_to_double_buffer_counter);
    Serial.println(run_time_stats);
  #endif
}



