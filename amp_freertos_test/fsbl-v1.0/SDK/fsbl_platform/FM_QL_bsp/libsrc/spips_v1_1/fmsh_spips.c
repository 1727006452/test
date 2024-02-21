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
* @file fmsh_spips.c
* @addtogroup spips_v1_1
* @{
*
* Contains implements the interface functions of the FSpiPs driver.
* See fmsh_spips.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.10  hzq 11/26/20 
* 		     The string.h header has benn increased.
* 		     The FSpiPs_InterruptHandler API has been modified to increase  
* 		     data transfer operation.
* 		     The spi_dma_default has been increased which is a struct of
* 		     dma related parameters.
* 		     The spi_caps_default has been increased which is a struct of 
* 		     spi user configuration.
* 		     The FSpiPs_Initialize API has been increased to initialize 
* 		     FSpiPs with default configuration.
* 		     The FSpiPs_InitHw API has been increased to initialize 
* 		     FSpiPs with user defined configuration.
* 		     The FSpiPs_Transfer API has been increased to transfer data.
* 		     The FSpiPs_PolledTransfer API has been used to transfer data 
*            using poll. 		     
* 1.12 hzq 2022/03/04
* 			 The FSpiPs_InterruptHandler API has been modified to support
* 			 user handler completely
* </pre>
*
******************************************************************************/

#include <string.h>
#include "fmsh_ps_parameters.h"
#include "fmsh_slcr.h"
#include "fmsh_spips_lib.h" 

static void StubStatusHandler(void *callBackRef, u32 statusEvent,
                              u32 byteCount);

/*****************************************************************************
* This function initializes a specific FSpiPs_T device/instance. This function
* must be called prior to using the device to read or write any data.
*
* @param	spi is a pointer to the FSpiPs_T instance.
* @param	configPtr points to the FSpiPs_T device configuration structure.
*
* @return
*		- FMSH_SUCCESS if successful.
*		- FMSH_FAILURE if fail.
*
* @note		The user needs to first call the FSpiPs_LookupConfig() API
*		which returns the Configuration structure pointer which is
*		passed as a parameter to the FSpiPs_CfgInitialize() API.
*
******************************************************************************/
int FSpiPs_CfgInitialize(FSpiPs_T* spiPtr, FSpiPs_Config_T* configPtr)
{
    FMSH_ASSERT(spiPtr != NULL);
    FMSH_ASSERT(configPtr != NULL);
    
    // Set default value
    spiPtr->config.deviceId = configPtr->deviceId;
    spiPtr->config.baseAddress = configPtr->baseAddress;
    
    spiPtr->version = 112;
    spiPtr->flags = 0;

    spiPtr->isBusy = 0;
    spiPtr->isEnable = 0;
    spiPtr->isSlaveSelectDecode = 0;
    spiPtr->isSlaveManual = 0;
    spiPtr->slaveSelect = 0;
    spiPtr->mode = 0;
    
    spiPtr->dma = NULL;
    
    spiPtr->totalBytes = 0;
    spiPtr->remainingBytes = 0;
    spiPtr->requestedBytes = 0;
    spiPtr->sendBufferPtr = NULL;
    spiPtr->recvBufferPtr = NULL;
    
    spiPtr->statusHandler = StubStatusHandler;
    spiPtr->statusRef = NULL;
    spiPtr->priv = NULL;
    
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
void FSpiPs_Reset(FSpiPs_T* spiPtr)
{  
    if(spiPtr->config.deviceId == FPS_SPI0_DEVICE_ID)
    {
        FSlcrPs_ipSetRst(SLCR_SPI0_CTRL, SPI0_APB_RST);
        FSlcrPs_ipSetRst(SLCR_SPI0_CTRL, SPI0_REF_RST);
        FSlcrPs_ipReleaseRst(SLCR_SPI0_CTRL, SPI0_APB_RST);
        FSlcrPs_ipReleaseRst(SLCR_SPI0_CTRL, SPI0_REF_RST);
    }
    else if(spiPtr->config.deviceId == FPS_SPI1_DEVICE_ID)
    {
        FSlcrPs_ipSetRst(SLCR_SPI1_CTRL, SPI1_APB_RST);
        FSlcrPs_ipSetRst(SLCR_SPI1_CTRL, SPI1_REF_RST);
        FSlcrPs_ipReleaseRst(SLCR_SPI1_CTRL, SPI1_APB_RST);
        FSlcrPs_ipReleaseRst(SLCR_SPI1_CTRL, SPI1_REF_RST);
    }
}

/*****************************************************************************
* This function tests if spi device exists.
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SelfTest(FSpiPs_T* spiPtr)
{   
    u32 value;
    FSpiPs_SetTxEmptyLvl(spiPtr, 0xa);
    value =  FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_TXFTLR_OFFSET);
    if(value != 0xa)
        return FMSH_FAILURE;
    FSpiPs_SetTxEmptyLvl(spiPtr, 0x0);
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
void FSpiPs_SetStatusHandler(FSpiPs_T* spiPtr, void* callBackRef,
                             FSpiPs_StatusHandler funcPtr)
{
    FMSH_ASSERT(spiPtr != NULL);
    FMSH_ASSERT(funcPtr != NULL);
    
    spiPtr->statusHandler = funcPtr;
    spiPtr->statusRef = callBackRef;
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
static void StubStatusHandler(void *callBackRef, u32 statusEvent,
                              u32 byteCount)
{
    (void) callBackRef;
    (void) statusEvent;
    (void) byteCount;
}

/*****************************************************************************
* The interrupt handler for SPI interrupts. This function must be connected
* by the user to an interrupt source. This function does not save and restore
* the processor context such that the user must provide this processing.
*
* The interrupts that are handled are:
*
* - Multi Master Fault. This interrupt is generated if both device set as master
*   and try to transfer with the same slave. The driver aborts this transfer.
*   The upper layer software is informed of the error.
*
* - Data Receive Register (FIFO) Overrun. This interrupt is generated when the
*   SPI device attempts to write a received byte to an already full DRR/FIFO.
*   A full DRR/FIFO usually means software is not emptying the data in a timely
*   manner.  No action is taken by the driver other than to inform the upper
*   layer software of the error.
*
* - Data Receive Register (FIFO) Underrun. This interrupt is generated when the
*   SPI device attempts to read a received byte from an empty DRR/FIFO.
*   A empty DRR/FIFO usually means software is not fill the data in a timely
*   manner.  No action is taken by the driver other than to inform the upper
*   layer software of the error.
*
* - Data Transmit Register (FIFO) Overrun. This interrupt is generated when
*   the SPI device attempts to write data to an already full DTR/FIFO.  
*   An full DTR/FIFO usually means that software is not giving the
*   device data in a timely manner. No action is taken by the driver other than
*   to inform the upper layer software of the error.
*
* - Data Transmit Register (FIFO) Empty. This interrupt is generated when the
*   transmit register or FIFO is empty. The driver uses this interrupt during a
*   transmission to continually send/receive data until there is no more data
*   to send/receive.
*
* - Data Receive Register (FIFO) Full. This interrupt is generated when the
*   receive register or FIFO is full. The driver uses this interrupt during a
*   transmission, used as slave, to continually send/receive data until 
*   there is no more data to send/receive.
*
* @param	InstancePtr is a pointer to the FSpiPs_T instance to be worked on.
*
* @return	None.
*
* @note
*
* The slave select register is being set to deselect the slave when a transfer
* is complete.  This is being done regardless of whether it is a slave or a
* master since the hardware does not drive the slave select as a slave.
*
******************************************************************************/
void FSpiPs_InterruptHandler(void* instancePtr)
{
    FSpiPs_T* spiPtr = (FSpiPs_T*)instancePtr;
    FSpiPs_Caps* caps;
    u32 intrStatus;
    u32 rxw, txw;
    volatile int cnt;
    
    FMSH_ASSERT(instancePtr != NULL);
    
    if(spiPtr->isBusy == FALSE)
        return;
    
    /* Get & Clear interrupt status */
    intrStatus =  FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_ISR_OFFSET);
    (void)FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_ICR_OFFSET);
    
    /* Multi-Master Fault */
    if (intrStatus & SPIPS_INTR_MSTIS_MASK)
    {
        spiPtr->statusHandler(spiPtr,
                              SPIPS_INTR_MSTIS_MASK, 
                              0);
    }
    
    /* Check for overflow and underflow errors */
    if (intrStatus & SPIPS_INTR_RXOIS_MASK) 
    {
        spiPtr->statusHandler(spiPtr,
                              SPIPS_INTR_RXOIS_MASK, 
                              0);
    }
    
    if (intrStatus & SPIPS_INTR_RXUIS_MASK) 
    {
        spiPtr->statusHandler(spiPtr,
                              SPIPS_INTR_RXUIS_MASK, 
                              0);
    }
    
    if (intrStatus & SPIPS_INTR_TXOIS_MASK) 
    {
        spiPtr->statusHandler(spiPtr,
                              SPIPS_INTR_TXOIS_MASK, 
                              0);
    }
    
    /* Tx Empty */
    if (intrStatus & SPIPS_INTR_TXEIS_MASK) 
    {
        if((spiPtr->flags & SPI_INTR_USRHANDLED) == 0)
        {
        	if((spiPtr->remainingBytes != 0) || (spiPtr->requestedBytes != 0))
        	{
                caps = &(spiPtr->caps);
                
                cnt = FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_RXFLR_OFFSET);
                while(cnt > 0 && spiPtr->requestedBytes != 0)
                {
                	rxw = FSpiPs_Recv(spiPtr);
                    if(caps->frameSize == 8)
                    {
                        *(u8*)(spiPtr->recvBufferPtr) = (u8)rxw;
                    }
                    else if(caps->frameSize == 16)
                    {
                        *(u16*)(spiPtr->recvBufferPtr) = (u16)rxw;
                    }
                    else if(caps->frameSize == 32)
                    {
                        *(u32*)(spiPtr->recvBufferPtr) = (u32)rxw;
                    }
                    
                    spiPtr->recvBufferPtr += caps->frameSize >> 3;
                    spiPtr->requestedBytes -= caps->frameSize >> 3;
                    cnt--;
                }
                cnt = SPIPS_FIFO_DEPTH - caps->txEmptyLvl;
                while(cnt > 0 && spiPtr->remainingBytes != 0)
                {
                    if(caps->frameSize == 8)
                    {
                        FSpiPs_Send(spiPtr, *(u8*)spiPtr->sendBufferPtr);
                    }
                    else if(caps->frameSize == 16)
                    {
                        FSpiPs_Send(spiPtr, *(u16*)spiPtr->sendBufferPtr);
                    }
                    else if(caps->frameSize == 32)
                    {
                        FSpiPs_Send(spiPtr, *(u32*)spiPtr->sendBufferPtr);
                    }
                    spiPtr->sendBufferPtr += caps->frameSize >> 3;
                    spiPtr->remainingBytes -= caps->frameSize >> 3;
                    cnt--;
                }   
            }
            else if(!(FSpiPs_GetStatus(spiPtr) & SPIPS_SR_BUSY))
            {
                /* Transfer complete, diable interrupt & spi*/
                FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_IMR_OFFSET, 0);

                FSpiPs_SetEnable(spiPtr, 0);

                spiPtr->isBusy = FALSE;

                /* inform upper layer*/
                spiPtr->statusHandler(spiPtr,
                				      SPI_TRANSFER_DONE,
                                      0);
            }
        }
        /*upper layer has something else to do*/
        spiPtr->statusHandler(spiPtr,
                              SPIPS_INTR_TXEIS_MASK,
                              0);
    }
    
    /* Rx FIFO Full */
    if (intrStatus & SPIPS_INTR_RXFIS_MASK)
    {   
        if((spiPtr->flags & SPI_INTR_USRHANDLED) == 0)
        {
            caps = &(spiPtr->caps);
            while((FSpiPs_GetStatus(spiPtr) & SPIPS_SR_RFNE) && spiPtr->requestedBytes != 0)
            {
            	rxw = FSpiPs_Recv(spiPtr);
                if(caps->frameSize == 8)
                {
                    *(u8*)(spiPtr->recvBufferPtr) = (u8)rxw;
                }
                else if(caps->frameSize == 16)
                {
                    *(u16*)(spiPtr->recvBufferPtr) = (u16)rxw;
                }
                else if(caps->frameSize == 32)
                {
                    *(u32*)(spiPtr->recvBufferPtr) = (u32)rxw;
                }
                
                spiPtr->recvBufferPtr += caps->frameSize >> 3;
                spiPtr->requestedBytes -= caps->frameSize >> 3;
                cnt--;
            }
            if((spiPtr->remainingBytes == 0) && (spiPtr->requestedBytes == 0))
            {
                /* Transfer complete, diable interrupt & spi*/
                FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_IMR_OFFSET, 0);

                FSpiPs_SetEnable(spiPtr, 0);

                spiPtr->isBusy = FALSE;

                /* inform upper layer*/
                spiPtr->statusHandler(spiPtr,
                					  SPI_TRANSFER_DONE,
                                      0);

            }
        }
        /*upper layer has something else to do*/
        spiPtr->statusHandler(spiPtr,
                              SPIPS_INTR_RXFIS_MASK,
                              0);
    }
}

static FSpiPs_Caps spi_caps_default = {
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
    .frameLen = 256,
    .baudRate = 10, 
    .sampleDelay = 0,
};

static FSpiPs_Dma spi_dma_default = {
    .type = 0,
    .txIf = 2,
    .rxIf = 3,
    .io =  SPIPS_DR_OFFSET, 
};

/*****************************************************************************
* This function initializes controller struct
*
* @param
*       - DeviceId contains the ID of the device
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FSpiPs_Initialize(FSpiPs_T* spiPtr, u16 deviceId)
{
    FSpiPs_Config_T* cfgPtr;
    
    FMSH_ASSERT(spiPtr != NULL);
    //get config info table from parameter
    cfgPtr = FSpiPs_LookupConfig(deviceId);
    if (cfgPtr == NULL) 
    {
        return FMSH_FAILURE;
    }
    //initialize controller struct
    return FSpiPs_CfgInitialize(spiPtr, cfgPtr);  
}

/*****************************************************************************
* This function initializes controller hardware
*
* @param
*       - capsPtr point to FSpiPs_Caps
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FSpiPs_HwInit(FSpiPs_T* spiPtr, FSpiPs_Caps* capsPtr)
{
    int status = FMSH_SUCCESS;
    u32 configReg;
    FSpiPs_Caps* caps;
    
    FMSH_ASSERT(spiPtr != NULL);
    
    // Check whether there is another transfer in progress. Not thread-safe
    if(spiPtr->isBusy == TRUE) 
    {
        status = SPI_BUSY;
    }
    else
    {
        //config spi caps
        if(capsPtr == NULL)
            memcpy((void*)&(spiPtr->caps),
                   (void*)&spi_caps_default,
                   (unsigned int)sizeof(FSpiPs_Caps));
        else
            memcpy((void*)&(spiPtr->caps),
                   (void*)capsPtr,
                   (unsigned int)sizeof(FSpiPs_Caps));
        
        spiPtr->dma = &spi_dma_default;
        
        caps = &(spiPtr->caps);
        
        //reset hardware
        FSpiPs_Reset(spiPtr);
        
        //disable spi
        FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_SSIENR_OFFSET, 0x0);
        
        //config spi as master or slave
        if(caps->isMaster)
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_MSTR_OFFSET, 0x1);
        else
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_MSTR_OFFSET, 0x0);
        
        configReg = FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_CTRLR0_OFFSET);
        configReg &= ~SPIPS_CTRL0_MASK;
        //config spi frame size_32(4~32)
        if(caps->frameSize<4 || caps->frameSize > 32)
            status = SPI_INIT_FAIL;
        else
            configReg |= (caps->frameSize-1) << SPIPS_CTRL0_DFS32_SHIFT;
        //config spi transfer mode(0~3)  
        if(caps->tsfMode > 3)
            status = SPI_INIT_FAIL;
        else
            configReg |= caps->tsfMode << SPIPS_CTRL0_TMOD_SHIFT;
        //config spi cpol   
        if(caps->cpol)
            configReg |= 0x1 << SPIPS_CTRL0_SCPOL_SHIFT;
        //config spi cpha 
        if(caps->cpha)
            configReg |= 0x1 << SPIPS_CTRL0_SCPH_SHIFT;
        //config loop(none or internal or slcr)
        if(caps->loop == 1)
            configReg |= 0x1 << SPIPS_CTRL0_SRL_SHIFT;
        
        FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_CTRLR0_OFFSET, configReg); 
        
        //config baudrate & frame length
        if(caps->isMaster)
        {
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_BAUDR_OFFSET, caps->baudRate);  
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_CTRLR1_OFFSET, caps->frameLen - 1);
        }
        //config rx sample delay
        FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_RX_SAMPLE_OFFSET, caps->sampleDelay);  
        
        // Config IMR
        FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_IMR_OFFSET, 0);
        
        // Config Tx/Rx Threshold
        if(caps->hasIntr)
        {
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_TXFTLR_OFFSET, caps->txEmptyLvl);
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_RXFTLR_OFFSET, caps->rxFullLvl);
        }
        
        if(caps->hasDma)
        {
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_DMATDLR_OFFSET, caps->txEmptyLvl);
            FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_DMARDLR_OFFSET, caps->rxFullLvl);
        }
        
        //config slave selsct
        if(caps->isMaster)
        {
            if(spiPtr->isSlaveSelectDecode == 1)
            	FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_SER_OFFSET, 0x1 << spiPtr->slaveSelect);
            else
            	FMSH_WriteReg(spiPtr->config.baseAddress, SPIPS_SER_OFFSET, spiPtr->slaveSelect);
        }

    }
    
    if(status != FMSH_SUCCESS)
        return FMSH_FAILURE;
    else
        return FMSH_SUCCESS;
}

/*****************************************************************************
* This function transfers data
*
* @param
*       - sendBuffer is a point to write data
*       - recvBuffer is a point to read data
*       - byteCount is a number of bytes to transfer
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FSpiPs_Transfer(FSpiPs_T* spiPtr, void* sendBuffer, void* recvBuffer, u32 byteCount)
{
    int status = FMSH_SUCCESS;
    FSpiPs_Caps* caps;
    u32 intrMask = 0;
    
    FMSH_ASSERT(spiPtr != NULL);
    
    // Check whether there is another transfer in progress. Not thread-safe
    if(spiPtr->isBusy == TRUE)
    {
        status = SPI_BUSY;  
    }
    else
    {        
        //Get spi caps
        caps = &(spiPtr->caps);
        
        switch(caps->tsfMode)
        {
        case SPIPS_TRANSFER_STATE:
            if(sendBuffer == NULL || recvBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->remainingBytes = byteCount;   
            spiPtr->requestedBytes = byteCount;   
            intrMask |= SPIPS_INTR_TXEIS_MASK;
            break;
        case SPIPS_TRANSMIT_ONLY_STATE:
            if(sendBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->requestedBytes = 0;
            spiPtr->remainingBytes = byteCount;
            intrMask |= SPIPS_INTR_TXEIS_MASK;
            break;
        case SPIPS_RECEIVE_ONLY_STATE:
            if(recvBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->requestedBytes = byteCount;
            spiPtr->remainingBytes = 0;
            FSpiPs_SetDFNum(spiPtr, byteCount);
            intrMask |= SPIPS_INTR_RXFIS_MASK;
            break;
        case SPIPS_EEPROM_STATE:
            if(sendBuffer == NULL || recvBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->requestedBytes = byteCount;
            spiPtr->remainingBytes = 3;
            FSpiPs_SetDFNum(spiPtr, byteCount);
            intrMask |= SPIPS_INTR_RXFIS_MASK;
            break;
        default:
            status = FMSH_FAILURE;
            break;
        }
        
        spiPtr->totalBytes = byteCount;
        if(sendBuffer != NULL)
            spiPtr->sendBufferPtr = (u8*)sendBuffer;
        if(recvBuffer != NULL)
            spiPtr->recvBufferPtr = (u8*)recvBuffer;
        
        //clear all interrupts
        FSpiPs_DisableIntr(spiPtr, 0x3f);
        FSpiPs_ClearIntrStatus(spiPtr);
        
        //config interrupts
        if(caps->hasIntr)
        {
            intrMask |= SPIPS_INTR_RXOIS_MASK | SPIPS_INTR_RXUIS_MASK 
                | SPIPS_INTR_TXOIS_MASK;
            if(caps->isMaster)
                intrMask |= SPIPS_INTR_MSTIS_MASK;
            FSpiPs_EnableIntr(spiPtr, intrMask);
        }
        
        //Set the busy flag, cleared when transfer is done
        spiPtr->isBusy = TRUE; 
        
        //config slave select
        if(caps->isMaster)
            FSpiPs_SetSlave(spiPtr, spiPtr->slaveSelect);
        
        //enable spi to start transfer
        FSpiPs_SetEnable(spiPtr, 1);
        
        if(caps->hasDma)
        {
            if(sendBuffer != NULL)
                FSpiPs_EnableDMATx(spiPtr);
            if(recvBuffer != NULL)
                FSpiPs_EnableDMARx(spiPtr);
        }
        
        if(caps->tsfMode == SPIPS_RECEIVE_ONLY_STATE)
            /* Write one dummy data word to Tx FIFO */
            FSpiPs_Send(spiPtr, 0xffffffff);
    }
    
    if(status != FMSH_SUCCESS)
        return FMSH_FAILURE;
    else
        return FMSH_SUCCESS;
}

/*****************************************************************************
* This function transfers data with polled
*
* @param
*       - sendBuffer is a point to write data
*       - recvBuffer is a point to read data
*       - byteCount is a number of bytes to transfer
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FSpiPs_PolledTransfer(FSpiPs_T* spiPtr, void* sendBuffer, void* recvBuffer, u32 byteCount)
{
    int status = FMSH_SUCCESS;
    FSpiPs_Caps* caps;
    int cnt;
    u32 txLvl;
    u32 txEmptyLvl;
    
    FMSH_ASSERT(spiPtr != NULL);
    
    // Check whether there is another transfer in progress. Not thread-safe
    if(spiPtr->isBusy == TRUE)
    {
        status = SPI_BUSY;  
    }
    else
    {    
        //Get spi caps
        caps = &(spiPtr->caps);
        
        switch(caps->tsfMode)
        {
        case SPIPS_TRANSFER_STATE:
            if(sendBuffer == NULL || recvBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->remainingBytes = byteCount;   
            spiPtr->requestedBytes = byteCount;   
            break;
        case SPIPS_TRANSMIT_ONLY_STATE:
            if(sendBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->requestedBytes = 0;
            spiPtr->remainingBytes = byteCount;
            break;
        case SPIPS_RECEIVE_ONLY_STATE:
            if(recvBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->requestedBytes = byteCount;
            spiPtr->remainingBytes = 0;
            FSpiPs_SetDFNum(spiPtr, byteCount);
            break;
        case SPIPS_EEPROM_STATE:
            if(sendBuffer == NULL || recvBuffer == NULL)
                return FMSH_FAILURE;
            spiPtr->requestedBytes = byteCount;
            spiPtr->remainingBytes = 3;
            FSpiPs_SetDFNum(spiPtr, byteCount);
            break;
        default:
            status = FMSH_FAILURE;
            break;
        }
        
        spiPtr->totalBytes = byteCount;
        if(sendBuffer != NULL)
            spiPtr->sendBufferPtr = (u8*)sendBuffer;
        if(recvBuffer != NULL)
            spiPtr->recvBufferPtr = (u8*)recvBuffer;
        
        //config slave select
        FSpiPs_SetSlave(spiPtr, spiPtr->slaveSelect);
        
        //disable interrupt
        FSpiPs_DisableIntr(spiPtr, SPIPS_INTR_ALL);
        
        //Set the busy flag, cleared when transfer is done
        spiPtr->isBusy = TRUE; 
        
        //enable spi
        FSpiPs_SetEnable(spiPtr, 1);
        
        if(caps->tsfMode == SPIPS_RECEIVE_ONLY_STATE)
            /* Write one dummy data word to Tx FIFO */
            FSpiPs_Send(spiPtr, 0xffffffff);

        //polling tx fifo level until transfer complete
        txEmptyLvl = caps->txEmptyLvl;
        while(spiPtr->remainingBytes !=0 || spiPtr->requestedBytes != 0) 
        {
            txLvl = FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_TXFLR_OFFSET);
            if(txLvl <= txEmptyLvl)
            {
                cnt = FMSH_ReadReg(spiPtr->config.baseAddress, SPIPS_RXFLR_OFFSET);
                while(cnt > 0 && spiPtr->requestedBytes != 0)
                {
                    if(caps->frameSize == 8)
                    {
                        *(u8*)(spiPtr->recvBufferPtr) = (u8)FSpiPs_Recv(spiPtr);
                    }
                    else if(caps->frameSize == 16)
                    {
                        *(u16*)(spiPtr->recvBufferPtr) = (u16)FSpiPs_Recv(spiPtr);
                    }
                    else if(caps->frameSize == 32)
                    {
                        *(u32*)(spiPtr->recvBufferPtr) = (u32)FSpiPs_Recv(spiPtr);
                    }
                    
                    spiPtr->recvBufferPtr += caps->frameSize >> 3;
                    spiPtr->requestedBytes -= caps->frameSize >> 3;
                    cnt--;
                }
                cnt = SPIPS_FIFO_DEPTH - txEmptyLvl;
                while(cnt > 0 && spiPtr->remainingBytes != 0)
                {
                    if(caps->frameSize == 8)
                    {
                        FSpiPs_Send(spiPtr, *(u8*)spiPtr->sendBufferPtr);
                    }
                    else if(caps->frameSize == 16)
                    {
                        FSpiPs_Send(spiPtr, *(u16*)spiPtr->sendBufferPtr);
                    }
                    else if(caps->frameSize == 32)
                    {
                        FSpiPs_Send(spiPtr, *(u32*)spiPtr->sendBufferPtr);
                    }
                    spiPtr->sendBufferPtr += caps->frameSize >> 3;
                    spiPtr->remainingBytes -= caps->frameSize >> 3;
                    cnt--;
                }           
            }
        }
        
        //disable spi
        while(FSpiPs_GetStatus(spiPtr) & SPIPS_SR_BUSY);
        
        FSpiPs_SetEnable(spiPtr, 0);
        
        //Clear the busy flag
        spiPtr->isBusy = FALSE;
        
    }
    
    if(status != FMSH_SUCCESS)
        return FMSH_FAILURE;
    else
        return FMSH_SUCCESS;
}