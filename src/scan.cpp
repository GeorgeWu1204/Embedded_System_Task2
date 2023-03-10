# include "scan.h"
# include "reorganize.h"

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
  uint32_t previous_keys = 0;
  uint32_t current_keys;
  uint32_t current_keys_shifted;
  uint32_t xor_keys;
  uint8_t local_octave;
  bool pressed;

  std::vector<uint16_t> local_pressed_keys;

  while (1) {
    # ifndef TEST_SCANKEYS
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    # endif
    // vTaskDelayUntil blocks execution of the thread until xFrequency ticks have happened since the last execution of the loop.
    for (uint8_t i = 0; i < 7; i++){
        setRow(i);
        digitalWrite(OUT_PIN, outBits[i]); //output handshake signal
        delayMicroseconds(3);
        localkeyArray[i] = readCols();
    }
    xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
    std::copy(localkeyArray,localkeyArray+7,keyArray);
    xSemaphoreGive(keyArrayMutex);

    /// send message if any key changed
    current_keys = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
    previous_keys = previouslocalkeyArray[2] << 8 | previouslocalkeyArray[1] << 4 | previouslocalkeyArray[0];
    xor_keys = current_keys ^ previous_keys;
    // for sound output
    # ifdef TEST_SCANKEYS
    xor_keys = 1;
    # endif
    
    /// modified sound map for local key press 
    current_keys_shifted = current_keys;
    
    if ((xor_keys) != 0){
      local_octave = __atomic_load_n(&octave, __ATOMIC_RELAXED);
      for (int i = 0; i < 12; i++){
        if ((xor_keys & 1) == 1){
          pressed = (current_keys_shifted & 1);
          sendMessage(i, pressed);
          modified_soundMap(local_octave, i, pressed);
        }
        xor_keys = xor_keys >> 1;
        current_keys_shifted = current_keys_shifted >> 1;
      }
      
      /// set it to previous_keys in the end. And only if current_keys is different from previous_keys
      previous_keys = current_keys; 
    }


    /// detect knob rotation
    knob3_current_val = localkeyArray[3] & 3 ;
    knob3.updateRotationValue(knob3_current_val);
    knob3Rotation = knob3.getRotationValue();
    std::copy(localkeyArray, localkeyArray + 7, previouslocalkeyArray); 


    bool current_west = !((localkeyArray[5] >> 3) & 1);
    bool current_east = !((localkeyArray[6] >> 3) & 1);
    // Serial.println("current");
    // Serial.println(current_west);
    // Serial.println(current_east);
    if ((previous_west != current_west) || (previous_east != current_east)){
      sendHandshakeMessage('S',0,0);
      vTaskSuspendAll();
      reorganizePositions();
      xTaskResumeAll();
    }
    #ifdef TEST_SCANKEYS
    break;
    #endif
  }
}

