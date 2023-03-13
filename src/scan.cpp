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

// uint8_t checkDiff(uint8_t localkeyArray[7],  uint8_t previouslocalkeyArray[7]){
//     Serial.println((previouslocalkeyArray[5] >> 3) & 1);
//     Serial.println(previouslocalkeyArray[6]);
//     bool current_west = !((localkeyArray[5] >> 3) & 1);
//     bool current_east = !((localkeyArray[6] >> 3) & 1);
//     bool previous_west = !((previouslocalkeyArray[5] >> 3) & 1);
//     bool previous_east = !((previouslocalkeyArray[6] >> 3) & 1);
//     // Serial.println("current");
//     // Serial.println(current_west);
//     // Serial.println(current_east);
//     // Serial.println("previous");
//     // Serial.println(previouslocalkeyArray[5]);
//     // Serial.println(previouslocalkeyArray[6]);
//     if ((previous_west != current_west) || (previous_east != current_east)){
//       Serial.println("Found difference");
//       return 1;
//     }
//     else{
//       return 0;
//     }
// }

void scanKeysTask(void *pvParameters)
{
  const TickType_t xFrequency = 50 / portTICK_PERIOD_MS;
  // xFrequency initiation interval of task set to 50ms
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // xLastWakeTime will store the time (tick count) of the last initiation.
  uint8_t knob3_current_val;
  uint8_t localkeyArray[7] = {0,0,0,0,0,0,0};
  uint8_t previouslocalkeyArray[7];
  uint32_t previous_keys = 0;
  uint32_t current_keys;
  uint32_t current_keys_shifted;
  uint32_t xor_keys;
  uint8_t local_octave;
  bool pressed;
  uint8_t count = 0;

  std::vector<uint16_t> local_pressed_keys;


  // sendMessage('S',0,0);

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

    if (count == 0){
      std::copy(localkeyArray,localkeyArray+7,previouslocalkeyArray);
      count += 1;
    }
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
      for (uint8_t i = 0; i < 12; i++){
        if ((xor_keys & 1) == 1){
          pressed = (current_keys_shifted & 1);

          uint8_t local_octave =  __atomic_load_n(&octave,__ATOMIC_RELAXED);
          // if (pressed){
          //   sendMessage('P', local_octave, i);
          // }
          // else{
          //   sendMessage('R', local_octave, i);
          // }
          
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
    
    if (count == 0){
      previous_west = !(previouslocalkeyArray[5] >> 3);
      previous_east = !(previouslocalkeyArray[6] >> 3);
      count += 1;
    }

    if (configFlag == false){
      // Serial.println("==0");
      // Serial.println(previouslocalkeyArray[5]);
      // Serial.println(previouslocalkeyArray[5] >> 3);
      uint8_t current_west = !(localkeyArray[5] >> 3);
      uint8_t current_east = !(localkeyArray[6] >> 3);
      
      // Serial.println("current");
      // Serial.println(current_west);
      // Serial.println(current_east);
      // Serial.println("previous");
      // Serial.println(previouslocalkeyArray[5]);
      // Serial.println(previouslocalkeyArray[6]);
      // Serial.println(current_west);
      // Serial.println(current_east);
      // Serial.println("previous");
      // Serial.println(previous_west);
      // Serial.println(previous_east);
 
      if ((previous_west != current_west) || (previous_east != current_east)){
        Serial.println("Found difference");
        // Serial.println("cahnge detecte");
        // delayMicroseconds(100); 
        sendMessage('S',0,0);
        Serial.println("S sent");
        configFlag = true;
      }
      
    }
    // std::copy(localkeyArray, localkeyArray + 7, previouslocalkeyArray); 
    #ifdef TEST_SCANKEYS
    break;
    #endif
  }
}

