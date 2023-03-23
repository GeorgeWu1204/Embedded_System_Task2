# include "scan.h"


Knob knob3;
Knob knob2;

/// @brief
/// Return a 4-bit value (C3 | C2 | C1 | C0)
/// @return 
uint8_t readCols()
{
  uint8_t C0 = digitalRead(C0_PIN);
  uint8_t C1 = digitalRead(C1_PIN);
  uint8_t C2 = digitalRead(C2_PIN);
  uint8_t C3 = digitalRead(C3_PIN);
  uint8_t concate_result = (C3 << 3) | (C2 << 2) | (C1 << 1) | C0;
  return concate_result;
}


/// @brief
/// Selecting which row to detect.
/// @return 
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
  const TickType_t xFrequency = 20 / portTICK_PERIOD_MS;
  // xFrequency initiation interval of task set to 50ms
  TickType_t xLastWakeTime = xTaskGetTickCount();
  // xLastWakeTime will store the time (tick count) of the last initiation.
  uint8_t knob3_current_val;
  uint8_t knob1_current_val;
  // A bool
  uint8_t knob3_pressed;
  uint8_t knob2_pressed;
  /// A 8x4 matrix, represented at array of eight 4-bit value.
  uint8_t localkeyArray[7];
  uint32_t previous_keys = 4095;
  uint32_t current_keys;
  uint32_t current_keys_shifted;
  uint32_t xor_keys;
  uint8_t local_octave;
  bool pressed;
  uint8_t count = 0;
  uint8_t previous_knob3_pressed = 1;
  uint8_t previous_knob2_pressed = 1;
  uint8_t octaveChange = 0;
  

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

    xSemaphoreTake(westeastArrayMutex, portMAX_DELAY);
    std::copy(localkeyArray + 5,localkeyArray+7,westeastArray);
    xSemaphoreGive(westeastArrayMutex);
   

    /// send message if any key changed
    current_keys = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
    xor_keys = current_keys ^ previous_keys;

    
    // #ifdef TEST_SCANKEYS || TEST_OVERALL
    #if defined(TEST_SCANKEYS) || defined(TEST_OVERALL)
    xor_keys = 0b111111111111; // to send sound message for all keys
    #endif
    
    /// modified sound map for local key press 
    current_keys_shifted = current_keys;
    
    if ((xor_keys) != 0){
      local_octave = __atomic_load_n(&octave, __ATOMIC_RELAXED);
    
      for (int i = 1; i < 13; i++){
        if ((xor_keys & 1) == 1){
          pressed = !(current_keys_shifted & 1);
          modified_soundMap(local_octave, i, pressed);
          if (main_speaker == false){
            if(pressed){
              sendMessage('P', local_octave, i);
            }
            else{
              sendMessage('R', local_octave, i);
            }
          }
        }
        xor_keys = xor_keys >> 1;
        current_keys_shifted = current_keys_shifted >> 1;
      }
      /// set it to previous_keys in the end. And only if current_keys is different from previous_keys
      previous_keys = current_keys; 
    }
    /// detect knob3 rotation for volume
    knob3_current_val = localkeyArray[3] & 3 ;
    knob3.updateRotationValue(knob3_current_val);
    knob3Rotation = knob3.getRotationValue();

    /// detect knob1 rotation for octave
    knob1_current_val = localkeyArray[4] & 3 ;
    knob1.updateRotationValue(knob1_current_val);
    knob1Rotation = knob1.getRotationValue();
    if (knob1Rotation != prev_knob1Rotation){
      if (knob1Rotation > prev_knob1Rotation){
        octaveChange = knob1Rotation - prev_knob1Rotation;
        octave += octaveChange;
        if (!single_board){
          sendMessage('O',1,octaveChange); // if octave increase
        }
      }
      else{
        octaveChange = -knob1Rotation + prev_knob1Rotation;
        octave -= octaveChange;
        if (!single_board){
          sendMessage('O',0,octaveChange); // if octave decrease
        }
      }

      prev_knob1Rotation = knob1Rotation;
    }

    /// detect knob3 pressed for main speaker identification
    knob3_pressed = (localkeyArray[5] & 2) >> 1; // Knob 3 S 
    if ((previous_knob3_pressed == 0) & (knob3_pressed==1) ){
      // send if button is pressed
      main_speaker = true;
      if (!single_board){
        sendMessage('M', 0, 0);
      }
    }
    previous_knob3_pressed = knob3_pressed;

    /// detect knob2  press - for muting
    knob2_pressed = (localkeyArray[5] & 1); // Knob 2 S 
    if ((previous_knob2_pressed == 0) & (knob2_pressed==1) ){
      // send if button is pressed
      if(!single_board){
        if (mute == true){
          sendMessage('U', 0, 0);
        }
        else{
          sendMessage('U', 1, 0);
        }
      }
      mute = !mute;
    }
    previous_knob2_pressed = knob2_pressed;
 
    
  
    if (count == 0){
      previous_west = !(localkeyArray[5] >> 3);
      previous_east = !(localkeyArray[6] >> 3);
      count += 1;
    }

    if (configFlag == false){
      uint8_t current_west = !(localkeyArray[5] >> 3);
      uint8_t current_east = !(localkeyArray[6] >> 3);
      if ((previous_west != current_west) || (previous_east != current_east)){
        sendMessage('S',0,0);
        configFlag = true;
      }  
    }

    #ifdef TEST_SCANKEYS
    break;
    #endif

  }
}

