/*
 * user space program for application developer to 
 * talk to leds on the LPC3250 board.
 * 
 * By: Stefan Grimminck & Skip Geldens
 * 
 */

#include <stdio.h>
#include <stdint.h>

#include "led_controller.h"

#define MAX_ADDRESSES 64
#define I2CFILENAME "/dev/i2c-0"

int main( int argc, const char* argv[] )
{
   //SetSingleLed(0, LED_PWM1);
   SetBlink(2);
   SetPWM(100);
   //SetSingleLed(2, LED_ON);
}