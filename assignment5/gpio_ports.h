#ifndef GPIO_PORTS_H
#define GPIO_PORTS_H

#include <stdbool.h>
#define LPC3250_GPIO_BASE	0x40028000
#define _GPREG(x) io_p2v(LPC3250_GPIO_BASE + (x))

#define P0_INP_STATE		0x40028040
#define P0_OUTP_SET			0x40028044
#define P0_OUTP_CLR			0x40028048
#define P0_OUTP_STATE		0x4002804C
#define P0_DIR_SET			0x40028050
#define P0_DIR_CLR			0x40028054
#define P0_DIR_STATE		0x40028058

#define P1_INP_STATE		0x40028060
#define P1_OUTP_SET			0x40028064
#define P1_OUTP_CLR			0x40028068
#define P1_OUTP_STATE		0x4002806C
#define P1_DIR_SET			0x40028070
#define P1_DIR_CLR			0x40028074
#define P1_DIR_STATE		0x40028078

#define P2_DIR_SET			0x40028010
#define P2_DIR_CLR			0x40028014
#define P2_DIR_STATE		0x40028018
#define P2_INP_STATE		0x4002801C
#define P2_OUTP_SET			0x40028020
#define P2_OUTP_CLR			0x40028024
#define P2_MUX_SET			0x40028028
#define P2_MUX_CLR			0x4002802C
#define P2_MUX_STATE		0x40028030

#define P3_INP_STATE		0x40028000
#define P3_OUTP_SET			0x40028004
#define P3_OUTP_CLR			0x40028008
#define P3_OUTP_STATE		0x4002800C

#define LCDCLK_CTRL			0x40004054

#define PIN_TO_BIT(x)	(1 << (x))
#define GPIO3_PIN_TO_BIT(x)		(1 << ((x) + 25))

struct Port {
	unsigned int inp_state;
	unsigned int outp_state;
	unsigned int outp_set;
	unsigned int outp_clr;
	unsigned int dir_set;
	unsigned int dir_clr;
};


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


typedef struct Pinformation{
	int pin;
	int jumper;
	struct DIR dir;
    struct OUTP output;
	struct INP input;
	int LOC_IN_REG;
}Pinfo;


Pinfo pinToPort(int pin, int jumper);

#endif


