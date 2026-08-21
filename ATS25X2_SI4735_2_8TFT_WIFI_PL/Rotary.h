// Rotary encoder library for Arduino.
#include "Arduino.h"

#ifndef rotary_h
#define rotary_h

// Enable this to emit codes twice per step
// #define HALF_STEP

#define ENABLE_PULLUPS  // Enable weak pullups

// Values returned by 'process'
#define DIR_NONE 0x0    // No complete step yet
#define DIR_CW   0x10   // Clockwise step
#define DIR_CCW  0x20   // Anti-clockwise step

class Rotary
{
  public:
    Rotary(uint8_t, uint8_t);
    // Process pin(s) - wywolywane z ISR, dlatego IRAM_ATTR (bezpieczne przy
    // rownoczesnych operacjach na flash - OTA, EEPROM, serwer WWW)
    unsigned char IRAM_ATTR process();
  private:
    unsigned char state;
    unsigned char pin1;
    unsigned char pin2;
};
#endif
