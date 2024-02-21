/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_fnand.c
*
* This file contains fmsh_fnand.h.
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
#include "fmsh_fnand.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
FNfcPs_T NandInstance;
FNfcPs_T* NandInstancePtr;

/************************** Function Prototypes ******************************/
u32 FmshFsbl_NandInit(void)
{
        int Status;
        NandInstancePtr = &NandInstance;
        Status = FNfcPs_Initialize(&NandInstance, FPS_NFC_S_DEVICE_ID);
        if (Status != 0) {
                return 1;
        }
        FNfcPs_Reset(&NandInstance);
        FNfcPs_SetTiming(&NandInstance);
        Status = FNfcPs_InitController(&NandInstance,NFCPS_READ_STATUS_EN_MASK);
        if (Status != 0) {
                return 1;
        }
        return 0;
}

u32 FmshFsbl_NandCopy(u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
        int Status;
        Status = SkipBlockNandflash_Read(NandInstancePtr, SourceAddress, LengthBytes, (u8*)DestinationAddress);
        if (Status != 0) {
                return 1;
        }
        return 0;
}

