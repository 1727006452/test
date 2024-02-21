/**
 * @file amp_led_ctrl.c
 *
 * @brief Example application main file.
 *
 * @version V1.0
 *
 * @date 2022-9-16
 **/
#include <stdio.h>
#include "platform.h"
#include "fmsh_common.h"
#include "ps_init.h"
#include "fmsh_print.h"
#include "fmsh_gic.h"

#define COMM_TX_FLAG	(*(volatile unsigned long *)(0xE1FE0000))
#define COMM_TX_DATA	(*(volatile unsigned long *)(0xE1FE0004))
#define COMM_RX_FLAG	(*(volatile unsigned long *)(0xE1FE0008))
#define COMM_RX_DATA	(*(volatile unsigned long *)(0xE1FE000C))
#define COMM_VAL_FLAG	(*(volatile unsigned long *)(0xE1FE0010))
#define COMM_VAL	(*(volatile unsigned long *)(0xE1FE0014))
#define GPIOA_REG_DR	(*(volatile unsigned long *)(0xE0003000)) /* GPIOA DATA */
#define GPIOA_REG_DDR	(*(volatile unsigned long *)(0xE0003004)) /* GPIOA direction */
#define LED_PIN		(1 << 29) /* MIO29 */

int main()
{
	u32 led_val;
	u32 led_reg;

	COMM_VAL = 0;
	COMM_VAL_FLAG = 1;
	COMM_RX_FLAG = 0;

	/* Set gpio direction output */
	led_reg = GPIOA_REG_DDR;
	GPIOA_REG_DDR = led_reg | LED_PIN;

	while (1)
	{
		COMM_VAL += 1;      
		delay_ms(1000);

		if (1 == COMM_RX_FLAG)
		{
			led_reg = GPIOA_REG_DR;
			led_val = COMM_RX_DATA;
			if (1 == led_val)
				led_reg = led_reg | LED_PIN;
			else
				led_reg = led_reg & ~LED_PIN;

			GPIOA_REG_DR = led_reg;
			COMM_RX_FLAG = 0;
		}
	}

	return 0;
}
