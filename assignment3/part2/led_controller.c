#include "led_controller.h"
#include "i2c_develop.h"
#include <stdio.h>

/* Function to set the mode of a single led (on/off/pwm1/pwm2) */
int SetSingleLed(int ledNum, int mode){
    // TODO: first read the current status of the register for the led we want
    uint8_t currentStatus;
    read_i2c(LED_CONTROLLER_ADDRESS, I2CFILENAME, &currentStatus, 1, INPUT0);

    printf("output: %x", currentStatus);

    // TODO: do some bit magic to set or unset the led

    // TODO: write the value to the bus
}

/* Function to set the mode of multiple leds (on/off/pwm1/pwm2)*/
int SetMultipleLeds(int* ledNums, int mode){
    // TODO: go through list, and call the SetSingleLed function

}

/* Function to set the PWM0 and PSC0 registers. dutycycle = 0 <> 100 */
int SetPWM(int dutyCycle){
    // TODO: calculate the correct values for the PWM0 register
    uint8_t buf[2];
    buf[0] = PSC0;
    buf[1] = PSC_DIM;
    write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
    // TODO: write the values to the PWM0 and PSC0 registers
}

/* Function to set the PWM1 and PSC1 registers */
int SetBlink(int speed){
    // TODO: calculate the correct values for the PWM1 and PSC1 registers
    uint8_t buf[2];
    buf[0] = PWM1;
    buf[1] = PWM_BLINK;
    write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
    // TODO: write the values to the PWM1 and PSC1 registers
}
