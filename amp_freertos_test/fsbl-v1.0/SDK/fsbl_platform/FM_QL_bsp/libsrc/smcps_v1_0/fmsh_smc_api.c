/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_api.c
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

/***************************** Include Files *********************************/

#include <string.h>
#include "fmsh_smc_lib.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*****************************************************************************
*
* @description
* This function invokes different associate function to 
* implement a RESET command.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address offset.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void NORFLASH_Reset(NorFlash_T *pNorFlash, uint32_t address)
{
    if ((pNorFlash->cmdSet) == CMD_SET_AMD) {
        AMD_Reset(pNorFlash);
    }else {
        INTEL_Reset(pNorFlash, address);
    }
}

/*****************************************************************************
*
* @description
* This function invokes associate function to 
* implement a read device ID command.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*
* @return	
*           Returns 0 or data if the operation was successful.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NORFLASH_ReadDeviceID(NorFlash_T *pNorFlash)
{
    uint32_t ret;
	
    if ((pNorFlash->cmdSet) == CMD_SET_AMD) {
        ret = AMD_ReadDeviceID(pNorFlash);
    } else {
        ret = INTEL_ReadDeviceID(pNorFlash);
    }
    return ret;
}

/*****************************************************************************
*
* @description
* This function erases the specified block of the device.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address offset to be erase.
*
* @return	
*           Returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t NORFLASH_EraseSector(NorFlash_T *pNorFlash, uint32_t address)
{
    uint8_t ret;
	
    if ((pNorFlash->cmdSet) == CMD_SET_AMD) {
        ret = AMD_EraseSector(pNorFlash,address);
    }else {
        ret = INTEL_EraseSector(pNorFlash,address);
    }
    return ret;
}

/*****************************************************************************
*
* @description
* This function erases the specified block of the device.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address offset to be erase.
*
* @return	
*           Returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t NORFLASH_EraseChip(NorFlash_T *pNorFlash)
{
    uint8_t ret;
	
    if ((pNorFlash->cmdSet) == CMD_SET_AMD) {
        ret = AMD_EraseChip(pNorFlash);
    } else {
        ret = INTEL_EraseChip(pNorFlash);
    }
    return ret;
}

/*****************************************************************************
*
* @description
* This function sends data to the pNorFlash chip from the provided buffer.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Start address offset to be wrote.
*           buffer Buffer where the data is stored.
*           size Number of bytes that will be written.
*
* @return	
*           Returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t NORFLASH_WriteData(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size)
{
    uint8_t ret;
	
    if ((pNorFlash->cmdSet) == CMD_SET_AMD) {
        ret = AMD_Write_Data(pNorFlash,address, buffer, size);
    } else {
        ret = INTEL_Write_Data(pNorFlash,address, buffer, size);
    }
    return ret;
}

/*****************************************************************************
*
* @description
* This function reads data from the NorFlash chip into the provided buffer.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address offset to be read.
*           buffer  Buffer where the data will be stored.
*           size  Number of bytes that will be read.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void NORFLASH_ReadData(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size)
{
    uint32_t busAddress;
    uint32_t i;
    
    busAddress = pNorFlash->baseAddress + address;
    for(i = 0; i < size; i++) {
        ReadRawData(busAddress, buffer);
        buffer++;
        busAddress++;
    }
}
