/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_csudma.h
*
* This file contains fmsh_sac.h and fmsh_common.h
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

#ifndef _FMSH_CSUDMA_H_
#define _FMSH_CSUDMA_H_	

#ifdef __cplusplus
extern "C" {
#endif
  
/***************************** Include Files *********************************/  
#include "fmsh_sac.h" 
#include "fmsh_common.h"

/************************** Constant Definitions *****************************/
#define IXR_DMA_DONE_MASK          0x00002000
#define IXR_D_P_DONE_MASK          0x00001000
#define MAX_COUNT                  1000000000


#define IXR_ERROR_FLAGS_MASK	 0x00148040

/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
void FmshFsbl_CsuDmaPollDone(u32 MaskValue, u32 MaxCount);

void FmshFsbl_CsuInitiateDma(u32 Source, u32 Dest, u32 SrcWordLength, u32 DestWordLength);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */
