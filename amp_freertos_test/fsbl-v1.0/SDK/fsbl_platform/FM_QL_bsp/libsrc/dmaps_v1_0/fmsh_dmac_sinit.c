/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_dmac_sint.c
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
* 0.01   yl  08/27/2019  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "fmsh_dmac.h"          // DMA Controller header
#include "fmsh_ps_parameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern FDmaPs_Config FDmaPs_ConfigTable[];

/************************** Function Prototypes ******************************/

/****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. The table
* contains the configuration info for each device in the system.
*
* @param DeviceId contains the ID of the device
*
* @return
*
* A pointer to the configuration structure or NULL if the specified device
* is not in the system.
*
* @note
*
* None.
*
******************************************************************************/
FDmaPs_Config *FDmaPs_LookupConfig(u16 DeviceId)
{
    FDmaPs_Config *CfgPtr = NULL;

    int i;

    for (i = 0; i < FPAR_DMAPS_NUM_INSTANCES; i++)
    {
        if (FDmaPs_ConfigTable[i].DeviceId == DeviceId)
        {
            CfgPtr = &FDmaPs_ConfigTable[i];
            break;
        }
    }

    return CfgPtr;
}

