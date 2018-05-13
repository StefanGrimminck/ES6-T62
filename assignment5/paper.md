# Controlling the GPIO pins on the LPC3250
Stefan Grimminck & Skip Geldens
T62

## Part 1: Discovery & reading joystick values
We set out to make a kernel module for customer that wants to set the GPIO state, write values to the GPIO pins and read them.
The customer doens't want to work with the registers itself, so our program is going to map the pins located on the board to the corresponding registers

First, we start with connecting the joystick pins to the pins on the jumpers on the board to see where they're connected to:

![Checking Joystick Pins](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment5/images/Checking_Joystick_Pins.jpg)

As you can see in the image, the resistance between the joystick pin and the header pin is +/- 470 ohm, which corresponds with the following chart on sheet 4/8 of the "LPC3250 OEM Board User Guide":
![5-key joystick switch](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment5/images/5-key%20joystick%20switch.png)

The mapping of the ports are visualised in the diagram below:
![expansion headers](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment5/images/Expansion_Connectors.png)

Now we can determen that:

Port	| pin | jumper | Joystick state 
---------|-----------|-----------|-----------
P2.26-MAT3.0		|57 | J3 | R  UP
P2.27-MAT3.1		|49	| J3 | R  DOWN
P2.25-CKE-1		  |48	| J3 | L  UP
P2.23    		    |56 | J3 | L  DOWN
P2.22 		      |47	| J3 | PUSH

Now we know to where the joystick output is mapped to we can read the corresponding registers. According to Table 662 (Port 2 Multiplexer Set Register) in the UM10326 datasheet we need to set bet 3 to tell the multiplexer we want to use pins 31:19 as GPIO. Now that the pins can be used as GPIO, we need to set the P2_DIR_CLR register so that the GPIO pins are set as input. After we've done that we can read trey values from the P2_INP_STATE register:

## Part 2: Creating a kernel module for reading and writing from and to the GPIO pins
The LPC3250 has GPIO aswel as GPO and GPI port. The program we created only uses the GPIO pins. These pins are described in Table 613, 614 and 615 of the UM10326 datahsheet:

![GPIO on LPC3250](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment5/images/LPC3250_GPIO.png)

Port | GPIO Pins
-----|-----
P0   | P0.0 - P0.7
P1   | P1.0 - P1.23
P2   | P2.0 - P.12

These port have to be mapped with the corresponding input, output & direction registers. In our code we've done this by using a structure named Pinfo;
```c
typedef struct Pinformation{
	int pin;
	int jumper;
	struct DIR dir;
	struct OUTP output;
	struct INP input;
	int LOC_IN_REG;
}Pinfo;
```
This structure mappes the pin on the board to the corresponding registers. As you can see this struct contains 3 other structures with registers for setting the direction, output and getting the input state of the ports. These are structured as follows:
```c
struct DIR {
	unsigned int set;
	unsigned int clr;
	unsigned int state;
};

struct INP {
	unsigned int state;
};

struct OUTP {
	unsigned int state;
	unsigned int set;
	unsigned int clr;
};
```
By storing our data this way we can easily edit the registers of a physical pin, for example:
```c
 *(unsigned int*)(io_p2v(pinformatie.dir.set)) =  PIN_TO_BIT(pinformatie.LOC_IN_REG);
```
Here we set the direction of the pin to input by setting the correspoding bit (`PIN_TO_BIT(pinformatie.LOC_IN_REG)`) in register `Px_DIR_SET`.
