// UniControl MPU
//
// uc_ad5755.c -- Driver for MPU onboard DAC (implementation)
//
// Autor: Roman Grewenig
//
// ----------------------------------------------------------------------------

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

#include "uc_ad5755.h"

/* ----------------------------------------------------------------------------
 *
 *  Send a 24-Bit word to the AD5755.
 *
 *  Most significant byte of input argument is discarded.
 *
 * ---------------------------------------------------------------------------- */

void AD5755_Put(uint32_t TxWord)
{
     SSIDataPut(SSI3_BASE, TxWord >> 16);
     SSIDataPut(SSI3_BASE, TxWord >> 8);
     SSIDataPut(SSI3_BASE, TxWord);
     
     while(SSIBusy(SSI3_BASE))
    {
    }
}

/* ----------------------------------------------------------------------------
 *
 *  Configure the AD5755 to a default setting after startup (reset).
 *
 *  No DC/DC register settings are made, because the DC/DC feature is not used.
 *
 *  Default setting scales all DAC outputs to 10V bipolar range.
 *
 * ---------------------------------------------------------------------------- */

void AD5755_Cfg_Default()
{
	//
	// Send control data to DAC without OUTEN bit
	//
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_A | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_B | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_C | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_D | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);

	//
	// Write to DAC data registers with all zero data
	//
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_A);
	SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_B);
	SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_C);
	SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_D);

	//
	// Wait 500 탎 to reduce output glitch
	//
	SysCtlDelay(SysCtlClockGet() / 2000 / 3);

	//
	// Send control data to DAC with OUTEN bit
	//
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_A | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_OUTEN | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_B | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_OUTEN | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_C | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_OUTEN | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
	AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_CON | AD5755_DAC_D | AD5755_CON_DAC | AD5755_CON_DAC_INT_ENABLE | AD5755_CON_DAC_OUTEN | AD5755_CON_DAC_RSET_EXT | AD5755_CON_DAC_RANGE_BIP_10V);
}


/* ----------------------------------------------------------------------------
 *
 *  Simultaneously update all DAC Channels.
 *
 *  !!! Between every DAC write, a 5 탎 delay is needed
 *  (See datasheet timing details) !!!
 *
 *  Input argument format: unsigned int 16, straight binary.
 *
 * ---------------------------------------------------------------------------- */

void AD5755_Channel_Put(uint16_t ChA, uint16_t ChB, uint16_t ChC, uint16_t ChD)
{
     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);
     
     AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_A | (AD5755_VALUE_MASK & ChA));
     SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
     AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_B | (AD5755_VALUE_MASK & ChB));
     SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
     AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_C | (AD5755_VALUE_MASK & ChC));
     SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
     AD5755_Put(AD5755_WRITE | AD5755_DUT_0 | AD5755_REG_DATA | AD5755_DAC_D | (AD5755_VALUE_MASK & ChD));
     SysCtlDelay(ui32SysClock / 200000 / 3); // 5 탎
                
     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 0);
}

//*****************************************************************************
//
// Configure the AD5755
//
// Standard configuration is loaded into the DAC registers.
//
//*****************************************************************************

void InitDAC(void)
{
	AD5755_Cfg_Default();
	return;
}
       
