#include "knob.h"

Knob::Knob(){
    previous_state = 0;
    rotation_state = 0;
    rotation_direction = 0;
    maxRotationValue = 8;
    minRotationValue = 0;
} 

void Knob::updateRotationValue(uint8_t current_state){
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

void Knob::setRotationState(int8_t Value){
    rotation_state = Value;
}

int8_t Knob::getRotationValue() { 
    return rotation_state;
}

