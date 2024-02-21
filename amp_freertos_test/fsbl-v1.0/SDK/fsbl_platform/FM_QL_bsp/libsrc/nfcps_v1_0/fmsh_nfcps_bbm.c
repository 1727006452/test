/******************************************************************************
*
* Copyright (C) FMSH, Corp.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* FMSH BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the FMSH shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from FMSH.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file fmsh_nfcps_bbm.c
* @addtogroup nfcps_v1_0
* @{
*
* Contains implements the interface functions to support bad block management.
* See fmsh_nfcps_bbm.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 11/26/20 
*            API has been modified to support assign data buffer dynamically.
* 1.02  hzq 2022/02/25
*   		 Fix bug that the position of isBadBlockWrite in FNfcPs_WriteBadBlockTable
*            does not cover whole function
* </pre>
*
******************************************************************************/

#include <string.h>
#include "fmsh_nfcps_lib.h"

static int FNfcPs_ReadBadBlockTable(FNfcPs_T* nfc);
static int FNfcPs_SearchBadBlockTable(FNfcPs_T* nfc, FNfcPs_BbtDesc_T* desc);
static int FNfcPs_WriteBadBlockTable(FNfcPs_T* nfc, FNfcPs_BbtDesc_T* Desc, FNfcPs_BbtDesc_T* MirrorDesc);
static int FNfcPs_UpdateBadBlockTable(FNfcPs_T* nfc);
static int FNfcPs_MarkBadBlockTable(FNfcPs_T* nfc, FNfcPs_BbtDesc_T* Desc);

/**
* This function initializes the Bad Block Table(BBT) descriptors with a
* predefined pattern for searching Bad Block Table(BBT) in flash.
*/
void FNfcPs_InitBadBlockTableDesc(FNfcPs_T* nfc)
{ 
    int index;
    
    FMSH_ASSERT(nfc != NULL);

    /* Initialize primary Bad Block Table(BBT) */
    nfc->bbtDesc.blockOffset = NANDFLASH_BBT_BLOCK_OFFSET;
    if (nfc->eccMode == NFCPS_ECC_ONDIE) 
    {
        nfc->bbtDesc.sigOffset = 0x4;
        nfc->bbtDesc.verOffset = 0x14;
    } 
    else 
    {
        nfc->bbtDesc.sigOffset = NANDFLASH_BBT_SIG_OFFSET;
        nfc->bbtDesc.verOffset = NANDFLASH_BBT_VER_OFFSET;
    }
    nfc->bbtDesc.sigLength = NANDFLASH_BBT_SIG_LEN;
    nfc->bbtDesc.maxBlocks = NANDFLASH_BBT_MAX_BLOCKS;
    strcpy(&nfc->bbtDesc.signature[0], "bbt0");
    nfc->bbtDesc.version = 0;
    nfc->bbtDesc.valid = 0;
    
    /* Initialize mirror Bad Block Table(BBT) */
    nfc->bbtMirrorDesc.blockOffset = NANDFLASH_BBT_BLOCK_OFFSET;
    if (nfc->eccMode == NFCPS_ECC_ONDIE) 
    {
        nfc->bbtMirrorDesc.sigOffset = 0x4;
        nfc->bbtMirrorDesc.verOffset = 0x14;
    } 
    else 
    {
        nfc->bbtMirrorDesc.sigOffset = NANDFLASH_BBT_SIG_OFFSET;
        nfc->bbtMirrorDesc.verOffset = NANDFLASH_BBT_VER_OFFSET;
    }
    nfc->bbtMirrorDesc.sigLength = NANDFLASH_BBT_SIG_LEN;
    nfc->bbtMirrorDesc.maxBlocks = NANDFLASH_BBT_MAX_BLOCKS;
    strcpy(&nfc->bbtMirrorDesc.signature[0], "1tbb");
    nfc->bbtMirrorDesc.version = 0;
    nfc->bbtMirrorDesc.valid = 0;
    
    /* Initialize Bad block search pattern structure */
    if (nfc->model.PageSizeInBytes > 512) 
    {
        /* For flash page size > 512 bytes */
        nfc->bbPattern.options = NANDFLASH_BBT_SCAN_2ND_PAGE;
        nfc->bbPattern.offset = NANDFLASH_BB_PATTERN_OFFSET_LARGE_PAGE;
        nfc->bbPattern.length = NANDFLASH_BB_PATTERN_LENGTH_LARGE_PAGE;
    } 
    else 
    {
        nfc->bbPattern.options = NANDFLASH_BBT_SCAN_2ND_PAGE;
        nfc->bbPattern.offset = NANDFLASH_BB_PATTERN_OFFSET_SMALL_PAGE;
        nfc->bbPattern.length = NANDFLASH_BB_PATTERN_LENGTH_SMALL_PAGE;
    }
    for(index=0; index < 2; index++) 
    {
        nfc->bbPattern.pattern[index] = NANDFLASH_BB_PATTERN;
    }
}

/** This function scans the NAND flash for factory marked bad blocks and creates
*  a RAM based Bad Block Table(BBT).
*/
static int FNfcPs_CreateBadBlockTable(FNfcPs_T* nfc)
{  
    int error;
    
    u32 blockIndex; /* loop all blocks in flash */
    u32 blockOffset;
    u32 blockShift;
    u32 bbInfoLen;
    u32 bbMapLen;
    bbInfoLen = (nfc->model.BlockNum) >> 2;
    bbMapLen = (nfc->model.maxBadBlockNum) << 2;
    memset(nfc->bbInfo, 0xff, bbInfoLen);
    memset(nfc->bbMap, 0xffffffff, bbMapLen);
    
    /* scan all blocks for factory marked bad blocks */
    for(blockIndex = 0; blockIndex < nfc->model.BlockNum; blockIndex++)
    {
        /* Block offset in Bad Block Table(BBT) entry */
        blockOffset = blockIndex >> 2;
        /* Block shift value in the byte */
        blockShift = (blockIndex << 1) & 0x6;
        /* Search for the bad block pattern */
        error = FNfcPs_CheckBlock(nfc, blockIndex);
        if(error)
        {
            /* Marking as bad block (bbInfo[]) */
            nfc->bbInfo[blockOffset] &= ((~(NANDPS_BLOCK_TYPE_MASK << blockShift)) |
                                         (NANDPS_BLOCK_FACTORY_BAD << blockShift));   
        }
    }
    
    return FMSH_SUCCESS;
}

/**
* This function reads the Bad Block Table(BBT) if present in flash. If not it
* scans the flash for detecting factory marked bad blocks and creates a bad
* block table and write the Bad Block Table(BBT) into the flash.
*/
int FNfcPs_ScanBadBlockTable(FNfcPs_T* nfc)
{
    int error;
    
    FMSH_ASSERT(nfc != NULL);
    
    error = FNfcPs_ReadBadBlockTable(nfc);
    if (error) 
    {
        /* Create memory based Bad Block Table(BBT) */
        FNfcPs_CreateBadBlockTable(nfc);
        /* Write the Bad Block Table(BBT) to the flash */
        error = FNfcPs_WriteBadBlockTable(nfc, &nfc->bbtDesc, &nfc->bbtMirrorDesc);
        if (error ) 
        {
            return FMSH_FAILURE;
        }
        /* Write the Mirror Bad Block Table(BBT) to the flash */
        error = FNfcPs_WriteBadBlockTable(nfc, &nfc->bbtMirrorDesc, &nfc->bbtDesc);
        if (error ) 
        {
            return FMSH_FAILURE;
        }
        /* Mark the blocks containing Bad Block Table(BBT) as Reserved */
        FNfcPs_MarkBadBlockTable(nfc, &nfc->bbtDesc);
        FNfcPs_MarkBadBlockTable(nfc, &nfc->bbtMirrorDesc);
    } 
    return FMSH_SUCCESS;
}

/**
* This function searches the Bad Bloock Table(BBT) in flash and loads into the
* memory based Bad Block Table(BBT).
*/
static int FNfcPs_ReadBadBlockTable(FNfcPs_T* nfc)
{
    int error, error1, error2;
    FNfcPs_BbtDesc_T*  desc;
    FNfcPs_BbtDesc_T*  mirrorDesc;
    u8* bbInfo;
    u8* bbMap;
    u32 bbInfoLen;  
    u32 bbMapLen;
    u32 blockSize;
    u32 pageSize;
    u64 bbInfoAddr, bbMapAddr; /* bad block table address */
    
    desc = &(nfc->bbtDesc);
    mirrorDesc = &(nfc->bbtMirrorDesc);
    blockSize = FNfcPs_GetBlockSizeInBytes(nfc->model);
    pageSize = FNfcPs_GetPageSizeInBytes(nfc->model);
    bbInfo = (u8*)nfc->bbInfo;
    bbMap = (u8*)nfc->bbMap;
    bbInfoLen = (nfc->model.BlockNum) >> 2;
    bbMapLen = (nfc->model.maxBadBlockNum) << 2;
    
    /* Search the Bad Block Table(BBT) in flash & config desc */
    error1 = FNfcPs_SearchBadBlockTable(nfc, desc);
    error2 = FNfcPs_SearchBadBlockTable(nfc, mirrorDesc);
    if (error1 && error2) 
    {
        return FMSH_FAILURE;
    }
    
    if (desc->valid && mirrorDesc->valid)
    {
        /* Valid BBT & Mirror BBT found load newer bbt*/
        if (desc->version > mirrorDesc->version) 
        {
            bbInfoAddr = (desc->blockOffset * blockSize);
            bbMapAddr = ((desc->blockOffset+1) * blockSize) - pageSize;
            FNfcPs_Read(nfc, bbInfoAddr, bbInfoLen, bbInfo, 0);
            FNfcPs_Read(nfc, bbMapAddr, bbMapLen, bbMap, 0);
            /* updata mirror BBT, Write the BBT to Mirror BBT location in flash */
            mirrorDesc->version = desc->version;
            error = FNfcPs_WriteBadBlockTable(nfc, mirrorDesc, desc);
            if (error) 
            {
                return error;
            }
        } 
        else if (desc->version < mirrorDesc->version) 
        {
            bbInfoAddr = (mirrorDesc->blockOffset * blockSize);
            bbMapAddr = ((desc->blockOffset+1) * blockSize) - pageSize;
            FNfcPs_Read(nfc, bbInfoAddr, bbInfoLen, bbInfo, 0);
            FNfcPs_Read(nfc, bbMapAddr, bbMapLen, bbMap, 0);
            /* updata BBT, Write the Mirror BBT to BBT location in flash */
            desc->version = mirrorDesc->version;
            error = FNfcPs_WriteBadBlockTable(nfc, desc, mirrorDesc);
            if(error ) 
            {
                return error;
            }
        } 
        else 
        {
            bbInfoAddr = (desc->blockOffset * blockSize);
            bbMapAddr = ((desc->blockOffset+1) * blockSize) - pageSize;
            /* Both are up-to-date */
            FNfcPs_Read(nfc, bbInfoAddr, bbInfoLen, bbInfo, 0);
            FNfcPs_Read(nfc, bbMapAddr, bbMapLen, bbMap, 0);    
        }
    }
    else if (desc->valid)
    {
        bbInfoAddr = (desc->blockOffset * blockSize);
        bbMapAddr = ((desc->blockOffset+1) * blockSize) - pageSize;
        /* Valid Primary BBT found */
        FNfcPs_Read(nfc, bbInfoAddr, bbInfoLen, bbInfo, 0);
        FNfcPs_Read(nfc, bbMapAddr, bbMapLen, bbMap, 0);   
        /* Write the BBT to Mirror BBT location in flash */
        mirrorDesc->version = desc->version;
        error = FNfcPs_WriteBadBlockTable(nfc, mirrorDesc, desc);
        if(error) 
        {
            return error;
        }
    } 
    else 
    {
        bbInfoAddr = (mirrorDesc->blockOffset * blockSize);
        bbMapAddr = ((desc->blockOffset+1) * blockSize) - pageSize;
        FNfcPs_Read(nfc, bbInfoAddr, bbInfoLen, bbInfo, 0);
        FNfcPs_Read(nfc, bbMapAddr, bbMapLen, bbMap, 0);
        /* Write the Mirror BBT to BBT location in flash */
        desc->version = mirrorDesc->version;
        error = FNfcPs_WriteBadBlockTable(nfc, desc, mirrorDesc);
        if(error) 
        {
            return error;
        }
    }
    return FMSH_SUCCESS;
}

/**
* This function searches the BBT in flash.
*/
static int FNfcPs_SearchBadBlockTable(FNfcPs_T* nfc, FNfcPs_BbtDesc_T* desc)
{
    int error;  
    u32 maxBlocks;
    u32 maxBlockNum;
    u32 blockIndex;
    u32 block;
    u32 sigOffset;
    u32 sigLength;
    u32 verOffset;
    u32 offset;
    u8* spare = nfc->spareBufferPtr;
    
    maxBlocks = desc->maxBlocks;
    maxBlockNum = nfc->model.BlockNum - 1; 
    sigOffset = desc->sigOffset;
    sigLength = desc->sigLength;
    verOffset = desc->verOffset;
    
    /* Read the last 3 blocks for Bad Block Table(BBT) signature */
    for(blockIndex = 0; blockIndex < maxBlocks; blockIndex++) 
    {
        block = maxBlockNum - blockIndex;
        error = Nandflash_ReadPage(nfc, block, 0, 0, 0, spare);
        if (error) 
        {
            continue;
        }
        
        /* Check the Bad Block Table(BBT) signature */
        for(offset=0; offset<sigLength; offset++) 
        {
            if (spare[sigOffset + offset] != desc->signature[offset])
            {
                /* Check the next blocks */
                break;  
            }
        }
        if (offset >= sigLength) 
        {
            /* Bad Block Table(BBT) found */
            desc->blockOffset = block;
            desc->version = spare[verOffset];
            desc->valid = 1;
            return FMSH_SUCCESS;
        }
    }
    
    /* Bad Block Table(BBT) not found */
    return FMSH_FAILURE;
}

/**
* This function writes Bad Block Table(BBT)(Include bbInfo) from RAM to flash.
* save at 1st page in block
*/
static int FNfcPs_WriteBadBlockTable(FNfcPs_T* nfc, FNfcPs_BbtDesc_T* desc, FNfcPs_BbtDesc_T* mirrorDesc)
{
    int error;
    
    u32 index; 
    u32 blockNum; /* total number of blocks in flash */
    u32 block; /* block which write bad block info to */
    u32 blockOffset; 
    u32 blockShift;
    u8 blockType;
    u32 bbInfoLen; /* number of bad block info bytes */
    u32 bbMapLen;
    u8* bbInfo;
    u8* bbMap;
    u32 blockSize;
    u32 pageSize;
    u8* spare = nfc->spareBufferPtr;
    if(nfc->isBadBlockWrite)
    {
        bbInfo = (u8*)nfc->bbInfo;
        bbMap = (u8*)nfc->bbMap;
        bbInfoLen = (nfc->model.BlockNum) >> 2;
        bbMapLen = (nfc->model.maxBadBlockNum) << 2;
        blockSize = FNfcPs_GetBlockSizeInBytes(nfc->model);
        pageSize = FNfcPs_GetPageSizeInBytes(nfc->model);
        
        /* start from the last block*/
        blockNum = nfc->model.BlockNum - 1;
        /* Find a good block to write the Bad Block Table(BBT) */
        if (!desc->valid) 
        {
            for(index=0; index<desc->maxBlocks; index++) 
            {
                block  = (blockNum - index);
                /* one byte contain 4 bad block info*/  
                blockOffset = block >> 2;
                /* each bad block info occupy 2 bits, offset0,2,4,6 */
                blockShift = (block << 1) & 0x6;
                blockType = (nfc->bbInfo[blockOffset] >> blockShift) & NANDPS_BLOCK_TYPE_MASK;
                switch(blockType)
                {
                case NANDPS_BLOCK_BAD:
                case NANDPS_BLOCK_FACTORY_BAD:
                    continue;
                default: /* Good Block */
                    break;
                }
                /* good block found */
                desc->blockOffset = block;
                
                if (desc->blockOffset != mirrorDesc->blockOffset) 
                {
                    /* Free block found */
                    desc->valid = 1;
                    break;
                }
            }
            
            /* Block not found for writing Bad Block Table(BBT) */
            if (index >= desc->maxBlocks) 
            {
                return FMSH_FAILURE;
            }
        } 
        else 
        {
            block = desc->blockOffset;
        }
        
        /* Write the signature and version in the spare data area */
        memset(spare, 0xff, nfc->model.SpareSizeInBytes);
        memcpy(spare + desc->sigOffset, &desc->signature[0], desc->sigLength);
        memcpy(spare + desc->verOffset, &desc->version, 1);
        
        /* Write the BBT to page offset */
        error = Nandflash_EraseBlock(nfc, block);
        if (error) 
        {
            return FMSH_FAILURE;
        }
        error = Nandflash_WritePage(nfc, desc->blockOffset, 0, 0, 0, spare);
        if (error) 
        {
            return FMSH_FAILURE;
        }
        error = FNfcPs_Write(nfc, desc->blockOffset * blockSize, bbInfoLen, bbInfo, 0);
        if (error) 
        {
            return FMSH_FAILURE;
        }
        error = FNfcPs_Write(nfc, (desc->blockOffset+1) * blockSize - pageSize, bbMapLen, bbMap, 0);
        if (error) 
        {
            return FMSH_FAILURE;
        }
    }
    return FMSH_SUCCESS;
}

/**
* This function marks the block containing Bad Block Table as reserved
* and updates the BBT.
*/
static int FNfcPs_MarkBadBlockTable(FNfcPs_T* nfc, FNfcPs_BbtDesc_T* desc)
{
    int error;
    u32 blockNum;
    u32 blockOffset;
    u32 blockShift;
    u32 blockIndex;
    u32 block;
    u8 oldVal, newVal;
    u32 updateBbt = 0;
    
    /* Mark the last 3 blocks as Reserved */
    blockNum = nfc->model.BlockNum - 1;
    for(blockIndex=0; blockIndex<desc->maxBlocks; blockIndex++,block++) 
    {
        block = blockNum - blockIndex;
        blockOffset = block >> NANDPS_BBT_BLOCK_SHIFT;
        blockShift = (blockIndex << 1) & 0x6;
        oldVal = nfc->bbInfo[blockOffset];
        newVal = oldVal & 
            ((~(NANDPS_BLOCK_TYPE_MASK << blockShift)) |
             (NANDPS_BLOCK_RESERVED << blockShift));
        nfc->bbInfo[blockOffset] = newVal;
        
        if (oldVal != newVal) 
        {
            updateBbt = 1;
        }
    }
    /* Update the BBT to flash */
    if (updateBbt) 
    {
        error = FNfcPs_UpdateBadBlockTable(nfc);
        if (error) 
        {
            return FMSH_FAILURE;
        }
    }
    return FMSH_SUCCESS;
}

/**
* This function updates the primary and mirror Bad Block Table(BBT) in the flash.
*/
static int FNfcPs_UpdateBadBlockTable(FNfcPs_T* nfc)
{
    int error;
    
    u8 version;
    
    /* Update the version number */
    version = nfc->bbtDesc.version;
    nfc->bbtDesc.version = (version + 1) % 256;
    version = nfc->bbtMirrorDesc.version;
    nfc->bbtMirrorDesc.version = (version + 1) % 256;
    
    /* Update the primary Bad Block Table(BBT) in flash */
    error = FNfcPs_WriteBadBlockTable(nfc, &nfc->bbtDesc, &nfc->bbtMirrorDesc);
    if (error) 
    {
        return error;
    }
    /* Update the mirrored Bad Block Table(BBT) in flash */
    error = FNfcPs_WriteBadBlockTable(nfc, &nfc->bbtMirrorDesc, &nfc->bbtDesc);
    if (error) 
    {
        return error;
    }
    return FMSH_SUCCESS;
}

int FNfcPs_CheckBlock(FNfcPs_T* nfc, u16 block) 
{
    int error;
    u32 numPages;
    u32 pageIndex; /* Page index which stores bad block pattern */
    u32 length; /* Pattern length index */
    
    FMSH_ASSERT(nfc != NULL);
    
    u8* spare = nfc->spareBufferPtr;
    
    /* Number of pages to search for bad block pattern */
    if (nfc->bbPattern.options & NANDFLASH_BBT_SCAN_2ND_PAGE)
    {
        numPages = 2;
    } 
    else 
    {
        numPages = 1;
    }
    /* Search for the bad block pattern */
    for(pageIndex=0; pageIndex<numPages; pageIndex++) 
    {
        error = Nandflash_ReadPage(nfc, block, pageIndex, 0, 0, spare);
        if (error) 
        {
            return FMSH_FAILURE;
        }
        
        /* Read the spare bytes to check for bad block pattern */
        for(length = 0; length < nfc->bbPattern.length; length++) 
        {
            if (spare[nfc->bbPattern.offset + length] !=
                nfc->bbPattern.pattern[length])
            {
                /* Bad block found, return error to marking as bad block */
                return FMSH_FAILURE;
            }
        }
    }
    return FMSH_SUCCESS;
}

/**
* This function marks a block as bad in the RAM based Bad Block Table(nfc->bbInfo[]). It
* also updates the Bad Block Table(BBT) in the flash.
*/
int FNfcPs_MarkBlockBad(FNfcPs_T* nfc, u32 block) 
{
    int error; 
    
    u8 numPages;
    u8 pageIndex;
    u32 blockOffset;/* byte offset in bbt contain bad block info */
    u32 blockShift;/* shift in byte save bad block info */
    u8 data; /* contain bad block info for this block */
    u8 oldVal, newVal;
    
    FMSH_ASSERT(nfc != NULL);
    
    u8* spare = nfc->spareBufferPtr;
    
    if(block >= nfc->model.BlockNum)
    {
        return FMSH_FAILURE;
    }
    
    /* write bad block pattern to block spare */ 
    memset(spare, 0xff, nfc->model.SpareSizeInBytes);
    memset(&spare[nfc->bbPattern.offset], 0xBA, nfc->bbPattern.length);
    if (nfc->bbPattern.options & NANDFLASH_BBT_SCAN_2ND_PAGE)
    {
        numPages = 2;
    } 
    else 
    {
        numPages = 1;
    }
    for(pageIndex=0; pageIndex<numPages; pageIndex++) 
    {
        error = Nandflash_WritePage(nfc, block, pageIndex, 0, 0, spare);
        if (error) 
        {
            return FMSH_FAILURE;
        }
    }     
    /* one byte contain 4 bad block info*/
    blockOffset = block >> NANDPS_BBT_BLOCK_SHIFT;
    data = nfc->bbInfo[blockOffset];
    /* each bad block info occupy 2 bits, offset0,2,4,6 */
    blockShift = (block  << 1) & 0x6;      
    /* Mark the block as bad in the RAM based Bad Block Table */
    oldVal = data;
    data &= ((~(NANDPS_BLOCK_TYPE_MASK << blockShift)) |
             (NANDPS_BLOCK_BAD << blockShift));
    newVal = data;
    nfc->bbInfo[blockOffset] = data;
    
    /* Update the Bad Block Table(BBT) in flash */
    if (oldVal != newVal) 
    {
        error = FNfcPs_UpdateBadBlockTable(nfc);
        if(error)
        {
            return FMSH_FAILURE;
        }
    }
    return FMSH_SUCCESS;
}

int FNfcPs_IsBlockBad(FNfcPs_T* nfc, u16 block) 
{
    u8 data;
    u32 blockOffset;
	u32 blockShift;
	u8 blockType;
    
    FMSH_ASSERT(nfc != NULL);
    FMSH_ASSERT(block < nfc->model.BlockNum);
    
    blockOffset = block >> NANDPS_BBT_BLOCK_SHIFT;
    blockShift = (block << 1) & NANDPS_BLOCK_SHIFT_MASK;
    data = nfc->bbInfo[blockOffset];
    blockType = (data >> blockShift) & NANDPS_BLOCK_TYPE_MASK;

	if (blockType != NANDPS_BLOCK_GOOD)
		return FMSH_FAILURE;
	else
		return FMSH_SUCCESS;
}