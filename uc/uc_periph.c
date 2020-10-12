// UniControl MPU
//
// uc_periph.h -- Driver for MPU periherals (implementation)
//
// ----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>
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
#include "driverlib/timer.h"

#include "uc_periph.h"

// Startup sequence
//
// 1) Configure GPIOs
// 2) Reset the ADC
// 3) Configure SSI1 for the ADC
// 4) Configure SSI3 for the DAC

void Init_MPU_Peripherals()
{
	//
	// Set the oscillator to a system clock of 120 MHz.
	//
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	//
    // Enable the GPIO port that is used for the on-board LEDs.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);

    //
    // Enable the GPIO pins on Port F and M
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, (GPIO_PIN_4 | GPIO_PIN_5 | 
                          GPIO_PIN_6));
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_1);

    // -- DAC LDAC pin
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6,  GPIO_PIN_6);
    
    // -- ADC reset (this is logic high!)
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_PIN_5);

    // -- DAC reset (this is logic low!)
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, 0);

    //
    // The SSI1 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

    //
    // SSI1 has got pins on ports B, D, E. Enable these ports.
    // 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    //
    // Configure the pin muxing for SSI0 functions on port Q0, Q1, Q2, and Q3.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PB5_SSI1CLK); 	// CLK
    GPIOPinConfigure(GPIO_PB4_SSI1FSS);		// Chip Select
    GPIOPinConfigure(GPIO_PE4_SSI1XDAT0);	//
    GPIOPinConfigure(GPIO_PE5_SSI1XDAT1);   //
    GPIOPinConfigure(GPIO_PD4_SSI1XDAT2);	//
    GPIOPinConfigure(GPIO_PD5_SSI1XDAT3);   // unused
	        

    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // 
    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    GPIOPinTypeSSI(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI1,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    // See AD7656 timing diagram: Clock idle state is 'high' -> SPO = 1

    SSIConfigSetExpClk(SSI1_BASE,ui32SysClock, SSI_FRF_MOTO_MODE_2,
                       SSI_MODE_MASTER, ADC_SPI_SPEED, 16);


    //
    // Enable the SSI1 module.
    //
    SSIEnable(SSI1_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(SSIDataGetNonBlocking(SSI1_BASE, &pui32SsiDataRx[0]))
    {
    }

	//
	// The SSI3 peripheral must be enabled for use.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

	//
	// For this example SSI0 is used with PortA[5:2].  The actual port and pins
	// used may be different on your part, consult the data sheet for more
	// information.  GPIO port A needs to be enabled so these pins can be used.
	// TODO: change this to whichever GPIO port you are using.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);


	//
	// Configure the pin muxing for SSI0 functions on port Q0, Q1, Q2, and Q3.
	// This step is not necessary if your part does not support pin muxing.
	// TODO: change this to select the port/pin you are using.
	//
	GPIOPinConfigure(GPIO_PQ0_SSI3CLK); 	// CLK
	GPIOPinConfigure(GPIO_PQ1_SSI3FSS);		// Chip Select
	GPIOPinConfigure(GPIO_PQ3_SSI3XDAT1);	// RX
	GPIOPinConfigure(GPIO_PQ2_SSI3XDAT0);   // TX

	//
	// Configure the GPIO settings for the SSI pins.  This function also gives
	// control of these pins to the SSI hardware.  Consult the data sheet to
	// see which functions are allocated per pin.
	// The pins are assigned as follows:
	//      PQ2 - SSI3Tx
	//      PQ3 - SSI3Rx
	//      PQ1 - SSI3Fss
	//      PQ0 - SSI3CLK
	// TODO: change this to select the port/pin you are using.
	//
	GPIOPinTypeSSI(GPIO_PORTQ_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_1 |
				   GPIO_PIN_0);

	//
	// Configure and enable the SSI port for SPI master mode.  Use SSI3,
	// system clock supply, idle clock level low and active low clock in
	// freescale SPI mode, master mode, 20MHz SSI frequency, and 8-bit data.
	// For SPI mode, you can set the polarity of the SSI clock when the SSI
	// unit is idle.  You can also configure what clock edge you want to
	// capture data on.  Please reference the datasheet for more information on
	// the different SPI modes.
	//

	SSIConfigSetExpClk(SSI3_BASE,ui32SysClock, SSI_FRF_MOTO_MODE_1,
					   SSI_MODE_MASTER, DAC_SPI_SPEED, 8);

	//
	// Enable the SSI3 module.
	//
	SSIEnable(SSI3_BASE);

	//
	// Read any residual data from the SSI port.  This makes sure the receive
	// FIFOs are empty, so we don't read any unwanted junk.  This is done here
	// because the SPI SSI mode is full-duplex, which allows you to send and
	// receive at the same time.  The SSIDataGetNonBlocking function returns
	// "true" when data was returned, and "false" when no data was returned.
	// The "non-blocking" function checks if there is any data in the receive
	// FIFO and does not "hang" if there isn't.
	//
	while(SSIDataGetNonBlocking(SSI3_BASE, &pui32SsiDataRx[0]))
	{
	}

	//
	// -- Configure PE0, PE1, PE2 as outputs, for CONVSTx operation
	// -- on the ADC.
	//
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
						  GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2,
				 0x00);

	//
	// -- Configure PE3 as input, for BUSY operation
	// -- on the ADC.
	//
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);

// -- Timer1 controls the ADC read cycle - 100 us
// -- Its HWI priority is set in the TI RTOS configuration file.

	//
	// Enable the Timer1 peripheral and wait for it to be ready.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));

	//
	// Configure Timer1 to be a full-length periodic timer (down-counting).
	//
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	TimerClockSourceSet(TIMER1_BASE, TIMER_CLOCK_SYSTEM);
	TimerLoadSet(TIMER1_BASE, TIMER_A, TIMER1_PERIOD);

	//
	// Enable Timer1 and its interrupt.
	//
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER1_BASE, TIMER_BOTH);

// -- Timer2 controls the fast-running Task - 10ms
// -- Its HWI priority is set in the TI RTOS configuration file.

	//
	// Enable the Timer2 peripheral and wait for it to be ready.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2));

	//
	// Configure Timer2 to be a full-length periodic timer (down-counting).
	//
	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
	TimerClockSourceSet(TIMER2_BASE, TIMER_CLOCK_SYSTEM);
	TimerLoadSet(TIMER2_BASE, TIMER_A, TIMER2_PERIOD);

	//
	// Enable Timer2 and its interrupt.
	//
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER2_BASE, TIMER_BOTH);

// -- Timer3 controls the slow-running task - 100 ms
// -- Its HWI priority is set in the TI RTOS configuration file.

	//
	// Enable the Timer3 peripheral and wait for it to be ready.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3));

	//
	// Configure Timer3 to be a full-length periodic timer (down-counting).
	//
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
	TimerClockSourceSet(TIMER3_BASE, TIMER_CLOCK_SYSTEM);
	TimerLoadSet(TIMER3_BASE, TIMER_A, TIMER3_PERIOD);

	//
	// Enable Timer3 and its interrupt.
	//
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER3_BASE, TIMER_BOTH);

	 // -- ADC reset (this is logic high!)
	 GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0);

	 // -- DAC reset (this is logic low!)
	 GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);

	//
	// Wait 100 ms (3-cycle-noop per execution)
	//
	SysCtlDelay(ui32SysClock/10/3);
}

