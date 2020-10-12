/*
 * Init_Motor.c
 *
 *  Created on: 26.08.2020
 *      Author: grewenig
 */

#include <stdbool.h>
#include <stdint.h>
#include "Init_Motor.h"
#include "uc/EPOS_command_send.h"
#include "driverlib/can.h"
#include "uc/uc_can.h"
#include "inc/hw_memmap.h"

#include "EPOS_command_send.h"
enum state{ FAULTRESET, OUT, ON, ENABLEOP, CURRENTMODE, SETTINGCURRENT};
enum state machineState;

/*
 * Hier wird auch jedes mal wieder der Error ausgebügelt !!!
 */
bool initMotor(void)
{
        switch(machineState)
        {
            case FAULTRESET:
                //Fault reset
                pack(WRITING_SEND, CONTROL_WORD, 0,FAULT_RESET);
                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
                machineState = OUT;
                break;
            case OUT:
                //Shut down
                pack(WRITING_SEND, CONTROL_WORD, 0,SHUTDOWN);
                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
                //damit es keinen Pointer Fehler gibt
                //char ch[2];
                //printf("Epos enable\n");
                machineState = ON;
                break;
            case ON:
                //Enable operation
                pack(WRITING_SEND, CONTROL_WORD, 0,ENABLE_OPERATION);
                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
                // printf("EPOS enabled\n");
                machineState = ENABLEOP;
                break;
            case ENABLEOP:
                pack(WRITING_SEND, CONTROL_WORD, 0,ENABLE_OPERATION);
                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
                // printf("EPOS enabled\n");
                machineState = CURRENTMODE;
                break;
            case CURRENTMODE:
                //Current Mode
                pack(WRITING_SEND, MODES_OF_OPERATION, 0,DIGITAL_CURRENT);
                CANMessageSet(CAN0_BASE, 5, &sMsgObjectDataTx, MSG_OBJ_TYPE_TX);
                machineState = SETTINGCURRENT;
                break;
        }
        if(machineState == SETTINGCURRENT)
        {
            return false;
        }
        else
        {
            return true;
        }
}
void resetMotor(void)
{
    machineState = FAULTRESET;
}

// This function packs the different communication objects and the value in CANopen format.
void pack(uint8_t SDO_Byte, uint16_t index, uint8_t sub_index, int32_t value)
{
    uint8_t *pointer_index_Tx = &index;
    int8_t *pointer_value_Tx = &value;
    pui8TxBuffer [0] = SDO_Byte;
    pui8TxBuffer [1] = *pointer_index_Tx;
    pui8TxBuffer [2] = *(pointer_index_Tx+1);
    pui8TxBuffer [3] = sub_index;
    pui8TxBuffer [4] = *pointer_value_Tx;
    pui8TxBuffer [5] = *(pointer_value_Tx+1);
    pui8TxBuffer [6] = *(pointer_value_Tx+2);
    pui8TxBuffer [7] = *(pointer_value_Tx+3);
    return;
}

// This function unpacks the incoming CANopen massage and hand the received communication objects to global variables.
uint32_t unpack(uint8_t *SDO_Byte, uint16_t *index, uint8_t *sub_index, int32_t *value)
{
    uint16_t *pointer_index_Rx;
    int32_t *pointer_value_Rx;
    uint32_t *pointer_display;
    uint8_t index_array_Rx [2];
    int8_t value_array_Rx [4];
    uint8_t display_array [4];
    *SDO_Byte = pui8RxBuffer [0];
    index_array_Rx [0] = pui8RxBuffer [1];
    index_array_Rx [1]= pui8RxBuffer [2];
    *sub_index = pui8RxBuffer [3];
    value_array_Rx [0] = pui8RxBuffer [4];
    value_array_Rx [1] = pui8RxBuffer [5];
    value_array_Rx [2] = pui8RxBuffer [6];
    value_array_Rx [3] = pui8RxBuffer [7];
    display_array [0] = pui8RxBuffer [3];
    display_array [1] = pui8RxBuffer [1];
    display_array [2] = pui8RxBuffer [2];
    display_array [3] = pui8RxBuffer [0];
    pointer_index_Rx = index_array_Rx;
    pointer_value_Rx = value_array_Rx;
    pointer_display = display_array;
    *index = *pointer_index_Rx;
    *value = *pointer_value_Rx;
    return(*pointer_display);
}



