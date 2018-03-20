/*
 * user space program for application developer to 
 * talk to leds on the LPC3250 board.
 * 
 * By: Stefan Grimminck & Skip Geldens
 * 
 */

#include <stdio.h>
#include <stdint.h>

#include "i2c_develop.h"

#define MAX_ADDRESSES 64
#define I2CFILENAME "/dev/i2c-0"

int main( int argc, const char* argv[] )
{
    /* Reading arguments */
    if(argc < 4 || argc > (MAX_ADDRESSES + 3)){
        printf("Wrong amount of arguments, usage: i2c_develop <r/w> <i2c address> <device address> <value> \n");
        return -1;
    }
    printf("nr of arguments: %i \n", argc);

    char mode = *argv[1];
    if(mode != 'w' && mode != 'r'){
        printf("Wrong mode specified, r or w \n");
        return -1;
    }

    printf("mode: %c \n", mode);
    unsigned long i2c_address = strtoul(argv[2], NULL, 16);
    printf("i2c_address: %lu \n", i2c_address);
    unsigned long base_address = strtoul(argv[3], NULL, 16);
    printf("base_address: %lu \n", base_address);
}