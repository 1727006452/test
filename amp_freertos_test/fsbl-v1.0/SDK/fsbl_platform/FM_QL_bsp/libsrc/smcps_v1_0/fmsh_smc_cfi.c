/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_cfi.c
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
* This function dumps the Common Flash Interface Definition Table.
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
static void NorFlash_CFI_DumpConfigruation(NorFlashCFI_T *pNorFlashCFI)
{
    uint8_t i;
    
    TRACE_OUT(DEBUG_OUT, "Common Flash Interface Definition Table\n\r");
    TRACE_OUT(DEBUG_OUT, "Addr. Data   Description \n\r");
    TRACE_OUT(DEBUG_OUT, "0x10  %04Xh  Query Unique ASCII string\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.queryUniqueString[0]);
    TRACE_OUT(DEBUG_OUT, "0x11  %04Xh  \n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.queryUniqueString[1]);
    TRACE_OUT(DEBUG_OUT, "0x12  %04Xh  \n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.queryUniqueString[2]);
    TRACE_OUT(DEBUG_OUT, "0x13  %04Xh  Primary OEM Command Set\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.primaryCode);
    TRACE_OUT(DEBUG_OUT, "0x15  %04Xh  Address for Primary Extended Table\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.primaryAddr);
    TRACE_OUT(DEBUG_OUT, "0x17  %04Xh  Alternate OEM Command Set\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.alternateCode);
    TRACE_OUT(DEBUG_OUT, "0x19  %04Xh  Address for Alternate OEM Extended Table\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.alternateAddr);
    TRACE_OUT(DEBUG_OUT, "0x1B  %04Xh  VCC min write/erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minVcc);
    TRACE_OUT(DEBUG_OUT, "0x1C  %04Xh  VCC max write/erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxVcc);
    TRACE_OUT(DEBUG_OUT, "0x1D  %04Xh  VPP min voltage\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minVpp);
    TRACE_OUT(DEBUG_OUT, "0x1E  %04Xh  VPP max voltage\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxVpp);
    TRACE_OUT(DEBUG_OUT, "0x1F  %04Xh  Typical timeout per single word write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutWrite);
    TRACE_OUT(DEBUG_OUT, "0x20  %04Xh  Typical timeout for Min. size buffer write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutBuffer);
    TRACE_OUT(DEBUG_OUT, "0x21  %04Xh  Typical timeout per individual block erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutBlockErase);
    TRACE_OUT(DEBUG_OUT, "0x22  %04Xh  Typical timeout for full chip erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.minTimeOutChipErase);
    TRACE_OUT(DEBUG_OUT, "0x23  %04Xh  Max. timeout for word write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutWrite);
    TRACE_OUT(DEBUG_OUT, "0x24  %04Xh  Max. timeout for buffer write\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutBuffer);
    TRACE_OUT(DEBUG_OUT, "0x25  %04Xh  Max. timeout per individual block erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutBlockErase);
    TRACE_OUT(DEBUG_OUT, "0x26  %04Xh  Max. timeout for full chip erase\n\r",
    pNorFlashCFI->norFlashCfiQueryInfo.maxTimeOutChipErase);
    
    TRACE_OUT(DEBUG_OUT, "0x27  %04Xh  Device Size = 2^N byte\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.deviceSize);
    TRACE_OUT(DEBUG_OUT, "0x28  %04Xh  Flash Device Interface description\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.deviceInterface);
    TRACE_OUT(DEBUG_OUT, "0x2A  %04Xh  Max. number of byte in multi-byte write\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.numMultiWrite);
    TRACE_OUT(DEBUG_OUT, "0x2C  %04Xh  Number of Erase Block Regions within device\n\r",
    pNorFlashCFI->norFlashCfiDeviceGeometry.numEraseRegion);
    for(i = 0; i < pNorFlashCFI->norFlashCfiDeviceGeometry.numEraseRegion; i++) {
        TRACE_OUT(DEBUG_OUT, "0x%2X  %04Xh  Number of Erase Blocks of identical size within region %x \n\r",
            0x2D + i * 4, pNorFlashCFI->norFlashCfiDeviceGeometry.eraseRegionInfo[i].Y, i );
        TRACE_OUT(DEBUG_OUT, "0x%2X  %04Xh  (z) times 256 bytes within region %x \n\r",
            0x2E + i * 4, pNorFlashCFI->norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z, i );
    }
}

/*****************************************************************************
*
* @description
* This function returns the numbers of block in all Norflash regions.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           total block number.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetDeviceNumOfBlocks(NorFlash_T *pNorFlash)
{
    uint8_t i;
    uint32_t blocks = 0;
    uint16_t numBlockRegion;
    
    numBlockRegion = pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;
   
    for (i = 0; i < numBlockRegion; i++) {
        blocks += (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Y) + 1;
    }
    return blocks;
}

/*****************************************************************************
*
* @description
* This function returns the minimun block size in all Norflash regions.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           the minimun block size.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetDeviceMinBlockSize(NorFlash_T *pNorFlash)
{
    uint8_t i;
    uint16_t numBlockRegion;
    unsigned long size ;
    numBlockRegion = pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;
    
    size = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[0].Z) * 256;
    
    for (i = 1; i < numBlockRegion; i++) {
        if (size > (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256) {
            size = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
        }
    }
    return size;
}

/*****************************************************************************
*
* @description
* This function returns the maximun block size in all Norflash regions.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           the maximun block size.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetDeviceMaxBlockSize(NorFlash_T *pNorFlash)
{
    uint8_t i;
    uint16_t numBlockRegion;
    unsigned long size ;
    numBlockRegion = pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;
    
    size = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[0].Z) * 256;
    
    for (i = 1; i < numBlockRegion; i++) {
        if (size < (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256) {
            size = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
        }
    }
    return size;
}

/*****************************************************************************
*
* @description
* This function returns the block size in giving block number.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           sector  Sector number.
*
* @return	
*           block size.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetDeviceBlockSize(NorFlash_T *pNorFlash, uint32_t sector)
{
    uint16_t i;
    uint16_t j;
    uint16_t numBlockRegion,numBlockPerRegion;
    uint32_t block = 0;
    
    numBlockRegion = pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;
    for (i = 0; i < numBlockRegion; i++) {
        numBlockPerRegion = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;
        for (j = 0; j < numBlockPerRegion; j++) {
            if (block == sector) {
                return (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
            }
            block++;
        }
    }
    return 0;
}

/*****************************************************************************
*
* @description
* This function returns sector number on specified memory offset.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           memoryOffset  Memory offset.
*
* @return	
*           sector num.
*
* @note		NA.
*
*****************************************************************************/
uint16_t NorFlash_GetDeviceSectorInRegion(NorFlash_T *pNorFlash, uint32_t memoryOffset)
{
    uint16_t numBlockRegion,numBlockPerRegion;
    uint16_t sectorId = 0;
    uint32_t size = 0;
    uint8_t done = 0;
    uint16_t i , j;
    
    numBlockRegion = pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;
    
    for (i = 0; i < numBlockRegion; i++) {
        numBlockPerRegion = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;
        for (j = 0; j < numBlockPerRegion; j++) {
            size+= (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
            if(size > memoryOffset) {
                done = 1;
                break;
            }
            sectorId++;
        }
        if (done) break;
    }
    
    return sectorId;
}

/*****************************************************************************
*
* @description
* This function returns start address of specified sector number.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           sector  Sector number.
*
* @return	
*           sector address.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetDeviceSectorAddress(NorFlash_T *pNorFlash, uint32_t sector)
{
    uint16_t numBlockRegion,numBlockPerRegion;
    uint16_t sectorId = 0;
    uint32_t address = 0;
    uint8_t done = 0;
    uint16_t i , j;
    
    numBlockRegion = pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion;
    for (i = 0; i < numBlockRegion; i++) {
        numBlockPerRegion = (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i]).Y + 1;
        for (j = 0; j < numBlockPerRegion; j++) {
            if (sector == sectorId) {
                done = 1;
                break;
            }
            address+= (pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i].Z) * 256 ;
            sectorId++;
        }
        if (done) break;
    }
    
    return address;
}

/*****************************************************************************
*
* @description
* This function converts address to byte addressing.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           offset Address offset.
*
* @return	
*           byte address.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetByteAddress(NorFlash_T *pNorFlash, uint32_t offset)
{
    return (offset * pNorFlash-> deviceChipWidth);
}

/*****************************************************************************
*
* @description
* This function converts address to byte addressing and 
* return the address in chip.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           offset Address offset.
*
* @return	
*           address in chip.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetByteAddressInChip(NorFlash_T *pNorFlash, uint32_t offset)
{
	offset = ((2*offset) + (!(offset & 0x1)));
    return (pNorFlash->baseAddress + offset);
}

/*****************************************************************************
*
* @description
* This function returns the address in chip.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*           offset Address offset.
*
* @return	
*           address in chip.
*
* @note		NA.
*
*****************************************************************************/
uint32_t NorFlash_GetAddressInChip(NorFlash_T *pNorFlash, uint32_t offset)
{
    return (pNorFlash->baseAddress + offset);
}

/*****************************************************************************
*
* @description
* This function returns bus width in bits of giving device.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           bus width.
*
* @note		NA.
*
*****************************************************************************/
uint8_t NorFlash_GetDataBusWidth( NorFlash_T *pNorFlash)
{
    return (pNorFlash->deviceChipWidth * 8);
}

/*****************************************************************************
*
* @description
* This function returns the size of the whole device in bytes.
*
* @param    
*           pNorFlash  Pointer to an NorFlash instance.
*
* @return	
*           device size.
*
* @note		NA.
*
*****************************************************************************/
unsigned long  NorFlash_GetDeviceSizeInBytes(NorFlash_T *pNorFlash)
{
    return ((unsigned long) 2 << ((pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.deviceSize) - 1));
}

/*****************************************************************************
*
* @description
* This function looks for query struct in Norflash common flash interface.
*
* @param    
*           pNorFlash Pointer to an pNorFlash instance.
*           hardwareBusWidth the bus width.
*
* @return	
*           returns 0 if a matching model has been found.
*           otherwise returns an error code.
*
* @note		NA.
*
*****************************************************************************/
uint8_t NorFlash_CFI_Detect(NorFlash_T *pNorFlash,  uint8_t hardwareBusWidth)
{
    uint8_t i;
    uint8_t pCfi[256];
    //uint8_t *pCfi = (uint8_t*)(&(pNorFlash->cfiDescription));
    uint32_t baseAddress, CfiQryAddr, cmd_addr;
	
    pNorFlash->cfiCompatible = 0;
    pNorFlash->deviceChipWidth = hardwareBusWidth;
	baseAddress = pNorFlash->baseAddress;
    CfiQryAddr = CFI_QUERY_OFFSET;
	cmd_addr = baseAddress + CFI_QUERY_ADDRESS * 2;
	
    for(i = 0; i< sizeof(NorFlash_T) ; i++){
		NOR_WRITE_COMMAND8(cmd_addr, CFI_QUERY_COMMAND);
		ReadRawData((baseAddress + (CfiQryAddr << 1)), &pCfi[i]);
        CfiQryAddr++;
    }
	
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.queryUniqueString[0] = pCfi[0];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.queryUniqueString[1] = pCfi[1];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.queryUniqueString[2] = pCfi[2];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.primaryCode = pCfi[4]<<8 | pCfi[3];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.primaryAddr = pCfi[6]<<8 | pCfi[5];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.alternateCode = pCfi[8]<<8 | pCfi[7];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.alternateAddr = pCfi[10]<<8 | pCfi[9];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.minVcc = pCfi[11];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.maxVcc = pCfi[12];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.minVpp = pCfi[13];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.maxVpp = pCfi[14];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.minTimeOutWrite = pCfi[15];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.minTimeOutBuffer = pCfi[16];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.minTimeOutBlockErase = pCfi[17];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.minTimeOutChipErase = pCfi[18];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.maxTimeOutWrite = pCfi[19];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.maxTimeOutBuffer = pCfi[20];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.maxTimeOutBlockErase = pCfi[21];
    pNorFlash->cfiDescription.norFlashCfiQueryInfo.maxTimeOutChipErase = pCfi[22];
    pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.deviceSize =  pCfi[23];

    pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.deviceInterface =  pCfi[25]<<8 | pCfi[24];
    pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numMultiWrite = pCfi[27]<<8 | pCfi[26];
    pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.numEraseRegion = pCfi[28];
    for (i = 0; i< 16*4; i+=4) {
        pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i>>2].Y = pCfi[30+i]<<8 | pCfi[29+i];
        pNorFlash->cfiDescription.norFlashCfiDeviceGeometry.eraseRegionInfo[i>>2].Z = pCfi[32+i]<<8 | pCfi[31+i];
    }
    // Check the query-unique ASCII string "QRY"
    if( (pNorFlash->cfiDescription.norFlashCfiQueryInfo.queryUniqueString[0] != 'Q' )
        || (pNorFlash->cfiDescription.norFlashCfiQueryInfo.queryUniqueString[1] != 'R') 
        || (pNorFlash->cfiDescription.norFlashCfiQueryInfo.queryUniqueString[2] != 'Y') ) {
        return NorCommon_ERROR_UNKNOWNMODEL;
    }
    
    NorFlash_CFI_DumpConfigruation(&(pNorFlash->cfiDescription));

    if ((pNorFlash->cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_AMD) ||
		(pNorFlash->cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_AMD_EXT)) {
        pNorFlash->cmdSet = CMD_SET_AMD;
    }
    else if ((pNorFlash->cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_INTEL_EXT) || 
             (pNorFlash->cfiDescription.norFlashCfiQueryInfo.primaryCode == CMD_SET_INTEL)) {
        pNorFlash->cmdSet = CMD_SET_INTEL;
    }
    else {
        return NorCommon_ERROR_UNKNOWNMODEL;
    }
    
    pNorFlash->cfiCompatible = 1;
    NORFLASH_Reset(pNorFlash, 0);
    return 0;
}

