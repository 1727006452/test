/**
 *
 * @file led_flash.c
 *
 * @brief Control the LED cycle on and off, with a time interval of 500ms.
  *
 * @version V1.0
 *
 * @date 2022-09-13
 *
 **/
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdio.h>
#include "platform.h"
#include "fmsh_common.h"
#include "ps_init.h"
#include "fmsh_print.h"
#include "fmsh_gpio_public.h"
#include "fmsh_ps_parameters.h"

#define GPIO_DeviceID FPAR_GPIOPS_0_DEVICE_ID

static u32 Led_Pin = (1 << 29);	/* LED PIN: MIO9 PS GPIO9 */

void FGpioPs_output_example(void *pvParameters)
{
	FGpioPs_T gpioDevA;
	FGpioPs_init(&gpioDevA, FGpioPs_LookupConfig(GPIO_DeviceID));
 
	 /* Set gpio direction output */
	FGpioPs_setBitDirection(&gpioDevA, Led_Pin, Gpio_output);
 
	while(1) {
		/* Output low level to gpio pins */
		FGpioPs_writeBit(&gpioDevA, Gpio_low, Led_Pin);
		delay_ms(1000);

		/* Output high level to gpio pins */
		FGpioPs_writeBit(&gpioDevA, Gpio_high, Led_Pin);
		delay_ms(1000);
	}
}

void led_flash(void)
{
	fmsh_print("GPIO LED Flashing Example\r\n");

	/* Create the tasks. */
	xTaskCreate(	FGpioPs_output_example, 	/* The function that implements the task. */
			"led_flash", 			/* Text name for the task, provided to assist debugging only. */
			configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
			NULL, 				/* The task parameter is not used, so set to NULL. */
			1,				/* The task runs at the idle priority. */
			NULL );				/* We are not using the task handle. */

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was either insufficient FreeRTOS heap memory available for the idle
	and/or timer tasks to be created, or vTaskStartScheduler() was called from
	User mode.  See the memory management section on the FreeRTOS web site for
	more details on the FreeRTOS heap http://www.freertos.org/a00111.html.  The
	mode from which main() is called is set in the C start up code and must be
	a privileged mode (not user mode). */
	while(1);
}

