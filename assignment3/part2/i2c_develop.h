#ifndef I2CDEVELOP
#define I2CDEVELOP

#include <stdint.h>

int open_dev(char* filename, uint8_t i2c_address);
int close_dev(int file_handle);
int write_buf(uint8_t i2c_address, char* filename, uint8_t* buf, int count);
int read_buf(uint8_t i2c_address, char* filename, uint8_t* buf, int count, uint8_t read_from);
#endif