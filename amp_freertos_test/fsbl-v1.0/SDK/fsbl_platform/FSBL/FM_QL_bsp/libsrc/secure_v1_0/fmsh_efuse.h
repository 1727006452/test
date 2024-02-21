/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_efuse.h
*
* This file contains header fmsh_common.h & fmsh_types.h
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

#ifndef _FMSH_EFUSE_H_
#define _FMSH_EFUSE_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/  
#include "fmsh_common.h"
  
/************************** Constant Definitions *****************************/
    
/**************************** Type Definitions *******************************/
 
/**
 * Register: EFUSE_SEC_CTRL
 */
#define EFUSE_SEC_CTRL    (FPS_CSU_BASEADDR  + 0x254U )
#define EFUSE_SEC_CTRL_RSA_EN_MASK    0X03000000U

/* Register PPK0_0 */
#define EFUSE_PPK0_START	(FPS_CSU_BASEADDR + 0x29C)

/* Register PPK1_0 */
#define EFUSE_PPK1_START	(FPS_CSU_BASEADDR + 0x2BC)

/* Register SPK ID */
#define EFUSE_SPKID	(FPS_CSU_BASEADDR + 0x258)

#define EFUSE_SECURE_BOOT_EN  (FPS_CSU_BASEADDR + 0x32c)

#define EFUSE_XADC_ALRM_INTERRUPT_EN  (FPS_CSU_BASEADDR + 0x338)
  
#define APU_ROM_DIS_ROW_NUMBER   74
  
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

u32 FmshFsbl_EfuseReadData(u32 bRowAddr);
u32 FmshFsbl_FindOneInNumber(u32 Data);
  


//   
//#define EFUSE_AES_SM4_KEY_OFFSET    0x0 
//   
//#define EFUSE_IV_OFFSET    0x34 
//
//#define EFUSE_PUF_HD_OFFSET    0x100 
//
//#define EFUSE_PUF_HD_PARITY_OFFSET    0x48 
//
//#define EFUSE_PUF_BLACK_KEY_TAG_OFFSET    0x24



//#define EFUSE_SMMU_INTERRUPT_EN  (FPS_CSU_BASEADDR + 0x33C)
//
//
//#define AES_KEY_ROW_NUMBER_START   0
//#define ENCRYPTED_KEY_TAG_ROW_NUMBER_START   9  
//#define AES_KEY_IV_ROW_NUMBER_START   13

//#define EFUSE_UART_DIS_ROW_NUMBER  17
//#define EFUSE_PPK0_ROW_NUMBER_START 20
//#define EFUSE_PPK1_ROW_NUMBER_START 29
//#define EFUSE_SPK_ID_ROW_NUMBER_START 38  
//   
//   
//#define  EFUSE_BOOT_IMG_RSA_EN_ROW_NUMBER  19




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */


