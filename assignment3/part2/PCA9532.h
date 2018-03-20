#ifndef PCA
#define PCA

/* Values to select certain registers to write to */
#define INPUT0  0x00
#define PSC0    0x02
#define PWM0    0x03
#define PSC1    0x04
#define PWM1    0x05
#define LS0     0x06
#define LS1     0x07

/* Led modes */
#define LED_ON      0x01
#define LED_OFF     0x00
#define LED_PWM1    0x02
#define LED_PWM2    0x03

/* Macro's to set a led on a mode */
#define SET_LED0(mode) (mode << 0)
#define SET_LED1(mode) (mode << 2)
#define SET_LED2(mode) (mode << 4)
#define SET_LED3(mode) (mode << 6)

#define PSC_DIM 0x01
#define PWM_BLINK 

#endif