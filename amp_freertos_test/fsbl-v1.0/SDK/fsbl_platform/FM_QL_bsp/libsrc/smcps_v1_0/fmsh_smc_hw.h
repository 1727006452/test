/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_hw.h
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
* 0.01   yl  12/20/2018  First Release
*</pre>
******************************************************************************/
#ifndef _FMSH_SMC_HW_H_
#define _FMSH_SMC_HW_H_

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

// define SMC register's offset
//  x = 0~7
#define SMC_SCONR				(0x0000)
#define SMC_STMG0R				(0x0004)
#define SMC_STMG1R				(0x0008)
#define SMC_SCTLR				(0x000C)
#define SMC_SREFR				(0x0010)
#define SMC_EXN_MODE_REG		(0x00AC)
#define SMC_SCRLRx_LOW(x)		(0x0014 + 0x0004 * x)
#define SMC_SMSKRx(x)			(0x0054 + 0x0004 * x)
#define SMC_CSALIAS0_LOW		(0x0074)
#define SMC_CSALIAS1_LOW		(0x0078)
#define SMC_CSREMAP0_LOW		(0x0084)
#define SMC_CSREMAP1_LOW		(0x0088)
#define SMC_SMTMGR_SET0			(0x0094)
#define SMC_SMTMGR_SET1			(0x0098)
#define SMC_SMTMGR_SET2			(0x009C)
#define SMC_FLASH_TRPDR			(0x00A0)
#define SMC_SMCTLR				(0x00A4)
/***/

 // SMSKRx registers
#define SMC_SMSKRx_REG_SELECT		(0x7 << 8)
#define SMC_SMSKRx_MEM_TYPE			(0x7 << 5)
#define SMC_SMSKRx_MEM_SIZE			(0x1F << 0)
// SMTMGR_SETx registers
#define SMC_SMSETx_SM_READ_PIPE		(0x3 << 28)
#define SMC_SMSETx_LOW_FREQ_SYNC_DEVICE		(0x1 << 27)
#define SMC_SMSETx_READY_MODE		(0x1 << 26)
#define SMC_SMSETx_PAGE_SIZE		(0x3 << 24)
#define SMC_SMSETx_PAGE_MODE		(0x1 << 23)
#define SMC_SMSETx_T_PRC		(0xF << 19)
#define SMC_SMSETx_T_BTA		(0x7 << 16)
#define SMC_SMSETx_T_WP		(0x3F << 10)
#define SMC_SMSETx_T_WR		(0x3 << 8)
#define SMC_SMSETx_T_AS		(0x3 << 6)
#define SMC_SMSETx_T_RC		(0x3F << 0)
// FLASH registers
#define SMC_FLASH_T_RPD		(0xFFF << 0)
// SMCTLR registers
#define SMC_SMCTLR_SM_DATA_WIDTH_SET2	(0x7 << 13)
#define SMC_SMCTLR_SM_DATA_WIDTH_SET1	(0x7 << 10)
#define SMC_SMCTLR_SM_DATA_WIDTH_SET0	(0x7 << 7)
#define SMC_SMCTLR_WP_N					(0x7 << 1)
#define SMC_SMCTLR_SM_RP_N				(0x1 << 0)
/*****/

#define SMC_SMTMGR_SETx_PARAM    (0x12354)
#define SMC_SMSKRx_PARAM         (0x4A)    // flash, 256Mb
#define SMC_SMCTLR_PARAM         (0x1201)

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

#endif /* #ifndef _FMSH_SMC_HW_H */