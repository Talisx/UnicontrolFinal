/*
 * uc_can.h
 *
 *  Created on: 10.08.2017
 *      Author: grewenig
 */

#ifndef UC_UC_CAN_H_
#define UC_UC_CAN_H_

//250000
//hier stand vorher 1000000
#define CAN0_BAUDRATE	1000000

//Can data buffers for RX and TX
uint8_t				pui8TxBuffer[8];
uint8_t             pui8TxBufferInit[8];
uint8_t             pui8TxBufferMagServo[8];
uint8_t             pui8RxBuffer[8];
uint8_t             pui8RxBufferInit[8];

//variables for ISR Can
//uint8_t Data_Encoder;
uint32_t Data_WinkelEncoder;
uint32_t Data_WinkelEncoder1;
uint32_t Data_WinkelEncoder2;
uint32_t Data_WinkelEncoder3;
uint32_t Data_WinkelEncoderGes;
uint32_t Data_Encoder;
uint32_t Data_Encoder1;
uint32_t Data_Encoder2;
uint32_t Data_Encoder3;
uint32_t Data_EncoderGes;
extern bool Data_ValidEnco;
extern bool Data_ValidWinkel;
extern bool hallSensorcheck;
extern bool initComplete;
extern bool connectionComplete;

// Can message object structures for RX and TX
tCANMsgObject		sMsgObjectDataTx0;
tCANMsgObject       sMsgObjectDataTx1;
tCANMsgObject       sMsgObjectDataTx2;
tCANMsgObject       sMsgObjectDataTx3;
tCANMsgObject       sMsgObjectDataRx0;
tCANMsgObject       sMsgObjectDataRx1;
tCANMsgObject       sMsgObjectDataRx2;
tCANMsgObject       sMsgObjectDataRx3;
tCANMsgObject       sMsgObjectDataRx4;

//Message structure for Kimmer
tCANMsgObject        sMsgObjectDataTx;
tCANMsgObject        sMsgObjectDataRx;

extern uint32_t ui32SysClock;
extern uint32_t ui32CanRxFlags;

/* Function prototypes */

extern void InitCAN0(void);
extern void InitCAN0MsgObjects(void);
extern void CAN0IntHandler(void);

void connectionSetK(bool);
bool connectionCheckK(void);

#endif /* UC_UC_CAN_H_ */
