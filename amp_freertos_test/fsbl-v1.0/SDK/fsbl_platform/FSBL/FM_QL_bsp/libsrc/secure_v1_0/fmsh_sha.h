/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sha.h
*
* This file contains header fmsh_uart_private.h & fmsh_uart_public.h
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   wfb  11/23/2018  First Release
*</pre>
******************************************************************************/
#ifndef _FMSH_SHA_H_
#define _FMSH_SHA_H_

#ifdef __cplusplus
extern "C" {
#endif
  
/***************************** Include Files *********************************/  
#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "fmsh_common.h"
#include "fmsh_sac.h" 
#include "fmsh_csudma.h"

/************************** Constant Definitions *****************************/
//#define WriteReg    Fmsh_Out32
//#define ReadReg    Fmsh_In32

#define SHA2_COMPUTE_RST_REG  0x144

#define SHA2_DIGEST_REG0_OFFSET 0x148
#define SHA2_DIGEST_REG1_OFFSET  0x14c
#define SHA2_DIGEST_REG2_OFFSET  0x150
#define SHA2_DIGEST_REG3_OFFSET  0x154
#define SHA2_DIGEST_REG4_OFFSET 0x158
#define SHA2_DIGEST_REG5_OFFSET  0x15c
#define SHA2_DIGEST_REG6_OFFSET  0x160
#define SHA2_DIGEST_REG7_OFFSET  0x164
#define MSG "51525354555657585960"
#define  MULTIH_MODE 0x00000060
#define PCFG_DATA_OP_AES_GCM  0x00400000

#define PCFG_DATA_OP_MASK  0x00f00000
#define PCFG_AES_MODE_MASK 0x00000018

#define PCFG_AES_CHMODE_MASK 0x00000060
#define PCFG_AES_EN 0x00000001
#define PCFG_AES_DIS 0xFFFFFFF0

#define MAXIMUM_SHA_PADDING_LEN  72U

#define SHA_BLOCK_SIZE 64U
#define SHA_PADDING_BOUNDARY_SIZE 56U
#define SHA_FILL_DATA 0x80U


#define PUF_SHA_OK  1U    
/**************************** Type Definitions *******************************/
 
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
//u32 ConvertStringToHexBE(const char * Str, u8 * Buf, u32 Len);
//u32 ConvertStringToHexBE_1(const char * Str, u8 * Buf, u32 Len);
  
u8 FmshFsbl_sha256(u8* Message, u32 MessageByteLen, u8* Digest);

u8 FmshFsbl_BurstSha256(u8* Message, u32 MessageByteLen, u8* Digest);

u8 FmshFsbl_SubgroupSha256(u8* Message, u32 MessageByteLen, u8* Digest);

u8 FmshFsbl_LinearBurstSha256(u8* Message, u32 MessageByteLen, u8* Digest);
//typedef struct
//{
//    u32 AesIv3;
//    u32 AesIv2;
//    u32 AesIv1;
//    u32 AesIv0;
//}IV ;




//u32 SetIvReg(Secure_Aes AesInstance);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */