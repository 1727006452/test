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
* @file fmsh_nfcps_model.c
* @addtogroup nfcps_v1_0
* @{
*
* Contains implements the functions to get nand flash infomation.
* See fmsh_nfcps_model.h for a detailed description of the 
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
* 		     The FNfcPs_TranslateFlashAddress API has been modified to 
* 		     delete error check.  
*
* </pre>
*
******************************************************************************/

#include "fmsh_nfcps_lib.h"

int FNfcPs_TranslateFlashAddress(const FNfcPs_Model_T model, u64 Address, unsigned int Size, 
                                    u16 *Block, u16 *Page, u16 *Offset)
{
    u16 tempBlock, tempPage, tempOffset;
    u32 BlockSizeInBytes;
    u16 PageSizeInBytes;
    
    BlockSizeInBytes = FNfcPs_GetBlockSizeInBytes(model);
    PageSizeInBytes = FNfcPs_GetPageSizeInBytes(model);  
    
    tempBlock = Address / BlockSizeInBytes;  
    Address -= tempBlock * BlockSizeInBytes;
    tempPage = Address / PageSizeInBytes;
    Address -= tempPage*PageSizeInBytes;
    tempOffset = Address;
    
    if(Block) 
    {
        *Block = tempBlock;
    }
    if(Page)
    {
        *Page = tempPage;
    }
    if(Offset)
    {
        *Offset = tempOffset;
    } 
    return FMSH_SUCCESS;
}

u16 FNfcPs_GetDeviceSizeInBlocks(const FNfcPs_Model_T model)
{
    return ((u64)model.DeviceSizeInMegaBytes << 10) / model.BlockSizeInKBytes;
}

u32 FNfcPs_GetDeviceSizeInPages(const FNfcPs_Model_T model)
{
    return ((u64)model.DeviceSizeInMegaBytes << 20) / model.PageSizeInBytes;
}

u16 FNfcPs_GetDeviceSizeInMegaBytes(const FNfcPs_Model_T model)
{
    return model.DeviceSizeInMegaBytes;
}

u64 FNfcPs_GetDeviceSizeInBytes(const FNfcPs_Model_T model)
{
    return (u64)model.DeviceSizeInMegaBytes << 20;
}

u16 FNfcPs_GetBlockSizeInPages(const FNfcPs_Model_T model)
{
    return ((u32)model.BlockSizeInKBytes << 10) / model.PageSizeInBytes;
}

u16 FNfcPs_GetBlockSizeInKBytes(const FNfcPs_Model_T model)
{
    return model.BlockSizeInKBytes ;
}

u32 FNfcPs_GetBlockSizeInBytes(const FNfcPs_Model_T model)
{
    return (u32)model.BlockSizeInKBytes << 10;
}

u16 FNfcPs_GetPageSizeInBytes(const FNfcPs_Model_T model)
{
    return model.PageSizeInBytes;
}

u16 FNfcPs_GetPageSpareSize(const FNfcPs_Model_T model)
{
    if (model.SpareSizeInBytes) 
    {
        return model.SpareSizeInBytes;
    }
    else 
    {
        return (model.PageSizeInBytes >> 5); /* Spare size is 16/512 of data size */
    }
}

u8 FNfcPs_GetDeviceId(const FNfcPs_Model_T model)
{
    return model.DeviceId;
}

u8 FNfcPs_GetRowAddrCycle(const FNfcPs_Model_T model)
{
    return model.RowAddrCycle;
}

u8 FNfcPs_GetColAddrCycle(const FNfcPs_Model_T model)
{
    return model.ColAddrCycle;
}

u8 FNfcPs_GetDataBusWidth(const FNfcPs_Model_T model)
{
    return ((model.Options==16) || (model.nfDataWidth==16))? 16: 8;
}

u8 FNfcPs_HasSmallBlocks(const FNfcPs_Model_T model)
{
    return (model.PageSizeInBytes <= 512 )? 1: 0;
}

u8 FNfcPs_GetNumLun(const FNfcPs_Model_T model)
{
    return model.LunNum;
}
