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

#ifndef _FMSH_SD_H_
#define _FMSH_SD_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "fmsh_common.h"
#include "ff.h"
 

/* FMSH FAT File System Library (FmshFFs) User Settings */
#define FILE_SYSTEM_INTERFACE_SD

/* Canonical definitions for peripheral SDMMC */
#define FPAR_SDMMCPS_HAS_CD 1
#define FPAR_SDMMCPS_HAS_WP 1
  
/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
  
u32 FmshFsbl_SdInit();

u32 FmshFsbl_SdCopy( u32 SourceAddress,
		u32 DestinationAddress,
		u32 LengthWords);

void FmshFsbl_ReleaseSD(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */

