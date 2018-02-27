/* First test application for ES6 assignment 1. 
 * This application will try to read the address of the 
 * rtc. 
 * 
 * Stefan Grimminck & Skip Geldens
 */

#include <stdio.h>
#include <stdint.h>

int main(){
	uint32_t* info = (uint32_t*)0x40024000;
	printf("the result is: %u \n", *info);
	return 0;
}

/*
 * Result from LPC: 3813867535  
 * Result from Lubuntu: Segmentation fault
 */
