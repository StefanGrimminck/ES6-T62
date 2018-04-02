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
Now we have an `i2c address`, a `base_address` and a r/w option. These parameters are used to populate 'i2c_buf`. 
Where i2c_buf[0] is our base address and other spaces in the buffer are populated with:
```c
for(i; i < (argc - 4); i++){
        i2c_buf[i+1] = (uint8_t)strtoul(argv[i+4], NULL, 16);
        printf("register %i = %x \n", i, i2c_buf[i+1]);
    }
```
TODO: EXPLAIN WHAT HAPPENS HERE!

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

This function calles 'int open_dev(char* filename, uint8_t i2c_address)' to open `I2CFILENAME` specified in main.c, which is `/dev/i2c-0` in our case. Than we use the `ioctl()` function to manipulate the underlying device parameters and write data to the I2C slave. 
Now we return back to the `write_i2c()' function and use  `write()` to write our i2c_buf to the bus.
After this operation the '/dev/i2c-0' file is closed.

```c
if(ioctl(i2c_file, I2C_SLAVE, (__u16)i2c_address) < 0){
        printf("Could not use the i2c bus, error: %s \n", strerror(errno));
        close(i2c_file);
        return -1;
    }
```
    

### Reading values from the I2C Bus ###

The implemententation ofcourse looks different the writing function but will also utilze `open_dev(char* filename, uint8_t i2c_address)` for the file handeling.

```c
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

        if(buf[1] > MAX_ADDRESSES){
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
```
We first open the `/dev/i2c-0` file and write the base address. We do this as follows: `write(file, &read_from, 1)`.
Than we call the `read()` function, which we use the read from the BUS.
We do this with:
```c
 if(read(file, buf, count) != count){
            printf("Error while reading from i2c bus \n");
            close_dev(file);
            return -1;
        }
```
 Where `file` is the file descriptor for '/dev/i2c-0', buf is the buffer that the read data is written to and count is the amount bytes that need to be read. After this operation the buffer contents is displayed to the user and the file is closed.
 
 
## Part 2

In this part of the assignment we write a user space program for our hardware engineer to conrtol the eight LEDs on the LPC2350 board. We will also create a simple CLI for the engineer to use where he or she can control the leds without needing the knowlage of the internal registers. Part 2 is built on the code of Part 1 and will use the `i2c_develop.c`	and `i2c_develop.h` again to communicate over the bus.

Because our hardware engineer doens't need knowlage about internal registers, we'll define them for him / her. We do this in `PCA9532.h`. 

```c
#define INPUT0  0x10
#define PSC0    0x12
#define PWM0    0x13
#define PSC1    0x14
#define PWM1    0x15
#define LS2     0x18
#define LS3     0x19

/* Led modes */
#define LED_ON      0x01
#define LED_OFF     0x00
#define LED_PWM1    0x02
#define LED_PWM2    0x03

/* Macro's to set a led on a mode */
#define SET_LED(mode, num) (mode << (num*2))
#define SET_LED_OFF(num) (0x03 << (num*2))


#define PSC_DIM 0x01
#define PWM_BLINK 0x80
```
We also define two Marco's to set the LEDs ON, OFF, in Blink mode or in dimm mode. Their functionality will be speciefd below.


First we start with `main.c`. This is where the CLI lives and calles the functions specified in 'led_controller.c'. These are:
```c
int SetSingleLed(int ledNum, int mode);
int SetPWM(int dutyCycle);
int SetBlink(int speed);
```

The `SetSingleLed()` function is as its name implies where we'll set our lets in the differt 'LED modes" specified in `PCA9532.h`.
```c
/* Function to set the mode of a single led (on/off/pwm1/pwm2) */
int SetSingleLed(int ledNum, int mode){
    if(ledNum < 0 || ledNum > 7){
        printf("Wrong led number");
        return -1;
    }
    uint8_t currentStatus;
    uint8_t buf[2];

            if(ledNum <= 3){
                if (read_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, &currentStatus, 1, LS2))
                {
			if (mode == LED_ON || mode == LED_PWM1 || mode == LED_PWM2)
			{
				currentStatus &= ~SET_LED_OFF(ledNum);
				currentStatus |= SET_LED(mode, ledNum);
			}else if (mode == LED_OFF)
			{
				currentStatus &= ~SET_LED_OFF(ledNum);
			}
			buf[0] = LS2;
			buf[1] = currentStatus;
			write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
			return write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
		}	
            }
            else{
                ledNum = ledNum - 4;
                if(read_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, &currentStatus, 1, LS3))
                {
			if (mode == LED_ON || mode == LED_PWM1 || mode == LED_PWM2)
			{
				currentStatus &= ~SET_LED_OFF(ledNum);
				currentStatus |= SET_LED(mode, ledNum);
			}else if (mode == LED_OFF)
			{
				currentStatus &= ~SET_LED_OFF(ledNum);
			}
			buf[0] = LS3;
			buf[1] = currentStatus;

			return write_buf(LED_CONTROLLER_ADDRESS, I2CFILENAME, buf, 2);
		}
            }
            
            return -1;
}
```

As you can see, the function consists of a big if-else statement. This is done to make sure LS2 is used for LED [1 - 4] and LS3 for LED [5 - 8].
These registers are found in the datasheet of the PCA9532 on page 6

![Contol register defenition](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment3/part2/PCA9532_LS.PNG?raw=true)


In both cases we first read the LED controller register with the `read_buf` function, wich we covered in part 1. 

To set the LEDS in the right mode we use Table 10 in the datahseet as a reference for our bitmanipulation. As seen below we'll conrtoll LED [8 - 15] wich correspond with LED [1 - 8] on the LPC board.


![LS0 to LS3 - LED selector regsiters](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment3/part2/LS_selector_registers.PNG?raw=true)







## Testing ##
https://youtu.be/XaHJRevon5Q



