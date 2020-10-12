/*
 * uc_tasks.c
 *
 *  Created on: 10.08.2017
 *      Author: grewenig
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#include "xcp/xcp_prot.h"
#include "xcp/can_if.h"

#include "UniControlSystem_ert_rtw/UniControlSystem.h"

#include "uc_functions.h"
#include "uc/uc_ad5755.h"
#include "uc/uc_ad7656.h"
#include "uc_tasks.h"

#include "uc/EPOS_command_send.h"

/* UniControl header files */
#include "uc/uc_globals.h"
#include "uc/uc_periph.h"
#include "uc/uc_ad5755.h"
#include "uc/uc_ad7656.h"
#include "uc/uc_can.h"
#include "uc/uc_tasks.h"
#include "uc/Init_Motor.h"


#include <stdio.h>
#include <string.h>

//Torstens Variablen(Quintus Teil)
bool Data_ValidEnco = true;
bool Data_ValidWinkel = true;
bool hallSensorcheck = true;
int Nachricht_prüfer = 0;
int Nachrichten_prüfer1 = 0;

//Torstens Variablen(Kimmer Teil)
int32_t vorherigerStrom = 0;
int32_t aktuellerStrom = 0;
int32_t aktuellerMag = 0;
int32_t vorherigerMag = 0;
int32_t vorherigerValueServo = 0;
int start_Motor = 0;
bool initComplete = true;
uint8_t _500ms = 0;

//Variablen der kompletten initialisierung
bool connectionComplete = false;
/* Global variables */
//static char ui8State = 0;    //User state machine

//*****************************************************************************
//
// The RTOS Idle task.
//
//*****************************************************************************

void TaskIdle(void)
{
	static uint8_t SysTickTimerEnabled = 1;

	/* During the first call of TaskIdle(), disable the SysTick Timer (Timer0) */
	if (SysTickTimerEnabled)
	{
		TimerDisable(TIMER0_BASE, TIMER_BOTH);
		SysTickTimerEnabled = 0;
	}

	DSXCP_background();

    return;
}


//*****************************************************************************
//
// The RTOS 100us task.
//
//*****************************************************************************

void Task100us(void)
{
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	/* Read the ADC and store the resulting samples in signed integer format*/
	AD7656_Read(i16AdcValue);

	return;
}

//*****************************************************************************
//
// The RTOS 10ms task.
//
//*****************************************************************************

void Task10ms(void)
{
	static uint8_t i;

	/* Clear the interrupt flag */
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	/* Convert ADC value to double using the respective dynamic range*/
	for(i=0; i<6; i++)
	{
		 dAdcValue[i] = (real_T)(i16AdcValue[i]) * 10.0 / 32768;
	}

	//Rückgabe Position zum Regler
	ModelInput[0] = Data_WinkelEncoderGes;

	/* Step the Simulink model */
	UniControlSystem_Step(	ModelInput[0],  ModelInput[1],  ModelInput[2],  ModelInput[3],
							ModelInput[4],  ModelInput[5],  ModelInput[6],  ModelInput[7],
							ModelInput[8],  ModelInput[9],  ModelInput[10], ModelInput[11],
							ModelInput[12], ModelInput[13], ModelInput[14], ModelInput[15],
							&ModelOutput[0],  &ModelOutput[1],  &ModelOutput[2],  &ModelOutput[3],
							&ModelOutput[4],  &ModelOutput[5],  &ModelOutput[6],  &ModelOutput[7],
							&ModelOutput[8],  &ModelOutput[9],  &ModelOutput[10], &ModelOutput[11],
							&ModelOutput[12], &ModelOutput[13], &ModelOutput[14], &ModelOutput[15]);

	/*Teil für die Funktion bei Task 100ms */
	aktuellerStrom = (int32_t)ModelOutput[0];
	aktuellerMag = (int32_t)But_Mag;

	/*Es muss erstmal die4 komplette initialisierung durchgeführt werden, damit der eigentliche Code ausführbar ist*/
	    if(connectionComplete == true)
	    {
	        /*Hier wird erstmal der Motor initialisiert, bevor etwas anderes gemacht wird*/
	        if(initComplete == true && But == 1)
	        {
	            initComplete = initMotor();
	            Data_ValidEnco = true;
	            Data_ValidWinkel = true;
	        }
	        else if(initComplete == false && But == 1)
	        {
	            //Hier wird geprüft ob die Verbindung überhaupt noch besteht zur Quintus Platine
	            if(Data_ValidEnco == true && Data_ValidWinkel == true)
	            {
	                Data_ValidEnco = false;
	                Data_ValidWinkel = false;
	                pui8TxBufferMagServo[0] = 200;
	                CANMessageSet(CAN0_BASE, 1, &sMsgObjectDataTx0, MSG_OBJ_TYPE_TX);
	                Nachrichten_prüfer1++;
	                //alle 50 Nachrichten, kann mal eine Fehlerhafte message ignoriert werden
	                if(Nachrichten_prüfer1 == 50)
	                {
	                    Nachricht_prüfer = 0;
	                    Nachrichten_prüfer1 = 0;
	                }
	            }
	            else
	            {
	                // man könnte eine if schleife hoch zählen, die ab bestimmten wert dann einfach alles ab schaltet
	                // zum vermeiden, das alles abkratzt wenn mal eine Nachricht zwischen drin falsch ist
	                if(Nachricht_prüfer == 0)
	                {
	                    Data_ValidEnco = true;
	                    Data_ValidWinkel = true;
	                    Nachricht_prüfer++;
	                }
	                //Fehler fals 2 Nachrichten nicht richtig hintereinander an kamen
	                else
	                {
	                    pack(WRITING_SEND, CURRENT_MODE_SETTING_VALUE,0,0);
	                    CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
	                    initComplete = true;
	                    But = 0;
	                    resetMotor();
	                }
	            }
	            /*Hier der Teil, verschickt eine Can Nachricht, sobald der Strom in ControlDesk geändert wird*/
	            if (aktuellerStrom == vorherigerStrom)
	            {
	                // nix ändern
	            }
	            else
	            {
	                if(aktuellerStrom < -2500)
	                {
	                    aktuellerStrom = -2500;
	                }
	                else if(aktuellerStrom > 2500)
	                {
	                    aktuellerStrom = 2500;
	                }
	                // neg Value left, pos Value right rotation
	                pack(WRITING_SEND, CURRENT_MODE_SETTING_VALUE,0,aktuellerStrom);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
	                vorherigerStrom = aktuellerStrom;
	            }
	            // fals Hall sensor ausgelöst, schalte alles ab
	            if(hallSensorcheck == false)
	            {
	                pack(WRITING_SEND, CURRENT_MODE_SETTING_VALUE,0,0);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
	                initComplete = true;
	                But = 0;
	                resetMotor();
	                hallSensorcheck = true;
	            }
	            // mit schalter auf 1 und 0 stellen, volle Auslenkung bei 1 und einziehen bei 0
	            if(Value_Servo == vorherigerValueServo)
	            {
	                // tue nichts
	            }
	            else
	            {
	                pui8TxBufferMagServo[0] = Value_Servo;
	                CANMessageSet(CAN0_BASE, 9, &sMsgObjectDataTx2, MSG_OBJ_TYPE_TX);
	                vorherigerValueServo = Value_Servo;
	            }
	            //Magnet Ansteuerung
	            if(aktuellerMag == vorherigerMag)
	            {
	                // tue nichts
	            }
	            else
	            {
	                pui8TxBufferMagServo[0] = aktuellerMag;
                    CANMessageSet(CAN0_BASE, 10, &sMsgObjectDataTx1, MSG_OBJ_TYPE_TX);
	                vorherigerMag = aktuellerMag;
	            }
	        }
	        //Notaus
	        else if(initComplete == false && But == 0)
	        {
	            pack(WRITING_SEND, CURRENT_MODE_SETTING_VALUE,0,0);
	            CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
	            initComplete = true;
	            resetMotor();
	        }
	        else
	        {
	            // tue nichts, da ausgeschaltet und nichts initialisiert.
	        }
	    }
	    else
	    {
	        //tue nichts, da die initialisierung noch nicht komplett abgeschlossen ist
	    }

	/* Update the DAC registers*/
	AD5755_Channel_Put(ui16DacValue[0], ui16DacValue[1], ui16DacValue[2], ui16DacValue[3]);

	DSXCP_service(0);
	return;
}

//*****************************************************************************
//
// The RTOS 100ms task.
//
//*****************************************************************************

void Task100ms(void)
{
	static uint8_t i = 0;
	static uint8_t k = 0;

	/* Clear the interrupt flag */
	TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

	//--------------------------------------------------------------------------------------------------------------------------
	 //initcompleteStart ist hier ein Button, connectionComplete die Variable die der MK auf true setzt
	if(connectionComplete == false && initcompleteStart == true && initComplete == true)
	{
	    initComplete = initMotor();
	}
	else if(connectionComplete == false && initcompleteStart == true && initComplete == false)
	{
	    pui8TxBufferInit[0] = 100;
	    CANMessageSet(CAN0_BASE, 11, &sMsgObjectDataTx3, MSG_OBJ_TYPE_TX);
	    initComplete = true;
        But = 0;
        resetMotor();
        initcompleteStart = false;
	}
    //--------------------------------------------------------------------------------------------------------------------------
	if(i == 7)
	{
		i = 0;
		ToggleLED();

		if(k < 7)
		{
			k++;
			ModelInput[6]  = dStimulus[k];
		}
		else
		{
			k = 0;
			ModelInput[6]  = dStimulus[k];
		}
	}
	else
	{
		i++;
	}

	//das hier muss in den die if schleife oben, das hier macht nur sinn abzufragen, wenn alles verbunden ist!!!
	/*
	switch(ui8State)
	    {
	        case 0: pack(READING_SEND, NUMBER_OF_ERRORS,ERROR_HISTORY_1, 0);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx,MSG_OBJ_TYPE_TX);
	                ui8State++;
	                break;
	        case 1: pack(READING_SEND, NUMBER_OF_ERRORS,ERROR_HISTORY_2, 0);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx,MSG_OBJ_TYPE_TX);
	                ui8State++;
	                break;
	        case 2: pack(READING_SEND, NUMBER_OF_ERRORS,ERROR_HISTORY_3, 0);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx,MSG_OBJ_TYPE_TX);
	                ui8State++;
	                break;
	        case 3: pack(READING_SEND, NUMBER_OF_ERRORS,ERROR_HISTORY_4, 0);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx,MSG_OBJ_TYPE_TX);
	                ui8State++;
	                break;
	        case 4: pack(READING_SEND, NUMBER_OF_ERRORS,ERROR_HISTORY_5, 0);
	                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx,MSG_OBJ_TYPE_TX);
	                ui8State++;
	                ui8State = 0;
	                break;
	    }
	    */
	return;
}
