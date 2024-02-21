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
* @file fmsh_nfcps_hw.c
* @addtogroup nfcps_v1_0
* @{
*
* Contains implements the low level interface functions of the FNfcPs driver.
* See fmsh_nfcps_hw.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 11/26/20 
* 		     The FNfcPs_SetIOWidth API has been modified to delete 
* 		     error check. 
* 		     The FNfcPs_SetEccBlockSize API has been modified to delete 
* 		     error check. 	     
* 		     The FNfcPs_SetBlockSize API has been modified to delete 
* 		     error check. 	
* 		     The FNfcPs_SetProtArea API has been modified to delete 
* 		     error check. 	
* 		     The FNfcPs_SetEccCtrl API has been modified to delete 
* 		     error check. 	
* 		     The FNfcPs_SetDataRegSize API has been modified to delete 
* 		     error check. 
* 		     The FNfcPs_SetDMACtrl API has been modified to delete 
* 		     error check. 
* 		     The FNfcPs_WaitForBit API has been modified to increase 
* 		     timeout time.
* 		     
* </pre>
*
******************************************************************************/

#include "fmsh_nfcps_lib.h"

/***************************
* CTRL
*************/
void FNfcPs_SetCtrl(FNfcPs_T* nfc, uint32_t ctrl)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, ctrl);
}

uint32_t FNfcPs_GetCtrl(FNfcPs_T* nfc)
{
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
}

/***************************
 * CTRL detail
 ***************/
void FNfcPs_EnableReadStatus(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_READ_STATUS_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableReadRNB(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_READ_STATUS_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableRetry(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_RETRY_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableRetry(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_RETRY_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableIntr(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_INT_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableIntr(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_INT_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableEcc(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_ECC_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableEcc(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_ECC_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableBbm(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_BBM_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableBbm(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_BBM_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableProt(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_PROT_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableProt(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_PROT_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableAddr0AutoIncr(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_ADDR0_AUTO_INCR_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableAddr0AutoIncr(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_ADDR0_AUTO_INCR_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableAddr1AutoIncr(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_ADDR1_AUTO_INCR_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableAddr1AutoIncr(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_ADDR1_AUTO_INCR_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}  

void FNfcPs_EnableSmallBlock(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_SMALL_BLOCK_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableSmallBlock(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_SMALL_BLOCK_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableMLun(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_MLUN_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableMLun(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_MLUN_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_EnableAutoReadStatus(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg |= NFCPS_AUTO_READ_STAT_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

void FNfcPs_DisableAutoReadStatus(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_AUTO_READ_STAT_EN_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
}

int FNfcPs_SetIOWidth(FNfcPs_T* nfc, uint32_t width)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    if(width == 8)
    {
        configReg &= ~NFCPS_IOWIDTH16_MASK;
    }
    else if(width == 16)
    {
        configReg |= NFCPS_IOWIDTH16_MASK;
    }
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);    
    
    return FMSH_SUCCESS;
}

int FNfcPs_SetEccBlockSize(FNfcPs_T* nfc, uint32_t size)
{
    uint32_t configReg;
    uint32_t sizeConfig;
    
    /* validity check */
    if(size == 256)
    {
        sizeConfig = 0;
    }
    else if(size == 512)
    {
        sizeConfig = 1;
    }
    else if(size == 1024)
    {
        sizeConfig = 2;
    }
    else if(size == 2048)
    {
        sizeConfig = 3;
    }
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_ECC_BLOCK_SIZE_MASK;
    configReg |= (sizeConfig << NFCPS_ECC_BLOCK_SIZE_SHIFT) & NFCPS_ECC_BLOCK_SIZE_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);
    
    return FMSH_SUCCESS;
}

int FNfcPs_SetBlockSize(FNfcPs_T* nfc, uint32_t size)
{
    uint32_t configReg;
    uint32_t sizeConfig;
    
    /* validity check */
    if(size == 32)
    {
        sizeConfig = 0;
    }
    else if(size == 64)
    {
        sizeConfig = 1;
    }
    else if(size == 128)
    {
        sizeConfig = 2;
    }
    else if(size == 256)
    {
        sizeConfig = 3;
    }
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET);
    configReg &= ~NFCPS_BLOCK_SIZE_MASK;
    configReg |= (sizeConfig << NFCPS_BLOCK_SIZE_SHIFT) & NFCPS_BLOCK_SIZE_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_CONTROL_OFFSET, configReg);
    
    return FMSH_SUCCESS;
}

/******************************
* MEM_CTRL
************/
void FNfcPs_EnableHwProt(FNfcPs_T* nfc)
{
    /* enable nandflash device write protect by setting wp# low */
    uint32_t configReg;

    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_MEM_CTRL_OFFSET);
    configReg &= ~NFCPS_CTRL_MEM0WP_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_MEM_CTRL_OFFSET, configReg);
}

void FNfcPs_DisableHwProt(FNfcPs_T* nfc)
{
    /* disable nandflash device write protect by setting wp# high */
    uint32_t configReg;

    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_MEM_CTRL_OFFSET);
    configReg |= NFCPS_CTRL_MEM0WP_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_MEM_CTRL_OFFSET, configReg);
}

uint32_t FNfcPs_GetHwProt(FNfcPs_T* nfc)
{ 
    uint32_t configReg;
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_MEM_CTRL_OFFSET);
    configReg &= NFCPS_CTRL_MEM0WP_MASK;
    configReg >>= NFCPS_CTRL_MEM0WP_SHIFT;
    return configReg;   
}

/**********************************
 * PROTECT
 ******************/
int FNfcPs_SetProtArea(FNfcPs_T* nfc, uint32_t lowBlock, uint32_t highBlock)
{
    uint32_t configReg;
    
    configReg = (highBlock << NFCPS_PROT_UP_SHIFT) | lowBlock; 
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_PROTECT_OFFSET, configReg);
    
    return FMSH_SUCCESS;
}

/***********************************
 * INT_MASK
 *****************/
void FNfcPs_SetIntrMask(FNfcPs_T* nfc, uint32_t mask)
{ 
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_INT_MASK_OFFSET, mask);
}

uint32_t FNfcPs_GetIntrMask(FNfcPs_T* nfc)
{ 
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_INT_MASK_OFFSET);
}

/***********************************
 * INT_STATUS
 *****************/
void FNfcPs_ClearIntrStatus(FNfcPs_T* nfc)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_INT_STATUS_OFFSET, ~NFCPS_INTR_ALL);
}

uint32_t FNfcPs_GetIntrStatus(FNfcPs_T* nfc)
{  
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_INT_STATUS_OFFSET);
}

/*************************************
 * ECC_CTRL
 * all configuration parameters required by the ECC
 * IntrSrc-correctable error/uncorrectable error/acceptable errors level overflow
 * Errlvl-number of errors that is acceptable for the host system
 * EccCap-2/4/8/16/24/32
 ************************/
int FNfcPs_SetEccCtrl(FNfcPs_T* nfc, uint32_t sel, uint32_t threshold, uint32_t cap)
{
    uint32_t configReg;
    
    configReg = (sel << NFCPS_ECC_SEL_SHIFT) | 
                 (threshold << NFCPS_ECC_THRESHOLD_SHIFT) | cap;   
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ECC_CTRL_OFFSET, configReg);
    
    return FMSH_SUCCESS;
}

/***********************************
 * ECC_OFFSET
 * offset value from the beginning of the page to the place where correction words will be stored
 * bigger than the value in the DATA_SIZE register
 ****************/
void FNfcPs_ClearEccStat(FNfcPs_T* nfc)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ECC_STAT_OFFSET, 0x0);
}

u32 FNfcPs_GetEccStat(FNfcPs_T* nfc)
{
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_ECC_STAT_OFFSET);
}

/***********************************
 * ECC_STAT
 * 
 *
 ****************/
void FNfcPs_SetEccOffset(FNfcPs_T* nfc, u16 offset)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_ECC_OFFSET_OFFSET, offset);
}

u16 FNfcPs_GetEccOffset(FNfcPs_T* nfc)
{
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_ECC_OFFSET_OFFSET);
}

/***********************************
 * FIFO
 ******************/
void FNfcPs_WriteFIFO(FNfcPs_T* nfc, uint32_t data)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_FIFO_DATA_OFFSET, data);
}

uint32_t FNfcPs_ReadFIFO(FNfcPs_T* nfc)
{
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_FIFO_DATA_OFFSET);
}

void FNfcPs_ClearFIFO(FNfcPs_T* nfc)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_FIFO_INIT_OFFSET, NFCPS_FIFO_INIT_MASK);
}

/************************************
 * DATA_REG_SIZE
 ******************/
int FNfcPs_SetDataRegSize(FNfcPs_T* nfc, uint32_t size)
{
    uint32_t configReg;
    
    configReg = size & NFCPS_DATAREG_SIZE_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_REG_SIZE_OFFSET, configReg);  
    
    return FMSH_SUCCESS;
}

/************************************
 * DATA_REG
 * storage of the data that is read in the registered mod
 *******************/
uint32_t FNfcPs_GetDataReg(FNfcPs_T* nfc)
{
    return FMSH_ReadReg(nfc->config.baseAddress, NFCPS_DATA_REG_OFFSET);
}

/***********************************
 * DATA_SIZE
 * set number of bytes per transferred block when ecc enabled
 * (ECC_BLOCK_SIZE) * n �� DATA_SIZE �� (ECC_BLOCK_SIZE +32) * n,
 **********************/
void FNfcPs_SetDataSizeInBytes(FNfcPs_T* nfc, uint32_t size)
{
    uint32_t configReg;

    configReg = size & NFCPS_DATA_SIZE_MASK;
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DATA_SIZE_OFFSET, configReg);
}

/************************************
 * STATUS_MASK
 *********************/
void FNfcPs_SetStatusMask(FNfcPs_T* nfc, uint32_t mask)
{  
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_STATUS_MASK_OFFSET, mask);
}

/***********************************
 * DMA
 **********************/
void FNfcPs_SetDMAAddr(FNfcPs_T* nfc, uint32_t address)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DMA_ADDR_L_OFFSET, address);
}

void FNfcPs_SetDMACnt(FNfcPs_T* nfc, uint32_t byteCount)
{
    uint32_t configReg;
    
    configReg = byteCount & (~0x3);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DMA_CNT_OFFSET, configReg);
}

void FNfcPs_SetDMATrigLvl(FNfcPs_T* nfc, uint32_t level)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DMA_TLVL_OFFSET, level);
}

int FNfcPs_SetDMACtrl(FNfcPs_T* nfc, uint32_t mode, uint32_t burstType, uint32_t start)
{
    uint32_t configReg;
    
    configReg = (mode  << NFCPS_DMA_MODE_SHIFT) | (burstType << NFCPS_DMA_BURSTTYPE_SHIFT);
    if(start != 0)
    {
        configReg |= NFCPS_DMA_START_MASK;
    }
    else 
    {
        configReg &= ~NFCPS_DMA_START_MASK;
    }
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DMA_CTRL_OFFSET, configReg);
    
    return FMSH_SUCCESS;
}

uint32_t FNfcPs_GetDMAStatus(FNfcPs_T* nfc)
{
    uint32_t configReg;
    
    configReg = FMSH_ReadReg(nfc->config.baseAddress, NFCPS_DMA_CTRL_OFFSET);
    configReg &= (NFCPS_DMA_READY_MASK | NFCPS_DMA_ERR_MASK);
    
    return configReg;
}

/********************************
 * TIMING
 ****************/
void FNfcPs_SetTiming(FNfcPs_T* nfc)
{
    uint32_t configReg ;
    configReg = ((FPS_NFC_TIMING_TWHR & 0x3f) << 24) | 
                ((FPS_NFC_TIMING_TRHW & 0x3f) << 16) | 
                ((FPS_NFC_TIMING_TADL & 0x3f) << 8) | 
                ((FPS_NFC_TIMING_TCCS & 0x3f) << 0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_TIME_SEQ_0_OFFSET, configReg);
    
    configReg = ((FPS_NFC_TIMING_TWW & 0x3f) << 16) | 
                ((FPS_NFC_TIMING_TRR & 0x3f) << 8) | 
                ((FPS_NFC_TIMING_TWB & 0x3f) << 0);
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_TIME_SEQ_1_OFFSET, configReg);
    
    configReg = ((FPS_NFC_TIMING_TRWH & 0xf) << 4) | 
                ((FPS_NFC_TIMING_TRWP & 0xf));
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_TIMINGS_ASYN_OFFSET, configReg);
}

/******************************
 * BBM
 *****************/
void FNfcPs_InitRemap(FNfcPs_T* nfc)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_BBM_CTRL_OFFSET, 0x1);
}

void FNfcPs_SetDevPtr(FNfcPs_T* nfc, uint32_t address)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DEV0_PTR_OFFSET, address);
}

void FNfcPs_SetDevSize(FNfcPs_T* nfc, uint32_t size)
{
    FMSH_WriteReg(nfc->config.baseAddress, NFCPS_DEV0_SIZE_OFFSET, size);
}

/********************************
 * Poll bit
 *******************/
int FNfcPs_WaitForBit(FNfcPs_T* nfc, uint32_t regOffset, uint32_t mask, u8 pollBit)
{
    uint32_t status; 
    uint32_t timeout = 0;
    
    while(1){
        status = FMSH_ReadReg(nfc->config.baseAddress, regOffset);
        if(pollBit == 0){
            status = ~status;
        }
        status &= mask;
        if(status == mask){
            return FMSH_SUCCESS;
        }
        delay_1us();
        timeout++;
        if(timeout >= 100000){
            return FMSH_FAILURE;
        }          
    }  
}
