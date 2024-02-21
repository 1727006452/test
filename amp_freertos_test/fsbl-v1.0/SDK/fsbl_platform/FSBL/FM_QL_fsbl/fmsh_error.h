/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_error.h
*
* This file contains header boot_main.h
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
#ifndef _FMSH_ERROR_H_
#define _FMSH_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "boot_main.h"
  
/************************** Constant Definitions *****************************/  
#define    ERROR_UNAVAILABLE_CPU                           0x20  
#define    PARTITION_SIGNATURE_VERIFY_FAILED               0x21
#define    INVALID_BOOT_MODE                               0x22
#define    INVALID_ID                                      0x23
#define    SECURITY_VIOLATION                              0x24
#define    NO_VALID_BOOT_IMG_HEADER                        0x25
#define    IMG_HEADER_CHECKSUM_ERROR                       0x26
#define    PPK_HASH_MISMATCH                               0x27
#define    SPK_ID_MISMATCH                                 0x28
#define    SPK_SIGNATURE_VERIFY_FAILED                     0x30
#define    BOOT_HEADER_SIGNATURE_VERIFY_FAILED             0x31
#define    ERROR_PH_CHECKSUM_FAILED			   0x32
#define    ERROR_NOT_PARTITION_OWNER		           0x33
#define    ERROR_XIP_AUTH_ENC_PRESENT                      0x34
#define    ERROR_APU_XIP_EXCUTION_ADDRESS		   0x35
#define    MISMATCH_FSBL_LENGTH			           0x36
#define    PARTITION_AUTHENTICATE_FAILURE                  0x37
#define    ERROR_INVALID_EXCUTION_ADDRESS                  0x38
#define    DECYPTION_FAILURE                               0x39
#define    PARTITION_CHECKSUM_ERROR                        0x3A
#define    SECURE_BOOT_FORCE_NOT_MATCH                     0x3B

#define    PARTITION_SKIP_LOAD                             0x55
  
#define WriteErrorCode(p)   Fmsh_Out32(REBOOT_STATUS_REG,Fmsh_In32(REBOOT_STATUS_REG)&0xFFFF0000U | p)
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
   
#ifdef __cplusplus
}
#endif

#endif  /* FSBL_ERROR_H */
