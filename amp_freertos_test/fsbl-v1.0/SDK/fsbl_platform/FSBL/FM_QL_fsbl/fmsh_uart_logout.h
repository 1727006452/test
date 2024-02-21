/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sd.h
*
* This file contains header fmsh_common.h & ff.h
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   xx  11/23/2018  First Release
*</pre>
******************************************************************************/

#ifndef _FMSH_UART_LOGOUT_H_
#define _FMSH_UART_LOGOUT_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "fmsh_uart_common.h"
#include "string.h"
#include "fmsh_uart_lib.h"
#include "fmsh_uart_public.h"
  
/************************** Constant Definitions *****************************/  
#define LOG_OUT(flag, ...)      \
do {                           \
	if(flag) {                     \
    uart_printf(__VA_ARGS__);    \
	}                          \
} while(0)


#define PRINT_ARRAY(flag, ...)      \
do {                           \
	if(flag) {                     \
    PrintArray_1(__VA_ARGS__);   \
	}                          \
} while(0)

/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  

/************************** Variable Definitions *****************************/
extern FUartPs_T g_UART;
/************************** Function Prototypes ******************************/
  
u8 FmshFsbl_UartInit(void);

u32 uart_printf(char *fmt,...);

char* FmshFsbl_GetErrorInfo(unsigned key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */

