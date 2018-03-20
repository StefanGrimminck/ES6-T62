#ifndef LEDCONTROLLER
#define LEDCONTROLLER

#include "PCA9532.h"

#define NUM_LEDS 8
#define LED_CONTROLLER_ADDRESS 0x60
#define I2CFILENAME "/dev/i2c-0"

#define ON 1
#define OFF 2
#define DIM 3
#define BLINK 4

int SetSingleLed(int ledNum, int mode);
int SetMultipleLeds(int* ledNums, int mode);
int SetPWM(int dutyCycle);
int SetBlink(int speed);

#endif