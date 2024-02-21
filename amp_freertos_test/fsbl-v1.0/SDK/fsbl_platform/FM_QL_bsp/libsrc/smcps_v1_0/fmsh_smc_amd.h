/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_amd.h
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
* 0.01   yl  12/20/2018  First Release
*</pre>
******************************************************************************/
#ifndef _NORFLASHAMD_H_
#define _NORFLASHAMD_H_

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void AMD_Reset(NorFlash_T *pNorFlash);
uint32_t AMD_ReadDeviceID(NorFlash_T *pNorFlash);
uint8_t AMD_EraseSector(NorFlash_T *pNorFlash, uint32_t sectorAddr);
uint8_t AMD_EraseChip(NorFlash_T *pNorFlash);
uint8_t AMD_Write_Data(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size);

#endif /* #ifndef _NORFLASHAMD_H_ */
