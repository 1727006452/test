/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_amd.c
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
 
/** Command for vendor command set CMD_SET_AMD. */
#define AMD_CMD_RESET         0x00F0 /* Reset or read mode */
#define AMD_CMD_CFI           0x0098 /* CFI command */
#define AMD_CMD_AUTO_SELECT   0x0090 /* Auto select command */
#define AMD_CMD_UNLOCK_1      0x00AA /* Unlock cycle 1 */
#define AMD_CMD_UNLOCK_2      0x0055 /* Unlock cycle 2 */
#define AMD_CMD_ERASE_SETUP   0x0080 /* Setup erase */
#define AMD_CMD_ERASE_RESUME  0x0030 /* Resume erase */
#define AMD_CMD_ERASE_CHIP    0x0010 /* Chip erase command */
#define AMD_CMD_ERASE_SECTOR  0x0030 /* Sector erase command */
#define AMD_CMD_PROGRAM       0x00A0 /* Program command */
#define AMD_CMD_UNLOCK_BYPASS 0x0020 /* Unlock bypass command */

/** Command offset for vendor command set CMD_SET_AMD */
#define AMD_OFFSET_UNLOCK_1   0x0555
#define AMD_OFFSET_UNLOCK_2   0x02AA
/** Query command address. */
#define FLASH_ADDRESS_CFI     0x0055

/** AMD norflash device Identifier infomation address offset. */
#define AMD_MANU_ID           0x00
#define AMD_DEVIDE_ID         0x01

/** Data polling mask for vendor command set CMD_SET_AMD */
#define AMD_POLLING_DQ7       0x80
#define AMD_POLLING_DQ6       0x60
#define AMD_POLLING_DQ5       0x20
#define AMD_POLLING_DQ3       0x08

#define DQ1_MASK   (0x02)  /* DQ1 mask for all interleave devices */
#define DQ2_MASK   (0x04)  /* DQ2 mask for all interleave devices */
#define DQ3_MASK   (0x08 * )  /* DQ3 mask for all interleave devices */
#define DQ5_MASK   (0x20)  /* DQ5 mask for all interleave devices */
#define DQ6_MASK   (0x40)  /* DQ6 mask for all interleave devices */
//#define DQ6_TGL_DQ1_MASK (dq6_toggles >> 5)   /* Mask for DQ1 when device DQ6 toggling */
//#define DQ6_TGL_DQ5_MASK (dq6_toggles >> 1)   /* Mask for DQ5 when device DQ6 toggling */

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
* @return	NA.
*
* @note		NA.
*
*****************************************************************************/
static void amd_sendCmd8(u32 BaseAddr, u32 Offset, u32 Cmd)
{
	Offset = ((2*Offset) + (!(Offset & 0x1)));
	NOR_WRITE_COMMAND8(((volatile u8*)BaseAddr) + Offset, Cmd);
}

/*****************************************************************************
*
* @description
* Implements a RESET command.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           address  Dummy data for AMD.
*
* @return	NA.
*
* @note		NA.
*
*****************************************************************************/
static void amd_Reset(NorFlash_T *pNorFlash)
{
	amd_sendCmd8(pNorFlash->baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(pNorFlash->baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
	amd_sendCmd8(pNorFlash->baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_RESET);
}

/*****************************************************************************
*
* @description
* Read specified manufactory id or device id.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*		    FMSH_SUCCESS if read id is performed.
*
* @note		NA.
*
*****************************************************************************/
static uint32_t amd_ReadIdentification(NorFlash_T *pNorFlash)
{
    uint32_t baseAddress = pNorFlash->baseAddress;
    
    // The amd_Read identification command sequence is initiated by first
    // writing two unlock cycles. 
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
                 
    // Followed by a third write cycle that contains the autoselect command.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_AUTO_SELECT);
    
    // The device then enters the autoselect mode. It may read at any address any 
    // number of times without initiating another autoselect command sequence. 
	ReadRawData((baseAddress + (AMD_MANU_ID << 1)), (uint8_t*)&pNorFlash->manufacture_id); 
	ReadRawData((baseAddress + (AMD_DEVIDE_ID << 1)), (uint8_t*)&pNorFlash->device_id1);

    // The system must write the exit command to return to the read mode
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_RESET);
	
    return FMSH_SUCCESS;
}

/*****************************************************************************
*
* @description
* Toggle the DQ6 bit to check whether the embedded program or 
* erase algorithm is in process or complete.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           Offset is the offset into the device(s) address space on which
*		    command is required to be written.
*
* @return	
*		    NorCommon_ERROR_NONE if operation is complete.
*		    NorCommon_ERROR_TIMEOUT if operation is timeout.
*
* @note		NA.
*
*****************************************************************************/
static uint8_t amd_pollByToggle(NorFlash_T *pNorFlash, u32 offset)
{
	u8 StatusReg1;
	u8 StatusReg2;
	u32 busAddress;
  	u32 polling_counter = 0xFFFFFFFF;

	busAddress = pNorFlash->baseAddress + offset;
	while(polling_counter--)
	{
		// Read first status.
		ReadRawData(busAddress, &StatusReg1);
		// Read second status
		ReadRawData(busAddress, &StatusReg2);
		
		//If DQ6 did not toggle between two reads, operation is complete.
		if((StatusReg1 & DQ6_MASK) == (StatusReg2 & DQ6_MASK))
		{
			// DQ6 == NO Toggle.
			return (NorCommon_ERROR_NONE);
		}
		else
		{
			// If DQ5 is zero then operation is not yet complete.
			if((StatusReg2 & DQ5_MASK) != DQ5_MASK)
				continue;
			// If DQ1 is zero then operation is not yet complete.
			if((StatusReg2 & DQ1_MASK) != DQ1_MASK)
				continue;
		}
		
		// Else (DQ5 == 1 or DQ1 == 1), read DQ6 again.
		ReadRawData(busAddress, &StatusReg1);
		ReadRawData(busAddress, &StatusReg2);

		// If DQ6 did not toggle between two reads, operation is complete.
		if((StatusReg1 & DQ6_MASK) == (StatusReg2 & DQ6_MASK))
		{
			// DQ6 == NO Toggle.
			return (NorCommon_ERROR_NONE);
		}
		else
		{
			// Else toggle fail, DQ5 == 1, operation is timeout.
			if((StatusReg2 & DQ5_MASK) == DQ5_MASK)
				return (NorCommon_ERROR_TIMEOUT);
			
			// Else toggle fail, DQ1 == 1, operation is timeout.
			if((StatusReg2 & DQ1_MASK) == DQ1_MASK)
				return (NorCommon_ERROR_TIMEOUT);
		}
		
	}
	
	return (NorCommon_ERROR_TIMEOUT);
}

/*****************************************************************************
*
* @description
* It implement a program word command.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           address Start address offset to be wrote.
*           data word to be written.
*
* @return	
*           Returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
static uint8_t amd_Program(NorFlash_T *pNorFlash, uint32_t address, uint32_t data)
{
    uint32_t busAddress;
    uint32_t baseAddress = pNorFlash->baseAddress;
	u8 status;
    
    // The program command sequence is initiated by writing two unlock write cycles.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_PROGRAM);
                 
    // The program address and data are written next, 
    // which in turn initiate the Embedded Program algorithm.
    busAddress = baseAddress + address;
    WriteRawData(busAddress, (uint8_t*)&data);

	status = amd_pollByToggle(pNorFlash, address);

	return status;
}
 
/*****************************************************************************
*
* @description
* This function implements a RESET command (amd command).
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void AMD_Reset(NorFlash_T *pNorFlash)
{
    amd_Reset(pNorFlash);
}

/*****************************************************************************
*
* @description
* This function instructs the device to output device id
* by Read Device Identifier command.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           Returns 0 if the operation was successful.
*
* @note		NA.
*
*****************************************************************************/
uint32_t AMD_ReadDeviceID(NorFlash_T *pNorFlash)
{
    return amd_ReadIdentification(pNorFlash);
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
uint8_t AMD_EraseSector(NorFlash_T *pNorFlash, uint32_t address)
{
    uint32_t busAddress;
    uint32_t baseAddress = pNorFlash->baseAddress;
	u8 status;
    
    //Programming is a six-bus-cycle operation. 
    // The erase command sequence is initiated by writing two unlock write cycles.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
    // Followed by the program set-up command.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_ERASE_SETUP);
    // Two additional unlock cycles are written.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
        
    // Followed by the address of the sector to be erased, and the sector erase command.
    busAddress = baseAddress + address;
    WriteCommand(busAddress, AMD_CMD_ERASE_SECTOR);

	status = amd_pollByToggle(pNorFlash, address);

	return status;
}

/*****************************************************************************
*
* @description
* This function erases all the block of the device.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           Returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t AMD_EraseChip(NorFlash_T *pNorFlash)
{
    uint32_t baseAddress = pNorFlash->baseAddress;
	u8 status;
    
    //Programming is a six-bus-cycle operation. 
    // The erase command sequence is initiated by writing two unlock write cycles.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
    // Followed by the program set-up command.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_ERASE_SETUP);
    // Two additional unlock cycles are written.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_UNLOCK_1);
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_2, AMD_CMD_UNLOCK_2);
                 
    // Then followed by the chip erase command.
	amd_sendCmd8(baseAddress, AMD_OFFSET_UNLOCK_1, AMD_CMD_ERASE_CHIP);

	status = amd_pollByToggle(pNorFlash, 0);

	return status;
}

/*****************************************************************************
*
* @description
* This function sends data to the NorFlash chip from the provided buffer.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           address Start address offset to be wrote.
*		    buffer Buffer where the data is stored.
*           size Number of bytes that will be written.
*
* @return	
*           Returns 0 if the operation was successful.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t AMD_Write_Data(NorFlash_T *pNorFlash, uint32_t address, uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    
    for(i = 0; i < size; i++)
	{
        if(amd_Program(pNorFlash, address, buffer[i]))
		{
            return NorCommon_ERROR_CANNOTWRITE;
        }
        address ++;
    }

    return 0;
}
