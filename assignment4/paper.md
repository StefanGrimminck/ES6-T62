PWMCLK-CTRL 0x4000 40B8

|**Bit**| 	**Function**|**Our value**|
|-------|---------------|--------------|
|0		|PWM1 block enable|	x|
|1		|PWM1 Clock select|	1 (PERIPH_CLOCK)|
|2		|PWM Block enable	|x|
|3		|PWM2 Clock select|	1 (PERPIH_CLOCK)|
|7:4	|	PWM1_FREQ|
|11:8	|	PWM2_FREQ|


PWM1_CTRL  0x4005 C000

|**Bit**|		**Function**|		**Our value**|
|-------|---------------|----------------|
|31|		PWM1_EN		|	1	|
|30	|	PWM1_PIN_LEVEL	|	n.v.t. | 
|15:8|		PWM1_RELOADV	|	Fout = [PWM_CLK / PWM_RELOADV] / 256|
|7:0	|	PWM1_DUTY	|	[LOW]/[HIGH] = [PWM_DUTY] / [256-PWM_DUTY]|
