/*
 * user space program for application developer to 
 * talk to leds on the LPC3250 board.
 * 
 * By: Stefan Grimminck & Skip Geldens
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "led_controller.h"

int chooseLed(int mode);
int choosePWM(int mode);

int main()
{
    bool escape = false;
    while (!escape) {
        int mode;
        printf("\n#########################\n");
        printf("#    I2C Assignment     #\n");
        printf("#########################\n");
        printf("1. Set Led On \n");
        printf("2. Set Led Off\n");
        printf("3. Set Led in Blink mode\n");
        printf("4. Set Led in Dimm mode\n");
        printf("5. Set Blink rate\n");
        printf("6. Set Dimmer value\n");
        printf("7. Quit\n");
        printf("choose > ");

        if (!scanf("%i", &mode)) {
            printf("ERROR: please choose a number from menu\n");
            return 1;
        }

        if (mode > 0 && mode < 8) {
            switch (mode) {
            case 1:
                /* Set Led On*/
                chooseLed(LED_ON);
                break;
            case 2:
                /* Set Led Off */
                chooseLed(LED_OFF);
                break;
            case 3:
                /* Set Led in Blink mode */
                chooseLed(LED_PWM2);
                break;
            case 4:
                /* Set Led in Dimm mode */
                chooseLed(LED_PWM1);
                break;
            case 5:
                /* Set Blink rate */
                choosePWM(LED_PWM1);
                break;
            case 6:
                /* Set Dimmer value */
                choosePWM(LED_PWM2);
                break;
            case 7:
                /* Quit */
                printf("Bye bye!\n");
                escape = true;
                break;
            }
        }
        else {
            printf("ERROR: please choose a number from menu\n");
            return 1;
        }
    }
    
    return 0;
}


int choosePWM(int mode)
{
    int ledval;
    if (mode == LED_PWM1) {
        printf("\nSet Blink Speed (Hz) [0 - 50]\n");
        printf("choice > ");

        if (!scanf("%i", &ledval)) {
            printf("ERROR: please choose a number from menu");
            return 1;
        }

        if (ledval < 0 || ledval > 100) {
            printf("ERROR: please choose a number from menu");
            return 1;
        }
        SetBlink(ledval);

    }
    else if (mode == LED_PWM2) {
        printf("\nSet Dimmer value (%) [0 - 100]\n");
        printf("choice > ");

        if (!scanf("%i", &ledval)) {
            printf("ERROR: please choose a number from menu");
            return 1;
        }

        if (ledval < 0 || ledval > 100) {
            printf("ERROR: please choose a number from menu");
            return 1;
        }
        if (SetPWM(ledval) < 2)
        {
			printf("An error occured while setting led value");
			return 1;
			
		}
		
    }
    return 0;
}

int chooseLed(int mode)
{
    int led;
    printf("\nChoose LED [1 - 8]\n");
    printf("choice > ");

    if (!scanf("%i", &led)) {
        printf("ERROR: please choose a number from menu");
        return 1;
    }

    if (led < 0 || led > NUM_LEDS) {
        printf("ERROR: please choose a number from menu");
        return 1;
    }
    /* The led values are defined as 0 - 7. However, the board specifies these leds as 1 - 8, so we subtract by 1. */ 
    led--;
    
    if(SetSingleLed(led, LED_OFF) != 2)
    {
		printf("An error occured while setting led value");
		return 1;
	}
	
	if(SetSingleLed(led, mode) != 2)
    {
		printf("An error occured while setting led value");
		return 1;
	}

    return 0;
}
