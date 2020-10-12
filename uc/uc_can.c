/*
 * uc_can.c
 *
 *  Created on: 10.08.2017
 *      Author: grewenig
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"


#include "inc/hw_qei.h" // Macros for usage with the quadrature
#include "driverlib/sysctl.h" // Prototypes for the SysCtl driver.
#include "driverlib/qei.h" // Macros for usage with the quadrature

#include "uc_can.h"
bool testConnectK = true;
int8_t TestVariable = 0;

//*****************************************************************************
//
// Initialize the CAN0 module.
//
// CAN0 is routed to pins PA0, PA1 and configured for 1 MBit operation.
// Interrupts are enabled for the CAN controller.
//
// CAN0 is used for process communication.
//
//*****************************************************************************

/*initialize Can 0 Module, hier dran auch nicht mehr rum fummeln*/
void InitCAN0(void)
{
    //Test Nachricht, welche gelöscht werden kann
	/* Enable port A and wait until ready */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
	/* Set PA0 and PA1 to be CAN0 RX and TX */
	GPIOPinConfigure(GPIO_PA0_CAN0RX);
	GPIOPinConfigure(GPIO_PA1_CAN0TX);
	GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	/* Enable CAN0 peripheral */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	/* Initialize the CAN to a known state*/
	CANInit(CAN0_BASE);
	/* Set up the CAN fpr 1 MBit operation */
	CANBitRateSet(CAN0_BASE, ui32SysClock, CAN0_BAUDRATE);
	/* Set auto-retry to off */
	CANRetrySet(CAN0_BASE, 0);
	/* Interrupts */
	CANIntEnable(CAN0_BASE, CAN_INT_MASTER);
	IntEnable(INT_CAN0);
	/* Take CAN0 out of INIT state*/
	CANEnable(CAN0_BASE);
	return;
}

//*****************************************************************************
//
// Configure the CAN0 message objects.
//
// TX and RX interrupts are configured.
//
//*****************************************************************************


void InitCAN0MsgObjects(void)
{
    sMsgObjectDataTx0.ui32MsgID  = 0x0011;
    sMsgObjectDataTx0.ui32Flags  = MSG_OBJ_TX_INT_ENABLE;
    sMsgObjectDataTx0.ui32MsgIDMask = 0x0000;
    sMsgObjectDataTx0.ui32MsgLen     = 8;
    sMsgObjectDataTx0.pui8MsgData    = pui8TxBufferMagServo;
    CANMessageSet(CAN0_BASE, 1, &sMsgObjectDataTx0, MSG_OBJ_TYPE_TX);

	sMsgObjectDataRx0.ui32MsgID = 0x0012;
	sMsgObjectDataRx0.ui32MsgIDMask = 0xFFFF;
	sMsgObjectDataRx0.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
	sMsgObjectDataRx0.ui32MsgLen = 8;
	CANMessageSet ( CAN0_BASE , 2, &sMsgObjectDataRx0 ,MSG_OBJ_TYPE_RX );

	sMsgObjectDataRx1.ui32MsgID = 0x0013;
	sMsgObjectDataRx1.ui32MsgIDMask = 0xFFFF;
	sMsgObjectDataRx1.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
	sMsgObjectDataRx1.ui32MsgLen = 8;
	CANMessageSet ( CAN0_BASE , 3, &sMsgObjectDataRx1 ,MSG_OBJ_TYPE_RX );

	sMsgObjectDataRx2.ui32MsgID = 0x0020;
	sMsgObjectDataRx2.ui32MsgIDMask = 0xFFFF;
	sMsgObjectDataRx2.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
	sMsgObjectDataRx2.ui32MsgLen = 8;
	CANMessageSet ( CAN0_BASE , 4, &sMsgObjectDataRx2 ,MSG_OBJ_TYPE_RX );

	//message objekt für den Magneten
	sMsgObjectDataTx1.ui32MsgID  = 0x0029;
	sMsgObjectDataTx1.ui32Flags  = MSG_OBJ_TX_INT_ENABLE;
	sMsgObjectDataTx1.ui32MsgIDMask = 0x0000;
	sMsgObjectDataTx1.ui32MsgLen     = 8;
	sMsgObjectDataTx1.pui8MsgData    = pui8TxBufferMagServo;
	CANMessageSet(CAN0_BASE, 10, &sMsgObjectDataTx1, MSG_OBJ_TYPE_TX);

	//message objekt für die initialisierung der kompletten Laufkatze
	sMsgObjectDataTx3.ui32MsgID  = 0x0030;
	sMsgObjectDataTx3.ui32Flags  = MSG_OBJ_TX_INT_ENABLE;
	sMsgObjectDataTx3.ui32MsgIDMask = 0x0000;
	sMsgObjectDataTx3.ui32MsgLen     = 8;
	sMsgObjectDataTx3.pui8MsgData    = pui8TxBufferInit;
	CANMessageSet(CAN0_BASE, 11, &sMsgObjectDataTx3, MSG_OBJ_TYPE_TX);

	//message objekt für die initialisierung der kompletten Laufkatze
	sMsgObjectDataRx4.ui32MsgID = 0x0031;
	sMsgObjectDataRx4.ui32MsgIDMask = 0xFFFF;
	sMsgObjectDataRx4.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
	sMsgObjectDataRx4.ui32MsgLen = 8;
	CANMessageSet (CAN0_BASE , 12, &sMsgObjectDataRx4 ,MSG_OBJ_TYPE_RX );

	//message objekt für Hall Sensor
	sMsgObjectDataRx3.ui32MsgID = 0x0026;
	sMsgObjectDataRx3.ui32MsgIDMask = 0xFFFF;
	sMsgObjectDataRx3.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
	sMsgObjectDataRx3.ui32MsgLen = 8;
	CANMessageSet ( CAN0_BASE , 7, &sMsgObjectDataRx3 ,MSG_OBJ_TYPE_RX );

	//message objekt für den Servo Motor
	sMsgObjectDataTx2.ui32MsgID  = 0x0027;
	sMsgObjectDataTx2.ui32Flags  = MSG_OBJ_TX_INT_ENABLE;
	sMsgObjectDataTx2.ui32MsgIDMask = 0x0000;
	sMsgObjectDataTx2.ui32MsgLen     = 8;
	sMsgObjectDataTx2.pui8MsgData    = pui8TxBufferMagServo;
	CANMessageSet(CAN0_BASE, 9, &sMsgObjectDataTx2, MSG_OBJ_TYPE_TX);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Message objects für Steuergerät
	sMsgObjectDataTx.ui32MsgID = 0x0600+0b00000001;
	sMsgObjectDataTx.ui32Flags = 0x0000;
	sMsgObjectDataTx.ui32MsgIDMask = 0x0000;
	sMsgObjectDataTx.ui32MsgLen = 8;
	sMsgObjectDataTx.pui8MsgData = pui8TxBuffer;
	CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);

	// Message object 6 is used for RX of message ID '0x581'
	sMsgObjectDataRx.ui32MsgID = 0x0580+0b00000001;
	sMsgObjectDataRx.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
	// All bits of the ID must match.
	sMsgObjectDataRx.ui32MsgIDMask = 0xFFFF;
	sMsgObjectDataRx.ui32MsgLen = 8;
	CANMessageSet(CAN0_BASE, 6, &sMsgObjectDataRx, MSG_OBJ_TYPE_RX);

	return;
}
//change Kimmer connection check Variable
void connectionSetK(bool change)
{
    testConnectK = change;
}
bool connectionCheckK(void)
{
    return testConnectK;
}


//*****************************************************************************
//
// CAN0 interrupt handler.
//
// Upon a CAN0 interrupt, the controller status is checked and saved.
// The status changes depending on which message object caused the interrupt.
//
// A 'receive mask' is used to store the receive event of each RX message.
// The processing task is suspended until the receive mask is complete, i.e.
// all new data has been received and updated.
//
// The TX handler is called for TX message objects. In case of a data request
// the interrupt flag is cleared by software and the receive mask is cleared.
//
// The RX handler is called for RX message objects. The interrupt flags are
// cleared accordingly by software. The messaged object data pointer is set to
// the first element of the global data variable (array). Data is then stored
// in the global data variable and the receive mask is added to.
//
//*****************************************************************************

void CAN0IntHandler(void)
{
	static uint32_t ui32Status = 0;

	/*gibt das Message Objekt (0-31) raus, welches eine Nachricht enthält*/
	ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
	/* Switch for message object */
	switch(ui32Status)
	{
		case 1:		CANIntClear(CAN0_BASE, 1);
		            break;

		case 2:     CANIntClear(CAN0_BASE, 2);
		            sMsgObjectDataRx0.pui8MsgData = pui8RxBuffer;
		            CANMessageGet(CAN0_BASE, 2, &sMsgObjectDataRx0, 0);
		            Data_WinkelEncoder = pui8RxBuffer[0];
		            Data_WinkelEncoder1 = pui8RxBuffer[1];
		            Data_WinkelEncoder1 = Data_WinkelEncoder1 << 8;
		            Data_WinkelEncoder2 = pui8RxBuffer[2];
		            Data_WinkelEncoder2 = Data_WinkelEncoder2 << 16;
		            Data_WinkelEncoder3 = pui8RxBuffer[3];
		            Data_WinkelEncoder3 = Data_WinkelEncoder3 << 24;
		            Data_WinkelEncoderGes = Data_WinkelEncoder + Data_WinkelEncoder1 + Data_WinkelEncoder2 + Data_WinkelEncoder3;
		            Data_ValidWinkel = true;
		            break;

		case 3:     CANIntClear(CAN0_BASE, 3);
		            sMsgObjectDataRx1.pui8MsgData = pui8RxBuffer;
		            CANMessageGet(CAN0_BASE, 3, &sMsgObjectDataRx1, 0);

		            Data_Encoder = pui8RxBuffer[0];
		            Data_Encoder1 = pui8RxBuffer[1];
		            Data_Encoder1 = Data_Encoder1 << 8;
		            Data_Encoder2 = pui8RxBuffer[2];
		            Data_Encoder2 = Data_Encoder2 << 16;
		            Data_Encoder3 = pui8RxBuffer[3];
		            Data_Encoder3 = Data_Encoder3 << 24;
		            Data_EncoderGes = Data_Encoder + Data_Encoder1 + Data_Encoder2 + Data_Encoder3;

		            Data_ValidEnco = true;
		            break;

		case 6:     CANIntClear(CAN0_BASE, 6);
		            connectionSetK(true);
		            break;

		case 7:     CANIntClear(CAN0_BASE, 7);
		            hallSensorcheck = false;
		            break;

		case 9:     CANIntClear(CAN0_BASE, 9);
		            break;

		//der muss gecleart werden ansonsten wird xcp immer beim versenden dieses Messages Objektes abstürzen!
		case 10:    CANIntClear(CAN0_BASE, 10);
		            break;

		case 11:    CANIntClear(CAN0_BASE, 11);
		            break;
		//message Objekt für initialisierung
		case 12:    CANIntClear(CAN0_BASE, 12);
		            sMsgObjectDataRx4.pui8MsgData = pui8RxBufferInit;
		            CANMessageGet(CAN0_BASE, 12, &sMsgObjectDataRx4, 0);
		            TestVariable = pui8RxBufferInit[0];
		            if(TestVariable == 75)
		            {
		                connectionComplete = true;
		            }
		            break;

		/*pending status error */
		case 32768: CANIntClear(CAN0_BASE, CAN_INT_INTID_STATUS);
		          //  printf("Status Error \n");
		            break;

		/* Unused message objects: These should never trigger */
		default:	break;
	}
	return;
}
