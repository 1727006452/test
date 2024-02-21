/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sdmmc_private.h
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
#ifndef _FMSH_SDMMC_PRIVATE_H_
#define _FMSH_SDMMC_PRIVATE_H_

/***************************** Include Files *********************************/

#include "fmsh_common.h"

/************************** Constant Definitions *****************************/

/* MMC Response length */
#define CMD_RES_LEN_SHORT		6
#define CMD_RES_LEN_LONG		4

/**************************** Type Definitions *******************************/

// Common requirements (preconditions) for all dmac driver functions
#define SDMMC_COMMON_REQUIREMENTS(p)             \
do {                                            \
    FMSH_ASSERT(p != NULL);                      \
    FMSH_ASSERT(p->instance != NULL);            \
    FMSH_ASSERT(p->comp_type == FMSH_ahb_sdmmc);    \
} while(0)


/* allow C++ to use this header */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
* DESCRIPTION
*  This data type is used to select the response type.
*
*****************************************************************************/
enum FSdmmcPs_resType {
	CMD_RES_TYPE_NONE = 0,
	CMD_RES_TYPE_R1,
	CMD_RES_TYPE_R1B,
	CMD_RES_TYPE_R2,
	CMD_RES_TYPE_R3,
	CMD_RES_TYPE_R4,
	CMD_RES_TYPE_R5,
	CMD_RES_TYPE_R6,
	CMD_RES_TYPE_R7
};

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef _FMSH_SDMMC_PRIVATE_H_ */


