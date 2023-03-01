#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>


class Knob { 
    private:  
        uint8_t previous_state;

        int8_t rotation_state;
        int8_t rotation_direction;

        uint8_t maxRotationValue;
        uint8_t minRotationValue; 

    public:   
        Knob(){     
            previous_state = 0;
            rotation_state = 0;
            rotation_direction = 0;
            maxRotationValue = 8;
            minRotationValue = 0;
        }         

        void updateRotationValue(uint8_t current_state) {  
           
            switch (previous_state){
                case (0):
                switch (current_state){
                    case (1):
                    rotation_state += 1;
                    rotation_direction = 1;
                    break;
                    case (3):
                    rotation_state += rotation_direction;
                    break;
                    default:
                    break;
                }
                break;
                case (1):
                switch (current_state){
                    case (0):
                    rotation_state -= 1;
                    rotation_direction = -1;
                    break;
                    default:
                    break;
                }
                break;
                case (2):
                switch (current_state){
                    case (3):
                    rotation_state -= 1;
                    rotation_direction = -1;
                    break;
                    default:
                    break;
                }
                case (3):
                switch (current_state){
                    case (0):
                    rotation_state += rotation_direction;
                    break;
                    case (2):
                    rotation_state += 1;
                    rotation_direction = 1;
                    break;
                    default:
                    break;
                }
                default:
                break; 
            }
            rotation_state = rotation_state < minRotationValue ? minRotationValue : (rotation_state > maxRotationValue ? maxRotationValue : rotation_state);
            previous_state = current_state;
        }

        void setUpper(uint8_t Value) {  
            maxRotationValue = minRotationValue;
        }

        void setLower(uint8_t Value) {  
            minRotationValue = minRotationValue;
        }

        int8_t getRotationValue() { 
            return rotation_state;
        }
};
