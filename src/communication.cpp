// #include "global_variables.h"
#include <algorithm>
#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>
#include <ES_CAN.h>
#include <functional>

QueueHandle_t msgInQ;

// void CAN_RX_ISR(void) {
// 	uint8_t RX_Message_ISR[8];
// 	uint32_t ID;
// 	CAN_RX(ID, RX_Message_ISR);
// 	xQueueSendFromISR(msgInQ, RX_Message_ISR, NULL);
// }

class Communication{

 private:
    uint32_t ID;
    uint32_t Mask;
    bool self_looping;
    uint8_t octave;
    uint8_t TX_Message[8] = {0};
    uint8_t RX_Message[8] = {0};
    
public:
    Communication(uint32_t ID_input){
        ID = ID_input;
        Mask = 0x7ff;
        self_looping = false;
        octave = 4;
    }

    void sendMessage(uint8_t previous_array [], uint8_t current_array []){
        uint32_t current_keys = current_array[2] << 8 | current_array[1] << 4 | current_array[0];
        uint32_t previous_keys = previous_array[2] << 8 | previous_array[1] << 4 | previous_array[0];
        uint32_t xor_keys = current_keys ^ previous_keys;
        int8_t index = -1;
        for (int i = 0; i < 32; i++){
            if(((xor_keys>>i) & 1 )== 1){
            index = i;
            Serial.print("index_update");
            Serial.println(index);
            break;
            }
        }
        if (index != -1){
            if (((current_keys >> index) & 1) == 0){
            // pressed
            TX_Message[0] = 'P';
            TX_Message[1] = octave;
            TX_Message[2] = index;
            }
            else{
            // released
            TX_Message[0] = 'R';
            TX_Message[1] = octave;
            TX_Message[2] = index;
            }
            CAN_TX(0x123, TX_Message);
        } 
    }

    // void decodeTask(void *pvParameters){
    //     while (1){
    //         xQueueReceive(msgInQ, RX_Message, portMAX_DELAY);
    //         uint8_t press_release = RX_Message[0];
    //         uint8_t octave_number = RX_Message[1];
    //         uint8_t note_number = RX_Message[2];
    //         uint8_t stepSize;
    //         if (press_release == 'P'){
    //             // convert the note number to a step size
    //             stepSize = stepSizes[note_number];
    //             stepSize = stepSize << (octave_number-4);
    //         }
    //         else if(press_release == 'R'){
    //             stepSize = 0;
    //         }
    //         __atomic_store_n(&currentStepSize, stepSize, __ATOMIC_RELAXED);
    //     }
    // }

    void Initialize_CAN(){
        msgInQ = xQueueCreate(36,8);
        CAN_Init(true);
        setCANFilter(0x123,0x7ff);
        //CAN_RegisterRX_ISR(CAN_RX_ISR);
        CAN_Start();
    }
    void updateOctave(uint8_t new_octave){
        octave = new_octave;
    }
    uint8_t * getTXMessageValue(){
        return TX_Message;
    }
    uint8_t * getRXMessageValue(){
        return RX_Message;
    }
};