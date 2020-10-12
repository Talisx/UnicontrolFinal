/*
 * uc_functions.c
 *
 *  Created on: 10.08.2017
 *      Author: grewenig
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"


//*****************************************************************************
//
// A simple LED toggle funciton using the TivaWare GPIO API functions.
//
//*****************************************************************************

void ToggleLED(void)
{
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
	}
	return;
}
