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
* @file fmsh_nfcps_flash.c
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
*            API Nandflash_GetFlashInfo has been modified to support devices 
*            which are not compatibe with onfi 1.0.
*            API Nandflash_ReadPage has been modified to use internal dma 
*            to read page from flash.
*            API Nandflash_WritePage has been modified to use internal dma 
*            to write page to flash.
*            API Nandflash_ChangeWriteColumn has been modified to use internal  
*            dma.
*            API Nandflash_ProgramPage1 has been modified to use internal  
*            dma.
*
* </pre>
*
******************************************************************************/

#include "fmsh_nfcps_lib.h"

static FNfcPs_Command_T s_CommandTable[] = 
{ 
    /*  Reset commands */
    {NANDFLASH_RESET, 0x0000ff00},                  /* seq0 */
    {NANDFLASH_SYNCH_RESET,0x0000fc00},             /* seq0 */
    {NANDFLASH_LUN_RESET,0x0000fa34},               /* seq20 */
    /*  Identification operations */
    {NANDFLASH_READ_ID,0x00009021},                 /* seq1 */
    {NANDFLASH_READ_PARAMETER_PAGE,0x0000ec22},     /* seq2 */
    {NANDFLASH_UNIQUE_ID,0x0000ed22},               /* seq2 */
    /*  Configuration operations */
    {NANDFLASH_GET_FEATURE,0x0000ee22},             /* seq2 */
    {NANDFLASH_SET_FEATURE,0x0000ef03},             /* seq3 */
    /*  Status Operations */
    {NANDFLASH_READ_STATUS,0x00007024},             /* seq4 */
    {NANDFLASH_SELECT_LUN_WITH_STATUS,0x00007825},  /* seq5 */
    {NANDFLASH_LUN_STATUS,0x00007125},              /* seq5 */
    {NANDFLASH_DEVICE_STATUS,0x00007224},           /* seq4 */
    {NANDFLASH_VOLUME_SELECT,0x0000e115},           /* seq21 */
    /*  Column adress operations */
    {NANDFLASH_CHANGE_READ_COLUMN,0xe0000526},      /* seq6 */
    {NANDFLASH_SELECT_CACHE_REGISTER,0xe0000627},   /* seq7 */
    {NANDFLASH_CHANGE_WRITE_COLUMN,0x00008508},     /* seq8 */
    {NANDFLASH_CHANGE_ROW_ADDRESS,0x0011850c},      /* seq12 */
    /*  Read operations */ 
    {NANDFLASH_READ_PAGE,0x3000002a},               /* seq10 */
    {NANDFLASH_READ_PAGE_CACHE,0x0000312b},         /* seq11 */
    {NANDFLASH_READ_PAGE_CACHE_LAST,0x00003f2b},    /* seq11 */
    {NANDFLASH_READ_MULTIPLANE,0x00320029},         /* seq9 */
    {NANDFLASH_TWO_PLANE_PAGE_READ,0x0030002f},     /* seq15 */
    {NANDFLASH_QUEUE_PAGE_READ,0x00370736},         /* seq22 */
    /*  Program operations */
    {NANDFLASH_PROGRAM_PAGE,0x0010800c},            /* seq12 */
    {NANDFLASH_PROGRAM_PAGE_IMD,0x00108017},        /* seq23 */
    {NANDFLASH_PROGRAM_PAGE_DEL,0x00138017},        /* seq23 */
    {NANDFLASH_PROGRAM_PAGE_1,0x0000800d},          /* seq13 */
    {NANDFLASH_PROGRAM_PAGE_CACHE,0x0015800c},      /* seq12 */
    {NANDFLASH_PROGRAM_MULTIPLANE,0x0011800c},      /* seq12 */
    {NANDFLASH_WRITE_PAGE,0x00001000},              /* seq0 */
    {NANDFLASH_WRITE_PAGE_CACHE,0x00001500},        /* seq0 */
    {NANDFLASH_WRITE_MULTIPLANE,0x00001100},        /* seq0 */
    /*  Erase operations */
    {NANDFLASH_ERASE_BLOCK,0x00d0600e},             /* seq14 */
    {NANDFLASH_ERASE_MULTIPLANE,0x00d1600e},        /* seq14 */
    /*  Copyback operations */
    {NANDFLASH_COPYBACK_READ,0x3500002a},           /* seq10 */
    {NANDFLASH_COPYBACK_PROGRAM,0x00108529},        /* seq9 */
    {NANDFLASH_COPYBACK_PROGRAM_1,0x0000850d},      /* seq13 */
    {NANDFLASH_COPYBACK_MULTIPLANE,0x0011850c},     /* seq12 */
    /*  OTP operations */
    {NANDFLASH_PROGRAM_OTP,0x0010a00c},             /* seq12 */
    {NANDFLASH_DATA_PROTECT_OTP,0x0010a529},        /* seq9 */
    {NANDFLASH_READ_PAGE_OTP,0x3000af2a},           /* seq10 */
};

/****************************
* get command
*
***************/
u32 FNfcPs_FindInstruction(FNfcPs_T* nfc, u32 command)
{
    /*  return value needed by Command Register */
    int index;
    for(index = 0; index < NANDFLASH_COMMAND_NUM; index++){
        if(command == s_CommandTable[index].command){
            return s_CommandTable[index].value;
        }
    }
    return FMSH_FAILURE;
}

int Nandflash_GetFlashInfo(FNfcPs_T* nfc)
{ 
    int error;
    u32 first_word, second_word;
    u8 Id1, Id2, Id3, Id4, Id5;
    FNfcPs_Model_T model;
    
    nfc->cap1.jedec_supp = 0;
    nfc->cap1.onfi_supp = 0;
    
    //Reset Nandflash chip
    error = Nandflash_Reset(nfc);
    if(error){
        return FMSH_FAILURE;
    }
    
    /** read jedec id */
    error = Nandflash_ReadId(nfc, &first_word, &second_word);
    if(error){
        return FMSH_FAILURE;
    }
    nfc->cap1.jedec_supp = 1;
    // calculate ID
    Id1 = first_word & 0xff;
    Id2 = (first_word >> 8) & 0xff;
    Id3 = (first_word >> 16) & 0xff;
    Id4 = (first_word >> 24) & 0xff;
    Id5 = second_word & 0xff;
    //Set NandflashModel
    model.Options = 0;
    model.Manufacture = Id1;
    model.DeviceId = Id2;
    // LUN number
    switch(Id3 & 0x03) {
    case 0x00: model.LunNum = 1; break;
    case 0x01: model.LunNum = 2; break;
    case 0x02: model.LunNum = 4; break;
    case 0x03: model.LunNum = 8; break;
    }
    // Page Size
    switch(Id4 & 0x03) {
    case 0x00: model.PageSizeInBytes = 1024; break;
    case 0x01: model.PageSizeInBytes = 2048; break;
    case 0x02: model.PageSizeInBytes = 4096; break;
    case 0x03: model.PageSizeInBytes = 8192; break;
    }
    // Spare Size
    switch(Id4 & 0x04){
    case 0x00: model.SpareSizeInBytes = model.PageSizeInBytes / 512 * 8; break;
    case 0x04: model.SpareSizeInBytes = model.PageSizeInBytes / 512 * 16; break;
    }
    // Block Size
    switch(Id4 & 0x30) {
    case 0x00: model.BlockSizeInKBytes = 64;  break;
    case 0x10: model.BlockSizeInKBytes = 128; break;
    case 0x20: model.BlockSizeInKBytes = 256; break;
    case 0x30: model.BlockSizeInKBytes = 512; break;
    }
    // IO width
    switch(Id4 & 0x40){
    case 0x00: model.nfDataWidth = 8;  break;
    case 0x40: model.nfDataWidth = 16; break;
    }
    // ECC Level
    switch(Id5 & 0x03){
    case 0x00: model.eccLevel = 1; break;
    case 0x01: model.eccLevel = 2; break;
    case 0x02: model.eccLevel = 4; break;
    case 0x04: model.eccLevel = 8; break;
    }
    // Plane Num
    switch(Id5 & 0x0c){
    case 0x00: model.PlaneNum = 1; break;
    case 0x04: model.PlaneNum = 2; break;
    case 0x08: model.PlaneNum = 4; break;
    case 0x0c: model.PlaneNum = 8; break;
    }
    // Plane Size
    switch(Id5 & 0x70){
    case 0x00: model.PlaneSizeInMegaBytes = 8; break;
    case 0x10: model.PlaneSizeInMegaBytes = 16; break;
    case 0x20: model.PlaneSizeInMegaBytes = 32; break;
    case 0x30: model.PlaneSizeInMegaBytes = 64; break;
    case 0x40: model.PlaneSizeInMegaBytes = 128; break;
    case 0x50: model.PlaneSizeInMegaBytes = 256; break;
    case 0x60: model.PlaneSizeInMegaBytes = 512; break;
    }
    // ON-DIE ECC
    switch(Id5 & 0x80){
    case 0x00: model.onDieEcc = 0; break;
    case 0x80: model.onDieEcc = 1; break;
    }
    // calculate other parameters
    model.DeviceSizeInMegaBytes = (model.PlaneSizeInMegaBytes * model.PlaneNum);
    model.LunSizeInMegaBytes = (model.DeviceSizeInMegaBytes / model.LunNum);
    model.PlanePerLun = (model.LunSizeInMegaBytes / model.PlaneSizeInMegaBytes);
    model.BlockPerLun = ((model.LunSizeInMegaBytes << 10) / model.BlockSizeInKBytes);
    model.PagePerBlock = ((model.BlockSizeInKBytes << 10) / model.PageSizeInBytes);
    model.BlockNum =  model.BlockPerLun * model.LunNum;
    model.PageNum = model.PagePerBlock * model.BlockNum;
    int temp1 = model.DeviceSizeInMegaBytes /32;
    int temp2 = model.DeviceSizeInMegaBytes %32;
    if(temp2 == 0)
        model.maxBadBlockNum = temp1 * 5;
    else
        model.maxBadBlockNum = (temp1+1) * 5;
    model.RowAddrCycle = 3;
    model.ColAddrCycle = 2;
    
    /** read onfi id */
    if(Nandflash_ReadOnfiId(nfc) ==  NANDFLASH_ONFI_ID){
        FNfcPs_ParaPage_T* ParaPage;
        // device support onfi then read parameter page(256 bytes)
        error = Nandflash_ReadParaPage(nfc, &(nfc->ParaPage));
        if(error){
            return FMSH_FAILURE;
        }
        nfc->cap1.onfi_supp = 1;
        ParaPage = &(nfc->ParaPage);
        //model
        model.Manufacture = ParaPage->JedecManufacturerId;
        model.PageSizeInBytes = ParaPage->BytesPerPage;
        model.SpareSizeInBytes = ParaPage->SpareBytesPerPage;
        model.BlockSizeInKBytes = (u64)(ParaPage->PagesPerBlock * ParaPage->BytesPerPage) >> 10;
        model.DeviceSizeInMegaBytes = (u64)(ParaPage->NumLuns * ParaPage->BlocksPerLun *
                                        model.BlockSizeInKBytes) >> 10; 
        model.PlaneSizeInMegaBytes = model.DeviceSizeInMegaBytes / model.PlaneNum;
        model.LunSizeInMegaBytes = model.DeviceSizeInMegaBytes / ParaPage->NumLuns;
        model.PlanePerLun = (model.LunSizeInMegaBytes / model.PlaneSizeInMegaBytes);
        model.BlockPerLun = ParaPage->BlocksPerLun;
        model.PagePerBlock = ParaPage->PagesPerBlock;  
        model.LunNum = ParaPage->NumLuns;
        model.BlockNum =  ParaPage->BlocksPerLun * ParaPage->NumLuns;
        model.PageNum = model.BlockNum * ParaPage->PagesPerBlock;
        u8* addr_temp = (u8*)&(ParaPage->MaxBadBlocksPerLun);
        u32 maxBadBlockNum_temp = *addr_temp | ((u16)(*(addr_temp+1)) << 8);
        model.maxBadBlockNum = maxBadBlockNum_temp * ParaPage->NumLuns;
        model.RowAddrCycle = ParaPage->AddrCycles & 0xf;
        model.ColAddrCycle = (ParaPage->AddrCycles >> 4) & 0xf;
        model.eccLevel = ParaPage->EccBits;
    }
    
    nfc->config.flashWidth = model.nfDataWidth;
    nfc->model = model;
    
    return FMSH_SUCCESS;
}

int Nandflash_Reset(FNfcPs_T* nfc)
{
    int error;
    u32 command;
    
    /* Set Command (Reset) */ 
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_RESET);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command);
    // Wait For FIFO Empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    /* Wait for Ready */
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error)
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

int Nandflash_ReadId(FNfcPs_T* nfc, u32* first_word, u32* second_word)
{
    int error;
    u32 Command;
    
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, 0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, 0);
    // Set Data Size
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, 5);
    FNfcPs_ClearFIFO(nfc);
    // Set Command ( FIFO & SIU & RDID)
    Command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_ID);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, Command);
    // Wait For FIFO Empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error)
    {
        return FMSH_FAILURE;
    }
    // Wait for DFR not empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_DFIFO_R_EMPTY_MASK, 0);
    if(error)
    {
        return FMSH_FAILURE;
    }
    // Get Id
    *first_word = FNfcPs_ReadFIFO(nfc); 
    *second_word = FNfcPs_ReadFIFO(nfc); 
    
    return FMSH_SUCCESS;
}

u32 Nandflash_ReadOnfiId(FNfcPs_T* nfc)
{
    int error;
    u32 Command;
    u32 onfi_id;
    
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, 0x20);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, 0);
    // Set Data Reg Size (4 Bytes)
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_REG_SIZE_OFFSET, 0x3);
    FNfcPs_ClearFIFO(nfc);
    // Set Command (Data Reg & Read ONFI ID)
    Command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_ID);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, Command | 0x1<<7); //use data reg
    // Wait For FIFO Empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }   
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error)
    {
        return FMSH_FAILURE;
    }
    // Wait For Data Reg Avaliable
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_DATA_REG_RDY_MASK, 1);
    if(error)
    {
        return FMSH_FAILURE;
    }
    // Get Status
    onfi_id = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_DATA_REG_OFFSET);
    
    return onfi_id;
}

int Nandflash_ReadParaPage(FNfcPs_T* nfc, void* ParaPage)
{
    int error;
    u32 Command;
    int ByteCount;
    
    u32* parapage = (u32*)ParaPage;
    
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, 0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, 0);
    // Set Data Size
    ByteCount = 256;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, ByteCount);
    FNfcPs_ClearFIFO(nfc);
    // Set Command ( FIFO & SIU & RD PARA PAGE)
    Command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_PARAMETER_PAGE);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, Command);
    // Wait For FIFO Empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait for DFR not empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_DFIFO_R_EMPTY_MASK, 0);
    if(error){
        return FMSH_FAILURE;
    }
    // Get ParaPage
    while(ByteCount > 0)
    {
        *parapage = FNfcPs_ReadFIFO(nfc); 
        parapage ++;
        ByteCount -= 4;
    }
    
    return FMSH_SUCCESS;
}

u8 Nandflash_ReadStatus(FNfcPs_T* nfc)
{
    int error;
    u32 Command;
    
    // Set Data Reg Size (Single Byte)
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_REG_SIZE_OFFSET, 0x0);
    // Set Command (Data Reg & Read Status)
    Command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_STATUS);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, Command | 0x1<<7); //use data reg
    // Wait For FIFO Empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Get Status
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_DATA_REG_OFFSET);
}

u32 Nandflash_GetFeature(FNfcPs_T* nfc, u8 Feature)
{
    int error;
    u32 Command;
    u32 feature;
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, Feature);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, 0);
    // Set Data Reg Size (4 Bytes)
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_REG_SIZE_OFFSET, 0x3);
    FNfcPs_ClearFIFO(nfc);
    // Set Command (Data Reg & Get Feature)
    Command = FNfcPs_FindInstruction(nfc, NANDFLASH_GET_FEATURE);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, Command | (0x1<<7));
    // Wait For FIFO Empty
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return 0xffffffff;
    }
    // Wait For Data Reg Avaliable
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_DATA_REG_RDY_MASK, 1);
    if(error){
        return 0xffffffff;
    }
    // Get Status
    feature =  FMSH_ReadReg(nfc->config.baseAddress, NFCPS_DATA_REG_OFFSET);
    return feature;
}

int Nandflash_SetFeature(FNfcPs_T* nfc, u8 Feature, u32 Value)
{
    int error;
    u32 Command;
    
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, Feature);  
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, 0);
    // Set Data Size
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, 4);
    // Set FIFO
    FNfcPs_ClearFIFO(nfc);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_FIFO_DATA_OFFSET, Value);
    // Set Command (Program)
    Command = FNfcPs_FindInstruction(nfc, NANDFLASH_SET_FEATURE);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, Command);
    // Wait for FIFO_STATE.DF_EMPTY
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_DFIFO_W_EMPTY_MASK | 
                              NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

/*******************************
* Erase Blcok
* @block 
***********/
int Nandflash_EraseBlock(FNfcPs_T* nfc, u32 block) 
{
    int err;
    u32 command;
    
    /* Set Address */
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, 0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, block << 6);
    /* Set Command(seq14) */
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_ERASE_BLOCK);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command);
    /* Wait for Flash Ready */
    err = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(err)
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

/**
* Nandflash_ReadPage: read a page
* Nandflash_ReadSpare: read spare area
*/
int Nandflash_ReadPage(FNfcPs_T* nfc, u16 block, u16 page, u16 offset, void* data, void* spare)
{
    int error;
    u32 command;
    u32 colAddr, rowAddr;
    u32 byteCount; 
    
    FMSH_ASSERT(nfc != NULL);
    FMSH_ASSERT(data || spare);
    
    if(data)
    {
        FNfcPs_ClearEccStat(nfc);
        
        /* ByteCount is page size */
        byteCount = nfc->model.PageSizeInBytes;
        /* config dma*/
        FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
        FNfcPs_SetDMAAddr(nfc, (u32)data);
        FNfcPs_SetDMACnt(nfc, byteCount);
        FNfcPs_SetDMATrigLvl(nfc, 0x40);
        /* Set Address */
        colAddr = 0;
        rowAddr = (block << 6) | page;
        if(nfc->model.nfDataWidth == 16)
        {
            colAddr = colAddr >> 1;
        }
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, colAddr);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, rowAddr);
        /* Set Data Size */
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byteCount);
        /* Set Command (Read) & DMA mode*/
        command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_PAGE);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command | 0x1<<6);
        /* Wait For CFIFO Empty */
        error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
        if(error)
        {
            return FMSH_FAILURE;
        }
        /* Wait For Ready */
        error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
        if(error)
        {
            return FMSH_FAILURE;
        }       
        /* poll dma status */
        do
        {
            error = FNfcPs_GetDMAStatus(nfc);
            if(error & 0x2)
            {
                return FMSH_FAILURE;
            }
        }while((error & 0x1) == 0);  
        
        /* check ECC_STAT */
        u32 status = FNfcPs_GetEccStat(nfc);
        if((status & 0x10000) == 0x10000)
        {
            //return FMSH_FAILURE;
        }
    }
    
    if(spare)
    {
        /* ByteCount is page size */
        byteCount = nfc->model.SpareSizeInBytes;
        /* config dma*/
        FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
        FNfcPs_SetDMAAddr(nfc, (u32)spare);
        FNfcPs_SetDMACnt(nfc, byteCount);
        FNfcPs_SetDMATrigLvl(nfc, 0x40);
        /* Set Address */
        colAddr = nfc->model.PageSizeInBytes;
        rowAddr = (block << 6) | page;
        if(nfc->model.nfDataWidth == 16)
        {
            colAddr = colAddr >> 1;
        }
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, colAddr);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, rowAddr);
        /* Set Data Size */
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byteCount);
        /* Set Command (Read) & DMA mode*/
        command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_PAGE);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command | 0x1<<6);
        /* Wait For CFIFO Empty */
        error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
        if(error)
        {
            return FMSH_FAILURE;
        }
        /* Wait For Ready */
        error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
        if(error)
        {
            return FMSH_FAILURE;
        }       
        /* poll dma status */
        do
        {
            error = FNfcPs_GetDMAStatus(nfc);
            if(error & 0x2)
            {
                return FMSH_FAILURE;
            }
        }while((error & 0x1) == 0);  
    }
    
    return FMSH_SUCCESS;
}

/**
* Nandflash_WritePage: write a page
*/
int Nandflash_WritePage(FNfcPs_T* nfc, u16 block, u16 page, u16 offset, void *data, void *spare)
{
    int error;
    u32 command;
    u32 colAddr, rowAddr;
    u32 byteCount;     
    
    FMSH_ASSERT(nfc != NULL);
    FMSH_ASSERT(data || spare);
    
    if(data)
    {
        byteCount = nfc->model.PageSizeInBytes;
        /* config dma*/
        FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
        FNfcPs_SetDMAAddr(nfc, (u32)data);
        FNfcPs_SetDMACnt(nfc, byteCount);
        FNfcPs_SetDMATrigLvl(nfc, 0x40);
        /* Set Address */
        colAddr = 0;
        rowAddr = (block << 6) | page;
        if(nfc->model.nfDataWidth == 16)
        {
            colAddr = colAddr >> 1;
        }            
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, colAddr);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, rowAddr);
        /* Set Data Size to be transfered */
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byteCount);
        FNfcPs_ClearFIFO(nfc);
        /* Set Command (Program) */
        command = FNfcPs_FindInstruction(nfc, NANDFLASH_PROGRAM_PAGE);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command  | 0x1<<6);
        /* Wait For Ready */
        error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
        if(error)
        {
            return FMSH_FAILURE;
        }
        /* poll dma status */
        do{
            error = FNfcPs_GetDMAStatus(nfc);
            if(error & 0x2)
            {
                return FMSH_FAILURE;
            }
        }while((error & 0x1) == 0);
    }
    
    if(spare)
    {        
        byteCount = nfc->model.SpareSizeInBytes;
        /* config dma*/
        FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
        FNfcPs_SetDMAAddr(nfc, (u32)spare);
        FNfcPs_SetDMACnt(nfc, byteCount);
        FNfcPs_SetDMATrigLvl(nfc, 0x40);
        /* Set Address */
        colAddr = nfc->model.PageSizeInBytes;
        rowAddr = (block << 6) | page;
        if(nfc->model.nfDataWidth == 16)
        {
            colAddr = colAddr >> 1;
        }            
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, colAddr);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, rowAddr);
        /* Set Data Size to be transfered */
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byteCount);
        FNfcPs_ClearFIFO(nfc);
        /* Set Command (Program) */
        command = FNfcPs_FindInstruction(nfc, NANDFLASH_PROGRAM_PAGE);
        FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command  | 0x1<<6);
        /* Wait For Ready */
        error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
        if(error)
        {
            return FMSH_FAILURE;
        }
        /* poll dma status */
        do{
            error = FNfcPs_GetDMAStatus(nfc);
            if(error & 0x2)
            {
                return FMSH_FAILURE;
            }
        }while((error & 0x1) == 0);
    }  
    
    return FMSH_SUCCESS; 
}  

int Nandflash_IntDataMove(FNfcPs_T* nfc, u32 src_row,  u32 dest_row)
{
    int error;
    u32 command;
    
    FMSH_ASSERT(nfc != NULL);
    
    ///read for internal data move (00h-35h)
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, 0x0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, src_row);
    FNfcPs_ClearFIFO(nfc);
    // Set Command (Read)
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_COPYBACK_READ);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command);
    // Wait for FIFO_STATE.CF_EMPTY
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    
    ///write for internal data move (85h-10h)
    // Set Address
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, 0x0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, dest_row);
    // Set Command (Read)
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_COPYBACK_PROGRAM);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command);
    // Wait for FIFO_STATE.CF_EMPTY
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    // Wait For Ready
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    
    FNfcPs_ClearFIFO(nfc);
    
    return FMSH_SUCCESS;    
}

int Nandflash_ChangeReadColumn(FNfcPs_T* nfc, u32 col_addr, signed int byte_count, void* recv_buffer)
{
    int error;
    u32 command;
    u32 ColAddr;
    
    /* Wait For Ready */
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    /* config dma */
    FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
    FNfcPs_SetDMAAddr(nfc, (u32)recv_buffer);
    FNfcPs_SetDMACnt(nfc, byte_count);
    FNfcPs_SetDMATrigLvl(nfc, 0x40);     
    /* Set Address */
    ColAddr = col_addr;
    if(nfc->model.nfDataWidth == 16){
        ColAddr = ColAddr >> 1;
    }
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, ColAddr);
    /* Set Data Size */
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byte_count);
    /* Set Command (Read) */
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_CHANGE_READ_COLUMN);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command | (0x1 << 6));
    /* Wait for FIFO_STATE.CF_EMPTY */
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    /* poll dma status */
    do
    {
        error = FNfcPs_GetDMAStatus(nfc);
        if(error & 0x2)
        {
            return FMSH_FAILURE;
        }
    }while((error & 0x1) == 0);  
    
    return FMSH_SUCCESS;
}

int Nandflash_ChangeWriteColumn(FNfcPs_T* nfc, u32 col_addr, signed int byte_count, void* send_buffer)
{
    int error;
    u32 command;
    u32 ColAddr;
    
    /* config dma*/
    FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
    FNfcPs_SetDMAAddr(nfc, (u32)send_buffer);
    FNfcPs_SetDMACnt(nfc, byte_count);
    FNfcPs_SetDMATrigLvl(nfc, 0x40);
    /* Set Address */
    ColAddr = col_addr;
    if(nfc->model.nfDataWidth == 16){
        ColAddr = ColAddr >> 1;
    }
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, ColAddr);
    /* Set Data Size */
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byte_count);
    FNfcPs_ClearFIFO(nfc);
    /* Set Command (Program) */
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_CHANGE_WRITE_COLUMN);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command | (0x1<<6));
    /* poll dma status */
    do{
        error = FNfcPs_GetDMAStatus(nfc);
        if(error & 0x2)
        {
            return FMSH_FAILURE;
        }
    }while((error & 0x1) == 0);
   
    return FMSH_SUCCESS;
}

int Nandflash_ProgramPage1(FNfcPs_T* nfc, u16 block, u16 page, u16 offset, signed int byte_count, void* send_buffer)
{
    int error;
    u32 command;
    u32 RowAddr, ColAddr;
 
    /* config dma*/
    FNfcPs_SetDMACtrl(nfc, NFCPS_DMA_SFR_STATE, 0x2, 1);
    FNfcPs_SetDMAAddr(nfc, (u32)send_buffer);
    FNfcPs_SetDMACnt(nfc, byte_count);
    FNfcPs_SetDMATrigLvl(nfc, 0x40);
    /* Set Address */
    ColAddr = offset;
    RowAddr = (block << 6) | page;
    if(nfc->model.nfDataWidth == 16){
        ColAddr = ColAddr >> 1;
    }
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_COL_OFFSET, ColAddr);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ADDR0_ROW_OFFSET, RowAddr);
    /* Set Data Size */
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, byte_count);
    FNfcPs_ClearFIFO(nfc);
    /* Set Command (send) */
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_PROGRAM_PAGE_1);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command | (0x1<<6));
    /* Wait for FIFO_STATE.CF_EMPTY */
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }    
    /* poll dma status */
    do{
        error = FNfcPs_GetDMAStatus(nfc);
        if(error & 0x2)
        {
            return FMSH_FAILURE;
        }
    }while((error & 0x1) == 0);
    
    return FMSH_SUCCESS;
}

int Nandflash_ProgramPage(FNfcPs_T* nfc)
{
    int error;
    u32 command; 
    
    command= FNfcPs_FindInstruction(nfc, NANDFLASH_WRITE_PAGE);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command);
    /* Wait for FIFO_STATE.CF_EMPTY */
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    /* Wait For Ready */
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

int Nandflash_ReadPageCacheLast(FNfcPs_T* nfc)
{
    int error;
    u32 command;
    
    /* Set Command (Read) */
    command = FNfcPs_FindInstruction(nfc, NANDFLASH_READ_PAGE_CACHE_LAST);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_COMMAND_OFFSET, command);
    /* Wait for FIFO_STATE.CF_EMPTY */
    error = FNfcPs_WaitForBit(nfc, NFCPS_FIFO_STATE_OFFSET, NFCPS_STAT_CFIFO_EMPTY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    /* Wait For Ready */
    error = FNfcPs_WaitForBit(nfc, NFCPS_STATUS_OFFSET, NFCPS_STAT_MEM0_RDY_MASK, 1);
    if(error){
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

