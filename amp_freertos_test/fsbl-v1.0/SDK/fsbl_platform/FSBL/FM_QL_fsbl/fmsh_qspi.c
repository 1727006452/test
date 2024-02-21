/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_qspi.c
*
* This file contains fmsh_qspi.h.
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   xxx  11/23/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include "fmsh_gic.h"
#include "fmsh_qspi.h"
#include "fmsh_dmac_config.h"
#include "fmsh_dma.h"

#if DEVC_QSPI_DMA_ENABLE
#include "boot_main.h"
#endif

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
FQspiPs_T QspiInstance;

FQspiPs_T* QspiInstancePtr;

#define  QSPIPS_CLOCK_DELAY_MASK         (0xf << 1)
#define  SIZE_1MB 1048576
#define   QSPI_16MB_SIZE 0x1000000

volatile BOOL g_QSPI_DMA_Flag=FALSE;
volatile u8 g_QSPI_High_Addr_Flag=0;

/************************** Function Prototypes ******************************/
static u32 intrStatus;
void FQspiPs_Handler(void *callBackRef, u32 statusEvent, u32 byteCount)
{
    if(statusEvent == QSPIPS_INTR_RX_NOT_EMPTY_MASK | 
                        QSPIPS_INTR_RX_FULL_MASK)
    {

    }
    if(statusEvent == QSPIPS_INTR_TX_NOT_FULL_MASK)
    {
        
    }
    if(statusEvent == QSPIPS_INTR_TX_FULL_MASK)
    {
       
    }    
    if(statusEvent == QSPIPS_INTR_RX_OVERFLOW_MASK)
    {
       
    }        
    if(statusEvent == QSPIPS_INTR_REQ_REJECT_MASK)
    {
        
    }
    if(statusEvent == QSPIPS_INTR_WRITE_PROTECT_MASK)
    {
        
    } 
    if(statusEvent == QSPIPS_INTR_ILLEGAL_AHB_ACCESS_MASK)
    {

    }  
    if(statusEvent == QSPIPS_INTR_POLL_EXPIRED_MASK)
    {
      
    }  
    if(statusEvent == QSPIPS_INTR_WATERMARK_LEVEL_BREACHED_MASK | 
                            QSPIPS_INTR_SRAM_READ_FULL_MASK)
    {
        intrStatus |= QSPIPS_INTR_SRAM_READ_FULL_MASK;
    }     
    if(statusEvent == QSPIPS_INTR_STIG_COMPLETE_MASK)
    {
    }      
    if(statusEvent == QSPIPS_INTR_INDIRECT_COMPLETE_MASK)
    {
        intrStatus |= QSPIPS_INTR_INDIRECT_COMPLETE_MASK;
    }      
}

u32 FQspiPs_AddressMap(FQspiPs_T* qspi, u32 addr)
{
    u32 mappedAddr;
    u8 highAddr;
    
    highAddr = addr >> 24;
    FQspiPs_SetFlashSegment(qspi, highAddr);
    mappedAddr = addr & 0xffffff;  
    
    return mappedAddr;
}

void FQspiPs_SetDumyClk(FQspiPs_T* qspi, uint32_t delayNumber)
{
    uint32_t configReg;
    configReg = FMSH_ReadReg(qspi->config.baseAddress, QSPIPS_RDCR_OFFSET);
    configReg &= ~QSPIPS_CLOCK_DELAY_MASK;
    configReg |= (delayNumber << 1) & QSPIPS_CLOCK_DELAY_MASK;
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_RDCR_OFFSET, configReg);
}

u32 FmshFsbl_InitQspi(void)
{
        int Status;
        QspiInstancePtr = &QspiInstance;

#if XIPMODE_SUPPORT
        FQspiPs_Config_T* cfgPtr;
        cfgPtr = FQspiPs_LookupConfig(FPS_QSPI0_DEVICE_ID);
        if (cfgPtr == NULL) 
            return FMSH_FAILURE;

        Status = FQspiPs_CfgInitialize(QspiInstancePtr, cfgPtr);
#else
        Status = FQspiPs_Initialize(QspiInstancePtr, FPS_QSPI0_DEVICE_ID);
#endif        
        
        if (Status != FMSH_SUCCESS) {
                return FMSH_FAILURE;
        }
#if 0
        u8 dummy_clk=4;
        dummy_clk=(QSPI_FREQ+1000)/50000000;
        if(dummy_clk==0)
          dummy_clk=1;
        FQspiPs_SetCaptureDelay(QspiInstancePtr, dummy_clk);
#endif 
        
#if     DEVC_QSPI_DMA_ENABLE
        FQspiPs_SetStatusHandler(&QspiInstance, &QspiInstance, FQspiPs_Handler);
        FGicPs_Connect(&IntcInstance,46, FQspiPs_InterruptHandler, &QspiInstance); //qspi0:46, qspi1:47
        FGicPs_Enable(&IntcInstance, 46);
#endif

        return Status;
}

int dma_config(u32 addr,u32 len)
{
    int Status = FMSH_SUCCESS;

    // Set the source and destination transfer width
    g_DMA_ctlSrcTrWidth = Dmac_trans_width_32;
    g_DMA_ctlDstTrWidth = Dmac_trans_width_32;
    // Set the block size for the DMA transfer
    // Block size is the number of words of size Dmac_trans_width
    g_DMA_blockSize = len>>2;
    // Set the Address increment type for the source and destination
    g_DMA_ctlSinc = Dmac_addr_increment;
    g_DMA_ctlDinc = Dmac_addr_increment;
    // Set the source and destination burst transaction length
    g_DMA_ctlSrcMsize = Dmac_msize_32;
    g_DMA_ctlDstMsize = Dmac_msize_32;
    // set scatter/gather enable and parameters
    g_DMA_ctlDstScatterEn = FMSH_clear;
    g_DMA_ctlSrcGatherEn = FMSH_clear;
    // Set the hardware handshaking interface
    g_DMA_cfgDestPer = Dmac_hs_if6;
    g_DMA_cfgSrcPer = Dmac_hs_if7;
    // Set the FIFO mode
    g_DMA_cfgFifoMode = Dmac_fifo_mode_half;
    // Set the handshaking interface polarity
    g_DMA_cfgSrcHsPol = Dmac_active_high;
    g_DMA_cfgDstHsPol = Dmac_active_high;
    // Set the handshaking select
    g_DMA_cfgHsSelSrc = Dmac_hs_hardware;
    g_DMA_cfgHsSelDst = Dmac_hs_hardware;
    

    // Set the Source and destination addresses
    g_DMA_srcAddress = FPS_QSPI0_D_BASEADDR;
    g_DMA_dstAddress = addr;
   
    // Set the transfer device type and flow controller
    g_DMA_ctlTtFc = Dmac_prf2mem_dma;
 
    s_DMA_tfrFlag = FALSE;
    
    Status = FDmaPs_dmacConfigure(&g_DMA_dmac);
    if(Status!=FMSH_SUCCESS)
       return Status;
    
    return Status;
}

int FQspiPs_WaitForIndacDone(FQspiPs_T* qspi, int isRead)
{
    u32 timeout = 0;
    u32 byteCount;
    
    while(1)
    {
        if(intrStatus & QSPIPS_INTR_SRAM_READ_FULL_MASK)
        {
            timeout = 0;
            intrStatus &= ~(u32)QSPIPS_INTR_SRAM_READ_FULL_MASK;
            if(isRead)
            { 
                // Read  Watermark 
                byteCount = FMSH_ReadReg(qspi->config.baseAddress, QSPIPS_SFLR_OFFSET)*4;
                // Read data from SRAM 
                memcpy((void*)qspi->recvBufferPtr,
                       (const void*)qspi->config.dataBaseAddress,
                       (size_t)byteCount);
                qspi->recvBufferPtr += byteCount;
                qspi->remainingBytes -= byteCount;  
            }
            else
            {  
                byteCount = QSPIFLASH_PAGE_SIZE;
                if(qspi->remainingBytes < QSPIFLASH_PAGE_SIZE) 
                {
                    byteCount = qspi->remainingBytes;
                }
                memcpy((void*)qspi->config.dataBaseAddress,
                       (const void*)qspi->sendBufferPtr,
                       (size_t)byteCount);
                qspi->sendBufferPtr += byteCount;
                qspi->remainingBytes -= byteCount; 
            }
        }
        
        if(intrStatus & QSPIPS_INTR_INDIRECT_COMPLETE_MASK){ //0x4
            intrStatus &= ~(u32)QSPIPS_INTR_INDIRECT_COMPLETE_MASK;
            if(isRead){
                FQspiPs_WaitForBit(qspi, QSPIPS_IRTCR_OFFSET, 0x20, 1);   
                FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IRTCR_OFFSET, 0x20); 
            }
            else
            {
                FQspiPs_WaitForBit(qspi, QSPIPS_IWTCR_OFFSET, 0x20, 1);
                FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IWTCR_OFFSET, 0x20); 
            }
            return FMSH_SUCCESS;            
        }
        
        delay_1us();
        timeout++;
        if(timeout >= 10000){
            return FMSH_FAILURE;
        }        
    } 
}


int FQspiPs_XIPRecvBytes(FQspiPs_T* qspi, u32 offset, u32 byteCount, u8 *recvBuffer)
{
    u32 address;
    
    /* Read from flash */
    address = qspi->config.dataBaseAddress + offset;
    memcpy((void*)recvBuffer,
           (void*)address,
           (size_t)byteCount); 
    
    qspi->isBusy = FALSE;
    
    return FMSH_SUCCESS;
}



u32 FmshFsbl_QspiAccess( u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
  u32 Status=FMSH_SUCCESS; 
  u32 TmpAddr=0;
  
#if DEVC_QSPI_DMA_ENABLE
  if(DestinationAddress==FSBL_PL_DDRADDR && LengthBytes>SIZE_1MB){
    FQspiPs_EnableIntr(QspiInstancePtr, 0x04);
    //Set DPCR
    u32 NumBurstType = 0x5<<8;
    u32 NumSingleType = 0x0;
    FMSH_WriteReg(QspiInstancePtr->config.baseAddress, QSPIPS_DPCR_OFFSET, NumBurstType | NumSingleType); 
    //Enable DMA, cfg.15
    u32 temp = FMSH_ReadReg(QspiInstancePtr->config.baseAddress, QSPIPS_CFG_OFFSET);
    FMSH_WriteReg(QspiInstancePtr->config.baseAddress, QSPIPS_CFG_OFFSET, temp | (0x1<<15));
    
    Status=FmshFsbl_DmaInit();
    if(Status!=FMSH_SUCCESS)
       return Status;

    g_DMA_transType = Dmac_transfer_row3;//Dmac_transfer_row7; // Select a transfer type
    g_DMA_blockSize = 256; // Select the size of a block (8KB)
    g_DMA_numBlocks = LengthBytes/(g_DMA_blockSize*4)+1;  // Select the number of blocks (total 2MB)
    FQspiPs_FastRecvBytes(QspiInstancePtr, SourceAddress, g_DMA_numBlocks*g_DMA_blockSize*4, (u8*)DestinationAddress); 
    Status=dma_config(DestinationAddress,g_DMA_blockSize*4);
    if(Status!=FMSH_SUCCESS)
       return Status;
    g_QSPI_DMA_Flag=TRUE;
  }  
  else
    Status = FQspiPs_RecvBytes(QspiInstancePtr, SourceAddress, LengthBytes, (u8*)DestinationAddress);
#else
  #if XIPMODE_SUPPORT
    Status = FQspiPs_XIPRecvBytes(QspiInstancePtr, SourceAddress, LengthBytes, (u8*)DestinationAddress);
  #else
    if((SourceAddress+LengthBytes)>QSPI_16MB_SIZE)
    {
        if(SourceAddress>=QSPI_16MB_SIZE)
        {
            if(g_QSPI_High_Addr_Flag==0)
            {
              TmpAddr=FQspiPs_AddressMap(QspiInstancePtr,SourceAddress);
              g_QSPI_High_Addr_Flag=1;
            }
            else
            {
              TmpAddr=SourceAddress&(QSPI_16MB_SIZE-1);
            }
            Status = FQspiPs_RecvBytes(QspiInstancePtr, TmpAddr, LengthBytes, (u8*)DestinationAddress);
        }
        else
        {
            if(g_QSPI_High_Addr_Flag==1)
            {
              TmpAddr=FQspiPs_AddressMap(QspiInstancePtr,SourceAddress);
              g_QSPI_High_Addr_Flag=0;
            }
            else
            {
              TmpAddr=SourceAddress&(QSPI_16MB_SIZE-1);
            }
            Status = FQspiPs_RecvBytes(QspiInstancePtr, TmpAddr, QSPI_16MB_SIZE-SourceAddress, (u8*)DestinationAddress);
            
            TmpAddr = FQspiPs_AddressMap(QspiInstancePtr,QSPI_16MB_SIZE);
            g_QSPI_High_Addr_Flag=1;
            
            Status = FQspiPs_RecvBytes(QspiInstancePtr, TmpAddr, LengthBytes-(QSPI_16MB_SIZE-SourceAddress), (u8*)DestinationAddress+(QSPI_16MB_SIZE-SourceAddress));

        }
    }
    else
    {
      if(g_QSPI_High_Addr_Flag==1){
          FQspiPs_AddressMap(QspiInstancePtr,SourceAddress);
          g_QSPI_High_Addr_Flag=0;
      }
       Status = FQspiPs_RecvBytes(QspiInstancePtr, SourceAddress, LengthBytes, (u8*)DestinationAddress);
    }
  #endif
#endif
  
  return Status;
}