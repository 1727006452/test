/**
*
*@file led_flash.c
*
*@brief Control the LED cycle on and off, with a time interval of 1000ms
*
*@vsrsion V1.0
*
*@data 2023-09-14
*
**/
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "fmsh_gic.h"
#include <stdio.h>
#include "platform.h"
#include "fmsh_common.h"
#include "ps_init.h"
#include "fmsh_print.h"
#include "fmsh_gpio_public.h"
#include "fmsh_ps_parameters.h"
#include "string.h" 
#include "fmsh_uart_lib.h"
#include "fmsh_uart_example.h"

#define GPIO_DeviceID FPAR_GPIOPS_0_DEVICE_ID

#define mainQUEUE_UART_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_LEDFLICKER_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

#define PL_RAM_BASE     0x1A000000

#define COMM_TX_FLAG    (*(volatile unsigned long *)(0xE1FE0000))
#define COMM_TX_DATA    (*(volatile unsigned long *)(0xE1FE0004))
#define COMM_RX_FLAG    (*(volatile unsigned long *)(0xE1FE0008))
#define COMM_RX_DATA    (*(volatile unsigned long *)(0xE1FE000C))
#define COMM_VAL_FLAG	(*(volatile unsigned long *)(0xE1FE0010))
#define COMM_VAL	(*(volatile unsigned long *)(0xE1FE0014))
//#define GPIOA_REG_DR	(*(volatile unsigned long *)(0xE0003000)) /* GPIOA DATA */
//#define GPIOA_REG_DDR	(*(volatile unsigned long *)(0xE0003004)) /* GPIOA direction */
static u32 Led_Pin = (1 << 29); /*LED PIN:MIO29 PS GPIO29 */


/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

static void prvQueueReceiveTask( void *pvParameters );



extern FUartPs_T g_PS_UART;
extern u8 g_UartRXBuffer[256];
u8 fmsh_uart_init(void);

void FGpioPS_output_example(void *pvParameters)
{
  TickType_t xNextWakeTime;
  const unsigned long ulValueToSend = 100UL;
  
  /* Remove compiler warning about unused parameter. */
  ( void ) pvParameters;

  /* Initialise xNextWakeTime - this only needs to be done once. */
  xNextWakeTime = xTaskGetTickCount();
  
  FGpioPs_T gpioDevA;
  FGpioPs_init(&gpioDevA, FGpioPs_LookupConfig(GPIO_DeviceID));
  
   /* Set gpio direction output */
  FGpioPs_setBitDirection(&gpioDevA, Led_Pin, Gpio_output);
  
  while(1) {
                fmsh_print("GPIO LED Flashing Example by ddj2\r\n");
    		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, 0U );

    
                /*output low level to gpio pins*/
                FGpioPs_writeBit(&gpioDevA, Gpio_low, Led_Pin);
                vTaskDelay(500);
                
                /* Output high level to gpio pins */
                FGpioPs_writeBit(&gpioDevA, Gpio_high, Led_Pin);
                vTaskDelay(500);
  }

}

u32 FGicPs_Init(FGicPs *InstancePtr)
{	 
	u32 Status;

	Status = FGicPs_SetupInterruptSystem(InstancePtr);
	if(Status != GIC_SUCCESS)
	{
		return GIC_FAILURE ;
	}
	 
	FMSH_ExceptionRegisterHandler(FMSH_EXCEPTION_ID_IRQ_INT, (FMSH_ExceptionHandler)FGicPs_InterruptHandler_IRQ, InstancePtr);		
		
	return Status;
}

void write_data(int data)
{
    while(0 == COMM_TX_FLAG)
    {
        COMM_TX_FLAG = 0x01;
        COMM_TX_DATA = *((volatile unsigned long *)(PL_RAM_BASE+data));
      
    
    }

}


static void prvQueueReceiveTask(void *pvParameters)
{
    int i=0;
    unsigned long ulReceivedValue;
    const unsigned long ulExpectedValue = 100UL;
    while(1)
    {
      xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );
      if( ulReceivedValue == ulExpectedValue )
      {
          fmsh_print("uart echo Example by ddj3\r\n");
          vTaskDelay(1000);
          ulReceivedValue = 0U;
      }

      write_data(i);
      vTaskDelay(1000);
      i += 0x04;
      if(i > 0x200)
        i=0;
    } 
  
  
#if 0 
	int Status;
	int numChars = 0;
        
        unsigned long ulReceivedValue;
        const unsigned long ulExpectedValue = 100UL;
        
        /* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
        
        fmsh_print("uart echo Example by ddj\r\n");
	Status = FGicPs_Init(&IntcInstance);
	if(Status != GIC_SUCCESS)
	{
		TRACE_OUT(DEBUG_OUT, " GIC Setup Failed!\r\n");
	}

	fmsh_uart_init();
        fmsh_print("uart echo Example by ddj2\r\n");
	while(1) {
                /* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );
          
                if( ulReceivedValue == ulExpectedValue )
		{
                    fmsh_print("uart echo Example by ddj3\r\n");
                    delay_ms(1000);
                    ulReceivedValue = 0U;
                }
                
		/* Poll mode receive£¬The interrupt enable interface FUartPs_setListener()
	  	 * needs to be annotated in the initialization function.
		 **/
		numChars = FUartPs_getRxFifoLevel(&g_PS_UART);
		if(numChars > 0)
		{
		    FUartPs_burstRead(&g_PS_UART, g_UartRXBuffer, numChars);  
		    /* Receive data processing */
		    FUartPs_burstWrite(&g_PS_UART, g_UartRXBuffer, numChars);
		    memset(g_UartRXBuffer, 0, sizeof(g_UartRXBuffer));
		    numChars = 0;
		}
                vTaskDelay(1000);

	}
#endif
}


void led_flash(void)
{
  fmsh_print("GPIO LED Flashing Example by ddj\r\n");

  
    /* Create the queue. */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );
    if(xQueue != NULL )
    {
        /* Create the tasks. */
        xTaskCreate( FGpioPS_output_example, //The function that implements the task.
                    "led_flash",             //Text name for the task,provided to assist debugging only.
                    configMINIMAL_STACK_SIZE, //The stack allocated to the task.
                    NULL,   //The task parameter is not used, so set to NULL.
                    mainQUEUE_LEDFLICKER_TASK_PRIORITY,      //The task runs at the idle priority.
                    NULL);  //We are not using the task handle.
        
        xTaskCreate(prvQueueReceiveTask, "Uart_Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_UART_RECEIVE_TASK_PRIORITY,NULL);    
      
        /*Start thre tasks and timer running.*/
        vTaskStartScheduler();
    }
  
  /*If all is well, the scheduler will now be running, and the following line will never be reached. */
  while(1);

}


