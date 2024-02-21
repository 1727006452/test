/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* fmsh includes. */
#include "fmsh_ttc_lib.h"
#include "fmsh_gic.h"

#if !defined(TTC0_0_FREQ) && !defined(TTC1_0_FREQ)
#error "PS has not enable ttc0 or ttc1, there is no tick for freertos!"
#endif

#ifdef TTC0_0_FREQ
#ifndef FMSH_TIMER_CLOCK_HZ
#define FMSH_TIMER_CLOCK_HZ (TTC0_0_FREQ)
#endif
#endif

#ifdef TTC1_0_FREQ
#ifndef FMSH_TIMER_CLOCK_HZ
#define FMSH_TIMER_CLOCK_HZ (TTC1_0_FREQ)
#endif
#endif

static FTtcPs_T xTimer;
extern FGicPs xInterruptController; 	/* Interrupt controller instance */

/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.  This file contains a function
 * that is suitable for use on the FMQL.
 */
void vConfigureTickInterrupt( void )
{
BaseType_t xStatus;
extern void FreeRTOS_Tick_Handler( void );
FTtcPs_Config *pxTimerConfig;
// FGicPs_Config *pxGICConfig;
const uint8_t ucRisingEdge = 3;

	/* This function is called with the IRQ interrupt disabled, and the IRQ
	interrupt should be left disabled.  It is enabled automatically when the
	scheduler is started. */

	/* Ensure FGicPs_CfgInitialize() has been called.  In this demo it has
	already been called from prvSetupHardware() in main(). */
	// pxGICConfig = FGicPs_LookupConfig( GIC_DEVICE_ID );
	// xStatus = FGicPs_CfgInitialize( &xInterruptController, pxGICConfig, pxGICConfig->CpuBaseAddress );
	// configASSERT( xStatus == 0 );
	// ( void ) xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* The priority must be the lowest possible. */
	FGicPs_SetPriorityTriggerType( &xInterruptController, TIMER0_1_INT_ID,
						portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT, ucRisingEdge );

	/* Install the FreeRTOS tick handler. */
	xStatus = FGicPs_Connect( &xInterruptController, TIMER0_1_INT_ID, (FMSH_InterruptHandler) FreeRTOS_Tick_Handler, ( void * ) &xTimer );
	configASSERT( xStatus == 0 );
	( void ) xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Initialise the timer. */
	pxTimerConfig = FTtcPs_LookupConfig( FPAR_TTCPS_0_DEVICE_ID );
	xStatus = FTtcPs_init( &xTimer, pxTimerConfig );
	configASSERT( xStatus == 1 );
	( void ) xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Enable Timer definedCount mode. */
	FTtcPs_setTimerMode(&xTimer, timer1, user_DefinedCount_mode);
	/* Load the timer counter register. */
	FTtcPs_TimerNLoadCount(&xTimer, timer1, FMSH_TIMER_CLOCK_HZ / configTICK_RATE_HZ);
	
	/* Enable the interrupt for the xTimer in the interrupt controller. */
	FGicPs_Enable( &xInterruptController, TIMER0_1_INT_ID );

	/* Enable the interrupt in the xTimer itself. */
	vClearTickInterrupt();
	FTtcPs_setTimerEnble( &xTimer, timer1, 1 );
}
/*-----------------------------------------------------------*/

void vClearTickInterrupt( void )
{
	FTtcPs_ClearTimerNInterrupt( &xTimer, timer1 );
}
/*-----------------------------------------------------------*/

/* This is the callback function which is called by the FreeRTOS Cortex-A port
layer in response to an interrupt.  If the function is called
vApplicationFPUSafeIRQHandler() then it is called after the floating point
registers have been saved.  If the function is called vApplicationIRQHandler()
then it will be called without first having saved the FPU registers.  See
http://www.freertos.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html for
more information */
void vApplicationFPUSafeIRQHandler( uint32_t ulICCIAR )
{
extern const FGicPs_Config FGicPs_ConfigTable[];
static const FGicPs_VectorTableEntry *pxVectorTable = FGicPs_ConfigTable[ GIC_DEVICE_ID ].HandlerTable;
uint32_t ulInterruptID;
const FGicPs_VectorTableEntry *pxVectorEntry;

	/* Re-enable interrupts. */
	__asm ( "cpsie i" );

	/* The ID of the interrupt is obtained by bitwise anding the ICCIAR value
	with 0x3FF. */
	ulInterruptID = ulICCIAR & 0x3FFUL;
	if( ulInterruptID < FGicPs_MAX_NUM_INTR_INPUTS )
	{
		/* Call the function installed in the array of installed handler functions. */
		pxVectorEntry = &( pxVectorTable[ ulInterruptID ] );
		pxVectorEntry->Handler( pxVectorEntry->CallBackRef );
	}
}


