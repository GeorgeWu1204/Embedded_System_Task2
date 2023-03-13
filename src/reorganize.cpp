# include "scan.h"
# include "reorganize.h"

// uint8_t HS_TX_Message[8] = {0};

// void sendHandshakeMessage(int8_t msg0, uint8_t msg1, uint8_t msg2){
//     HS_TX_Message[0] = msg0;
//     HS_TX_Message[1] = msg1;
//     HS_TX_Message[2] = msg2;
//     xQueueSend( msgOutQ, HS_TX_Message, portMAX_DELAY);
//     std::copy(HS_TX_Message, HS_TX_Message + 8, globalTX_Message); 
// }

// void suspendTasks(){
//     vTaskSuspend(scanKeysHandle);
//     vTaskSuspend(displayHandle);
//     vTaskSuspend(decodeHandle);
//     vTaskSuspend(writeToDoubleHandle);
//     vTaskSuspend(checkDiffHandle);
// }

// void resumeTasks(){
//     xTaskResume
// }

// void sendS(uint8_t msg0, uint8_t msg1, uint8_t msg2){
//     uint8_t HS_TX_Message[8] = {msg0, msg1, msg2, 0, 0, 0, 0};
//     // xQueueSend( msgOutQ, HS_TX_Message, portMAX_DELAY);
//     Serial.println(xQueueSend( msgOutQ, HS_TX_Message, 0) == pdTRUE);
//     std::copy(HS_TX_Message, HS_TX_Message + 8, globalTX_Message); 
// }

// void sendHandshakeMessage(uint8_t msg0, uint8_t msg1, uint8_t msg2){
//     uint8_t HS_TX_Message[8] = {msg0, msg1, msg2, 0, 0, 0, 0};
//     // xQueueSend( msgOutQ, HS_TX_Message, portMAX_DELAY);
//     // Serial.println(xQueueSend( msgOutQ, HS_TX_Message, 0) == pdTRUE);
    
//     // uint8_t msgOut[8];

//     // xQueueReceive(msgOutQ, msgOut, portMAX_DELAY);

//     xSemaphoreTake(CAN_TX_Semaphore, portMAX_DELAY);
//     CAN_TX(0x123, HS_TX_Message);
//     xSemaphoreGive(CAN_TX_Semaphore);

//     // std::copy(HS_TX_Message, HS_TX_Message + 8, globalTX_Message); 
// }

uint8_t getHashedID(){
    uint32_t uid_w0, uid_w1, uid_w2;
    
    // Get the UID words
    uid_w0 = HAL_GetUIDw0();
    uid_w1 = HAL_GetUIDw1();
    uid_w2 = HAL_GetUIDw2();
    Serial.println(uid_w0);
    Serial.println(uid_w1);
    Serial.println(uid_w2);

    std::hash<uint32_t> hasher;
    std::uint8_t hash = hasher(uid_w0) % 100;

    return hash;
}

void configTask(void *pvParameters){
    const TickType_t xFrequency = 200 / portTICK_PERIOD_MS;
    // xFrequency initiation interval of task set to 50ms
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint8_t localkeyArray[7];
    uint8_t west, east;
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        if (configFlag == true){
            Serial.println("inconfig");
            delayMicroseconds(100000);
            positionTable.clear();
            endConfigFlag = false;
            delayMicroseconds(100000);
            
            // wait until most West
            xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
            std::copy(keyArray,keyArray+7,localkeyArray);
            xSemaphoreGive(keyArrayMutex);
            // west = !(localkeyArray[5] >> 3);
            // east = !(localkeyArray[6] >> 3);
            // Serial.print("west");
            // Serial.println(west);
            // Serial.print("east");
            // Serial.println(east);
            west = 1;
            while (west == 1){ // if I am not the most west keyboard, wait until I am the most west keyboard
                xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
                std::copy(keyArray,keyArray+7,localkeyArray);
                xSemaphoreGive(keyArrayMutex);
                west = !(localkeyArray[5] >> 3);
                east = !(localkeyArray[6] >> 3);
            }
            Serial.println("Has become the most west");
            delayMicroseconds(100000);
            // for (const auto& pair : positionTable) {
            //     Serial.println(pair.first);
            // }
            position = positionTable.size()+1;
            positionTable[ownID] = position;
            sendMessage('L',ownID,position);
            Serial.print("position");
            Serial.println(position);
            if (east == 0){ // If I am the last one
                sendMessage('E',0,0);
                endConfigFlag = true;
            }
            else{
                delayMicroseconds(100000);
                outBits[6] = false;
            }
            while (endConfigFlag == false){
            }
            outBits[6] = true;
            delayMicroseconds(500000);
            xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
            std::copy(keyArray,keyArray+7,localkeyArray);
            xSemaphoreGive(keyArrayMutex);
            previous_west = !(localkeyArray[5] >> 3);
            previous_east = !(localkeyArray[6] >> 3);
            Serial.println(previous_west);
            Serial.println(previous_east);

            delayMicroseconds(100000);
            configFlag = false;
            delayMicroseconds(100000);
            Serial.print("tablesize:");
            Serial.println(positionTable.size());
            octave = 4+(positionTable[ownID]-(positionTable.size()+1)/2);
            Serial.print("octave:");
            Serial.println(octave);
        }
    }
} 

