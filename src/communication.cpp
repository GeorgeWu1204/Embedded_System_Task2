#include "communication.h"




uint8_t TX_Message[8] = {0};

void decodeTask(void *pvParameters){
    uint8_t localRX_Message[8];
    //Serial.println("start decodeTask");
    
    while (1){

        xQueueReceive(msgInQ, localRX_Message, portMAX_DELAY);
        xSemaphoreTake(RX_MessageMutex, portMAX_DELAY);
        std::copy(localRX_Message,localRX_Message+8,globalRX_Message);
        xSemaphoreGive(RX_MessageMutex);

        
        uint8_t first_message_bit = localRX_Message[0];
        if (first_message_bit == 'S'){
            vTaskSuspendAll();
            reorganizePositions();
            xTaskResumeAll();
        }
        else{
            uint8_t octave_number = localRX_Message[1];
            uint8_t note_number = localRX_Message[2];
            if (first_message_bit == 'P'){
                modified_soundMap(octave_number, note_number, true);
                // xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
                // sound_table.find(octave_number)->second.push_back(note_number);
                // xSemaphoreGive(sound_tableMutex);
            }
            else if(first_message_bit == 'R'){
                modified_soundMap(octave_number, note_number, false);
                // xSemaphoreTake(sound_tableMutex, portMAX_DELAY);
                // sound_table.find(octave_number)->second.erase(std::remove(sound_table.find(octave_number)->second.begin(), sound_table.find(octave_number)->second.end(), note_number), sound_table.find(octave_number)->second.end());
                // xSemaphoreGive(sound_tableMutex);
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
        xSemaphoreGive(CAN_TX_Semaphore);
        #ifdef TEST_TRANSMIT
        break;
        #endif
	}
}

void sendMessage(int8_t index, bool press){
    if (press){
        // pressed
        TX_Message[0] = 'P';
        TX_Message[1] = __atomic_load_n(&octave,__ATOMIC_RELAXED);
        TX_Message[2] = index;
    }
    else{
        // released
        TX_Message[0] = 'R';
        TX_Message[1] = __atomic_load_n(&octave,__ATOMIC_RELAXED);
        TX_Message[2] = index;
    }
    xQueueSend( msgOutQ, TX_Message, portMAX_DELAY);
    std::copy(TX_Message, TX_Message + 8, globalTX_Message); 
} 





void CAN_RX_ISR(void) {
	uint8_t RX_Message_ISR[8];
	uint32_t ID;
	CAN_RX(ID, RX_Message_ISR);
    // Serial.println(RX_Message_ISR[0]);
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
    CAN_RegisterRX_ISR(CAN_RX_ISR);
    CAN_RegisterTX_ISR(CAN_TX_ISR);
    CAN_Start();
}


