/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_api.h
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
#ifndef _FMSH_SMC_API_H_
#define _FMSH_SMC_API_H_

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void NORFLASH_Reset(NorFlash_T *pNorFlash, uint32_t address);
uint32_t NORFLASH_ReadDeviceID(NorFlash_T *pNorFlash);
uint8_t NORFLASH_EraseSector(NorFlash_T *pNorFlash, uint32_t sectorAddr);
uint8_t NORFLASH_EraseChip(NorFlash_T *pNorFlash);
uint8_t NORFLASH_WriteData(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size);
void NORFLASH_ReadData(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size);


#endif /* #ifndef _FMSH_SMC_API_H_ */

