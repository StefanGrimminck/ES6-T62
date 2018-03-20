/*
 * i2c user space program to read or write
 * bytes to devices. 
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

    uint8_t i2c_buf[MAX_ADDRESSES + 1];
    int i = 0;
    i2c_buf[0] = (uint8_t)base_address;

    for(i; i < (argc - 4); i++){
        i2c_buf[i+1] = (uint8_t)strtoul(argv[i+4], NULL, 16);
        printf("register %i = %x \n", i, i2c_buf[i+1]);
    }
    int amount_read = i+1;

    if(mode == 'w'){
        if(write_buf(i2c_address, I2CFILENAME, i2c_buf, amount_read) == -1){
            return -1;
        }
        return 0;
    }
    else if(mode == 'r'){
        uint8_t read_buf[amount_read];
        if(read_i2c(i2c_address, I2CFILENAME, read_buf, i2c_buf[1], i2c_buf[0]) == -1){
            return -1;
        }

        int i = 0;
        for(i; i < i2c_buf[1]; i++){
            printf("Value: %x \n", read_buf[i]);
        }

        return 0;
    }
    else{
        /* do nothing */
    }

    return 0;
}