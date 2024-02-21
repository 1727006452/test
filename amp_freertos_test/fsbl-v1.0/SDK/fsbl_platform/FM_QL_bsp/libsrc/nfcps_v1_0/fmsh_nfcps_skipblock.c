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
* @file fmsh_nfcps_skipblock.c
* @addtogroup nfcps_v1_0
* @{
*
* Contains implements the functions of using nand flash skipping bad block.
* See fmsh_nfcps_skipblock.h for a detailed description of the 
* device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 11/26/20 
* 		     The SkipBlockNandflash_EraseBlock API has been modified to 
* 		     fix the bug that erase the nonexistent block after the end 
* 		     of device.  
* 		     The SkipBlockNandflash_Read API has been modified to return 
* 		     number of bytes left to be read.
* 1.02  hzq 2022/02/25
*  			 Fix bug that size calc operation may not correctly exit when 
*			 all blocks are bad.
* </pre>
*
******************************************************************************/

#include "fmsh_nfcps_lib.h"

static u32 SkipBlockNandflash_CalculateLength(FNfcPs_T* nfc, u64 Offset, u32 Length);
static int SkipBlockNandflash_CalculateBadBlockNum(FNfcPs_T* nfc, u16 block);

/*****************************************************************/
int SkipBlockNandflash_EraseBlock(FNfcPs_T* nfc, u64 offset, u32 byteCount) 
{
    int error;
    
    u32 blockSize;
	u32 startBlock;
	u32 numOfBlocks;
	u32 block;
    u16 skipBlockNum = 0;
    
    blockSize = FNfcPs_GetBlockSizeInBytes(nfc->model);
	startBlock = offset / blockSize;
    skipBlockNum = SkipBlockNandflash_CalculateBadBlockNum(nfc, startBlock);
	numOfBlocks = byteCount / blockSize;
    if(byteCount % blockSize != 0)
        numOfBlocks = numOfBlocks + 1;
    
	for (block = startBlock + skipBlockNum; block < (startBlock + numOfBlocks + skipBlockNum); block++) 
    {
        if(block < nfc->model.BlockNum)
        {
            /* Check if the block is bad */
            if (FNfcPs_IsBlockBad(nfc, block) != FMSH_SUCCESS) 
            {
                skipBlockNum++;
                continue;
            }
            
            /* Erase the Nand flash block */
            
            error = Nandflash_EraseBlock(nfc, block);
            if (error != 0) 
            {
                error = FNfcPs_MarkBlockBad(nfc, block);
                if (error != 0) 
                {
                    return FMSH_FAILURE;
                }
                else
                {
                    skipBlockNum++;
                }
            }
        }
	}
    
	return FMSH_SUCCESS;
}

int SkipBlockNandflash_Read(FNfcPs_T* nfc, u64 srcAddr, u32 byteCount, u8* destPtr)
{    
    int error;
    
    u64 deviceSize;
    u32 blockSize;
    u32 blockOffset;
    u32 block;
    u32 bytesLeft;
    u32 actLen;
    u32 readLen;
	u32 blockReadLen;
    u16 srcBlock;
    u16 skipBlockNum = 0;
	u8 *bufPtr = (u8 *)destPtr;
    
    deviceSize = FNfcPs_GetDeviceSizeInBytes(nfc->model); 
    blockSize = FNfcPs_GetBlockSizeInBytes(nfc->model);
    srcBlock = srcAddr / blockSize;
    skipBlockNum = SkipBlockNandflash_CalculateBadBlockNum(nfc, srcBlock);
    srcAddr = srcAddr + skipBlockNum * blockSize;
    
    /* Calculate the actual length including bad blocks */
	actLen = SkipBlockNandflash_CalculateLength(nfc, srcAddr, byteCount);
    /* Check if the actual length cross flash size */
	if (srcAddr + actLen >= deviceSize) 
    {
		return FMSH_FAILURE;
	}
    
    bytesLeft = byteCount;
	while (bytesLeft > 0) {
        /* bytes remaind in this block*/
		blockOffset = srcAddr & (blockSize - 1);
        blockReadLen = blockSize - blockOffset;
		block = (srcAddr & ~(blockSize - 1))/blockSize;
        /* Check if the block is bad */
		if(FNfcPs_IsBlockBad(nfc, block) != FMSH_SUCCESS)
        {
			/* Move to next block */
			srcAddr += blockSize;
			continue;
		}
        /* Check if we cross block boundary */
		if (bytesLeft < blockReadLen) 
        {
			readLen = bytesLeft;
		} 
        else 
        {
			readLen = blockReadLen;
		}
        /* Read from the NAND flash */
        error = FNfcPs_Read(nfc, srcAddr, readLen, bufPtr, NULL);
		if (error != 0) 
        {
			return bytesLeft;
		}
        
		bytesLeft -= readLen;
		srcAddr += readLen;
		bufPtr += readLen;
	}
    
	return FMSH_SUCCESS;
}

int SkipBlockNandflash_Write(FNfcPs_T* nfc, u64 destAddr, u32 byteCount, u8* srcPtr)
{
    int error;
    
    u64 deviceSize;
    u32 blockSize;
    u32 blockOffset;
    u32 block;
    u32 bytesLeft; 
	u32 actLen; /* bytes including bad blcok*/
    u32 writeLen; /* bytes write to this block*/
    u32 blockWriteLen; /* bytes remaining empty in this block*/
    u16 destBlock;
    u16 skipBlockNum = 0;
	u8 *bufPtr = (u8 *)srcPtr;
    
    deviceSize = FNfcPs_GetDeviceSizeInBytes(nfc->model); 
    blockSize = FNfcPs_GetBlockSizeInBytes(nfc->model);
    destBlock = destAddr / blockSize;
    skipBlockNum = SkipBlockNandflash_CalculateBadBlockNum(nfc, destBlock);
    destAddr = destAddr + skipBlockNum * blockSize;
    
	/* Calculate the actual length including bad blocks */
	actLen = SkipBlockNandflash_CalculateLength(nfc, destAddr, byteCount);
	/* Check if the actual length cross flash size */
	if (destAddr + actLen >= deviceSize) 
    {
		return FMSH_FAILURE;
	}
    
    bytesLeft = byteCount;
	while (bytesLeft > 0) 
    {
        /* bytes remaind in this block*/
		blockOffset = destAddr & (blockSize - 1);
        blockWriteLen = blockSize - blockOffset;
		block = (destAddr & ~(blockSize - 1))/blockSize;
		/* Check if the block is bad */
		if(FNfcPs_IsBlockBad(nfc, block) != FMSH_SUCCESS)
        {
			/* Move to next block */
			destAddr += blockSize;
			continue;
		}
        
		/*  Check if we cross block boundary */
		if (bytesLeft < blockWriteLen) 
        {
			writeLen = bytesLeft;
		} 
        else 
        {
			writeLen = blockWriteLen;
		}
        
		/* write to the NAND flash */
		error = FNfcPs_Write(nfc, destAddr, writeLen, bufPtr, NULL);
		if (error != 0) 
        {
            error = FNfcPs_MarkBlockBad(nfc, block);
            if (error != 0) 
            {
                return FMSH_FAILURE;
            }
            else
            {
                destAddr += blockSize;
                continue;
            }
		}
        
		bytesLeft -= writeLen;
		destAddr += writeLen;
		bufPtr += writeLen;
	}
    
	return FMSH_SUCCESS;
}

/******************************************************************************
* This function returns the length of bytes including bad blocks from a given offset and
* length.
* @offset: address of start point
* @length: byte number
***********************************/
static u32 SkipBlockNandflash_CalculateLength(FNfcPs_T* nfc, u64 offset, u32 length)
{
    u64 deviceSize;
	u32 blockSize;
	u32 curBlockLen;
	u32 curBlock;
	u32 validLen = 0;
	u32 actLen = 0;
    
    FMSH_ASSERT(nfc != NULL);
    
    deviceSize = FNfcPs_GetDeviceSizeInBytes(nfc->model);
    blockSize = FNfcPs_GetBlockSizeInBytes(nfc->model);
    
	while (validLen < length) 
    {
		curBlockLen = blockSize - (offset & (blockSize - 1));
		curBlock = (offset & ~(blockSize - 1)) / blockSize;
		/* Check if the block is bad */
        if(FNfcPs_IsBlockBad(nfc, curBlock) == FMSH_SUCCESS)
        {
            /* good block */
			validLen += curBlockLen;
		}
		actLen += curBlockLen;
		offset += curBlockLen;
		if (offset >= deviceSize) 
        {
			break;
		}
	}
    
	return actLen;
}

static int SkipBlockNandflash_CalculateBadBlockNum(FNfcPs_T* nfc, u16 block)
{
    u16 blockIndex;
    u16 skipBlockNum = 0;
    
    for(blockIndex = 0; blockIndex < block + skipBlockNum; blockIndex++)
    {
        if(FNfcPs_IsBlockBad(nfc, blockIndex) != FMSH_SUCCESS)
        {
            skipBlockNum++;
        }
    }
    return skipBlockNum;
}