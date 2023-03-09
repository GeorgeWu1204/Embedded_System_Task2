#include "display.h"

// Display driver object
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Function to set outputs using key matrix
void setOutMuxBit(const uint8_t bitIdx, const bool value)
{
  digitalWrite(REN_PIN, LOW);
  digitalWrite(RA0_PIN, bitIdx & 0x01);
  digitalWrite(RA1_PIN, bitIdx & 0x02);
  digitalWrite(RA2_PIN, bitIdx & 0x04);
  digitalWrite(OUT_PIN, value);
  digitalWrite(REN_PIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(REN_PIN, LOW);
}


void displayUpdateTask(void *pvParameters)
{
    const TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t output;
    uint8_t key_index;
    const char *output_key;
    uint8_t localkeyArray[7];

    // Update display
    while (1)
    {
        # ifndef TEST_DISPLAY
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        # endif
        
        u8g2.clearBuffer();                 // clear the internal memory
        u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
        xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
        std::copy(keyArray,keyArray+7,localkeyArray);
        xSemaphoreGive(keyArrayMutex);
        output = localkeyArray[2] << 8 | localkeyArray[1] << 4 | localkeyArray[0];
        key_index = 0;
        for (int g = 0; g < 12; g++){
        if (((output >> g) & 1) == 0)
        {
            key_index = g + 1;
        }
        }
    
        u8g2.setCursor(2,30);
        u8g2.print((char) globalTX_Message[0]);
        u8g2.print(globalTX_Message[1]);
        u8g2.print(globalTX_Message[2]);

        u8g2.setCursor(50,30);
        u8g2.print((char) globalRX_Message[0]);
        u8g2.print(globalRX_Message[1]);
        u8g2.print(globalRX_Message[2]);
 

        u8g2.setCursor(98,30);
        u8g2.print("O");
        u8g2.print(octave);
        u8g2.sendBuffer();

        //Toggle LED
        digitalToggle(LED_BUILTIN);
        # ifdef TEST_DISPLAY
        break;
        #endif
    }
}

void initialiseDisplay(){
    setOutMuxBit(DRST_BIT, LOW); // Assert display logic reset
    delayMicroseconds(2);
    setOutMuxBit(DRST_BIT, HIGH); // Release display logic reset
    u8g2.begin();
    setOutMuxBit(DEN_BIT, HIGH); // Enable display power supply
}