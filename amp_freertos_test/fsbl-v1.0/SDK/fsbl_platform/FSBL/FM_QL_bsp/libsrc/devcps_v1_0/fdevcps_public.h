/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_devc_lib.h
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
* 0.01   wfb  11/23/2018  First Release
*</pre>
******************************************************************************/

#ifndef _FDEVCPS_PUBLIC_H_		/* prevent circular inclusions */
#define _FDEVCPS_PUBLIC_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
 
/***************************** Include Files *********************************/
        
#include "fdevcps_common.h"

/************************** Constant Definitions *****************************/
        
/*FOR FMSH 325T MUST PATCH IT USING PROCISE DEVELOPED BY FMSH LTD BEFORE USE IT*/
//#define FMSH_325T /********for FMSH 325T, must open the define********/
//#define TEST_TIME /********for output dma carry time, must open the define********/

#define DEVC_POLL_DONE_MS 1500
    
#define PCAP_WR_DATA_ADDR 0xFFFFFFFF
#define PCAP_RD_DATA_ADDR 0xFFFFFFFF
#define FMSH_DMA_INVALID_ADDRESS         PCAP_RD_DATA_ADDR
    
//Types of PCAP transfers
#define FMSH_NON_SECURE_PCAP_WRITE		0
#define FMSH_SECURE_PCAP_WRITE			1
#define FMSH_PCAP_READBACK			2
#define FMSH_PCAP_LOOPBACK			3
#define FMSH_NON_SECURE_PCAP_WRITE_DUMMMY	4
    
/*
 * Addresses of the Configuration Registers
 */
#define CRC		0	/* Status Register */
#define FAR		1	/* Frame Address Register */
#define FDRI		2	/* FDRI Register */
#define FDRO		3	/* FDRO Register */
#define CMD		4	/* Command Register */
#define CTL0		5	/* Control Register 0 */
#define MASK		6	/* MASK Register */
#define STAT		7	/* Status Register */
#define LOUT		8	/* LOUT Register */
#define COR0		9	/* Configuration Options Register 0 */
#define MFWR		10	/* MFWR Register */
#define CBC		11	/* CBC Register */
#define IDCODE		12	/* IDCODE Register */
#define AXSS		13	/* AXSS Register */
#define COR1		14	/* Configuration Options Register 1 */
#define WBSTAR		16	/* Warm Boot Start Address Register */
#define TIMER		17	/* Watchdog Timer Register */
#define BOOTSTS		22	/* Boot History Status Register */
#define CTL1		24	/* Control Register 1 */

/**************************** Type Definitions *******************************/
    
/*download mode*/ 
enum download_mode 
{
    DOWNLOAD_BITSTREAM = 0x0,
    READBACK_BITSTREAM = 0x2, 
    DATA_LOOPBACK = 0x3,     
    SECURE_DOWNLOAD_BITSTREAM = 0x08   
};

/***************** Macros (Inline Functions) Definitions *********************/  
#define TMP_PL_BUF_LEN 1024        
/************************** Variable Definitions *****************************/
extern u8 tmpPlBuffer[TMP_PL_BUF_LEN];
/************************** Function Prototypes ******************************/

u8 FDevcPs_unLockCSU(FDevcPs_T *devcDev);
u8 FDevcPs_adjustFreq(u8* flag);
u8 FDevcPs_recoverFreq(void);

u8 FDevcPs_init(FDevcPs_T *devcDev, u32 addr);
u8 FDevcPs_fabricInit(FDevcPs_T *devcDev, u32 TransferType);

u8 FDevcPs_KUPKEY(FDevcPs_T *devcDev, u32 *p, u32 len);
u8 FDevcPs_IV(FDevcPs_T *devcDev, u32 *p, u32 len);

u8 FDevcPs_pollFpgaDone(FDevcPs_T *devcDev, u32 maxcount);

u8 FDevcPs_getConfigdata(FDevcPs_T *devcDev, 
                           u32 *DestinationDataPtr, 
                           u32 DestinationLength, 
                           u32 addr, 
                           u32 ConfigReg);

u8 FDevcPs_pcapLoadPartition(FDevcPs_T *devcDev, 
                               u32 *SourceDataPtr, 
                               u32 *DestinationDataPtr, 
                               u32 SourceLength, 
                               u32 DestinationLength, 
                               u32 SecureTransfer);

u8 FDevcPs_keyRollingDownload(FDevcPs_T *dev, u8 alg_flag, u8 opkey_flag, u32 srcPtr, u32 bitlen);
        
u8 FDevcPs_download_non_secure(u32 srcAddress,u32 len);

u8 FDevcPs_encryptDownload_AES_NoOp(u32* devc_iv, u32 srcPtr,u32 bitlen);

u8 FDevcPs_encryptDownload_AES_UseOp(u32* devc_iv, u32 srcPtr,u32 bitlen);

u8 FDevcPs_encryptDownload_SM4_NoOp(u32* devc_iv, u32 srcPtr,u32 bitlen);

u8 FDevcPs_encryptDownload_SM4_UseOp(u32* devc_iv, u32 srcPtr,u32 bitlen);

u8 FDevcPs_devcInitDownload(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */

