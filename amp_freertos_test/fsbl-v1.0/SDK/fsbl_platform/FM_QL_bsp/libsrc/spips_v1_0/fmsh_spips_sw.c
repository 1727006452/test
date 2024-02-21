
#include "fmsh_spips_lib.h" 

/*****************************************************************************
* This function initializes controller
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
int FSpiPs_Initialize(FSpiPs_T* spi, u16 deviceId)
{
    FSpiPs_Config_T* cfgPtr;
    
    FMSH_ASSERT(spi != NULL);
    
    cfgPtr = FSpiPs_LookupConfig(deviceId);
    if (cfgPtr == NULL) 
    {
        return FMSH_FAILURE;
    }
    return FSpiPs_CfgInitialize(spi, cfgPtr);  
}

int FSpiPs_Initialize_Master(FSpiPs_T* spi)
{
    int err = 0;
    
    FMSH_ASSERT(spi != NULL);
    
    // Check whether there is another transfer in progress. Not thread-safe
    if(spi->isBusy == TRUE) 
    {
        return FMSH_FAILURE;
    }
    
    // Disable device
    FSpiPs_Disable(spi);
    // Select device as Master
    FSpiPs_Mst(spi);
    // CTRL (TMode, CkMode, BaudRate, DFSize, DFNum, isLoopBack)
    err |= FSpiPs_SetTMod(spi, SPIPS_TRANSFER_STATE);
    err |= FSpiPs_SetSckMode(spi, 3);
    err |= FSpiPs_SetSckDv(spi, 100);
    err |= FSpiPs_SetDFS32(spi, 8); 
    err |= FSpiPs_SetLoopBack(spi, FALSE);
    err |= FSpiPs_SetDFNum(spi, 128); 
    
    // Config Tx/Rx Threshold
    err |= FSpiPs_SetTxEmptyLvl(spi, 20);
    err |= FSpiPs_SetRxFullLvl(spi, 12);
    FSpiPs_SetDMATLvl(spi, 12);
    FSpiPs_SetDMARLvl(spi, 20);
    
    // Config IMR
    FSpiPs_DisableIntr(spi, SPIPS_INTR_ALL);
    
    // SlaveSelect
    err |= FSpiPs_SetSlave(spi, 1);
    
    if(err)
    {
        return FMSH_FAILURE;
    }
    // Enable device
    FSpiPs_Enable(spi);
    
    return FMSH_SUCCESS;
}

int FSpiPs_Initialize_Slave(FSpiPs_T* spi)
{
    int err = 0;
    
    FMSH_ASSERT(spi != NULL);
    
    // Check whether there is another transfer in progress. Not thread-safe
    if(spi->isBusy == TRUE) 
    {
        return FMSH_FAILURE;
    }
    
    // Disable device
    FSpiPs_Disable(spi);  
    // Select device as Slave
    FSpiPs_Slv(spi);
    // Config CTRLR0 (TMode, CkMode, DFSize, IsSlaveOut, isLoopBack)
    err |= FSpiPs_SetTMod(spi, SPIPS_TRANSFER_STATE);
    err |= FSpiPs_SetSckMode(spi, 3);
    err |= FSpiPs_SetDFS32(spi,8); 
    err |= FSpiPs_SetLoopBack(spi, FALSE);
    err |= FSpiPs_SetSlvOut(spi, TRUE);
    
    // Config Tx/Rx Threshold
    err |= FSpiPs_SetTxEmptyLvl(spi, 20);
    err |= FSpiPs_SetRxFullLvl(spi, 12);
    FSpiPs_SetDMATLvl(spi, 12);
    FSpiPs_SetDMARLvl(spi, 0);
    
    // Config IMR
    FSpiPs_DisableIntr(spi, SPIPS_INTR_ALL);
        
    if(err)
    {
        return FMSH_FAILURE;
    }
    
    // Enable device
    FSpiPs_Enable(spi);
    
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
int FSpiPs_Transfer(FSpiPs_T* spi, u8* sendBuffer, u8* recvBuffer, u32 byteCount)
{
    u8 tmod;
    
    FMSH_ASSERT(spi != NULL);
    FMSH_ASSERT(sendBuffer != NULL);
    
    /* Check whether there is another transfer in progress. Not thread-safe */
    if(spi->isBusy == TRUE)
    {
        return FMSH_FAILURE;  
    }
    /* Set the busy flag, cleared when transfer is done */
    spi->isBusy = TRUE;
    spi->sendBufferPtr = sendBuffer;
    spi->recvBufferPtr = recvBuffer;
    
    FSpiPs_DisableIntr(spi, SPIPS_INTR_ALL);
    FSpiPs_Enable(spi);
    
    /* Get transfer mode */
    tmod = (FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET) & SPIPS_CTRL0_TMOD_MASK)
        >> SPIPS_CTRL0_TMOD_SHIFT;
    
    if(tmod == SPIPS_TRANSFER_STATE)
    {
        spi->totalBytes = byteCount;
        spi->remainingBytes = byteCount;   
        spi->requestedBytes = byteCount;   
        FSpiPs_EnableIntr(spi, SPIPS_INTR_ALL);
    }
    else if(tmod == SPIPS_TRANSMIT_ONLY_STATE)
    {
        spi->totalBytes = byteCount;
        spi->requestedBytes = 0;
        spi->remainingBytes = byteCount;
        FSpiPs_EnableIntr(spi, SPIPS_INTR_ALL & ~SPIPS_INTR_RXFIS_MASK);
    }
    else if(tmod == SPIPS_RECEIVE_ONLY_STATE)
    {
        spi->totalBytes = byteCount;
        spi->requestedBytes = byteCount;
        spi->remainingBytes = 0;
        /* Write one dummy data word to Tx FIFO */
        FSpiPs_Send(spi, 0xff);
        FSpiPs_EnableIntr(spi, SPIPS_INTR_ALL & ~SPIPS_INTR_TXEIS_MASK);
    }
    else if(tmod == SPIPS_EEPROM_STATE)
    {         
        spi->totalBytes = byteCount;
        spi->requestedBytes = byteCount;
        spi->remainingBytes = byteCount;
        FSpiPs_EnableIntr(spi, SPIPS_INTR_ALL);
    }
    
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
int FSpiPs_PolledTransfer(FSpiPs_T* spi, u8* sendBuffer, u8* recvBuffer, u32 byteCount)
{
    u8 tmod;
    
    FMSH_ASSERT(spi != NULL);
    
    // Check whether there is another transfer in progress. Not thread-safe 
    if(spi->isBusy == TRUE)
    {
        return FMSH_FAILURE;  
    }
    
    // Get transfer mode
    tmod = (FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET) & SPIPS_CTRL0_TMOD_MASK)
        >> SPIPS_CTRL0_TMOD_SHIFT;
    
    if(tmod == SPIPS_TRANSFER_STATE)
    {
        if(sendBuffer == NULL || recvBuffer == NULL)
        {
            return FMSH_FAILURE;  
        }
        spi->totalBytes = byteCount;
        spi->remainingBytes = byteCount;   
        spi->requestedBytes = byteCount;   
        spi->sendBufferPtr = sendBuffer;
        spi->recvBufferPtr = recvBuffer;
    }
    else if(tmod == SPIPS_TRANSMIT_ONLY_STATE)
    {
        if(sendBuffer == NULL)
        {
            return FMSH_FAILURE;  
        }
        spi->totalBytes = byteCount;
        spi->requestedBytes = 0;
        spi->remainingBytes = byteCount;
    }
    else if(tmod == SPIPS_RECEIVE_ONLY_STATE)
    {
        if(recvBuffer == NULL)
        {
            return FMSH_FAILURE;  
        }
        spi->totalBytes = byteCount;
        spi->requestedBytes = byteCount;
        spi->remainingBytes = 0;
        /* Write one dummy data word to Tx FIFO */
        FSpiPs_Send(spi, 0xff);
    }
    else if(tmod == SPIPS_EEPROM_STATE)
    {
        if(sendBuffer == NULL || recvBuffer == NULL)
        {
            return FMSH_FAILURE;  
        }
        spi->totalBytes = byteCount;
        spi->requestedBytes = 0;
        spi->remainingBytes = byteCount;
    }
    
    // Set the busy flag, cleared when transfer is done 
    spi->isBusy = TRUE;
    
    //disable interrupt
    FSpiPs_DisableIntr(spi, SPIPS_INTR_ALL);
    
    //enable spi
    FSpiPs_Enable(spi);
    
    //polling tx fifo level until transfer complete
    int cnt;
    u32 txLvl;
    u32 txEmptyLvl = 10;
    while(spi->remainingBytes !=0 || spi->requestedBytes != 0) 
    {
        txLvl = FMSH_ReadReg(spi->config.baseAddress, SPIPS_TXFLR_OFFSET);
        if(txLvl <= txEmptyLvl)
        {
            cnt = FMSH_ReadReg(spi->config.baseAddress, SPIPS_RXFLR_OFFSET);
            while(cnt > 0 && spi->requestedBytes != 0)
            {
                if(spi->frameSize == 8)
                {
                    *(u8*)(spi->recvBufferPtr) = (u8)FSpiPs_Recv(spi);
                }
                else if(spi->frameSize == 16)
                {
                    *(u16*)(spi->recvBufferPtr) = (u16)FSpiPs_Recv(spi);
                }
                else if(spi->frameSize == 32)
                {
                    *(u32*)(spi->recvBufferPtr) = (u32)FSpiPs_Recv(spi);
                }
                
                spi->recvBufferPtr += spi->frameSize >> 3;
                spi->requestedBytes -= spi->frameSize >> 3;
                cnt--;
            }
            cnt = SPIPS_FIFO_DEPTH - txEmptyLvl;
            while(cnt > 0 && spi->remainingBytes != 0)
            {
                if(spi->frameSize == 8)
                {
                    FSpiPs_Send(spi, *(u8*)spi->sendBufferPtr);
                }
                else if(spi->frameSize == 16)
                {
                    FSpiPs_Send(spi, *(u16*)spi->sendBufferPtr);
                }
                else if(spi->frameSize == 32)
                {
                    FSpiPs_Send(spi, *(u32*)spi->sendBufferPtr);
                }
                spi->sendBufferPtr += spi->frameSize >> 3;
                spi->remainingBytes -= spi->frameSize >> 3;
                cnt--;
            }           
        }
    }
    
    //Clear the busy flag
    spi->isBusy = FALSE;
    
    //disable spi
    FSpiPs_Disable(spi);

    return FMSH_SUCCESS;  
}
