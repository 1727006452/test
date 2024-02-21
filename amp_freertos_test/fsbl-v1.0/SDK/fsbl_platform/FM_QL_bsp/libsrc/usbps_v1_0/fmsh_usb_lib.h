/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  
*
* This file contains 
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   wfb  11/23/2018  First Release
*</pre>
******************************************************************************/

#ifndef _FMSH_USB_LIB_H_		/* prevent circular inclusions */
#define _FMSH_USB_LIB_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
 
/***************************** Include Files *********************************/
#include "fmsh_usb_private.h"
#include "fmsh_usb_dev.h"
  
/************************** Constant Definitions *****************************/
    
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
int FUsbPs_irqHandler(FUsbPs_T *dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */