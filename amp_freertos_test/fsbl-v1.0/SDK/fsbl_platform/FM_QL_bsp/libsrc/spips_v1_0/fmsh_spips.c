
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
int FSpiPs_CfgInitialize(FSpiPs_T* spi, FSpiPs_Config_T* configPtr)
{
    FMSH_ASSERT(spi != NULL);
    FMSH_ASSERT(configPtr != NULL);
    
    // Set default value
    spi->config = *configPtr;
    
    spi->flag = 0;
    spi->isEnable = FALSE;
    spi->isBusy = FALSE;
    spi->isMaster = FALSE;
    
    spi->totalBytes = 0;
    spi->remainingBytes = 0;
    spi->requestedBytes = 0;
    spi->sendBufferPtr = NULL;
    spi->recvBufferPtr = NULL;
    
    spi->statusHandler = StubStatusHandler;
    
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
void FSpiPs_Reset(FSpiPs_T* spi)
{  
    if(spi->config.deviceId == FPS_SPI0_DEVICE_ID)
    {
        FSlcrPs_ipSetRst(SLCR_SPI0_CTRL, SPI0_APB_RST);
        FSlcrPs_ipSetRst(SLCR_SPI0_CTRL, SPI0_REF_RST);
        FSlcrPs_ipReleaseRst(SLCR_SPI0_CTRL, SPI0_APB_RST);
        FSlcrPs_ipReleaseRst(SLCR_SPI0_CTRL, SPI0_REF_RST);
    }
    else if(spi->config.deviceId == FPS_SPI1_DEVICE_ID)
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
int FSpiPs_SelfTest(FSpiPs_T* spi)
{   
    u32 value;
    FSpiPs_SetTxEmptyLvl(spi, 0xa);
    value = FSpiPs_GetTxLevel(spi);
    if(value != 0xa)
        return FMSH_FAILURE;
    FSpiPs_SetTxEmptyLvl(spi, 0x0);
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
void FSpiPs_SetStatusHandler(FSpiPs_T* spi, void* callBackRef,
                             FSpiPs_StatusHandler funcPtr)
{
    FMSH_ASSERT(spi != NULL);
    FMSH_ASSERT(funcPtr != NULL);
    
    spi->statusHandler = funcPtr;
    spi->statusRef = callBackRef;
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
    FSpiPs_T* spi = (FSpiPs_T*)instancePtr;
    u32 intrStatus;
    u32 byteCount;
    
    FMSH_ASSERT(instancePtr != NULL);
    
    if(spi->isBusy == FALSE)
    {
        return;
    }
    
    /* Get & Clear interrupt status */
    intrStatus =  FMSH_ReadReg(spi->config.baseAddress, SPIPS_ISR_OFFSET);
    FMSH_ReadReg(spi->config.baseAddress, SPIPS_ICR_OFFSET);
    
    /* Multi-Master Fault */
    if (intrStatus & SPIPS_INTR_MSTIS_MASK)
    {
        FSpiPs_Disable(spi);
        byteCount = spi->totalBytes - spi->remainingBytes;
        spi->isBusy = FALSE;
        spi->statusHandler(spi->statusRef, 
                           SPIPS_INTR_MSTIS_MASK, 
                           byteCount);
    }
    
    /* Check for overflow and underflow errors */
    if (intrStatus & SPIPS_INTR_RXOIS_MASK) 
    {
        FSpiPs_Disable(spi);
        byteCount = spi->totalBytes - spi->requestedBytes;
        spi->isBusy = FALSE;
        spi->statusHandler(spi->statusRef,
                           SPIPS_INTR_RXOIS_MASK, 
                           byteCount);
    }
    
    if (intrStatus & SPIPS_INTR_RXUIS_MASK) 
    {
        FSpiPs_Disable(spi);
        byteCount = spi->totalBytes - spi->requestedBytes;
        spi->isBusy = FALSE;
        spi->statusHandler(spi->statusRef,
                           SPIPS_INTR_RXUIS_MASK, 
                           byteCount);
    }
    
    if (intrStatus & SPIPS_INTR_TXOIS_MASK) 
    {
        FSpiPs_Disable(spi);
        byteCount = spi->totalBytes - spi->remainingBytes;
        spi->isBusy = FALSE;
        spi->statusHandler(spi->statusRef,
                           SPIPS_INTR_TXOIS_MASK, 
                           byteCount);
    }
    
    /* Tx Empty */
    if (intrStatus & SPIPS_INTR_TXEIS_MASK) 
    {
        byteCount = spi->totalBytes - spi->remainingBytes;
        spi->statusHandler(spi->statusRef,
                           SPIPS_INTR_TXEIS_MASK,
                           byteCount);    
        /* Transfer complete */
        if((spi->remainingBytes == 0) && (spi->requestedBytes == 0))
        {
            byteCount = spi->totalBytes;
            spi->isBusy = FALSE;
            spi->statusHandler(spi->statusRef,
                               SPIPS_TRANSFER_DONE,
                               byteCount);
        }
    }
    
    /* Rx FIFO Full */
    if (intrStatus & SPIPS_INTR_RXFIS_MASK)
    {
        byteCount = spi->totalBytes - spi->requestedBytes;
        spi->statusHandler(spi->statusRef,
                           SPIPS_INTR_RXFIS_MASK,
                           byteCount);
        /* transfer complete */
        if((spi->remainingBytes == 0) && (spi->requestedBytes == 0))
        {
            byteCount = spi->totalBytes;
            spi->isBusy = FALSE;
            spi->statusHandler(spi->statusRef,
                               SPIPS_TRANSFER_DONE,
                               byteCount);
        }
    }
    
}