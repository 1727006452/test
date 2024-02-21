/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_efuse.c
*
* This file contains fmsh_efuse.h.
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

u32 FmshFsbl_EfuseReadData(u32 bRowAddr)
{
  
    u32 read_reg=0;
    u32 status_reg=0;

    WriteReg(FPS_CSU_BASEADDR+0x200+0xc,bRowAddr);
       
    do
    {
      status_reg = ReadReg(FPS_CSU_BASEADDR+0x200+0x04);
    }while((status_reg & 0x2)!=0x2);
     
    read_reg = ReadReg(FPS_CSU_BASEADDR+0x200+0x10);
    
    return read_reg;
}


u32 FmshFsbl_FindOneInNumber(u32 Data)
{
    u32 n;
    for(n=0;Data;n++)
        Data &= Data-1;
    return n;
}
    
    
    
    
    
