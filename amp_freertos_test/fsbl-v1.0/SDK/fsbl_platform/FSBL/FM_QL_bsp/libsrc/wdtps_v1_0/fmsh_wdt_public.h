/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_wdt_public.h
*
* This file contains public constant & function define of public.
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

#ifndef _FMSH_WDT_PUBLIC_H_ /* prevent circular inclusions */
#define _FMSH_WDT_PUBLIC_H_ /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/     
        
#include "fmsh_wdt_common.h"

/************************** Constant Definitions *****************************/
enum prl_value
{
    _2_pclk = 0x0,
    _4_pclk = 0x1,
    _8_pclk = 0x2,
    _16_pclk = 0x3,
    _32_pclk = 0x4,
    _64_pclk = 0x5,
    _128_pclk = 0x6,
    _256_pclk = 0x7,
};

/**************************** Type Definitions *******************************/
           
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

BOOL FWdtPs_init(FWdtPs_T *dev, u32 addr);
u8 FWdtPs_setWDT_EN(FWdtPs_T *dev, enum FMSH_state state);
u8 FWdtPs_setRMOD(FWdtPs_T *dev, enum FMSH_state state);
u8 FWdtPs_setRPL(FWdtPs_T *dev, enum prl_value value);
u8 FWdtPs_setTOP(FWdtPs_T *dev, u32 value);
u8 FWdtPs_setTOP_INIT(FWdtPs_T *dev, u32 value);
u32 FWdtPs_getCurrentCounterValue(FWdtPs_T *dev);
u32 FWdtPs_restart(FWdtPs_T *dev);
u32 FWdtPs_getInterruptStatus(FWdtPs_T *dev);
u32 FWdtPs_clearInterruptStatus(FWdtPs_T *dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */

