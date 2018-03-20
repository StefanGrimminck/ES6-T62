#include "led_controller.h"
#include "PCA9532.h"
#include "i2c_develop.h"

/* Function to set the mode of a single led (on/off/pwm1/pwm2) */
int SetSingleLed(int ledNum, bool on){
    // TODO: first read the current status of the register for the led we want

    // TODO: do some bit magic to set or unset the led

    // TODO: write the value to the bus
}

/* Function to set the mode of multiple leds (on/off/pwm1/pwm2)*/
int SetMultipleLeds(int* ledNums, bool on){
    // TODO: go through list, and call the SetSingleLed function
}

/* Function to set the PWM0 and PSC0 registers */
int SetPWM(int dutyCycle){
    // TODO: calculate the correct values for the PWM0 and PSC0 registers

    // TODO: write the values to the PWM0 and PSC0 registers
}

/* Function to set the PWM1 and PSC1 registers */
int SetBlink(int speed){
    // TODO: calculate the correct values for the PWM1 and PSC1 registers

    // TODO: write the values to the PWM1 and PSC1 registers
}
