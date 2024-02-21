/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_nor.c
*
* This file contains fmsh_nor.h.
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

/***************************** Include Files *********************************/
#include "boot_main.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
 static void Nor_initMemctl()
 {
    u32 reg_tmp;


    reg_tmp = 0x12354;
    Fmsh_Out32(FPS_SMC_CTL_BASEADDR+MEMCTL_SMTMGR_SET0, reg_tmp);
    Fmsh_Out32(FPS_SMC_CTL_BASEADDR+MEMCTL_SMTMGR_SET1, reg_tmp);
    Fmsh_Out32(FPS_SMC_CTL_BASEADDR+MEMCTL_SMTMGR_SET2, reg_tmp);

    // flash, 256Mb
    reg_tmp = 0x4A;
    Fmsh_Out32(FPS_SMC_CTL_BASEADDR+MEMCTL_SMSKRx(0), reg_tmp);
    Fmsh_Out32(FPS_SMC_CTL_BASEADDR+MEMCTL_SMSKRx(1), reg_tmp);

    reg_tmp = 0x1201;
    Fmsh_Out32(FPS_SMC_CTL_BASEADDR+MEMCTL_SMCTLR, reg_tmp);	 

 }



u32 FmshFsbl_NorInit(u32 DeviceFlags)
{
   /**/

  
  
  Nor_initMemctl();
    
	/*
	 * Set up the base address for access
	 */
//	FlashReadBaseAddress = PS_NOR_BASEADDR;
    return FMSH_SUCCESS;
}

u32 FmshFsbl_NorCopy(u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
	u32 Data;
	u32 Count;
	u32 *SourceAddr;
	u32 *DestAddr;
	u32 LengthWords;

	/*
	 * check for non-word tail
	 * add bytes to cover the end
	 */
	if ((LengthBytes%4) != 0){

		LengthBytes += (4 - (LengthBytes & 0x00000003));
	}

	LengthWords = LengthBytes >> WORD_LENGTH_SHIFT;

	SourceAddr = (u32 *)(SourceAddress + FPS_SMC_NORSRAM0_BASEADDR);
	DestAddr = (u32 *)(DestinationAddress);

	/*
	 * Word transfers, endianism isn't an issue
	 */
	for (Count=0; Count < LengthWords; Count++){

		Data = Fmsh_In32((u32)(SourceAddr++));
		Fmsh_Out32((u32)(DestAddr++), Data);
	}

	return FMSH_SUCCESS;
}

u32 FmshFsbl_NorRelease(u32 DeviceFlags)
{
	u32 Status =FMSH_SUCCESS;

	return Status;
}
