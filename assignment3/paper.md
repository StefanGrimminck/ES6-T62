# Controlling the I²C bus on the LPC3250
Stefan Grimminck & Skip Geldens
T62

## Part 1 
We set out to make an user space program for a hardware engineer that can write registers to the  I²C bus.
The Hardware Engineer is mainly focused what he can see on this oscilloscope. So he wants to have a
program where he can enter the same bytes as he expects to see on the scope.


First we need to parse the input of the Engineer sothat we can write / read specific data to the I²C bus. 
We start with reading the input of the user and checking for the following format:

`i2c_develop <r/w> <i2c address> <device address> <value>`

After we validated the users input we'll parse the I²C and device address as an integral number of base 16.
This parsing is done with the following lines of code:
```c
    unsigned long i2c_address = strtoul(argv[2], NULL, 16);
    printf("i2c_address: %lu \n", i2c_address);
    unsigned long base_address = strtoul(argv[3], NULL, 16);
    printf("base_address: %lu \n", base_address);

```
Now we have an `i2c address`, a `base_address` and a r/w option. These parameters are used to populate 'i2c_buf) where its contents is used as parameters in the following funtions:

IF the user specifies he wants to write to a certain address the following function is called:
```c
write_buf(i2c_address, I2CFILENAME, i2c_buf, amount_read) == -1)
```

If the user specifies he wants to read from a certain adress the read functions is called:
```c
read_i2c(i2c_address, I2CFILENAME, read_buf, i2c_buf[1], i2c_buf[0]
```

### Writing values to the I2C Bus ###

The function that is used to write values goes as follows:
```c
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
```

This function calles 'int open_dev(char* filename, uint8_t i2c_address)' to open `I2CFILENAME` specified in main.c, which is `/dev/i2c-0` in our case. Than we use the `ioctl()` function to manipulate the underlying device parameters and write data to the I2C slave. After this operation the '/dev/i2c-0' file is closed.

```c
if(ioctl(i2c_file, I2C_SLAVE, (__u16)i2c_address) < 0){
        printf("Could not use the i2c bus, error: %s \n", strerror(errno));
        close(i2c_file);
        return -1;
    }
    ```
    






### Reading values from the I2C Bus ###


Now we open the i2c master device and test if the operation has been succesful, if not display the errormessage to the user.
```c
 int i2c_file = open(I2CFILENAME, O_RDWR);
    if(i2c_file < 0){
        printf("Could not open the file, error: %s \n", strerror(errno));
        return -1;
    }
```
Following the last operation, we need to select the right slave for our master to talk to. We do this by executing
`ioctl(i2c_file, I2C_SLAVE, (__u16)i2c_address)`
and checking for it's output.

Depending on the mode the user selected we can read from or write to this address.

## Part 2
