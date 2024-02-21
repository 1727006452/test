
/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_nor.h
*
* This file contains all .
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
#ifndef _FMSH_NOR_H_	/* prevent circular inclusions */
#define _FMSH_NOR_H_

#ifdef __cplusplus
extern "C" {
#endif
  
/***************************** Include Files *********************************/
#include "fmsh_common.h"

/************************** Constant Definitions *****************************/
#define MEMCTL_SMTMGR_SET0			(0x0094)
#define MEMCTL_SMTMGR_SET1			(0x0098)
#define MEMCTL_SMTMGR_SET2			(0x009C)
#define MEMCTL_SMSKRx(x)			(0x0054 + 0x0004 * x)
#define MEMCTL_SMCTLR				(0x00A4)
#define WORD_LENGTH_SHIFT	2
#define PS_NOR_BASEADDR		(0xE0200000)

/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
u32 FmshFsbl_NorInit(u32 DeviceFlags);
u32 FmshFsbl_NorRelease(u32 DeviceFlags);
u32 FmshFsbl_NorCopy(u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes);

#endif