/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_intel.h
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
#ifndef _FMSH_SMC_INTEL_H_
#define _FMSH_SMC_INTEL_H_

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void INTEL_Reset(NorFlash_T *pNorFlash, uint32_t address);
uint32_t INTEL_ReadDeviceID(NorFlash_T *pNorFlash);
uint8_t INTEL_EraseSector(NorFlash_T *NorFlash, uint32_t sectorAddr);
uint8_t INTEL_EraseChip(NorFlash_T *NorFlash);
uint8_t INTEL_Write_Data(NorFlash_T *NorFlash, uint32_t address, uint8_t *buffer, uint32_t size);


#endif /* #ifndef _FMSH_SMC_INTEL_H_ */

