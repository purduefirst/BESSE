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
