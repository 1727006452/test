
#include <stdio.h>
#include "platform.h"
#include "fmsh_common.h"
#include "ps_init.h"
#include "fmsh_print.h"
#include "fmsh_gic.h"
#include "fmsh_ps_parameters.h"
#include "fmsh_gpio_public.h"

#define GPIO_DeviceID FPAR_GPIOPS_0_DEVICE_ID
static u32 Led_Pin = (1 << 29);	/* LED PIN: MIO29 PS GPIO29 */

void FGpioPs_output_example()
{
	FGpioPs_T gpioDevA;
	FGpioPs_init(&gpioDevA, FGpioPs_LookupConfig(GPIO_DeviceID));
 
	 /* Set gpio direction output */
	FGpioPs_setBitDirection(&gpioDevA, Led_Pin, Gpio_output);
 
	while(1) {
		/* Output low level to gpio pins */
		FGpioPs_writeBit(&gpioDevA, Gpio_low, Led_Pin);
		delay_ms(500);

		/* Output high level to gpio pins */
		FGpioPs_writeBit(&gpioDevA, Gpio_high, Led_Pin);
		delay_ms(500);
	}
}


int main()
{
	/* Init ps and debug uart */
	init_platform();

	fmsh_print("GPIO LED Flashing Example\r\n");
	FGpioPs_output_example();

	return 0;
}