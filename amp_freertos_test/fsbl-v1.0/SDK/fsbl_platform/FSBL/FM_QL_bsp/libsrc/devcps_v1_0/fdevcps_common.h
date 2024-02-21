/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_devc_lib.h
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

#ifndef _FDEVCPS_COMMON_H_		/* prevent circular inclusions */
#define _FDEVCPS_COMMON_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
 
/***************************** Include Files *********************************/
    
#include "fmsh_common.h"
    
/************************** Constant Definitions *****************************/
    
/**************************** Type Definitions *******************************/
    
typedef struct FDevcPs 
{
    void *base_address;
}FDevcPs_T;

/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */