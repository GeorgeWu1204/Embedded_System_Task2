#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>

#ifndef KNOB_H
#define KNOB_H



class Knob{
    private:  
        uint8_t previous_state;

        int8_t rotation_state;
        int8_t rotation_direction;

        uint8_t maxRotationValue;
        uint8_t minRotationValue; 
    public:
        Knob();
        void updateRotationValue(uint8_t current_state);
        void setRotationState(int8_t Value);
        int8_t getRotationValue();

};

#endif