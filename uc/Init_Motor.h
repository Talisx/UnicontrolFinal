/*
 * Init_Motor.h
 *
 *  Created on: 26.08.2020
 *      Author: grewenig
 */

#ifndef UC_INIT_MOTOR_H_
#define UC_INIT_MOTOR_H_

bool initMotor(void);
void resetMotor(void);
void pack(uint8_t, uint16_t, uint8_t, int32_t);
uint32_t unpack(uint8_t *SDO_Byte, uint16_t *index, uint8_t *sub_index, int32_t *value);
//bool initComplete = true;
//extern enum state{ FAULTRESET, OUT, ON, ENABLEOP, CURRENTMODE, SETTINGCURRENT};
//extern enum state machineState;



#endif /* UC_INIT_MOTOR_H_ */
