/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_dma.c
*
* This file contains 
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   lq  11/23/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdlib.h>
#include <string.h>
#include "boot_main.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern FQspiPs_T* QspiInstancePtr;

FDmaPs_T g_DMA_dmac;
FDmaPs_Param_T g_DMA_param;
FDmaPs_Instance_T g_DMA_instance;
FMSH_LIST_HEAD(g_DMA_list);

volatile BOOL s_DMA_tfrFlag;
static volatile BOOL s_DMA_errFlag;
static volatile BOOL s_DMA_srcTranFlag;
static volatile BOOL s_DMA_dstTranFlag;

/************************** Function Prototypes ******************************/

/*********************************************************************/
void FDmaPs_IRQ (void *InstancePtr)
{
	FDmaPs_irqHandler((FDmaPs_T *)InstancePtr);
}

void userCallback(void *pDev, int eCode)
{
    // -------
    // This function is called by the IRQ handler.
    // Here we are going to set a global flag to inform the
    // application that the DMA transfer has completed.
    // -------
  s_DMA_tfrFlag = TRUE;
}

void userListener(void *pDev, int eCode)
{
    // Check the source of the interrupt
    if (eCode == Dmac_irq_err)
	{
        // Just inform the application that this interrupt has occured.
        s_DMA_errFlag = TRUE;
    }

    if (eCode == Dmac_irq_srctran)
    {
        // Just inform the application that this interrupt has occured.
        s_DMA_srcTranFlag = TRUE;
    }

    if (eCode == Dmac_irq_dsttran)
    {
        // Just inform the application that this interrupt has occured.
        s_DMA_dstTranFlag = TRUE;
    }

    if (eCode == Dmac_irq_block)
    {
        // We fully expect this interrupt to fire each time the DMA
        // controller has completed the transfer of a block of data.
        // For this example we are not concerned that a block has
        // completed, only that the transfer completes. So we do
        // nothing here.
        
    }
}

void FDmaPs_setCHxParam(FDmaPs_ChannelConfig_T *ch_config)
{
    // Set the Source and destination addresses
    ch_config->sar = g_DMA_srcAddress;
    ch_config->dar = g_DMA_dstAddress;
    // Set the source and destination transfer width
    ch_config->ctl_src_tr_width = g_DMA_ctlSrcTrWidth;
    ch_config->ctl_dst_tr_width = g_DMA_ctlDstTrWidth;
    // Set the Address increment type for the source and destination
    ch_config->ctl_sinc = g_DMA_ctlSinc;
    ch_config->ctl_dinc = g_DMA_ctlDinc;
    // Set the source and destination burst transaction length
    ch_config->ctl_src_msize = g_DMA_ctlSrcMsize;
    ch_config->ctl_dst_msize = g_DMA_ctlDstMsize;
	// set scatter/gather enable and parameters
	ch_config->ctl_dst_scatter_en = g_DMA_ctlDstScatterEn;
	ch_config->ctl_src_gather_en = g_DMA_ctlSrcGatherEn;
	if (ch_config->ctl_dst_scatter_en == FMSH_set)
	{
		ch_config->dsr_dsc = g_DMA_dsrDsc;
		ch_config->dsr_dsi = g_DMA_dsrDsi;
	}
	if (ch_config->ctl_src_gather_en == FMSH_set)
	{
		ch_config->sgr_sgc = g_DMA_sgrSgc;
		ch_config->sgr_sgi = g_DMA_sgrSgi;
	}
	
    // Set the block size for the DMA transfer
    // Block size is the number of words of size Dmac_trans_width
    ch_config->ctl_block_ts = g_DMA_blockSize;
    // Set the transfer device type and flow controller
    ch_config->ctl_tt_fc = g_DMA_ctlTtFc;
	// Set the hardware handshaking interface
	ch_config->cfg_dst_per = g_DMA_cfgDestPer;
	ch_config->cfg_src_per = g_DMA_cfgSrcPer;
	// Set the FIFO mode
	ch_config->cfg_fifo_mode = g_DMA_cfgFifoMode;
	// Set the handshaking interface polarity
	ch_config->cfg_src_hs_pol = g_DMA_cfgSrcHsPol;
	ch_config->cfg_dst_hs_pol = g_DMA_cfgDstHsPol;
	// Set the handshaking select
	ch_config->cfg_hs_sel_src = g_DMA_cfgHsSelSrc;
	ch_config->cfg_hs_sel_dst = g_DMA_cfgHsSelDst;
}

int FDmaPs_setLliEntry(FDmaPs_T *pDmac, int num_blocks,
                             enum FDmaPs_transferType transType,
                             FMSH_listHead *listHead,
                             FDmaPs_ChannelConfig_T *pChConfig)
{
	u32 i;
	u32 sar, dar;
    enum FMSH_state ctl_llp_src_en;
    enum FMSH_state ctl_llp_dst_en;
	FDmaPs_LliItem_T *lliItem;
	
	// backup before LLI setting
	sar = pChConfig->sar;
	dar = pChConfig->dar;
	ctl_llp_src_en = pChConfig->ctl_llp_src_en;
	ctl_llp_dst_en = pChConfig->ctl_llp_dst_en;

	for (i = 0; i < num_blocks; i++)
	{
		// create new node
		lliItem = (FDmaPs_LliItem_T *) malloc (sizeof(FDmaPs_LliItem_T));
		//lliItem = (FDmaPs_LliItem *) (LLI_MEMORY_BASE + i * LLI_MEMORY_OFFSET);
		if(lliItem == NULL)
	    {
	        TRACE_OUT(DEBUG_OUT, "\nCann't create node[%d]!\r\n", i);
	        return FMSH_EINVAL;
	    }
		memset(lliItem, 0, sizeof(FDmaPs_LliItem_T));
		FMSH_INIT_LIST_HEAD(&lliItem->list);

		if (i == num_blocks - 1)
		{
			pChConfig->ctl_llp_dst_en = FMSH_clear;
			pChConfig->ctl_llp_src_en = FMSH_clear;
		}
		
		FDmaPs_addLliItem(listHead, lliItem, pChConfig);

		if (transType == Dmac_transfer_row7)
			pChConfig->sar = pChConfig->sar;
		else
			pChConfig->sar = pChConfig->sar + pChConfig->ctl_block_ts * 4;

		if (transType == Dmac_transfer_row9)
			pChConfig->dar = pChConfig->dar;
		else
			pChConfig->dar = pChConfig->dar + pChConfig->ctl_block_ts * 4;
	}

	// recover after LLI setting
	pChConfig->ctl_llp_src_en = ctl_llp_src_en;
	pChConfig->ctl_llp_dst_en = ctl_llp_dst_en;
	pChConfig->sar = sar;
	pChConfig->dar = dar;

#if DEBUG_LLI_OUT
	FMSH_listHead *pos;
	// for checking lli has setted
	FMSH_LIST_FOR_EACH(pos, listHead) {
		lliItem = FMSH_LIST_ENTRY(pos, FDmaPs_LliItem_T, list);
		TRACE_OUT(1, "sar: 0x%08x; dar: 0x%08x; llp: 0x%08x; ctl_l: 0x%08x; ctl_h: 0x%08x; sstat: 0x%08x; dstat: 0x%08x\r\n",
			   lliItem->sar, lliItem->dar, lliItem->llp, lliItem->ctl_l, lliItem->ctl_h, lliItem->sstat, lliItem->dstat);
	}
#endif

	// get the first LLI address by head node
	lliItem = FMSH_LIST_ENTRY(listHead, FDmaPs_LliItem_T, list);
	pChConfig->llp_loc = lliItem->llp >> 2;
#if DEBUG_LLI_OUT
    TRACE_OUT(1, "sar: 0x%08x; dar: 0x%08x; llp: 0x%08x; ctl_l: 0x%08x; ctl_h: 0x%08x; sstat: 0x%08x; dstat: 0x%08x\r\n",
		   lliItem->sar, lliItem->dar, lliItem->llp, lliItem->ctl_l, lliItem->ctl_h, lliItem->sstat, lliItem->dstat);
#endif

	return 0;
}

void FDmaPs_delLliEntry(FMSH_listHead *head)
{
	FMSH_listHead *pos, *p;
    FDmaPs_LliItem_T *lliItem;
	FDmaPs_LliItem_T *prev_entry;

	pos = head->next;
	while (pos != head)
	{
		p = pos->next;
		FMSH_listDelInit(pos);
		// free the memory
		prev_entry = FMSH_LIST_ENTRY(pos, FDmaPs_LliItem_T, list);
		free(prev_entry);
		pos = p;

		FMSH_LIST_FOR_EACH(p, head) {
		lliItem = FMSH_LIST_ENTRY(p, FDmaPs_LliItem_T, list);
#if DEBUG_LLI_OUT
		TRACE_OUT(0, "sar: 0x%08x; dar: 0x%08x; llp: 0x%08x; ctl_l: 0x%08x; ctl_h: 0x%08x; sstat: 0x%08x; dstat: 0x%08x\r\n",
			   lliItem->sar, lliItem->dar, lliItem->llp, lliItem->ctl_l, lliItem->ctl_h, lliItem->sstat, lliItem->dstat);
#endif
		}
	}
    lliItem = FMSH_LIST_ENTRY(head, FDmaPs_LliItem_T, list);
#if DEBUG_LLI_OUT
    TRACE_OUT(0, "sar: 0x%08x; dar: 0x%08x; llp: 0x%08x; ctl_l: 0x%08x; ctl_h: 0x%08x; sstat: 0x%08x; dstat: 0x%08x\r\n",
           lliItem->sar, lliItem->dar, lliItem->llp, lliItem->ctl_l, lliItem->ctl_h, lliItem->sstat, lliItem->dstat);
#endif

    memset(lliItem, 0, sizeof(FDmaPs_LliItem_T));
    lliItem = FMSH_LIST_ENTRY(head, FDmaPs_LliItem_T, list);
#if DEBUG_LLI_OUT
    TRACE_OUT(0, "sar: 0x%08x; dar: 0x%08x; llp: 0x%08x; ctl_l: 0x%08x; ctl_h: 0x%08x; sstat: 0x%08x; dstat: 0x%08x\r\n",
           lliItem->sar, lliItem->dar, lliItem->llp, lliItem->ctl_l, lliItem->ctl_h, lliItem->sstat, lliItem->dstat);
#endif

	FMSH_INIT_LIST_HEAD(head);
}
int FDmaPs_srcDstDataCompare(u32 sar, u32 dar, u32 block_ts, u32 block_num, enum FDmaPs_transferType transfer_type)
{
    s32 testStatus = FMSH_SUCCESS;
    int i, j, k;
    u32 dst_data, exp_data, *dst_addr, *src_addr;
	
    src_addr = (u32 *)sar;
    dst_addr = (u32 *)dar;
	if ((transfer_type == Dmac_transfer_row1) ||
		(transfer_type == Dmac_transfer_row5) ||
	    (transfer_type == Dmac_transfer_row6) ||
	    (transfer_type == Dmac_transfer_row8) ||
	    (transfer_type == Dmac_transfer_row10))
	{
        for (j = 0; j < block_ts; j++)
		{
            exp_data = *(u32 *) src_addr++;
            dst_data = *(volatile u32 *) dst_addr++;
            if (exp_data != dst_data)
			{
                testStatus = FMSH_FAILURE;
            	TRACE_OUT(DEBUG_OUT, "CH = %d, dst_addr = %x : dst_data = %x : exp_data = %x\r\n",
					      i, dst_addr-1, dst_data, exp_data);
				break;
            }
        }
	}
	else if ((transfer_type == Dmac_transfer_row2) ||
		     (transfer_type == Dmac_transfer_row9))
	{
		for (k = 0; k < block_num; k++)
		{
			// Row2 has auto-reload addressing on the destination
			if (k < block_num - 1)
			{
		        for (j = 0; j < block_ts; j++)
				{
		            (u32 *) src_addr++;
		        }
    		}
			else
			{
				dst_addr = (u32 *)dar;
		        for (j = 0; j < block_ts; j++)
				{
		            exp_data = *(u32 *) src_addr++;
		            dst_data = *(volatile u32 *) dst_addr++;
		            if (exp_data != dst_data)
					{
		                testStatus = FMSH_FAILURE;
		            	TRACE_OUT(DEBUG_OUT, "CH = %d, blkNum = %d, dst_addr = %x : dst_data = %x : exp_data = %x\r\n",
							      i, k, dst_addr-1, dst_data, exp_data);
						break;
		            }
		        }
			}
	    }
	}
	else if ((transfer_type == Dmac_transfer_row3) ||
		     (transfer_type == Dmac_transfer_row7))
	{
		for (k = 0; k < block_num; k++)
		{
			// Row3 has auto-reload addressing on the source
        	src_addr = (u32 *)sar;
	        for (j = 0; j < block_ts; j++)
			{
	            exp_data = *(u32 *) src_addr++;
	            dst_data = *(volatile u32 *) dst_addr++;
	            if (exp_data != dst_data)
				{
	                testStatus = FMSH_FAILURE;
	            	TRACE_OUT(DEBUG_OUT, "CH = %d, blkNum = %d, dst_addr = %x : dst_data = %x : exp_data = %x\r\n",
						      i, k, dst_addr-1, dst_data, exp_data);
					break;
	            }
	        }
	    }
	}
    else if (transfer_type == Dmac_transfer_row4)
	{
		// Row4 has auto-reload addressing on the source and destination
		// all the blocks has the same datas.
        for (j = 0; j < block_ts; j++)
		{
            exp_data = *(u32 *) src_addr++;
            dst_data = *(volatile u32 *) dst_addr++;
            if (exp_data != dst_data)
			{
                testStatus = FMSH_FAILURE;
            	TRACE_OUT(DEBUG_OUT, "CH = %d, blkNum = %d, dst_addr = %x : dst_data = %x : exp_data = %x\r\n",
					      i, k, dst_addr-1, dst_data, exp_data);
				break;
            }
        }
	}

	return testStatus;
}
s32 FDmaPs_dmacConfigure(FDmaPs_T *pDmac)
{
    s32 testStatus = FMSH_SUCCESS;
    int chIndex, errorCode;
    u32 blockSize, numBlocks;
    enum FDmaPs_channelNumber chNum;
    enum FDmaPs_transferType transType;
    FDmaPs_ChannelConfig_T ch_config;

    // Initialize the DMA controller
    FDmaPs_init(pDmac);

    // Transfer characteristics
    chNum = FDmaPs_getFreeChannel(pDmac); // Select a DMA channel
    transType = g_DMA_transType; // Select a transfer type
    blockSize = g_DMA_blockSize; // Select the size of a block
    numBlocks = g_DMA_numBlocks;  // Select the number of blocks

    // Get the channel index from the enumerated type
    chIndex = FDmaPs_getChannelIndex(chNum);

    // START : Channel configuration
    // -------
    errorCode = FDmaPs_setTransferType(pDmac, chNum, transType);
    if (errorCode != 0)
	{
        TRACE_OUT(DEBUG_OUT, "ERROR: Failed to set the transfer type row %d\r\n", transType);
    }
    
    // Enable the interrupts on Channel x
    errorCode = FDmaPs_enableChannelIrq(pDmac, chNum);
    if (errorCode != 0)
	{
        TRACE_OUT(DEBUG_OUT, "ERROR: Failed to enable channel %d interrupts\r\n", chIndex);
    }

    // Initialise the channel configuration structure.
    errorCode = FDmaPs_getChannelConfig(pDmac, chNum, &ch_config);
    if (errorCode != 0)
	{
        TRACE_OUT(DEBUG_OUT, "ERROR: Failed to initialise configuration ");
        TRACE_OUT(DEBUG_OUT, "structure from the DMA registers on channel %d\r\n", chIndex);
    }

    // Change the configuration structure members to initialise the
    // DMA channel for the chosen transfer.
	FDmaPs_setCHxParam(&ch_config);
	
	// set LLI items for multi blocks
	if ((numBlocks > 1) && (transType >= Dmac_transfer_row6))
	{
		errorCode = FDmaPs_setLliEntry(pDmac, numBlocks, transType, &g_DMA_list, &ch_config);
		if (errorCode != 0)
		{
			TRACE_OUT(DEBUG_OUT, "ERROR: Failed to set LLI item\r\n");
		}
	}

    // Write the new configuration setting into the DMA Controller device.
    errorCode = FDmaPs_setChannelConfig(pDmac, chNum, &ch_config);
    if (errorCode != 0)
	{
        TRACE_OUT(DEBUG_OUT, "ERROR: Failed to write configuration structure ");
        TRACE_OUT(DEBUG_OUT, "into the DMA controller registers on channel %d\r\n", chIndex);
    }
    // -------
    // END : Channel configuration

    // display the transfer information.
    TRACE_OUT(DEBUG_OUT, "/ -----\r\n");
    TRACE_OUT(DEBUG_OUT, "  Channel number       %d\r\n", chIndex);
    TRACE_OUT(DEBUG_OUT, "  Transfer type        Row %d\r\n", transType);
    TRACE_OUT(DEBUG_OUT, "  Block Size           %u\r\n", blockSize);
    TRACE_OUT(DEBUG_OUT, "  Number of blocks     %u\r\n", numBlocks);
    TRACE_OUT(DEBUG_OUT, "  Source address       %x\r\n", ch_config.sar);
    TRACE_OUT(DEBUG_OUT, "  Destination address  %x\r\n", ch_config.dar);
    TRACE_OUT(DEBUG_OUT, "/ -----\r\n");
	
	// Before beginning an interrupt driven transfer the user must
	// register a listener function in the DMA driver. 
	// -------
	FDmaPs_setListener(pDmac, chNum, userListener);
	
	// Initialise global flags to FALSE
	s_DMA_tfrFlag = FALSE;
	s_DMA_errFlag = FALSE;
	s_DMA_srcTranFlag = FALSE;
	s_DMA_dstTranFlag = FALSE;

    // Enable the DMA controller and begin the interrupt driven DMA transfer.
    FDmaPs_enable(pDmac);
    errorCode = FDmaPs_startTransfer(pDmac, chNum, numBlocks, userCallback);
    if (errorCode != 0)
    {
        TRACE_OUT(DEBUG_OUT, "ERROR: Failed to begin the interrupt transfer.\r\n");
		testStatus = FMSH_FAILURE;
    }

    return testStatus;
}

u8 FmshFsbl_DmaInit(void)
{
      int Status=FMSH_SUCCESS;
      FDmaPs_Config *pDmaCfg;
      /* Initialize the DMA Driver */
      pDmaCfg = FDmaPs_LookupConfig(FPAR_DMAPS_DEVICE_ID);
      if (pDmaCfg == NULL)
      {
          return FMSH_FAILURE;
      }
      FDmaPs_initDev(&g_DMA_dmac, &g_DMA_instance, &g_DMA_param, pDmaCfg);
      
      Status = FGicPs_registerInt(&IntcInstance, DMA_INT_ID,
              (FMSH_InterruptHandler)FDmaPs_IRQ, &g_DMA_dmac);
      if (Status != FMSH_SUCCESS)
      {
        return FMSH_FAILURE;
      }
      Status = FDmaPs_autoCompParams(&g_DMA_dmac);
      if (Status != FMSH_SUCCESS)
      {
        return FMSH_FAILURE;
      }
      return Status;
}

