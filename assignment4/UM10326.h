#ifndef PCA
#define PCA

/* Values to select certain registers to write to */
#define PWM1_CONTR_REG			0x4005C000
#define PWM2_CONTR_REG			0x4005C004
#define PWMCLOCK_REG			0x400040B8
#define LCDCONFIG_REG			0x40004054

/* Clock speed */
#define CLK_FREQ			32000

/* Masks */
#define PWM_EN 				0x80000000
#define PWM_FREQ			0x0000FF00
#define PWM_DUTY			0x000000FF
#define PWM_SHIFT_ENABLE		0x1F

#define PWMCLOCK			0x115
#define LCDCONFIG			0

void cleanup_module(void);
int get_pwm_state(int pwm_reg);
int get_duty_cycle(int pwm_reg);
int get_reload_value(int pwm_reg); 

#endif

