/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_common.h
*
* This file contains header fmsh_can_private.h & fmsh_can_public.h
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

#ifndef _FMSH_USB_COMMON_H_		/* prevent circular inclusions */
#define _FMSH_USB_COMMON_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
 
/***************************** Include Files *********************************/
#include "stdio.h" 
#include "fmsh_common.h"

/************************** Constant Definitions *****************************/


/**
 * This typedef contains configuration information for the device.
 */
typedef struct {
	u16 DeviceId;	 /**< Unique ID  of device */
	u32 BaseAddress; /**< Base address of device (IPIF) */
	u32 InputClockHz;/**< Input clock frequency */
} FUsbPs_Config;
  
/**************************** Type Definitions *******************************/

typedef struct FUsbPs {
    const char *name;
    u32 id;
    u32 base_address;
    u32 input_clock; 
}FUsbPs_T;

/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
FUsbPs_Config *FUsbPs_LookupConfig(u16 DeviceId);
int FUsbPs_init(FUsbPs_T *usbDev, FUsbPs_Config *cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */