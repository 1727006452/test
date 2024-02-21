/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_watchdog.h
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

#ifndef _FMSH_WATCHDOG_H_ /* prevent circular inclusions */
#define _FMSH_WATCHDOG_H_ /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/  
        
#include "fmsh_wdt_lib.h"
  
/***************************** Include Files *********************************/          

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
           
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

extern FWdtPs_T g_WDT;

/************************** Function Prototypes ******************************/
u8 FmshFsbl_WdtInit();

u8 FmshFsbl_WdtClose();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */
