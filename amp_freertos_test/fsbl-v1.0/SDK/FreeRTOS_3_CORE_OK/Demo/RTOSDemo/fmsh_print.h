#ifndef __FMSH_PRINT_H_
#define __FMSH_PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

  
/***************************** Include Files *********************************/
//#include "string.h"
#include "fmsh_uart_lib.h"
  
/************************** Constant Definitions *****************************/  

/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  

/************************** Variable Definitions *****************************/
  
extern FUartPs_T g_UART;

/************************** Function Prototypes ******************************/

extern void fmsh_print(const char *ptr,...);
extern void uart_printf(const char *ptr,...);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* end of protection macro */