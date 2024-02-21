/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_fnand.h
*
* This file contains all private & pbulic functions of nand.
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   xxx  11/23/2018  First Release
*</pre>
******************************************************************************/

#ifndef _FMSH_FNAND_H_		/* prevent circular inclusions */
#define _FMSH_FNAND_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
 
/***************************** Include Files *********************************/  
#include "string.h"
#include "fmsh_nfcps_lib.h" 
#include "fmsh_nfcps_model.h"


/************************** Constant Definitions *****************************/
    
/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
u32 FmshFsbl_NandInit(void);

u32 FmshFsbl_NandCopy(u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */
