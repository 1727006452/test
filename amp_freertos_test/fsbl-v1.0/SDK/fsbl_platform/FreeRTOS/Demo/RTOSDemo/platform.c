/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include "platform_config.h"
#include "fmsh_ps_parameters.h"
#include "fmsh_uart_lib.h"

/*
 * Uncomment one of the following two lines, depending on the target,
 * if ps7/psu init source files are added in the source directory for
 * compiling example outside of SDK.
 */
/*#include "ps7_init.h"*/
/*#include "psu_init.h"*/

#include "fmsh_uart_common.h"
#define UART_BAUD 115200

FUartPs_T g_UART;

u8
init_uart()
{
    u8 ret=FMSH_SUCCESS;
#ifdef STDOUT_IS_16550
    XUartNs550_SetBaud(STDOUT_BASEADDR, FPAR_FUARTPLNS550_CLOCK_HZ, UART_BAUD);
    XUartNs550_SetLineControlReg(STDOUT_BASEADDR, FUN_LCR_8_DATA_BITS);
#endif
    /* Bootrom/BSP configures PS7/PSU UART to 115200 bps */
#ifdef STDOUT_BASEADDRESS  
    FUartPs_Config *Config=NULL;
    
    /*Initialize UARTs and set baud rate*/
    Config= FUartPs_LookupConfig(STDOUT_BASEADDRESS==FPS_UART0_BASEADDR?FPAR_UARTPS_0_DEVICE_ID:FPAR_UARTPS_1_DEVICE_ID);
    if(Config==NULL)
      return FMSH_FAILURE;
    ret=FUartPs_init(&g_UART, Config);
    if(ret!=FMSH_SUCCESS)
      return ret;
    
    FUartPs_setBaudRate(&g_UART, STDOUT_BASEADDRESS==FPS_UART0_BASEADDR?FPAR_UARTPS_0_BAUDRATE : FPAR_UARTPS_1_BAUDRATE);
    /*line settings*/
    FUartPs_setLineControl(&g_UART, Uart_line_8n1);
    
    /*enable FIFOs*/   
    FUartPs_enableFifos(&g_UART);
#endif
    return ret;
}

void
init_platform()
{
//    ps_init();
    
    init_uart();
}

void
cleanup_platform()
{
}
