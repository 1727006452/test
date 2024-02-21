/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_watchdog.c
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

/***************************** Include Files *********************************/
#include "boot_main.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
FWdtPs_T g_WDT;

/****************************************************************************/
/**
*
* This function is used to initial g_WDT, register interrupt.
*
* @param    None.
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
u8 FmshFsbl_WdtInit()
{
    FWdtPs_init(&g_WDT, FPS_WDT_BASEADDR);
    
    FWdtPs_setRMOD(&g_WDT, FMSH_clear);
    
    FWdtPs_setRPL(&g_WDT, _128_pclk);
    
    FWdtPs_setTOP_INIT(&g_WDT, FSBL_WDT_TOP);
    FWdtPs_setTOP(&g_WDT,FSBL_WDT_TOP);
    
    FWdtPs_setWDT_EN(&g_WDT, FMSH_set);
    
    FWdtPs_restart(&g_WDT);
    
    return 0;
}

u8 FmshFsbl_WdtClose()
{
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x330,0x3);
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x330,0x0);
    return 0;
}
