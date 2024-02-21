#ifndef _FMSH_DMAC_CONFIG_H_
#define _FMSH_DMAC_CONFIG_H_

#include "fmsh_common.h"
#include "fmsh_ps_parameters.h"
#include "fmsh_dmac_lib.h"
#include "fmsh_dmac_hw.h"
#include "fmsh_dmac_private.h"

// Source memory location
#define SRC_MEMORY_BASE          (FPS_AHB_SRAM_BASEADDR)
// Destination memory location
#define DST_MEMORY_BASE          (FPS_AHB_SRAM_BASEADDR + 0x10000)
// LLI start address
#define LLI_MEMORY_BASE          (FPS_AHB_SRAM_BASEADDR + 0x5000)
#define LLI_MEMORY_OFFSET        (0x500)

#define DMAC_TEST_BLOCK_SIZE     2048

extern enum FMSH_compType g_DMA_compType; 		// dma component type
extern u32 g_DMA_compVersion;	// dma component version
extern enum FDmaPs_channelNumber g_DMA_chNum; 	// Select a DMA channel
extern enum FDmaPs_transferType g_DMA_transType; // Select a transfer type
// Set src/dst address
extern u32 g_DMA_srcAddress;
extern u32 g_DMA_dstAddress;
extern u32 g_DMA_blockSize; // Select the size of a block
extern u32 g_DMA_numBlocks; // Select the number of blocks
// Set the source and destination transfer width
extern enum FDmaPs_transferWidth g_DMA_ctlSrcTrWidth;
extern enum FDmaPs_transferWidth g_DMA_ctlDstTrWidth;
// Set the Address increment type for the source and destination
extern enum FDmaPs_addressIncrement g_DMA_ctlSinc;
extern enum FDmaPs_addressIncrement g_DMA_ctlDinc;
// Set the source and destination burst transaction length
extern enum FDmaPs_burstTransLength g_DMA_ctlSrcMsize;
extern enum FDmaPs_burstTransLength g_DMA_ctlDstMsize;
// set scatter and gather enable
extern enum FMSH_state g_DMA_ctlDstScatterEn;
extern enum FMSH_state g_DMA_ctlSrcGatherEn;
// Set the transfer device type and flow controller
extern enum FDmaPs_transferFlow g_DMA_ctlTtFc;

// Set handshaking select
extern enum FDmaPs_swHwHsSelect g_DMA_cfgHsSelSrc;
extern enum FDmaPs_swHwHsSelect g_DMA_cfgHsSelDst;
// Set handshaking interface polarity
extern enum FDmaPs_polarityLevel g_DMA_cfgSrcHsPol;
extern enum FDmaPs_polarityLevel g_DMA_cfgDstHsPol;
// Set FIFO mode select
extern enum FDmaPs_fifoMode g_DMA_cfgFifoMode;
// Set hardware handshaking interface
extern enum FDmaPs_hsInterface g_DMA_cfgDestPer;
extern enum FDmaPs_hsInterface g_DMA_cfgSrcPer;

// set source gather parameters
extern u32 g_DMA_sgrSgc;
extern u32 g_DMA_sgrSgi;
// set destination scatter parameters
extern u32 g_DMA_dsrDsc;
extern u32 g_DMA_dsrDsi;

typedef struct _dmac_channel_info {
	u8                            ch_en;
	u16                           block_num;
	enum FDmaPs_transferType      transfer_type;
    u32                           sar;
    u32                           dar;
    enum FDmaPs_transferWidth     ctl_dst_tr_width;
    enum FDmaPs_transferWidth     ctl_src_tr_width;
	u16                           ctl_block_ts;
	enum FDmaPs_transferFlow      ctl_tt_fc;
    enum FDmaPs_burstTransLength  ctl_dst_msize;
    enum FDmaPs_burstTransLength  ctl_src_msize;
    enum FDmaPs_addressIncrement  ctl_dinc;
    enum FDmaPs_addressIncrement  ctl_sinc;
	u8                            cfg_ch_prior;
	u8                            cfg_ch_susp;
    enum FDmaPs_fifoMode          cfg_fifo_mode;
    enum FMSH_state               ctl_src_gather_en;
    enum FMSH_state               ctl_dst_scatter_en;
    u32                           sgr_sgi;
    u16                           sgr_sgc;
    u32                           dsr_dsi;
    u16                           dsr_dsc;
	u8                            src_mem_type;
	u8                            dst_mem_type;
	FMSH_listHead                 list_head;
}FDmaPs_ChannelInfo;

#if 0
//set the top-level parameters
extern u32 g_DMA_BigEndian;	// select the interfaces big or little endian
extern u32 g_DMA_IntrIO;	// selects interrupt pins to appear as outputs
extern u32 g_DMA_Mabrst;	// limit the AMBA burst length by sw
extern u32 g_DMA_NumChannels;	// number of DMA channels
extern u32 g_DMA_NumMasterInt;	// number of AHB master interface
extern u32 g_DMA_SHdataWidth;	// slave interface data bus width
extern u32 g_DMA_M4HdataWidth;	// master 4 interface data bus width
extern u32 g_DMA_M3HdataWidth;	// master 3 interface data bus width
extern u32 g_DMA_M2HdataWidth;	// master 2 interface data bus width
extern u32 g_DMA_M1HdataWidth;	// master 1 interface data bus width
extern u32 g_DMA_NumHsInt;	// number of handshaking interface
extern u32 g_DMA_AddEncodedParams;	// add encoded parameters
extern u32 g_DMA_StaticEndianSelect;	// statically or dynamically configure endianness
// set the component parameters for chx
extern u32 g_DMA_Chx_Dtw[8];	// hardcode chx's dst transfer width
extern u32 g_DMA_Chx_Stw[8];	// hardcode chx's src transfer width
extern u32 g_DMA_Chx_StatDst[8];	// fetch status from dst of chx
extern u32 g_DMA_Chx_StatSrc[8];	// fetch status from src of chx
extern u32 g_DMA_Chx_DstScaEn[8];	// enable the scatter feature on chx
extern u32 g_DMA_Chx_SrcGatEn[8];	// enable the gather feature on chx
extern u32 g_DMA_Chx_LockEn[8];	// enable channel or bus locking on chx
extern u32 g_DMA_Chx_MultiBlkEn[8];	// enable multi-block DMA transfers on chx
extern u32 g_DMA_Chx_CtlWbEn[8];	// enable control register writeback after each block tansfer
extern u32 g_DMA_Chx_HcLlp[8];	// hardcode chx LLP register to 0
extern u32 g_DMA_Chx_Fc[8];	// hardcode chx's flow control device
extern u32 g_DMA_Chx_MaxMultSize[8];	// max value of burst transaction size
extern u32 g_DMA_Chx_Dms[8];	// hardcode the master interface attached to the dst of chx
extern u32 g_DMA_Chx_Lms[8];	// hardcode the AHB master interface attached to the LLP of chx
extern u32 g_DMA_Chx_Sms[8];	// hardcode the AHB master interface attached to the src of chx
extern u32 g_DMA_Chx_FifoDepth[8];	// chx FIFO depth in bytes
extern u32 g_DMA_Chx_MaxBlkSize[8];	// max block size in src transfer widths
extern u32 g_DMA_Chx_MultiBlkType[8];	// choose type of multi-blocks to be supported
#endif

#endif /* #ifndef _DMAC_CONFIG_H_ */
