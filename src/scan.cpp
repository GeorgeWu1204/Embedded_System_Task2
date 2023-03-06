# include "scan.h"

Knob knob3;

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

void scanKeysTask(void *pvParameters)
{
  const TickType_t xFrequency = 50 / portTICK_PERIOD_MS;
  // xFrequency initiation interval of task set to 50ms
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // xLastWakeTime will store the time (tick count) of the last initiation.
  uint8_t knob3_current_val;
  uint8_t localkeyArray[7];
  uint8_t previouslocalkeyArray[7];
  uint8_t currentIndex = 100;
  uint32_t previouskeys = 0;
  uint32_t keys;
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
    sendMessage(previouslocalkeyArray, localkeyArray);
    
    // for sound output
    keys = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
    if ((keys ^ previouskeys) != 0){
      Serial.println(keys);
      Serial.println(previouskeys);
      currentIndex = 100;
      for (int g = 0; g < 12; g++){
        if (((keys >> g) & 1) == 0){
          currentIndex = g;
        }
      }
      __atomic_store_n(&currentKey, currentIndex, __ATOMIC_RELAXED); 
      previouskeys = keys;
    }
  
    // detect knob rotation
    knob3_current_val = localkeyArray[3] & 3 ;
    knob3.updateRotationValue(knob3_current_val);
    knob3Rotation = knob3.getRotationValue();
    std::copy(localkeyArray, localkeyArray + 7, previouslocalkeyArray); 
  }
}