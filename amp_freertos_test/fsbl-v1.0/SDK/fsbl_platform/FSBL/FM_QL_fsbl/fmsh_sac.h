/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sac.h
*
* This file contains header fmsh_common.h
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   LQ  1/7/2019  First Release
*</pre>
******************************************************************************/


#ifndef _FMSH_SAC_H_
#define _FMSH_SAC_H_

#ifdef __cplusplus
extern "C" {
#endif
  
/***************************** Include Files *********************************/
#include "fmsh_common.h"


/************************** Constant Definitions *****************************/
#define SAC_UNLOCK_CONS_REG_OFFSET      0x2c
#define SAC_CFG_REG_OFFSET              0xC
#define SAC_CTRL_REG_OFFSET              0x8
#define SAC_UNLOCK                     0x757bdf0d
#define SAC_AES_EN_MASK  0x00000e00
#define SAC_AES_EN 0x00000e00
#define SAC_AES_EN_FLAG 1U
#define SAC_AES_DIS_FLAG 0U
#define  EFUSE_AES_KEY_RD_DIS_ADDR 0xFC4

#define  SPU_RAM_CLEAR 0x50
#define XIP_MODE_ADDR  0xcc
#define XIP_EXEC_ADDR  0xD0
#define SAC_DATA_OP_MASK               0x00f00000
#define SAC_DATA_SWAP_MASK               0x03060000

#define SAC_AES_GCM_MODE  0x00400000
#define SAC_SECURE_BITSTREAM_DOWNLOAD_MODE  0x00800000
#define SAC_SHA256_MODE  0x00600000
#define SAC_MULTH_MODE  0x00700000


#define SAC_OBFUSCATED_KEY_REG_OFFSET      0x8
#define SAC_OBFUSCATED_KEY_IV_REG_OFFSET      0xC
#define SAC_OBFUSCATED_KEY_PARITY_REG_OFFSET      0x10

#define SAC_BLACK_KEY_REG_OFFSET      0x18
#define SAC_BLACK_KEY_IV_REG_OFFSET      0x1C
#define SAC_PUF_KEK_PARITY_REG_OFFSET      0x20


#define SAC_TX_FIFO_DATA_BYTE_SWAP     0x02000000

#define SAC_RX_FIFO_DATA_BYTE_SWAP     0x00040000


#define SAC_NO_FIFO_DATA_BYTE_SWAP     0x00000000

/*SAC register offset definition*/

/*DMA register*/
#define  SAC_DMA_SRC_ADDR_OFFSET   0x1C
#define  SAC_DMA_DEST_ADDR_OFFSET  0x20
#define  SAC_DMA_SRC_LEN_OFFSET    0x24
#define  SAC_DMA_DEST_LEN_OFFSET   0x28
#define  SAC_INT_STS_OFFSET        0x10

#define PUF_CMD_ADDR  0x94
  
#define A7_SECURITY_RAM_BASE  (0xE1FE0000+0x10000)
#define   SPU_APU_ADDRESS_OFFSET 0x0

/*SHA256 Register*/
#define SHA2_COMPUTE_RST_REG     0x144
#define SHA2_DIGEST_REG0_OFFSET  0x148
#define SHA2_DIGEST_REG1_OFFSET  0x14c
#define SHA2_DIGEST_REG2_OFFSET  0x150
#define SHA2_DIGEST_REG3_OFFSET  0x154
#define SHA2_DIGEST_REG4_OFFSET  0x158
#define SHA2_DIGEST_REG5_OFFSET  0x15c
#define SHA2_DIGEST_REG6_OFFSET  0x160
#define SHA2_DIGEST_REG7_OFFSET  0x164

  
    
/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
void FmshFsbl_SacInit(u32 InitFlag);
void FmshFsbl_SacUnlock(void);
u32 FmshFsbl_SetSacMode(u32 Mode);
u32 FmshFsbl_SetSacDataSwap(u32 Mode);
void FmshFsbl_ByteSwap(u8* KeyPtr);
void FmshFsbl_SacAesSwitch(u32 Flag);
void FmshFsbl_OpenCfgLevelShifter(void);
void FmshFsbl_CloseUsrLevelShifter(void);
u8 FmshFsbl_IsBitDone(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */