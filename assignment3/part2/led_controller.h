#ifndef LEDCONTROLLER
#define LEDCONTROLLER

#include "PCA9532.h"
#include <stdint.h>

#define NUM_LEDS 8
#define LED_CONTROLLER_ADDRESS 0x60
#define I2CFILENAME "/dev/i2c-0"

int SetSingleLed(int ledNum, int mode);
int SetPWM(int dutyCycle);
int SetBlink(int speed);


uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);

#endif
