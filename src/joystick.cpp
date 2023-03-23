#include"joystick.h"

/// @brief 
/// JoystickTask 
/// This is a seperate task because analogRead take too much time.
/// @param pvParameters 
void joystickTask(void *pvParameters){
  int32_t joystickx_out;
  int32_t previous_sawTooth_selected;

  const TickType_t xFrequency = 10/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime= xTaskGetTickCount();
  while(1) {
    # ifndef TEST_JOYSTICK
      vTaskDelayUntil(&xLastWakeTime, xFrequency);
    # endif
    joystickx_out = 512 - analogRead(JOYX_PIN);
    #ifdef TEST_OVERALL
    joystickx_out = 250;
    #endif 
    
    if(joystickx_out > 200 | joystickx_out < -200){
        if(previous_sawTooth_selected){
            previous_sawTooth_selected = 0;
            sawTooth_selected = !sawTooth_selected;
          }
        }else{
            previous_sawTooth_selected = 1;
        }
      # ifdef TEST_JOYSTICK
      break;
      #endif

      #ifdef TEST_OVERALL
        joystick_counter += 1;
      #endif
      
  }
  
}
