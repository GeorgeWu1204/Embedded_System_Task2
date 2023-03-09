# include "reorganize.h"

uint8_t HS_TX_Message[8] = {0};

void sendHandshakeMessage(int8_t msg0, uint8_t msg1, uint8_t msg2){
    HS_TX_Message[0] = msg0;
    HS_TX_Message[1] = msg1;
    HS_TX_Message[2] = msg2;
    xQueueSend( msgOutQ, HS_TX_Message, portMAX_DELAY);
    std::copy(HS_TX_Message, HS_TX_Message + 8, globalTX_Message); 
}

uint8_t getHashedID(){
    uint32_t uid_w0, uid_w1, uid_w2;
    
    // Get the UID words
    uid_w0 = HAL_GetUIDw0();
    uid_w1 = HAL_GetUIDw1();
    uid_w2 = HAL_GetUIDw2();

    std::hash<uint32_t> hasher;
    std::uint8_t hash0 = hasher(uid_w0) % 256;
    std::uint8_t hash1 = hasher(uid_w1) % 256;
    std::uint8_t hash2 = hasher(uid_w2) % 256;
    
    std::uint8_t hash = static_cast<std::uint8_t>(hash0 ^ hash1 ^ hash2);
    return hash;
}

void reorganizePositions(void *pvParameters){
    Serial.println("in reorganisePositions task");
    position_table.empty();
    uint8_t position;
    uint8_t reorganizeKeyArray[7];
    xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
    std::copy(keyArray,keyArray+7,reorganizeKeyArray);
    xSemaphoreGive(keyArrayMutex);
    Serial.println(!((reorganizeKeyArray[5] >> 3) & 1) == 1);
    while (!((reorganizeKeyArray[5] >> 3) & 1) == 1){
        // wait until the left side signal is off
    }
    if (position_table.empty()){
        position = 1;
    }
    else{
        position = position_table.size()+1;
    }
    Serial.print("position: ");
    Serial.println(position);
    Serial.print("ID: ");
    Serial.println(ownID);
    
    position_table[ownID] = position;
    if (!(position == 1 && (((reorganizeKeyArray[6] >> 3) & 1) == 1))){ // if not the case that there is only one keyboard
        Serial.println("line55");
        sendHandshakeMessage('L',position,ownID);
        // turn myself signal off
        if (((reorganizeKeyArray[6] >> 3) & 1) == 1){
            // if this is the last one
            sendHandshakeMessage('E',0,0);
        }  
        else{
            outBits[5] = false;
            outBits[6] = false;
        }
        // variable 'reorganising' will be set to false outside
    }
    else{ // single board
        octave = 4;
        reorganising = false;  // this is set to false here for single board
    }
    vTaskDelete(reorganizeHandle);
}

    