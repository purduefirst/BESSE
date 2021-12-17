#define SBUS_BAUD 100000
#define SBUS_SERIAL Serial1
#define DEBUG 1
#define NUM_LED 25
#define LED_PIN 6


#include "Adafruit_NeoPXL8.h"

static int channels[18];

int8_t ledPins[8] = { 30, 31, 32, 33, 15, 14, 27, 26 };
Adafruit_NeoPXL8 leds(NUM_LED, ledPins, NEO_GRB);

void debugPrint(const char *msg)
{
    #ifdef DEBUG
        Serial.println(msg);
    #else
    #endif
}

void checkSBUS()
{
    static uint8_t buffer[25];
    static uint8_t errors = 0;
    static bool failsafe = 0;
    static uint8_t idx;
    static uint8_t lost = 0;
    uint8_t b;
    uint8_t i;
    //word results;

    if (Serial1.available()) {
        b = Serial1.read();

        // Wait for start byte (MSB so 11110000 = 0x0F)
        if (idx == 0 && b != 0x0F) { 
            // Do nothing
        }
        else {
            // Read until end byte is received
            buffer[idx++] = b; 
        }

        // Check that all 25 bytes have been read
        if (idx == 25) {
            idx = 0;

            // Check that there is a stop byte
            if (buffer[24] != 0x00) {
                errors++;
            }
            else {
                // Decode SBUS packet
                // 25 byte packet received is little endian so we have to swap some bytes.
                channels[1] = ((buffer[1] | buffer[2] << 8) & 0x07FF);
                channels[2] = ((buffer[2] >> 3 | buffer[3] << 5) & 0x07FF);
                channels[3] = ((buffer[3] >> 6 | buffer[4] << 2 | buffer[5] << 10) & 0x07FF);
                channels[4] = ((buffer[5] >> 1 | buffer[6] << 7) & 0x07FF);

                channels[5] = ((buffer[6] >> 4 | buffer[7] << 4) & 0x07FF);
                channels[6] = ((buffer[7] >> 7 | buffer[8] << 1 | buffer[9] << 9) & 0x07FF);
                channels[7] = ((buffer[9] >> 2 | buffer[10] << 6) & 0x07FF);
                channels[8] = ((buffer[10] >> 5 | buffer[11] << 3) & 0x07FF);

                channels[9] = ((buffer[12] | buffer[13] << 8) & 0x07FF);
                channels[10] = ((buffer[13] >> 3 | buffer[14] << 5) & 0x07FF);
                channels[11] = ((buffer[14] >> 6 | buffer[15] << 2 | buffer[16] << 10) & 0x07FF);
                channels[12] = ((buffer[16] >> 1 | buffer[17] << 7) & 0x07FF);

                channels[13] = ((buffer[17] >> 4 | buffer[18] << 4) & 0x07FF);
                channels[14] = ((buffer[18] >> 7 | buffer[19] << 1 | buffer[20] << 9) & 0x07FF);
                channels[15] = ((buffer[20] >> 2 | buffer[21] << 6) & 0x07FF);
                channels[16] = ((buffer[21] >> 5 | buffer[22] << 3) & 0x07FF);

                channels[17] = ((buffer[23]) & 0x0001) ? 2047 : 0;
                channels[18] = ((buffer[23] >> 1) & 0x0001) ? 2047 : 0;

                // Check for failsafe from receiver
                failsafe = ((buffer[23] >> 3) & 0x0001) ? 1 : 0;

                // Handle incomplete packages
                if ((buffer[23] >> 2) & 0x0001)
                    lost++;
            }
        }
    }
}

void setup()
{
    // GPIO Direction

  #ifdef DEBUG
      // Initialize Debug Serial (USB)
      Serial.begin(115200);
      //while(!Serial.available()){}
      debugPrint("Debug Serial Initialized!");
      Serial.println("Sent to Debug Print");
  #else
  #endif

    // Initialize SBUS (UART0)
    SBUS_SERIAL.begin(SBUS_BAUD, SERIAL_8E2);
    while (!SBUS_SERIAL.available())
        ;
    debugPrint("SBUS (UART0) Initialized!");
}

uint16_t loopCount = 0;

void loop()
{
    char msg [255];
    checkSBUS();

    if(loopCount == 25535){

        sprintf(msg, "CH1: %d    CH2: %d    CH3: %d    CH4: %d    CH5: %d    CH6: %d    CH7: %d    CH8: %d    CH9: %d    CH10: %d    CH11: %d    CH12: %d    CH13: %d    CH14: %d    CH15: %d    CH16: %d\r\n", channels[1], 
            channels[2], channels[3], channels[4], channels[5], channels[6], channels[7], channels[8], channels[9],
            channels[10], channels[11], channels[12], channels[13], channels[14], channels[15], channels[16]);
        Serial.print(msg);
    }

    loopCount++;
}

uint8_t frame = 0;

void setup1(){
    // Initialize LED strip
    leds.begin();
    leds.setBrightness(32);



}


void loop1(){
    // Test LED strip
    for(uint8_t r=0; r<8; r++) { // For each row...
        for(int p=0; p<NUM_LED; p++) { // For each pixel of row...
            leds.setPixelColor(r * NUM_LED + p, rain(r, p));
        }
    }
    leds.show();
    frame++;


    
}






uint8_t colors[8][3] = { // RGB colors for the 8 rows...
  255,   0,   0, // Row 0: Red
  255, 160,   0, // Row 1: Orange
  255, 255,   0, // Row 2: Yellow
    0, 255,   0, // Row 3: Green
    0, 255, 255, // Row 4: Cyan
    0,   0, 255, // Row 5: Blue
  192,   0, 255, // Row 6: Purple
  255,   0, 255  // Row 7: Magenta
};

// Gamma-correction table improves the appearance of midrange colors
#define _GAMMA_ 2.6
const int _GBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _G1_ (uint8_t)(pow((__COUNTER__ - _GBASE_) / 255.0, _GAMMA_) * 255.0 + 0.5),
#define _G2_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ // Expands to 8 lines
#define _G3_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ // Expands to 64 lines
const uint8_t gamma8[] = { _G3_ _G3_ _G3_ _G3_ };    // 256 lines

// Given row number (0-7) and pixel number along row (0 - (NUM_LED-1)),
// first calculate brightness (b) of pixel, then multiply row color by
// this and run it through gamma-correction table.
uint32_t rain(uint8_t row, int pixelNum) {
  uint16_t b = 256 - ((frame - row * 32 + pixelNum * 256 / NUM_LED) & 0xFF);
  return ((uint32_t)gamma8[(colors[row][0] * b) >> 8] << 16) |
         ((uint32_t)gamma8[(colors[row][1] * b) >> 8] <<  8) |
                    gamma8[(colors[row][2] * b) >> 8];
}