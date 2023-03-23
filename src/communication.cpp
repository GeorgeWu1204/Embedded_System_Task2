#include "communication.h"




uint8_t TX_Message[8] = {0};

void decodeTask(void *pvParameters){
    uint8_t localRX_Message[8];
    while (1){
        xQueueReceive(msgInQ, localRX_Message, portMAX_DELAY);
        uint8_t first_message_bit = localRX_Message[0];
        uint8_t second_message_bit = localRX_Message[1];
        uint8_t third_message_bit = localRX_Message[2];
        
        if (first_message_bit == 'P'){
            modified_soundMap(second_message_bit, third_message_bit, true);
        }
        else if(first_message_bit == 'R'){
            modified_soundMap(second_message_bit, third_message_bit, false);
        }
        else if (first_message_bit == 'S'){
            if (configFlag == false){
                configFlag = true;
            }
        }
        else if (first_message_bit == 'L'){
            positionTable[second_message_bit] = third_message_bit;
        }
        else if (first_message_bit == 'E'){
            endConfigFlag = true;

        }
        else if (first_message_bit == 'M'){
            main_speaker = false;
        }
        else if (first_message_bit == 'U'){
            if (second_message_bit == 0){
                mute = false;
            }
            else {
                mute = true;
            } 
        }
        else if (first_message_bit == 'O'){
            if (second_message_bit == 1){
                octave += third_message_bit;
            }
            else if (second_message_bit == 0){
                octave -= third_message_bit;
            }
        }
        #ifdef TEST_DECODE
        break;
        #endif
    }
}

void CAN_TX_Task (void * pvParameters) {
    //Serial.println("start CAN TX Task");
	uint8_t msgOut[8];
	while (1) {
	    xQueueReceive(msgOutQ, msgOut, portMAX_DELAY);
		xSemaphoreTake(CAN_TX_Semaphore, portMAX_DELAY);
		CAN_TX(0x123, msgOut);
        #ifdef TEST_TRANSMIT
        break;
        #endif
	}
}

void sendMessage(uint8_t msg0, uint8_t msg1, uint8_t msg2){
    xSemaphoreTake(critical_section_mutex, portMAX_DELAY);
    TX_Message[0] = msg0;
    TX_Message[1] = msg1;
    TX_Message[2] = msg2;
    xQueueSend( msgOutQ, TX_Message, portMAX_DELAY);
    xSemaphoreGive(critical_section_mutex);
 
}





void CAN_RX_ISR(void) {
	uint8_t RX_Message_ISR[8];
	uint32_t ID;
	CAN_RX(ID, RX_Message_ISR);
	xQueueSendFromISR(msgInQ, RX_Message_ISR, NULL);
}


void CAN_TX_ISR (void) {
	xSemaphoreGiveFromISR(CAN_TX_Semaphore, NULL);
}
    
void initializeCAN(){

    #ifndef TEST_DECODE
    msgInQ = xQueueCreate(36,8);
    #else // test mode: create a bigger queue since the decode function is looped 32 times 
    msgInQ = xQueueCreate(384,8);
    #endif

    #if !defined(TEST_SCANKEYS) && !defined(TEST_TRANSMIT) 
    msgOutQ = xQueueCreate(36,8);
    #else // test mode: create a bigger queue since the scankeys function is looped 32 times 
    msgOutQ = xQueueCreate(384,8);
    #endif
    
    CAN_Init(false);
    setCANFilter(0x123,0x7ff);
    #ifndef DISABLE_THREADS
    CAN_RegisterRX_ISR(CAN_RX_ISR);
    CAN_RegisterTX_ISR(CAN_TX_ISR);
    #endif
    CAN_Start();
}


