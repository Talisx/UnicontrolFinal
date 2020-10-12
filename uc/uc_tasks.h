/*
 * uc_tasks.h
 *
 *  Created on: 10.08.2017
 *      Author: grewenig
 */

#ifndef UC_UC_TASKS_H_
#define UC_UC_TASKS_H_

extern uint32_t 			ui32SysClock;						// System clock

extern int16_t				i16AdcValue[6];
extern uint32_t				ui32AdcValue[6];
extern uint16_t				ui16DacValue[4];
extern real_T				dAdcValue[6];
extern real_T				dDacVcal[4];

extern real_T				dStimulus[8];

extern real_T 				ModelInput[16];						// Simulink model input vector
extern real_T 				ModelOutput[16]; 					// Simulink model output vector


extern void TaskIdle(void);
extern void Task100us(void);
extern void Task10ms(void);
extern void Task100ms(void);

#endif /* UC_UC_TASKS_H_ */
