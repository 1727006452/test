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
* @file fmsh_nfcps_sw.c
* @addtogroup nfcps_v1_0
* @{
*
* Contains implements the basic functions of using nand flash.
* See fmsh_nfcps_sw.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 11/26/20 
* 		     The stdlib header file has been added.
* 		     The FNfcPs_InitController API has been modified to assign memory 
* 		     automatically to support 4K page size flash.    
* 		     The FNfcPs_IdCheck API has been modified to support 
* 		     SAMSUNG device.
* 		     The FNfcPs_EccMode API has been modified to support
* 		     4k page flash.
*            The FNfcPs_EraseBlock API has been modified to delete argunment 
*            check.
*            The FNfcPs_Write API has been modified to delete argunment 
*            check.
*            The FNfcPs_WriteSpareData API has been modified to delete argunment 
*            check.
*            The FNfcPs_Read API has been modified to delete argunment 
*            check.
*            The FNfcPs_ReadSpareData API has been modified to delete argunment 
*            check.
*
* </pre>
*
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "fmsh_nfcps_lib.h"

static int FNfcPs_IdCheck(FNfcPs_T* nfc);

int FNfcPs_Initialize(FNfcPs_T* nfc, u16 deviceId) 
{
    FNfcPs_Config_T* cfgPtr;
    
    FMSH_ASSERT(nfc != NULL);
    
    cfgPtr = FNfcPs_LookupConfig(deviceId);
    if (cfgPtr == NULL){
        return FMSH_FAILURE;
    }
    return FNfcPs_CfgInitialize(nfc, cfgPtr);  
}

/***********************************
* Write CONTROL
******************/
int FNfcPs_InitController(FNfcPs_T* nfc, u32 ctrl)
{
    int err;
    
    FMSH_ASSERT(nfc != NULL);
    
    /* clear RETRY_EN bit*/
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, 0x0);
    /* disable protect*/
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_MEM_CTRL_OFFSET, 0x0100);  
    FNfcPs_SetStatusMask(nfc, 0x0140);
    
    /* Initialize Device */
    err = Nandflash_GetFlashInfo(nfc);
    if(err)
    {
        return FMSH_FAILURE;
    }
    err = FNfcPs_IdCheck(nfc);
    if(err)
    {
        return FMSH_FAILURE;
    }
    
    /* set ctrl */
    FNfcPs_SetCtrl(nfc, ctrl & NFCPS_USER_CTRL_MASK);
    FNfcPs_SetIOWidth(nfc, nfc->config.flashWidth);
    FNfcPs_SetBlockSize(nfc, NFCPS_BLOCKSIZE(nfc->model.PagePerBlock));
    if(nfc->model.PageSizeInBytes <= 512) 
    {
        FNfcPs_EnableSmallBlock(nfc);
    }
    
    FNfcPs_SetIntrMask(nfc, ~NFCPS_INTR_ALL);
    
    /* assign memory */
    u8* data = malloc(nfc->model.PageSizeInBytes + nfc->model.SpareSizeInBytes);
    u8* eccCalc = malloc(nfc->model.SpareSizeInBytes);
    u8* eccCode = malloc(nfc->model.SpareSizeInBytes);
    u8* bbInfo = malloc((nfc->model.BlockNum) >> 2);
    u32* bbMap = malloc((nfc->model.maxBadBlockNum) << 2);
    if((data == 0) || (bbInfo == 0) || (bbMap == 0) ||
       (eccCalc == 0) || (eccCode == 0))
    {
        goto Nand_InitErr;
    }
    
    nfc->dataBufferPtr = data;
    nfc->spareBufferPtr = data + nfc->model.PageSizeInBytes;
    
    /* set ecc */
    nfc->eccCalc = eccCalc;
    nfc->eccCode = eccCode;
    FNfcPs_EccMode(nfc);
    
    /* set bbm */
    nfc->bbInfo = bbInfo;
    nfc->bbMap = bbMap;
    nfc->isBadBlockWrite = 0;
    FNfcPs_InitBadBlockTableDesc(nfc);
    err = FNfcPs_ScanBadBlockTable(nfc);
    if(err)
    {
        goto Nand_InitErr;
    }   
    if(ctrl & NFCPS_BBM_EN_MASK)
	{
	   	FNfcPs_SetDevPtr(nfc, (u32)nfc->bbMap);
    	FNfcPs_SetDevSize(nfc, (nfc->model.maxBadBlockNum >> 3));
    	FNfcPs_InitRemap(nfc); 
  	}
    return FMSH_SUCCESS;
    
Nand_InitErr:
    free(data);
    free(eccCalc);
    free(eccCode);
    free(bbInfo);
    free(bbMap);
    return FMSH_FAILURE;
}

static int FNfcPs_IdCheck(FNfcPs_T* nfc)
{
    u8 maker = nfc->model.Manufacture;
    u8 ioWidth = nfc->model.nfDataWidth;
    
    /* validity check */
    if ((maker != NAND_MICRON_ID) && (maker != NAND_SPANSION_ID) &&
        (maker != NAND_FMSH_ID) && (maker != NAND_SAMSUNG_ID))
    {
        return FMSH_FAILURE;
    } 
    
    if ((ioWidth != 8) && (ioWidth != 16)) 
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

int FNfcPs_EccMode(FNfcPs_T* nfc)
{
    u32 offset;
    u32 eccCap, eccBlcokNum, eccBytes;
    
    if(nfc->model.onDieEcc == 1)
    {
        nfc->eccMode = NFCPS_ECC_ONDIE;
    }
    else
    {
        nfc->eccMode = NFCPS_ECC_HW;
    }

    switch(nfc->eccMode)
    {
    case NFCPS_ECC_NONE:
    case NFCPS_ECC_ONDIE:
    case NFCPS_ECC_SW:
        FNfcPs_DisableEcc(nfc);
        break;
    case NFCPS_ECC_HW:
        FNfcPs_SetEccBlockSize(nfc, 512);    
        eccBlcokNum = nfc->model.PageSizeInBytes / 512;

        if(nfc->model.SpareSizeInBytes == 64)
        {
            nfc->model.eccLevel = 4;
        }        
        else if(nfc->model.SpareSizeInBytes == 128)
        {
            nfc->model.eccLevel = 8;
        }
        
        if(nfc->model.eccLevel <= 2)
        {
            eccCap = NFCPS_ECC_CAP2_STATE;
            eccBytes = 4 * eccBlcokNum;
        }
        else if(nfc->model.eccLevel <= 4)
        {
            eccCap = NFCPS_ECC_CAP4_STATE;
            eccBytes = 8 * eccBlcokNum;
        }
        else if(nfc->model.eccLevel <= 8)
        {
            eccCap = NFCPS_ECC_CAP8_STATE;
            eccBytes = 14 * eccBlcokNum;
        }
        else if(nfc->model.eccLevel <= 16)
        {
            eccCap = NFCPS_ECC_CAP16_STATE;
            eccBytes = 28 * eccBlcokNum;
        }
        else if(nfc->model.eccLevel <= 24)
        {
            eccCap = NFCPS_ECC_CAP24_STATE;
            eccBytes = 42 * eccBlcokNum;
        }
        else if(nfc->model.eccLevel <= 32)
        {
            eccCap = NFCPS_ECC_CAP32_STATE;
            eccBytes = 56 * eccBlcokNum;
        }
        FNfcPs_SetEccCtrl(nfc, NFCPS_ECC_UNC_STATE, nfc->model.eccLevel, eccCap);  
        offset = nfc->model.PageSizeInBytes + nfc->model.SpareSizeInBytes - eccBytes;
        if(nfc->model.nfDataWidth == 16)
        {
            offset = offset >> 1;
        } 
        FNfcPs_SetEccOffset(nfc, offset);
        FNfcPs_EnableEcc(nfc);
        break;
    default: 
        FNfcPs_DisableEcc(nfc);
        break;
    }

    return FMSH_SUCCESS;
}

int FNfcPs_EraseBlock(FNfcPs_T* nfc, u64 destAddr, u32 blockCount)
{
    int err;
    u16 block;
    u32 byteCount;
    
    FMSH_ASSERT(nfc != NULL);
    
    byteCount = blockCount * FNfcPs_GetBlockSizeInBytes(nfc->model);
    /* calculate address */
    FNfcPs_TranslateFlashAddress(nfc->model, destAddr, byteCount, 
                                 &block, 0, 0);
    /* Remaining blocks to erase */
    while( blockCount > 0)
    {
        err = Nandflash_EraseBlock(nfc, block);
        if(err)
        {
            return FMSH_FAILURE;
        }
        block++;
        blockCount--;
    }
    
    return FMSH_SUCCESS;
}

int FNfcPs_Write(FNfcPs_T *nfc, u64 destAddr, u32 byteCount, u8 *srcPtr, u8 *userSparePtr)
{
  int err;
  u16 block, page, offset;
  u32 length;
  
  FMSH_ASSERT(nfc != NULL);
  FMSH_ASSERT(srcPtr != NULL);
  
  u8* data = nfc->dataBufferPtr;
  u8* spare = nfc->spareBufferPtr;
  
  while(byteCount > 0)
  {
    /* calculate address */
    FNfcPs_TranslateFlashAddress(nfc->model, destAddr, byteCount, 
                                 &block, &page, &offset);
    /* length of bytes to be send to nandflash */
    length = nfc->model.PageSizeInBytes - offset;
    if(byteCount < length)
    {
      length = byteCount;
    }
    /* init & fill buffer */
    memset(data, 0xff, nfc->model.PageSizeInBytes);
    memcpy(data+offset, srcPtr, length);
    srcPtr += length;
    destAddr += length;
    byteCount -= length;
    /* write to nandflash */
    if(userSparePtr)
    {
		memcpy(&spare[0], userSparePtr, nfc->model.SpareSizeInBytes);  
        userSparePtr += nfc->model.SpareSizeInBytes;
        err = Nandflash_WritePage(nfc, block, page, 0, data, spare);
    }
    else
    {
        err = Nandflash_WritePage(nfc, block, page, 0, data, 0);
    }
    if(err)
    {
        return FMSH_FAILURE;   
    }
  }
  
  return FMSH_SUCCESS;
}

int FNfcPs_WriteSpareData(FNfcPs_T *nfc, u32 block, u32 page, u8 *userSparePtr)
{
    int err;
    
    FMSH_ASSERT(nfc != NULL);
    FMSH_ASSERT(userSparePtr != NULL);
 
    /* write to nandflash */
    err = Nandflash_WritePage(nfc, block, page, 0, 0, userSparePtr);
    if(err)
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

int FNfcPs_Read(FNfcPs_T* nfc, u64 srcAddr, u32 byteCount, u8* destPtr, u8* userSparePtr)
{
  int err = FMSH_SUCCESS;;
  u16 block, page, offset;
  u32 length;
  
  FMSH_ASSERT(nfc != NULL);
  FMSH_ASSERT(destPtr != NULL);
  
  u8* data = nfc->dataBufferPtr;
  u8* spare = nfc->spareBufferPtr;

  /* remaining data to be read */
  while(byteCount > 0)
  {
    /* calculate address */
    FNfcPs_TranslateFlashAddress(nfc->model, srcAddr, byteCount, 
                                 &block, &page, &offset);
    /* read entire page from nandflash */
    if(userSparePtr)
    {
        err = Nandflash_ReadPage(nfc, block, page, 0, data, spare);
        memcpy(userSparePtr, &spare[0], nfc->model.SpareSizeInBytes);
        userSparePtr += nfc->model.SpareSizeInBytes;
    }
    else
    {
        err = Nandflash_ReadPage(nfc, block, page, 0, data, 0);
    }
    if(err)
    {
        return FMSH_FAILURE;
    }
    /* maxium length of bytes needed */
    length = nfc->model.PageSizeInBytes - offset;
    /* check has more data to read */
    if(byteCount < length)
    {
      length = byteCount;
    }
    
    memcpy(destPtr, &data[offset], length);
    srcAddr += length;
    destPtr += length;
    byteCount -= length;    
  }
    
    return FMSH_SUCCESS;  
}

int FNfcPs_ReadSpareData(FNfcPs_T* nfc, u32 block, u32 page, u8* userSparePtr)
{
    int err;
    
    FMSH_ASSERT(nfc != NULL);
    FMSH_ASSERT(userSparePtr != NULL);
    
    err = Nandflash_ReadPage(nfc, block, page, 0, 0, userSparePtr);
    if(err)
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;  
}

/*******************************************************************/

