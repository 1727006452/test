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
* @file fmsh_nfcps.c
* @addtogroup nfcps_v1_0
* @{
*
* Contains implements the interface functions of the FNfcPs driver.
* See fmsh_nfcps.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 11/26/20 
*            The FNfcPs_CfgInitialize API has been modified to initialize 
*		     the member of FNfcPs_T struct.
*
* </pre>
*
******************************************************************************/

#include "fmsh_ps_parameters.h"
#include "fmsh_slcr.h"
#include "fmsh_nfcps_lib.h"

static void StubStatusHandler(void *callBackRef, u32 statusEvent,
                              u32 byteCount);

/*****************************************************************************
* This function initializes a specific FNfcPs_T device/instance. This function
* must be called prior to using the device to read or write any data.
*
* @param	nfc is a pointer to the FNfcPs_T instance.
* @param	configPtr points to the FNfcPs_T device configuration structure.
*
* @return
*		- FMSH_SUCCESS if successful.
*		- FMSH_FAILURE if fail.
*
* @note		The user needs to first call the FNfcPs_LookupConfig() API
*		which returns the Configuration structure pointer which is
*		passed as a parameter to the FNfcPs_CfgInitialize() API.
*
******************************************************************************/
int FNfcPs_CfgInitialize(FNfcPs_T *nfc, FNfcPs_Config_T *configPtr)
{
    FMSH_ASSERT(nfc != NULL);
    
    /* Init value */
    nfc->config.deviceId = configPtr->deviceId;
    nfc->config.baseAddress = configPtr->baseAddress;
    nfc->config.flashWidth = configPtr->flashWidth;
    
    nfc->flag = 0;
	
	nfc->cap1.jedec_supp = 0;
	nfc->cap1.onfi_supp = 0;
	nfc->eccCalc = NULL;
	nfc->eccCode = NULL;
	
	nfc->isBadBlockWrite = 0;
    nfc->bbInfo = NULL;
    nfc->bbMap = NULL;
	
    nfc->dataBufferPtr = NULL;
    nfc->spareBufferPtr = NULL;  
    
    nfc->statusHandler = StubStatusHandler; 
	nfc->statusRef = NULL;

    return FMSH_SUCCESS;
}

/*****************************************************************************
* This function resets spi device registers to default value.
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
void FNfcPs_Reset(FNfcPs_T* nfc)
{  
    FSlcrPs_ipSetRst(SLCR_NFC_CTRL, NFC_AHB_RST);
    FSlcrPs_ipSetRst(SLCR_NFC_CTRL, NFC_REF_RST);
    FSlcrPs_ipReleaseRst(SLCR_NFC_CTRL, NFC_AHB_RST);   
    FSlcrPs_ipReleaseRst(SLCR_NFC_CTRL, NFC_REF_RST);   

}

/*****************************************************************************
* This function tests if nfc device exists.
*
* @param
*
* @return
*		- FMSH_SUCCESS if nfc device exists.
*		- FMSH_FAILURE if nfc device not exists.
*
* @note		
*
******************************************************************************/
int FNfcPs_SelfTest(FNfcPs_T* nfc)
{   
    u32 value;
    FNfcPs_SetEccOffset(nfc, 0x5a);
    value = FNfcPs_GetEccOffset(nfc);
    if(value != 0x5a)
        return FMSH_FAILURE;
    FNfcPs_SetEccOffset(nfc, 0x0);
    return FMSH_SUCCESS;   
}

/*****************************************************************************
* This function sets point to status handler as well as its callback parameter .
*
* @param
*
* @return
*
* @note		
*       - this function is usually used called in interrupt 
*       - implemented by user
*
******************************************************************************/
void FNfcPs_SetStatusHandler(FNfcPs_T* nfc, void* callBackRef,
                          Nfc_StatusHandler funcPtr)
{
    FMSH_ASSERT(nfc != NULL);
    FMSH_ASSERT(funcPtr != NULL);
    
    nfc->statusHandler = funcPtr;
    nfc->statusRef = callBackRef;
}

/*****************************************************************************
* This is a stub for the status callback. The stub is here in case the upper
* layers forget to set the handler.
*
* @param	CallBackRef is a pointer to the upper layer callback reference
* @param	StatusEvent is the event that just occurred.
* @param	ByteCount is the number of bytes transferred up until the event
*		occurred.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubStatusHandler(void *CallBackRef, u32 StatusEvent,
                              unsigned ByteCount)
{
    (void) CallBackRef;
    (void) StatusEvent;
    (void) ByteCount;
}

void FNfcPs_InterruptHandler(void *instancePtr)
{
    FNfcPs_T *nfc;
    u32 intrStatus;
    
    nfc = (FNfcPs_T*)instancePtr;
    
    intrStatus = FNfcPs_GetIntrStatus(nfc) & FNfcPs_GetIntrMask(nfc);
    FNfcPs_ClearIntrStatus(nfc); 
    
    if(intrStatus & NFCPS_INTR_ECC_INT0_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_ECC_INT0_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_STAT_ERR_INT0_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_STAT_ERR_INT0_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_MEM0_RDY_INT_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_MEM0_RDY_INT_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_PG_SZ_ERR_INT_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_PG_SZ_ERR_INT_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_SS_READY_INT_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_SS_READY_INT_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_TRANS_ERR_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_TRANS_ERR_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_DMA_INT_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_DMA_INT_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_DATA_REG_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_DATA_REG_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_CMD_END_INT_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_CMD_END_INT_MASK, 
                           0);
    }
    if(intrStatus & NFCPS_INTR_PORT_INT_MASK)
    {
        nfc->statusHandler(nfc->statusRef, 
                           NFCPS_INTR_PORT_INT_MASK, 
                           0);
    }
    
}
