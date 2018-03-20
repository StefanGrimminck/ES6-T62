#include "i2c_develop.h"

#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_ADDRESSES 64

int open_dev(char* filename, uint8_t i2c_address){
    int i2c_file = open(filename, O_RDWR);
    if(i2c_file < 0){
        printf("Could not open the file, error: %s \n", strerror(errno));
        return -1;
    }
    printf("Opened file: %s bytes: %i, file descriptor: %i \n", filename, i2c_file, i2c_file);

    if(ioctl(i2c_file, I2C_SLAVE, (__u16)i2c_address) < 0){
        printf("Could not use the i2c bus, error: %s \n", strerror(errno));
        close(i2c_file);
        return -1;
    }
    return i2c_file;
}

int close_dev(int file_pointer){
    close(file_pointer);
}

int write_buf(uint8_t i2c_address, char* filename, uint8_t* buf, int count){
    int file = open_dev(filename, i2c_address);
    if(file == -1){
        return -1;
    }

     if(write(file, buf, count) != count){
            /* something went wrong with communication to the bus */
            printf("Error while writing to the i2c bus \n");
            close_dev(file);
            return -1;
        }

        printf("Written to i2c \n");
    close_dev(file);
    return count;
}

int read_i2c(uint8_t i2c_address, char* filename, uint8_t* buf, int count, uint8_t read_from){
    int file = open_dev(filename, i2c_address);
    if(file == -1){
        return -1;
    }

     if(write(file, &read_from, 1) != 1){
            /* something went wrong when sending the registers we want to read */
            printf("Error while writing to i2c bus 2\n");
            close_dev(file);
            return -1;
        }

        if(count > MAX_ADDRESSES){
            printf("Cannot read more than: %i addresses \n", MAX_ADDRESSES);
            close_dev(file);
            return -1;
        }

        uint8_t read_buf[count];
        if(read(file, buf, count) != count){
            printf("Error while reading from i2c bus \n");
            close_dev(file);
            return -1;
        }
    
    close_dev(file);
    return count;
}
