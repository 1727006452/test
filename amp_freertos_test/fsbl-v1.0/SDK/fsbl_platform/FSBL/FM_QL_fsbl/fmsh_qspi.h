/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_qspi.h
*
* This file contains header fmsh_qspips_lib.h
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

#ifndef _FMSH_QSPI_H_
#define _FMSH_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "stdio.h" 
#include "string.h"
#include "fmsh_common.h"
#include "fmsh_qspips_lib.h"

  
/************************** Constant Definitions *****************************/  
  
/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/
extern volatile BOOL g_QSPI_DMA_Flag;
extern volatile u8 g_QSPI_High_Addr_Flag;
/************************** Function Prototypes ******************************/
u32 FQspiPs_AddressMap(FQspiPs_T* qspi, u32 addr);

u32 FmshFsbl_InitQspi(void);

u32 FmshFsbl_QspiAccess( u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes);

int FQspiPs_WaitForIndacDone(FQspiPs_T* qspi, int isRead);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */

