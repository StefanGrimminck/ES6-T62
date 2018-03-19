/*
 * i2c user space program to read or write
 * bytes to devices. 
 * 
 * By: Stefan Grimminck & Skip Geldens
 * 
 */

#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_ADDRESSES 64
#define I2CFILENAME "/dev/i2c-0"

int main( int argc, const char* argv[] )
{
    /* Reading arguments */
    if(argc < 4 || argc > (MAX_ADDRESSES + 3)){
        printf("Not enough arguments, usage: i2c_develop <i2c address> <device address> <value> \n");
        return -1;
    }

    unsigned long i2c_address = strtoul(argv[1], NULL, 16);
    unsigned long base_address = strtoul(argv[2], NULL, 16);
    unsigned long values[MAX_ADDRESSES];
    int i = 0;
    for(i; i < (argc - 3); i++){
        values[i] = strtoul(argv[i+3], NULL, 16);
        printf("register %i = %lu \n", i+1, values[i]);
    }
    int amount_read = i;

    /* open the file for the i2c device, and check if it worked */
    int i2c_file = open(I2CFILENAME, O_RDWR);
    if(i2c_file < 0){
        printf("Could not open the file, error: %s \n", strerror(errno));
        return -1;
    }
    
    if(ioctl(i2c_file, I2C_SLAVE, i2c_address) < 0){
        printf("Could not use the i2c bus, error: %s \n", strerror(errno));
        return -1;
    }


    return 0;
}