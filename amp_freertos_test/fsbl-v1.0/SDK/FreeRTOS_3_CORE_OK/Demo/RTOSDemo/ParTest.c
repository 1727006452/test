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

/*-----------------------------------------------------------
 * Simple IO routines to control the LEDs.
 * This file is called ParTest.c for historic reasons.  Originally it stood for
 * PARallel port TEST.
 *-----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes. */
#include "partest.h"

/* fmsh includes. */
#include "fmsh_gpio_lib.h"
#include "fmsh_ps_parameters.h"

#define partstControlID			(FPAR_GPIOPS_0_DEVICE_ID)
#define partstNUM_LED			( Gpio_bit_0 )

/*-----------------------------------------------------------*/

static FGpioPs_T tGpio;

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
FGpioPs_Config *pxConfigPtr;
BaseType_t status;

	/* Initialise the GPIO driver. */
	pxConfigPtr = FGpioPs_LookupConfig( partstControlID );
	status = FGpioPs_init( &tGpio, pxConfigPtr );
	configASSERT( status == 0 );
	( void ) status; /* Remove compiler warning if configASSERT() is not defined. */

	/* Enable outputs and set low. */
	FGpioPs_setBitDirection( &tGpio, partstNUM_LED, Gpio_output );
	FGpioPs_writeBit( &tGpio, Gpio_low, partstNUM_LED );
}
/*-----------------------------------------------------------*/

void vParTestSetLED( UBaseType_t uxLED, BaseType_t xValue )
{
	( void ) uxLED;
	FGpioPs_writeBit( &tGpio, Gpio_high, partstNUM_LED );
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
  /*
	enum FGpioPs_state ledStatus;
	( void ) uxLED;
	ledStatus = FGpioPs_getBit( &tGpio, partstNUM_LED );
	ledStatus = (ledStatus == Gpio_high) ? Gpio_low : Gpio_high;	
	FGpioPs_writeBit( &tGpio, ledStatus, partstNUM_LED );
*/
	/* it is no LED, print # via uart*/
    u32 uartaddr = STDOUT_BASEADDRESS;

	*((u32 *)uartaddr) = '#';
}



