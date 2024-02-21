/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_intel.c
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
 
/** Command for vendor command set CMD_SET_INTEL. Device commands are written 
    to the Command User Interface (CUI) to control all flash memory device operations. */
#define INTEL_CMD_IDIN             0x0090
#define INTEL_CMD_BLOCK_ERASE_1    0x0020
#define INTEL_CMD_BLOCK_ERASE_2    0x00D0
#define INTEL_CMD_READ_STATUS      0x0070
#define INTEL_CMD_CLEAR_STATUS     0x0050
#define INTEL_CMD_BLOCK_LOCKSTART  0x0060
#define INTEL_CMD_BLOCK_LOCK       0x0001
#define INTEL_CMD_BLOCK_UNLOCK     0x00D0
#define INTEL_CMD_BLOCK_LOCKDOWN   0x002F
#define INTEL_CMD_PROGRAM_WORD     0x0010
#define INTEL_CMD_RESET            0x00FF


/** Intel norflash status resgister */
#define INTEL_STATUS_DWS    0x80
#define INTEL_STATUS_ESS    0x40
#define INTEL_STATUS_ES     0x20
#define INTEL_STATUS_PS     0x10
#define INTEL_STATUS_VPPS   0x08
#define INTEL_STATUS_PSS    0x04
#define INTEL_STATUS_BLS    0x02
#define INTEL_STATUS_BWS    0x01

/** Intel norflash device Identifier infomation address offset. */
#define INTEL_MANU_ID       0x00
#define INTEL_DEVIDE_ID     0x01
#define INTEL_LOCKSTATUS    0x02

/// Intel norflash device lock status.
#define INTEL_LOCKSTATUS_LOCKED         0x01
#define INTEL_LOCKSTATUS_LOCKDOWNED     0x02

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*****************************************************************************
*
* @description
* Writes a command using a 8-bit bus cycle.
*
* @param    
*           BaseAddr is the base address of device.
*           Offset is the offset into the device(s) address space on which
*		    command is required to be written.
*           Cmd is the command/data to write at BaseAddress + Offset.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
static void intel_sendCmd8(u32 BaseAddr, u32 Offset, u32 Cmd)
{
	NOR_WRITE_COMMAND8(((volatile u8*)BaseAddr) + Offset, Cmd);
}

/*****************************************************************************
*
* @description
* Implements a RESET command.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
static void intel_Reset(NorFlash_T *pNorFlash, uint32_t address)
{
    uint32_t baseAddress = pNorFlash->baseAddress;
	
	intel_sendCmd8(baseAddress, address, INTEL_CMD_RESET);
}

/*****************************************************************************
*
* @description
* The Read Device Identifier command instructs the device to output manufacturer
* code, device identifier code, block-lock status, protection register data, 
* or configuration register data by giving offset.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           offset Identifier address offset.
*
* @return	
*           Indentification data.
*
* @note		NA.
*
*****************************************************************************/
static uint32_t intel_ReadIdentification(    NorFlash_T *pNorFlash,    uint32_t offset)
{
    uint32_t data;
    uint32_t baseAddress = pNorFlash->baseAddress;
    
    // Issue Read Array Command - just in case that the flash is not in Read Array mode 
    intel_Reset(pNorFlash, 0);
	
    // Issue the Read Device Identifier command at specified address.
    intel_sendCmd8(baseAddress, offset, INTEL_CMD_IDIN);
    ReadRawData(baseAddress + offset, (uint8_t*)&data);
    ReadRawData(baseAddress + offset + 1, (uint8_t*)&data);
	
    intel_Reset(pNorFlash, 0);
    return data;
}

/*****************************************************************************
*
* @description
* Return the status register value.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address offset to be erase.
*
* @return	
*           Status value.
*
* @note		NA.
*
*****************************************************************************/
static uint8_t intel_ReadStatus(NorFlash_T *pNorFlash, uint32_t address)
{
    uint32_t status;
    uint32_t baseAddress = pNorFlash->baseAddress;
    
    /* Issue the Read Status Register command at any address. */
    intel_sendCmd8(baseAddress, address, INTEL_CMD_READ_STATUS);
    ReadRawData(baseAddress + address, (uint8_t*)&status);
    return status;
}

/*****************************************************************************
*
* @description
* Clear the status register.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
static void intel_ClearStatus(NorFlash_T *pNorFlash)
{
    uint32_t baseAddress = pNorFlash->baseAddress;
    
    // Issue the Clear Status Register command at any address
    intel_sendCmd8(baseAddress, 0, INTEL_CMD_CLEAR_STATUS);
}

/*****************************************************************************
*
* @description
* Unlocks the specified block of the device.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address in sector.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
static void intel_UnlockSector(NorFlash_T *pNorFlash, uint32_t address)
{
    uint32_t baseAddress = pNorFlash->baseAddress;
	
     // Issue Read Array Command - just in case that the flash is not in Read Array mode 
    intel_Reset(pNorFlash, 0);    
    // Clear the status register first.
    intel_sendCmd8(baseAddress, address, INTEL_CMD_BLOCK_LOCKSTART);
    intel_sendCmd8(baseAddress, address, INTEL_CMD_BLOCK_UNLOCK);
	
    intel_Reset(pNorFlash, 0);
}

/*****************************************************************************
*
* @description
* The Read Device Identifier command instructs the device to output block-lock 
* status.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Address in sector/block.
*
* @return	
*           Indentification data.
*
* @note		NA.
*
*****************************************************************************/
static uint32_t intel_GetBlockLockStatus(NorFlash_T *pNorFlash, uint32_t address)
{
    return intel_ReadIdentification(pNorFlash, (address + INTEL_LOCKSTATUS));
}

/*****************************************************************************
*
* @description
* It implement a program word command.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Start address offset to be wrote.
*           data word to be written.
*
* @return	
*           returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
static uint8_t intel_Program(    NorFlash_T *pNorFlash, uint32_t address, uint32_t data)
{
    uint32_t status;
    uint8_t done = 0;
    uint32_t baseAddress = pNorFlash->baseAddress;
	
     // Issue Read Array Command - just in case that the flash is not in Read Array mode 
    intel_Reset(pNorFlash, address);
    
    /*
    // Check if the data already have been erased.
    ReadRawData(busAddress, (uint8_t*)&datain);
    if((datain & data)!= data) {
        return NorCommon_ERROR_CANNOTWRITE;
    }
    */
    // Word programming operations are initiated by writing the Word Program Setup command to the device.
    intel_sendCmd8(baseAddress, address, INTEL_CMD_PROGRAM_WORD);
    // This is followed by a second write to the device with the address and data to be programmed.
    WriteRawData(baseAddress + address, (uint8_t*)&data);
    
    // Status register polling 
    do
	{
        status = intel_ReadStatus(pNorFlash,address);
        // Check if the device is ready.
        if ((status & INTEL_STATUS_DWS) == INTEL_STATUS_DWS )
		{
            // check if VPP within acceptable limits during program or erase operation.
            if ((status & INTEL_STATUS_VPPS) == INTEL_STATUS_VPPS )
			{
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // Check if the erase block operation is completed. 
            if ((status & INTEL_STATUS_PS) == INTEL_STATUS_PS )
			{
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // check if Block locked during program or erase, operation aborted.
            else if ((status & INTEL_STATUS_BLS) == INTEL_STATUS_BLS )
			{
                return NorCommon_ERROR_CANNOTWRITE;
            }
            else
			{
                done = 1;
            }
        }
    } while (!done);
    
    intel_ClearStatus(pNorFlash);
    intel_Reset(pNorFlash, address);
    return 0;
}

/*****************************************************************************
*
* @description
* This function implements a RESET command.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void INTEL_Reset(NorFlash_T *pNorFlash, uint32_t address)
{
    intel_Reset(pNorFlash, address);
}

/*****************************************************************************
*
* @description
* This function instructs the device to output device id
* by Read Device Identifier command.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*
* @return	
*           Device ID.
*
* @note		NA.
*
*****************************************************************************/
uint32_t INTEL_ReadDeviceID(NorFlash_T *pNorFlash)
{
    return intel_ReadIdentification(pNorFlash, INTEL_MANU_ID);
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
*           returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t INTEL_EraseSector(    NorFlash_T *pNorFlash,  uint32_t address)
{
    uint32_t status;
    uint8_t done = 0;
    uint32_t baseAddress = pNorFlash->baseAddress;
    
     // Issue Read Array Command - just in case that the flash is not in Read Array mode 
    intel_Reset(pNorFlash, address);
   
    // Check the lock status is locked.
    status = intel_GetBlockLockStatus(pNorFlash, address);
    if(( status & INTEL_LOCKSTATUS_LOCKED ) == INTEL_LOCKSTATUS_LOCKED)
	{
        intel_UnlockSector(pNorFlash, address);
    }
    // Clear the status register first.
    intel_ClearStatus(pNorFlash);
    // Block erase operations are initiated by writing the Block Erase Setup command to the address of the block to be erased.
	intel_sendCmd8(baseAddress, address, INTEL_CMD_BLOCK_ERASE_1);
    // Next, the Block Erase Confirm command is written to the address of the block to be erased.
	intel_sendCmd8(baseAddress, address, INTEL_CMD_BLOCK_ERASE_2);
    // Status register polling 
    do
	{
        status = intel_ReadStatus(pNorFlash,address);
        // Check if the device is ready.
        if ((status & INTEL_STATUS_DWS) == INTEL_STATUS_DWS )
		{
            // check if VPP within acceptable limits during program or erase operation.
            if ((status & INTEL_STATUS_VPPS) == INTEL_STATUS_VPPS )
			{
                intel_Reset(pNorFlash, 0);
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // Check if the erase block operation is completed. 
            if ((status & INTEL_STATUS_PS) == INTEL_STATUS_PS )
			{
                intel_Reset(pNorFlash, 0);
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // Check if the erase block operation is completed. 
            if ((status & INTEL_STATUS_ES) == INTEL_STATUS_ES )
			{
                intel_Reset(pNorFlash, 0);
                return NorCommon_ERROR_CANNOTWRITE;
            }
            // check if Block locked during program or erase, operation aborted.
            else if ((status & INTEL_STATUS_BLS) == INTEL_STATUS_BLS )
			{
                intel_Reset(pNorFlash, 0);
                return NorCommon_ERROR_CANNOTWRITE;
            }
            else
			{
                done = 1;
            }
        }
    } while (!done);
    intel_Reset(pNorFlash, address);
    return 0;
}

/*****************************************************************************
*
* @description
* This function erases all the block of the device.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*
* @return	
*           returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t INTEL_EraseChip(NorFlash_T *pNorFlash)
{
    // Interl flash have no independent Chip-erase command.
    uint32_t i;
    uint32_t sectors;
    sectors = NorFlash_GetDeviceNumOfBlocks(pNorFlash);
    for (i = 0; i < sectors; i++)
	{
        if (INTEL_EraseSector(pNorFlash, NorFlash_GetDeviceSectorAddress(pNorFlash, i)))
		{
            return NorCommon_ERROR_CANNOTERASE;
        }
    }
    return 0;
}

/*****************************************************************************
*
* @description
* This function sends data to the NorFlash chip from the provided buffer.
*
* @param    
*           pNorFlash Pointer to an NorFlash instance.
*           address Start address offset to be wrote.
*           buffer Buffer where the data is stored.
*           size Number of bytes that will be written.
*
* @return	
*           returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t INTEL_Write_Data(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size)
{
    uint32_t i;
	
    for(i = 0; i < size; i++)
	{
        if(intel_Program(pNorFlash, address, buffer[i]))
		{
            return NorCommon_ERROR_CANNOTWRITE;
        }
        address ++;
    }
	
    return 0;
}
