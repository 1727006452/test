/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc.h
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
#ifndef _FMSH_SMC_H_
#define _FMSH_SMC_H_

/***************************** Include Files *********************************/

#include "fmsh_smc_common.h"
#include "fmsh_smc_cfi.h"

/************************** Constant Definitions *****************************/

/* allow C++ to use this header */
#ifdef __cplusplus
extern "C" {
#endif

/**************************** Type Definitions *******************************/
#if 0
typedef struct _nor_info{
	u16 manufacture_id;
	u16 device_id1;
	u16 device_id2;
	u16 device_id3;
}NorInfo_T;

typedef struct _smc_param {
	NorInfo_T norInfo;
}FSmcPs_Param_T;
#endif

/*****************************************************************************
* DESCRIPTION
*  This structure is used to pass transfer information into
*  the driver, so that it can be accessed and manipulated by
*  the user function.
*
*****************************************************************************/
typedef struct _smc_instance {
    u32 sram_nor_baseAddr;
	u8 bus_width;
	NorFlash_T norFlash;
}FSmcPs_Instance_T;

/*****************************************************************************
* DESCRIPTION
*  This is the primary structure used when dealing with SMC controller.
*  It allows this code to support more than one device of the same type
*  simultaneously.  This structure needs to be initialized with
*  meaningful values before a pointer to it is passed to a driver
*  initialization function.
* PARAMETERS
*  name            name of device
*  dataWidth       bus data width of the device
*  baseAddress     physical base address of device
*  instance        device private data structure pointer
*  os              unused pointer for associating with an OS structure
*  compParam       pointer to structure containing device's
*                  coreConsultant configuration parameters structure
*  compVersion     device version identification number
*  compType        device identification number
*
*****************************************************************************/
typedef struct _smc_device
{
    unsigned data_width;
    u32 base_address;
    FSmcPs_Instance_T *instance;
    uint32_t comp_version;
    enum FMSH_compType comp_type;
    FMSH_listHead list;
}FSmcPs_T;

/***************** Macros (Inline Functions) Definitions *********************/

#define SMC_COMMON_REQUIREMENTS(p)             \
do {                                            \
    FMSH_ASSERT(p != NULL);                      \
    FMSH_ASSERT(p->instance != NULL);            \
    FMSH_ASSERT(p->base_address != NULL);        \
    FMSH_ASSERT(p->comp_type == FMSH_ahb_smc);    \
} while(0)

/*****************************************************
* Define Sram read/write macro                       *
*****************************************************/
#define SRAM_read(baseAddr, offSet)		    *((volatile u8 *)(baseAddr + offSet))
#define SRAM_write(baseAddr, offSet, data)	*((volatile u8 *)(baseAddr + offSet)) = data

/************************** Function Prototypes ******************************/

void FSmcPs_initDev(FSmcPs_T *pSmc, FSmcPs_Instance_T *pInstance);
void FSmcPs_resetController(void);
void FSmcPs_setTimingRegs(FSmcPs_T *pSmc, u8 index, u32 data);
u32 FSmcPs_getTimingRegs(FSmcPs_T *pSmc, u8 index);
void FSmcPs_setMaskRegs(FSmcPs_T *pSmc, u8 index, u32 data);
u32 FSmcPs_getMaskRegs(FSmcPs_T *pSmc, u8 index);
void FSmcPs_setCtlRegs(FSmcPs_T *pSmc, u32 data);
u32 FSmcPs_getCtlRegs(FSmcPs_T *pSmc);
int FSmcPs_resetInstance(FSmcPs_T *pSmc, u8 chip_sel, u8 bus_width);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* #ifndef _FMSH_SMC_H_ */


