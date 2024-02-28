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

#define	mainQUEUE_LEDFLICKER_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_UART_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_AD_C0LLECT_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )


/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

#define PL_RAM_BASE     0x1A000000

#define BIT(x)  (1<<x)

#define USART422_BASEADDR (void *)(0x43c20000)
#define USART422_SHIFTING 0x10000;

#define ADC_BASEADDR    (unsigned int)(0x43ca0000)
#define ADC_SHIFTING 0x10000;

#define GPIO_STATE_BASEADDR     (unsigned long *)(0x41200000)
#define GPIO_LED_BASEADDR       (unsigned long *)(0x41210000)

void * ads[4];

//GPIO

#define GPIO_OUT        (0)     /*gpio方向：输出*/
#define GPIO_IN         (1)     /*gpio方向：输入*/
#define GPIO_LOW        (0)     /*gpio值：0*/
#define GPIO_HIGH       (1)     /*gpio值：1*/
#define GPIO_MAX_XHANNEL_NUM    (2)
#define GPIO_MAX_PIN_NUM        (32)

#define CH1_DATA        (0x00)  /* 1通道数据寄存器 */
#define CH1_CTRL        (0x04)  /* 1通道控制寄存器 */
#define CH2_DATA        (0x08)  /* 2通道数据寄存器 */
#define CH2_CTRL        (0x0C)  /* 2通道控制寄存器 */


//公共底层部分 
//寄存器读
void public_write_value_1byte(void *address,unsigned int a,char command)
{
  void * _reg_addr_DATA = NULL;

  _reg_addr_DATA = ((unsigned int *)address + a);
  
  *(volatile unsigned long *)_reg_addr_DATA = command;
}

//寄存器写
unsigned int public_read_value_1byte(void *address,int a)
{
    void * _reg_addr_DATA = NULL;
    _reg_addr_DATA = ((unsigned int *)address + a);
    return *(volatile unsigned long *)_reg_addr_DATA;

}

//gpio 底层
int gpio_set_direction(unsigned long *address,unsigned char channel_id, unsigned char pin_id, unsigned char direction )
{
  void *reg_addr =NULL;
  
  if(channel_id == 1){
    reg_addr = address + CH2_CTRL/4;    //设置为输入，1
  }
  else{
    reg_addr = address + CH1_CTRL/4;    //设置为输出，0
  }
    
  //寄存器内部，0为output， 1为input
  if(direction == GPIO_IN){
    *(unsigned long *)reg_addr |= BIT(pin_id);  //设置为输入，1
  }else{
    *(unsigned long *)reg_addr &= ~BIT(pin_id); //设置为输出，0
  }
}

//gpio 设置值
int gpio_set_value(unsigned long *address,unsigned char channel_id, unsigned char pin_id, unsigned char value)
{
  void *reg_addr =NULL;
  
  if(channel_id == 1){
    reg_addr = address + CH2_DATA/4;    //设置为输入，1
  }
  else{
    reg_addr = address + CH1_DATA/4;    //设置为输出，0
  }
  
  if(value == GPIO_LOW){
    *(unsigned long *)reg_addr &= ~BIT(pin_id);  //设置为0
  }else{
    *(unsigned long *)reg_addr |= BIT(pin_id); //设置为1
  }

}


//gpio 初始化
int gpio_init(unsigned long *address, char num)
{
  int i;
  for(i=0; i<num ; i++)
  {
    gpio_set_direction(address, 0, i, GPIO_OUT);
    asm("nop");
    gpio_set_value(address, 0, i, GPIO_LOW);
    asm("nop");
  }
}

// ad 底层


//ad的初始化
void ad_init(unsigned int address)
{
  int i;
  
  for(i=0;i<4;i++)
  {
    if(i==2){
      ads[i] = (void *)(address-0xa0000);
      fmsh_print("ad%d = %x\r\n",i,ads[i]);
    }
    
    else
    {
      ads[i] = (void *)(address+(i*0x10000));
      fmsh_print("ad%d = %x\r\n",i,ads[i]);
    }
   
    public_write_value_1byte(ads[i],66,0); //工作的时候拉高，每次拉低就清空，等于复位
    asm("nop");
    public_write_value_1byte(ads[i],66,1);
  }
}

//ad的自检
void ad_selfcheck(void)
{
  int i,j;
  int temp_value;
  int temp_ad[4][8];
  float test_value;
  for(j=0; j<4; j++)
  {
    temp_value = 0;
    for(i=0;i<8;i++)
    {
      temp_ad[j][i] = public_read_value_1byte(ads[j],39);
      temp_value += temp_ad[j][i];
      delay_ms(1);
    }
    
    test_value = (float)temp_value*305/8000000;
    fmsh_print("value%d = %f\r\n",j,test_value);
  }
}



//rs422 串口 底层初始化
void usart_init(void * address, char baudrate)
{
  public_write_value_1byte(address,10,baudrate);
  delay_ms(1000);
  public_write_value_1byte(address,8,0);
  asm("nop");
  public_write_value_1byte(address,8,1);
}

//串口使能
void usart_enable(void *address) //发送使能 DE=slv_reg3 高有效 接收使能 RE=slv_reg6低有效
{
  public_write_value_1byte(address,3,0x01);
  public_write_value_1byte(address,6,0x00);
}

//撤销串口使能
void usart_disable(void *address)
{
  public_write_value_1byte(address,3,0x00);
  public_write_value_1byte(address,6,0x01);
}

//串口发送数据
int usart_send_datas(void *address, char *data_buffer, unsigned int n)
{
  int i;
  for(i=0; i<n; i++)
  {
    public_write_value_1byte(address,1,data_buffer[i]);
    public_write_value_1byte(address,2,0x01);
    asm("nop");
    public_write_value_1byte(address,2,0x00);
    asm("nop");
  }
  return 1;
}

//串口配置pl端寄存器
void usart_send_msg_to_pl(void *address,char reg)
{
  public_write_value_1byte(address, reg, 0x01);
  asm("nop");
  public_write_value_1byte(address, reg, 0x00);

}


//串口接收数据
int usart_receive_n_data(void *address, char * data_buffer, unsigned int n)
{
  int byte_received = 0;
  
  while(n--)
  {
    public_write_value_1byte(address,4,0x01);
    asm("nop");
    asm("nop");
    
    *data_buffer = public_read_value_1byte(address,0);
    data_buffer++;
    byte_received++;
    public_write_value_1byte(address,4,0x00);
    asm("nop");
  }
  return byte_received;
}

//串口接收任务

void FGpioPS_output_example(void *pvParameters)
{
  int j;
  char recbuffer[100];
  unsigned int sumn_num = 0;    //本次收到的串口总帧数
  unsigned int temp_uart_num = 0;       //本次的收到的串口总帧数 - 上一次收到的串口总帧数
  unsigned int uart_num = 0;    //上一次收到的串口总帧数
  int count = 0;
  fmsh_print("waiting data:\r\n");
  while(1) {
    if(public_read_value_1byte(USART422_BASEADDR,12))
    {
      usart_send_msg_to_pl(USART422_BASEADDR,13);
      sumn_num = public_read_value_1byte(USART422_BASEADDR,11);
      temp_uart_num = sumn_num - uart_num;
      uart_num = sumn_num;
      count = usart_receive_n_data(USART422_BASEADDR,recbuffer,temp_uart_num);
      fmsh_print("usart = %d, temp_uart_num= %d\r\n",0,temp_uart_num);
      for(j=0; j<temp_uart_num; j++)
      {
        fmsh_print("%x ",recbuffer[j]);
      }
      fmsh_print("\r\n");
       vTaskDelay(30);
    }
  }
}

//串口数据发送任务测试
static void prvQueueReceiveTask(void *pvParameters)
{
    char buffer[5]= {"hello"};
    usart_enable(USART422_BASEADDR);
    fmsh_print("fmsh_rs422_send: hello\r\n");
     
    while(1)
    {
      fmsh_print("fmsh_rs422_send: hello\r\n");
      usart_send_datas(USART422_BASEADDR,buffer,5);
      vTaskDelay(2003);
    } 
}

void get_ad_value(void)
{
  int i,j;
  int temp_ad_test[8];
  float Pulsevalue[7] = {0};				// FPGA测量的每个弹位的6路脉冲幅值 脉冲幅值
  unsigned int PulseWidth1[7] = {0};	// 脉宽
  unsigned int StartTime[7] = {0};		//
  char AdResultLog[100] = {0};
  char danwei[] = "danwei:";
  char redianchi2[] = "2#hot_battary: ";
  char redianchi1[] = "1#hot_battary: ";
  char ranqi1[] = "gas1: ";
  char ranqi2[] = "gas2: ";
  char shixu[] = "sequential: ";
  char jiebisuo[] = "jiebisuo: ";
  char maikuan[] = "pulse_width: ";
  char fuzhi[] = "pulse_value: ";
  char dianliu[] = "current: ";
//  float temp_ad[4][5] = {{1.0484,1.0797,1.1139,1.0780,1.0773},{1.1042,1.1385,1.1084,1.1104,1.0867},{1.1242,1.1961,1.1234,1.0449,1.0591},{1.0623,1.1515,1.0867,1.0424,1.0366}};//2号机箱
  float Pulsevalue_ori[5] = {0};
  float AD_power[4] = {0};
  for(i = 0; i < 4;i++)
    {
      AD_power[i] = (float)( public_read_value_1byte( ads[i], 32 ) ) * 305 / 1000000;
      fmsh_print("i=%d,AD_power=%2.2f\r\n",i,AD_power[i]);
      for(j = 0;j < 7;j++)
      {
        temp_ad_test[j] = public_read_value_1byte(ads[i],50+j+1);//value
//        Pulsevalue[j] = temp_ad_test[j]*305*2/1000000.0*temp_ad[i][j];
        Pulsevalue[j] = temp_ad_test[j]*305*2/1000000.0;
 //       Pulsevalue_ori[j] = temp_ad_test[j]*305*2/1000000.0;
        PulseWidth1[j] = public_read_value_1byte(ads[i],j+1);	//pulse
        PulseWidth1[j] = PulseWidth1[j]/100000;
        StartTime[j] = public_read_value_1byte( ads[i], j+17 );
      }
      //存储点火测量值
      sprintf(AdResultLog,"%s%d: \n",danwei,i);
      fmsh_print("%s\r\n",AdResultLog);
      
      //1#热电池点火
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",redianchi1,shixu,StartTime[0], maikuan, PulseWidth1[0],fuzhi,Pulsevalue[0]);
      fmsh_print("%s\r\n",AdResultLog);
      
      
      //2#热电池点火
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",redianchi2,shixu,StartTime[1], maikuan, PulseWidth1[1],fuzhi,Pulsevalue[1]);
      fmsh_print("%s\r\n",AdResultLog);
      

      
      //燃气发生器点火1
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",ranqi1,shixu,StartTime[2], maikuan, PulseWidth1[2],fuzhi,Pulsevalue[2]);
      fmsh_print("%s\n",AdResultLog);
      
      //燃气发生器点火2
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",ranqi2,shixu,StartTime[3], maikuan, PulseWidth1[3],fuzhi,Pulsevalue[3]);
      fmsh_print("%s\r\n",AdResultLog);
      
      //解闭锁
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",jiebisuo,shixu,StartTime[4], maikuan, PulseWidth1[4],fuzhi,Pulsevalue[4]);
      fmsh_print("%s\r\n",AdResultLog);
      
      //6
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",jiebisuo,shixu,StartTime[5], maikuan, PulseWidth1[5],fuzhi,Pulsevalue[5]);
      fmsh_print("%s\r\n",AdResultLog);
      //7
      sprintf(AdResultLog,"%s %s %dms %s %dms %s %02.2f \r\n",jiebisuo,shixu,StartTime[6], maikuan, PulseWidth1[6],fuzhi,Pulsevalue[6]);
      fmsh_print("%s\r\n\r\n",AdResultLog);
    }
}



//AD 数据采集任务测试
static void Ad_Collect_test(void *pvParameters)
{
  
  while(1)
  {
    get_ad_value();
    vTaskDelay(1000);
  }
}


void led_flash(void)
{
  
  fmsh_print("self check!\r\n");
  usart_init(USART422_BASEADDR,0x01); //422初始化
  fmsh_print("fmsh_init\r\n");
  
  gpio_init(GPIO_STATE_BASEADDR, 8);    //GPIO在位寄存器初始化
  gpio_init(GPIO_LED_BASEADDR, 10);     //GPIO中LED灯的寄存器初始化
  
  ad_init(ADC_BASEADDR);                //AD的初始化
  ad_selfcheck();                       //ad自检
  
  /* Create the tasks. */
  
  xTaskCreate( FGpioPS_output_example, //The function that implements the task.
              "led_flash",             //Text name for the task,provided to assist debugging only.
              configMINIMAL_STACK_SIZE, //The stack allocated to the task.
              NULL,   //The task parameter is not used, so set to NULL.
              mainQUEUE_LEDFLICKER_TASK_PRIORITY,      //The task runs at the idle priority.
              NULL);  //We are not using the task handle.
  
  xTaskCreate(prvQueueReceiveTask, "Uart_Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_UART_RECEIVE_TASK_PRIORITY,NULL);    

  xTaskCreate(Ad_Collect_test, "Ad_Collect_test", configMIDUAL_STACK_SIZE, NULL, mainQUEUE_AD_C0LLECT_TASK_PRIORITY,NULL);
  
  /*Start thre tasks and timer running.*/
  vTaskStartScheduler();
    
  /*If all is well, the scheduler will now be running, and the following line will never be reached. */
  while(1);
}


