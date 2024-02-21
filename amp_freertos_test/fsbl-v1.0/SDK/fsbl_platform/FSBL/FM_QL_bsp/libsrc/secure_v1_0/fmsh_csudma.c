/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_csudma.c
*
* This file contains fmsh_csudma.h.
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

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
void FmshFsbl_CsuInitiateDma(u32 Source, u32 Dest, u32 SrcWordLength, u32 DestWordLength)
{
    WriteReg(FPS_CSU_BASEADDR + SAC_DMA_SRC_ADDR_OFFSET, Source);

    WriteReg(FPS_CSU_BASEADDR + SAC_DMA_DEST_ADDR_OFFSET, Dest);

    WriteReg(FPS_CSU_BASEADDR + SAC_DMA_SRC_LEN_OFFSET, SrcWordLength);

    WriteReg(FPS_CSU_BASEADDR + SAC_DMA_DEST_LEN_OFFSET, DestWordLength);

}

/**
* This function will be in busy while loop until the data transfer is
* completed.
*/
void FmshFsbl_CsuDmaPollDone(u32 MaskValue, u32 MaxCount)
{
	u32 Count = MaxCount;
	u32 IntrStsReg = 0;

	//poll for the DMA done
	IntrStsReg = ReadReg(FPS_CSU_BASEADDR + SAC_INT_STS_OFFSET);

	while ((IntrStsReg & MaskValue) != MaskValue) 
        {
            IntrStsReg = ReadReg(FPS_CSU_BASEADDR + SAC_INT_STS_OFFSET);
       
            Count -=1;

            if(IntrStsReg & IXR_ERROR_FLAGS_MASK)
            {
              //	debug_printf("error in INT_STS reg!");
            }

	    if (!Count) 
            {
              //   debug_printf("PCAP transfer timed out!");
            }
        }
        WriteReg(FPS_CSU_BASEADDR + SAC_INT_STS_OFFSET, IntrStsReg & MaskValue);
   //   debug_printf("poll for dma/fpga done!");
}
