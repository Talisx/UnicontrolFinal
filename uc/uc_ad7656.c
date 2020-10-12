/*
 * uc_ad7656.c
 *
 *  Created on: 10.07.2017
 *      Author: grewenig
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"

#include "uc_ad7656.h"

// -- Define macros

#define GPIOPinWriteDirect(ulPort, ucPins, ucVal) \
	HWREG(ulPort + (GPIO_O_DATA + (ucPins << 2))) = ucVal

#define GPIOPinReadDirect(ulPort, ucPins) \
	HWREG(ulPort + (GPIO_O_DATA + (ucPins << 2)))

// -- Implementations

void AD7656_Read(int16_t Data[6])
{
		static uint8_t i;
		static uint32_t InternalData[6];

		//
	    // -- ADC conversion
	    //

		// -- Start conversion on Channel group A,B,C by pulling CONVSTA/B/C high
	    //GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
		HWREG(GPIO_PORTE_BASE + (GPIO_O_DATA + ((GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2) << 2))) = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);

	    // -- Check the BUSY pin, wait until it returns low
	    while(HWREG(GPIO_PORTE_BASE + (GPIO_O_DATA + (GPIO_PIN_3 << 2))));


	    // -- Retreive data from SAR registers using six 16-Bit read cycles
	    SSIDataPut(SSI1_BASE, 0x0000);
	    while(SSIBusy(SSI1_BASE));
	    SSIDataGetNonBlocking(SSI1_BASE, (uint32_t*) &InternalData[0]);

	    SSIDataPut(SSI1_BASE, 0x0000);
	    while(SSIBusy(SSI1_BASE));
	    SSIDataGetNonBlocking(SSI1_BASE, (uint32_t*) &InternalData[1]);

	    SSIDataPut(SSI1_BASE, 0x0000);
	    while(SSIBusy(SSI1_BASE));
	    SSIDataGetNonBlocking(SSI1_BASE, (uint32_t*) &InternalData[2]);

	    SSIDataPut(SSI1_BASE, 0x0000);
	    while(SSIBusy(SSI1_BASE));
	    SSIDataGetNonBlocking(SSI1_BASE, (uint32_t*) &InternalData[3]);

	    SSIDataPut(SSI1_BASE, 0x0000);
	    while(SSIBusy(SSI1_BASE));
	    SSIDataGetNonBlocking(SSI1_BASE, (uint32_t*) &InternalData[4]);

	    SSIDataPut(SSI1_BASE, 0x0000);
	    while(SSIBusy(SSI1_BASE));
	    SSIDataGetNonBlocking(SSI1_BASE, (uint32_t*) &InternalData[5]);

	    // -- End conversion on Channel group A,B,C by pulling CONVSTA/B/C low
	    //GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,
	    //    		 0x0000);
	    HWREG(GPIO_PORTE_BASE + (GPIO_O_DATA + ((GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2) << 2))) = 0x0000;

	    // -- Convert from two's complement unsigned integer to 16-Bit signed integer
	    for(i=0; i<6; i++)
	    {
	    	Data[i] = (int16_t) -(~InternalData[i] + 1);
	    }
	return;
}
