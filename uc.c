/* XDCtools Header files */
#include <xdc/std.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Custom header files (TivaWare drivers etc.) */
#include <stdbool.h>
#include <stdint.h>

/* XCP header files (dSPACE XCP service) */
#include "xcp/xcp_prot.h"
#include "xcp/can_if.h"

/* Simulink Coder header files */
#include "UniControlSystem_ert_rtw/UniControlSystem.h"


int main(void)
{
	/* Initialize the real time system */
	//UniControlSystem_Init();

	/* Initialize the MPU peripherals */
	Init_MPU_Peripherals();

	/* Initialize the DAC */
	InitDAC();

	/* Initialize the CAN0 interface */
	InitCAN0();

	/* Initialize CAN0 message objects */
	InitCAN0MsgObjects();

	/* Initialize dSPACE XCP service */
	DSXCP_init();

	/* Initialize the dSPACE CAN driver and message objects */
	DSXCP_can_init(0);

	/*Initialie Motor */
	//initMotor();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
