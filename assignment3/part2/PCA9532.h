#ifndef PCA
#define PCA

/* Values to select certain registers to write to */
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

#endif