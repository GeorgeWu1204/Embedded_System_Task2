#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <iostream>
#include <string>
#include <ES_CAN.h>

class Communication{

 private:
    uint32_t ID;
    uint32_t Mask;
    bool self_looping;
    uint8_t octave;
    uint8_t TX_Message[8] = {0};
    uint8_t RX_Message[8] = {0};
    QueueHandle_t msgInQ;
    
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
    void receiveMessage(){
        while (CAN_CheckRXLevel())
	    CAN_RX(ID, RX_Message);
    }
    void Initialize_CAN(){
        CAN_Init(true);
        setCANFilter(0x123,0x7ff);
        CAN_Start();
        msgInQ = xQueueCreate(36,8); // (number of params, size of each in bytes)
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