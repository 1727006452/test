/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_dmac_config.c
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
#include "fmsh_dmac_config.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
enum FMSH_compType g_DMA_compType = FMSH_ahb_dmac; // dma component type
u32 g_DMA_compVersion = 0x3230322A; // dma component version
enum FDmaPs_channelNumber g_DMA_chNum = Dmac_channel0; // Select a DMA channel
enum FDmaPs_transferType g_DMA_transType = Dmac_transfer_row1; // Select a transfer type
// Set src/dst address
u32 g_DMA_srcAddress;
u32 g_DMA_dstAddress;
u32 g_DMA_blockSize = DMAC_TEST_BLOCK_SIZE;//17; // Select the size of a block
u32 g_DMA_numBlocks = 1;  // Select the number of blocks
// Set the source and destination transfer width
enum FDmaPs_transferWidth g_DMA_ctlSrcTrWidth = Dmac_trans_width_32;
enum FDmaPs_transferWidth g_DMA_ctlDstTrWidth = Dmac_trans_width_32;
// Set the Address increment type for the source and destination
enum FDmaPs_addressIncrement g_DMA_ctlSinc = Dmac_addr_increment;
enum FDmaPs_addressIncrement g_DMA_ctlDinc = Dmac_addr_increment;
// Set the source and destination burst transaction length
enum FDmaPs_burstTransLength g_DMA_ctlSrcMsize = Dmac_msize_32;
enum FDmaPs_burstTransLength g_DMA_ctlDstMsize = Dmac_msize_32;
// set scatter and gather enable
enum FMSH_state g_DMA_ctlDstScatterEn = FMSH_clear;
enum FMSH_state g_DMA_ctlSrcGatherEn = FMSH_clear;
// Set the transfer device type and flow controller
enum FDmaPs_transferFlow g_DMA_ctlTtFc = Dmac_mem2prf_dma;

// Set handshaking select
enum FDmaPs_swHwHsSelect g_DMA_cfgHsSelSrc = Dmac_hs_hardware;
enum FDmaPs_swHwHsSelect g_DMA_cfgHsSelDst = Dmac_hs_hardware;
// Set handshaking interface polarity
enum FDmaPs_polarityLevel g_DMA_cfgSrcHsPol = Dmac_active_high;
enum FDmaPs_polarityLevel g_DMA_cfgDstHsPol = Dmac_active_high;
// Set FIFO mode select
enum FDmaPs_fifoMode g_DMA_cfgFifoMode = Dmac_fifo_mode_single;
// Set hardware handshaking interface
enum FDmaPs_hsInterface g_DMA_cfgDestPer = Dmac_hs_if12;
enum FDmaPs_hsInterface g_DMA_cfgSrcPer = Dmac_hs_if13;

// set source gather parameters
u32 g_DMA_sgrSgc = 0;
u32 g_DMA_sgrSgi = 0;
// set destination scatter parameters
u32 g_DMA_dsrDsc = 0;
u32 g_DMA_dsrDsi = 0;

/************************** Function Prototypes ******************************/



