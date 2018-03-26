#include "led_controller.h"
#include "i2c_develop.h"
#include <stdio.h>

/* Function to set the mode of a single led (on/off/pwm1/pwm2) */
int SetSingleLed(int ledNum, int mode){
    if(ledNum < 0 || ledNum > 7){
        printf("Wrong led number");
        return -1;
    }
    uint8_t currentStatus;
    uint8_t buf[2];

            if(ledNum <= 3){
                read_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, &currentStatus, 1, LS2);
                printf("before: %x \n", currentStatus);
                if (mode == LED_ON || mode == LED_PWM1 || mode == LED_PWM2)
                {
                    currentStatus |= SET_LED(mode, ledNum);
                }else if (mode == LED_OFF)
                {
                    currentStatus &= ~SET_LED_OFF(ledNum);
                }
                buf[0] = LS2;
                buf[1] = currentStatus;
                printf("after: %x \n", currentStatus);
                write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
            }
            else{
                ledNum = ledNum - 4;
                read_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, &currentStatus, 1, LS3);
                printf("before: %x \n", currentStatus);
                if (mode == LED_ON)
                {
                    currentStatus |= SET_LED(mode, ledNum);
                }else if (mode == LED_OFF)
                {
                    currentStatus &= ~SET_LED_OFF(ledNum);
                }
                buf[0] = LS3;
                buf[1] = currentStatus;
                printf("after: %x \n", currentStatus);
                write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
            }
}

/* Function to set the mode of multiple leds (on/off/pwm1/pwm2)*/
int SetMultipleLeds(int* ledNums, int mode){
    // TODO: go through list, and call the SetSingleLed function

}

/* Function to set the PWM0 and PSC0 registers. dutycycle = 0 <> 100 */
int SetPWM(int dutyCycle){
    if(dutyCycle < 0 || dutyCycle > 100 ){
        printf("dutyCycle should bee between 0 and 100");
        return -1;
    }
    
    uint8_t buf[3];
    buf[0] = PSC0;
    buf[1] = PSC_DIM;
    buf[2] = map(dutyCycle, 0, 100, 0, 255);
    write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 3);
    return 0;
}

/* Function to set the PWM1 and PSC1 registers speed = 1hz <> 50hz */
int SetBlink(int speed){
    uint8_t buf[3];

    /* calculating the value from the input Hz */
    double secs = 1 / (double)speed;
    double val = (secs*152) - 1;

    buf[0] = PSC1;
    buf[1] = (int)val;
    buf[2] = PWM_BLINK;
    write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 3);

}

/* From Arduino */
uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}