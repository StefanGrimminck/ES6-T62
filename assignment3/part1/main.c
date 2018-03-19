/*
 * i2c user space program to read or write
 * bytes to devices. 
 * 
 * By: Stefan Grimminck & Skip Geldens
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

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

    /* open the file for the i2c device, and check if it worked */
    int i2c_file = open(I2CFILENAME, O_RDWR);
    if(i2c_file < 0){
        printf("Could not open the file, error: %s \n", strerror(errno));
        return -1;
    }
    printf("Opened file: %s bytes: %i, file descriptor: %i \n", I2CFILENAME, i2c_file, i2c_file);

    if(ioctl(i2c_file, I2C_SLAVE, (__u16)i2c_address) < 0){
        printf("Could not use the i2c bus, error: %s \n", strerror(errno));
        close(i2c_file);
        return -1;
    }

    if(mode == 'w'){
        if(write(i2c_file, i2c_buf, amount_read) != amount_read){
            /* something went wrong with communication to the bus */
            printf("Error while writing to the i2c bus \n");
            close(i2c_file);
            return -1;
        }

        printf("Written to i2c \n");
        close(i2c_file);
        return 0;
    }
    else if(mode == 'r'){
        uint8_t read_from = 0x18;
        if(write(i2c_file, &i2c_buf[0], 1) != 1){
            /* something went wrong when sending the registers we want to read */
            printf("Error while writing to i2c bus 2\n");
            close(i2c_file);
            return -1;
        }

        if(i2c_buf[1] > MAX_ADDRESSES){
            printf("Cannot read more than: %i addresses \n", MAX_ADDRESSES);
            close(i2c_file);
            return -1;
        }

        uint8_t read_buf[i2c_buf[1]];
        if(read(i2c_file, read_buf, i2c_buf[1]) != i2c_buf[1]){
            printf("Error while reading from i2c bus \n");
            close(i2c_file);
            return -1;
        }
        
        int i = 0;
        for(i; i < i2c_buf[1]; i++){
            printf("Value: %x \n", read_buf[i]);
        }
        
        close(i2c_file);
        return 0;
    }
    else{
        /* do nothing */
    }

    return 0;
}