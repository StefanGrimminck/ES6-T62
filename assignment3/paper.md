# Controlling the I²C bus on the LPC3250
Stefan Grimminck & Skip Geldens
T62

## Part 1 
We set out to make an user space program for a hardware engineer that can write registers to the  I²C bus.
The Hardware Engineer is mainly focused what he can see on this oscilloscope. So he wants to have a
program where he can enter the same bytes as he expects to see on the scope.

The LPC3250 has various I²C busses and on one I²C bus an io-expanderPCA9532 is attached. This expander is used to control LED 1 - LED 8 on the LPC3250

First we need to parse the input of the Engineer sothat we can write / read specific data to the I²C bus. 
We start with reading the input of the user and checking for the following format:

`i2c_develop <r/w> <i2c address> <device address> <value>`

After we validated the users input we'll parse the I²C and device address as an integral number of base 16.
```c
    unsigned long i2c_address = strtoul(argv[2], NULL, 16);
    printf("i2c_address: %lu \n", i2c_address);
    unsigned long base_address = strtoul(argv[3], NULL, 16);
    printf("base_address: %lu \n", base_address);

```

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
