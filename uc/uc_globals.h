/*
 * uc_globals.h
 *
 *  Created on: 26.07.2017
 *      Author: grewenig
 */

#ifndef UC_UC_GLOBALS_H_
#define UC_UC_GLOBALS_H_

/* Global defines */
#define TASKSTACKSIZE   			2048

/* Global variables */

uint32_t 			ui32SysClock;						// System clock

uint32_t			pui32SsiDataRx[3];					// SSI receive buffer
uint32_t			pui32SsiDataTx[3];					// SSI transmit buffer

int16_t				i16AdcValue[6];
uint32_t			ui32AdcValue[6];
uint16_t			ui16DacValue[4];
real_T				dAdcValue[6];
real_T				dDacVcal[4] = {-0.01, 0.00, 0.00, 0.00};

real_T				dStimulus[8] = {2, 2, 0, 0, 2, 2, 0, 0};

real_T 				ModelInput[16];						// Simulink model input vector
real_T 				ModelOutput[16]; 					// Simulink model output vector

#endif /* UC_UC_GLOBALS_H_ */
