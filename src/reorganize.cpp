# include "scan.h"
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

void reorganizePositions(){
    std::map<uint8_t, uint8_t> position_table;
    uint8_t position;    
    uint8_t westeastArray[2] = {0,0};
    uint8_t localRX_Message[8];

    for (uint8_t i = 0; i < 2; i++){
        setRow(i+5);
        digitalWrite(OUT_PIN, true); //output handshake signal
        digitalWrite(REN_PIN, 1);
        delayMicroseconds(3);
        westeastArray[i] = digitalRead(C3_PIN);
        digitalWrite(REN_PIN, 0);
        Serial.println(westeastArray[i]);
    }

    // FIRST KEYBOARD
    if (westeastArray[0] == 1){ // if I am the most west keyboard
        if (westeastArray[1] == 1){ // if I am the single keyboard
            previous_west = 0;
            previous_east = 0;
            
            position_table[ownID] = 1;
            octave = 4;
            Serial.println("previous:");
            Serial.println(previous_west);
            Serial.println(previous_east);

            Serial.print("Octave: ");
            Serial.println(octave);
            return;
        }
        else{
            Serial.println("SentL");
            delayMicroseconds(10);
            sendHandshakeMessage('L',1,ownID);
            // turn off myself
            for (uint8_t i = 5; i < 7; i++){
                setRow(i);
                digitalWrite(OUT_PIN, false); 
                delayMicroseconds(3);
            }   
        }
    }
    // MIDDLE & LAST KEYBOARD
    else{
        Serial.println("I am middle or last keyboard");
    
        while (westeastArray[0] == 0){ // if I am not the most west keyboard, wait until I am the most west keyboard
            // read west
            for (uint8_t i = 0; i < 2; i++){
                setRow(i+5);
                digitalWrite(OUT_PIN, true); //output handshake signal
                digitalWrite(REN_PIN, 1);
                delayMicroseconds(3);
                westeastArray[i] = digitalRead(C3_PIN);
                digitalWrite(REN_PIN, 0);
            }   
        }
        while (xQueueReceive(msgInQ, localRX_Message, 0) == pdTRUE){
            uint8_t first_message_bit = localRX_Message[0];
            if (first_message_bit == 'L'){
                Serial.println("received");
                position_table[localRX_Message[2]] = localRX_Message[1]; 
            }
        }
        position = position_table.size()+1;
        delayMicroseconds(10);
        sendHandshakeMessage('L',position,ownID);
        position_table[position] = ownID;
        // turn off myself
        for (uint8_t i = 5; i < 7; i++){
            setRow(i);
            digitalWrite(OUT_PIN, false); //output handshake signal
            delayMicroseconds(3);
        }   
        // LAST KEYBOARD
        if (westeastArray[1] == 1){
            previous_west = 1;
            previous_east = 0;
            // if this  the last one
            sendHandshakeMessage('E',0,0);
            // turn on myself
            for (uint8_t i = 5; i < 7; i++){
                setRow(i);
                digitalWrite(OUT_PIN, true); 
                delayMicroseconds(3);
            }   
            octave = 4+(position_table[ownID]-(position_table.size()+1)/2);
            previous_west = 1;
            previous_east = 0;
            Serial.print("Octave: ");
            Serial.println(octave);
            return;
        }
        else{
            // MIDDLE KEYBOARD
            previous_west = 1;
            previous_east = 1;
        }
    }
    // LISTEN AFTER SEND MSG FOR OTHER KEYBOARD INFO   

    
    while (xQueueReceive(msgInQ, localRX_Message, 0) == pdTRUE){
        
        uint8_t first_message_bit = localRX_Message[0];
        if (first_message_bit == 'L'){
            Serial.println("received");
            position_table[localRX_Message[2]] = localRX_Message[1]; 
        }
        else if (first_message_bit == 'E'){
            for (uint8_t i = 5; i < 7; i++){
                setRow(i);
                digitalWrite(OUT_PIN, true); //output handshake signal
                delayMicroseconds(3);
            }   
            octave = 4+(position_table[ownID]-(position_table.size()+1)/2);
            
            Serial.print("Octave: ");
            Serial.println(octave);
            return;
        }
    }
    Serial.print("Octave: ");
    Serial.println(octave);

    delayMicroseconds(100000000);
}