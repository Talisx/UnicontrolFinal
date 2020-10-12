// UniControl MPU
//
// uc_periph.h -- Driver for MPU periherals
//
// ----------------------------------------------------------------------------

#ifndef UC_PERIPH_H
#define UC_PERIPH_H

// -- Definitions

#define DAC_SPI_SPEED				10000000		// Hertz
#define ADC_SPI_SPEED				10000000		// Hertz
#define FLASH_SD_SPI_SPEED			1000000			// Hertz

#define TIMER1_PERIOD				12000			// Clock ticks, set for 100us
#define TIMER2_PERIOD				1200000			// Clock ticks, set for 10ms
#define TIMER3_PERIOD				12000000		// Clock ticks, set for 100ms

// -- Global variables references

extern uint32_t 		ui32SysClock;
extern uint32_t			pui32SsiDataRx[3];
extern uint32_t			pui32SsiDataTx[3];

// -- Function prototypes

extern void Init_MPU_Peripherals(void);

#endif
