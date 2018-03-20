#ifndef LEDCONTROLLER
#define LEDCONTROLLER

#define NUM_LEDS 8
#define LED_CONTROLLER_ADDRESS 0x60
#define I2CFILENAME "/dev/i2c-0"

int SetSingleLed(int ledNum, bool on);
int SetMultipleLeds(int* ledNums, bool on);
int SetPWM(int ledNum, int dutyCycle);
int SetBlink(int ledNum, int speed);

#endif