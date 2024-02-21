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
* @file fmsh_spips_sw.c
* @addtogroup spips_v1_1
* @{
*
* Contains implements the low level interface functions of the FSpiPs driver.
* See fmsh_spips_sw.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.10  hzq 11/26/20 
* 		     This file has been deprecated and used for compatibility only 
*            which is not recommended to use in the new program.
*
* </pre>
*
******************************************************************************/

#include "fmsh_spips_lib.h"

SPI_CAPS(mst_default) = {
    .isMaster = 1,
    .loop = 0,
    .hasIntr = 0,
    .hasDma = 0,
    .txEmptyLvl = 10,
    .rxFullLvl = 0,
    .tsfMode = SPIPS_TRANSFER_STATE,
    .cpol = 0,
    .cpha = 0,
    .frameSize = 8,
    .frameLen = 128,
    .baudRate = 100, 
    .sampleDelay = 0,
};

int FSpiPs_Initialize_Master(FSpiPs_T* spiPtr)
{
    int status;
    
    spiPtr->slaveSelect = 1;
    status = FSpiPs_HwInit(spiPtr, GET_SPI_CAPS(mst_default)); 
    if(status)
        return FMSH_FAILURE;
    spiPtr->frameSize = spiPtr->caps.frameSize;
    FSpiPs_SetEnable(spiPtr, 1);
    
    return FMSH_SUCCESS;
}


SPI_CAPS(slv_default) = {
    .isMaster = 0,
    .loop = 0,
    .hasIntr = 0,
    .hasDma = 0,
    .txEmptyLvl = 10,
    .rxFullLvl = 0,
    .tsfMode = SPIPS_TRANSFER_STATE,
    .cpol = 0,
    .cpha = 0,
    .frameSize = 8,
    .frameLen = 128,
    .sampleDelay = 0,
};

int FSpiPs_Initialize_Slave(FSpiPs_T* spiPtr)
{
    int status;
    
    status = FSpiPs_HwInit(spiPtr, GET_SPI_CAPS(slv_default)); 
    if(status)
        return FMSH_FAILURE;
    spiPtr->frameSize = spiPtr->caps.frameSize;
    FSpiPs_SetEnable(spiPtr, 1);
    
    return FMSH_SUCCESS;
}