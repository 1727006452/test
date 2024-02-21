/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc.c
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

/***************************** Include Files *********************************/
#include <stdint.h>
#include <string.h>
#include "fmsh_smc_lib.h"
#include "fmsh_ps_parameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*****************************************************************************
*
* @description
* This function is initialize the SMC device.
*
* @param    
*           pSmc is the pointer to a smc device.
*           pInstance is the pointer to instance structure.
*
* @return   
*           NA.
*
* @note     NA.
*
*****************************************************************************/
void FSmcPs_initDev(FSmcPs_T *pSmc, FSmcPs_Instance_T *pInstance)
{
	// initial device
	pSmc->data_width = 32;
	pSmc->base_address = (u32)FPS_SMC_CTL_BASEADDR;
	pSmc->instance = pInstance;
	pSmc->comp_type = FMSH_ahb_smc;
	FMSH_INIT_LIST_HEAD(&pSmc->list);
}

/*****************************************************************************
*
* @description
* Reset the SMC controller by SLCR registers.
*
* @param	
*           NA.
*
* @return   
*           NA.
*
* @note		
*           NA.
*
*****************************************************************************/
void FSmcPs_resetController(void)
{
	u32 i;
	
	// unlock SLCR
	FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);
	// set rst_ctrl
	FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x278, 1);

	for(i = 0; i< 50; i++);
	
	// clear rst_ctrl
	FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x278, 0);
	// relock SLCR
	FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x004, 0xDF0D767B);
}

/*****************************************************************************
*
* @description
* This function sets value to the SMC timing registers.
*
* @param    
*           pSmc is the pointer to the SMC device.
*           index is the timing register numer.
*           data is the value which writes to the timing register.
*
* @return   
*           NA.
*
* @note     
*           NA.
*
*****************************************************************************/
void FSmcPs_setTimingRegs(FSmcPs_T *pSmc, u8 index, u32 data)
{
	u32 offset;

	SMC_COMMON_REQUIREMENTS(pSmc);

	offset = SMC_SMTMGR_SET0 + index * 0x4;
	FMSH_WriteReg(pSmc->base_address, offset, data);
}

/*****************************************************************************
*
* @description
* This function gets value from the SMC timing register.
*
* @param    
*           pSmc is the pointer to the SMC device.
*           index is the timing register numer.
*
* @return   
*           the value reads from the timing register.
*
* @note     NA.
*
*****************************************************************************/
u32 FSmcPs_getTimingRegs(FSmcPs_T *pSmc, u8 index)
{
	u32 offset, reg;
	
	SMC_COMMON_REQUIREMENTS(pSmc);

	offset = SMC_SMTMGR_SET0 + index * 0x4;
	reg = FMSH_ReadReg(pSmc->base_address, offset);

	return reg;
}

/*****************************************************************************
*
* @description
* This function sets value to the SMC address mask registers.
*
* @param    
*           pSmc is the pointer to the SMC device.
*           index is the address mask register numer.
*           data is the value which writes to the address mask register.
*
* @return   
*           NA.
*
* @note     
*           NA.
*
*****************************************************************************/
void FSmcPs_setMaskRegs(FSmcPs_T *pSmc, u8 index, u32 data)
{
	SMC_COMMON_REQUIREMENTS(pSmc);

	FMSH_WriteReg(pSmc->base_address, SMC_SMSKRx(index), data);
}

/*****************************************************************************
*
* @description
* This function gets value from the SMC address mask register.
*
* @param    
*           pSmc is the pointer to the SMC device.
*           index is the address mask register numer.
*
* @return   
*           the value reads from the address mask register.
*
* @note     
*           NA.
*
*****************************************************************************/
u32 FSmcPs_getMaskRegs(FSmcPs_T *pSmc, u8 index)
{
	u32 reg;
	
	SMC_COMMON_REQUIREMENTS(pSmc);

	reg = FMSH_ReadReg(pSmc->base_address, SMC_SMSKRx(index));

	return reg;
}

/*****************************************************************************
*
* @description
* This function sets value to the SMC control registers.
*
* @param    
*           pSmc is the pointer to the SMC device.
*           data is the value which writes to the control register.
*
* @return   
*           NA.
*
* @note     
*           NA.
*
*****************************************************************************/
void FSmcPs_setCtlRegs(FSmcPs_T *pSmc, u32 data)
{
	SMC_COMMON_REQUIREMENTS(pSmc);

	FMSH_WriteReg(pSmc->base_address, SMC_SMCTLR, data);
}

/*****************************************************************************
*
* @description
* This function gets value from the SMC control register.
*
* @param    
*           pSmc is the pointer to the SMC device.
*
* @return   
*           the value reads from the control register.
*
* @note     
*           NA.
*
*****************************************************************************/
u32 FSmcPs_getCtlRegs(FSmcPs_T *pSmc)
{
	u32 reg;

	SMC_COMMON_REQUIREMENTS(pSmc);

	reg = FMSH_ReadReg(pSmc->base_address, SMC_SMCTLR);

	return reg;
}

/*****************************************************************************
*
* @description
* This function is reset the SMC instance.
*
* @param    pSmc is the pointer to the SMC device.
*           chip_sel is the select of the flash memory.
*           bus_width is the width of the flash memory, in bits.
*
* @return   
*           FMSH_SUCCESS if reset is performed.
*           FMSH_ENXIO if the select of the flash memory is outof range.
*
* @note		
*           NA.
*
*****************************************************************************/
int FSmcPs_resetInstance(FSmcPs_T *pSmc, u8 chip_sel, u8 bus_width)
{
	FSmcPs_Instance_T *pInstance;

	SMC_COMMON_REQUIREMENTS(pSmc);

	pInstance = pSmc->instance;

	pInstance->bus_width = bus_width;
	if (chip_sel == 0)
		pInstance->sram_nor_baseAddr = FPS_SMC_NORSRAM0_BASEADDR;
	else if (chip_sel == 1)
		pInstance->sram_nor_baseAddr = FPS_SMC_NORSRAM1_BASEADDR;
	else
		return -FMSH_ENXIO;

	memset(&pInstance->norFlash, 0, sizeof(NorFlash_T));
	pInstance->norFlash.baseAddress = pInstance->sram_nor_baseAddr;
	
	return FMSH_SUCCESS;
}





