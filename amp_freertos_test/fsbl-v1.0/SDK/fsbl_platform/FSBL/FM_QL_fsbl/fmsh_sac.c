/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sac.c
*
* This file contains fmsh_sac.h.
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   XX  11/23/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include "boot_main.h" 

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void FmshFsbl_SacInit(u32 InitFlag)
{
  
    u32 CfgReg=0;
    FmshFsbl_SacUnlock();
    FmshFsbl_SacAesSwitch(InitFlag);   
    CfgReg = ReadReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET);
    WriteReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET, CfgReg|0x000000c0);

}

void FmshFsbl_SacUnlock(void)
{
   
    WriteReg(FPS_CSU_BASEADDR + SAC_UNLOCK_CONS_REG_OFFSET, SAC_UNLOCK); 

}

u32 FmshFsbl_SetSacMode(u32 Mode)
{
  
    u32 CfgReg;
    u32 Status = FMSH_SUCCESS;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET);
    CfgReg &= ~SAC_DATA_OP_MASK; 
    CfgReg |= Mode;/*config AES-GCM mode*/
    WriteReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET,CfgReg);
    CfgReg = ReadReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET);
    CfgReg &= SAC_DATA_OP_MASK;
    if (CfgReg != Mode)
    {
       Status = FMSH_FAILURE ;  
    }
 
    return Status;
}



u32 FmshFsbl_SetSacDataSwap(u32 Mode)
{
  
    u32 CfgReg;
    u32 Status = FMSH_SUCCESS;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET);
    CfgReg &= ~SAC_DATA_SWAP_MASK; 
    CfgReg |= Mode;/*config AES-GCM mode*/
    WriteReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET,CfgReg);
    if (ReadReg(FPS_CSU_BASEADDR + SAC_CFG_REG_OFFSET)& SAC_DATA_SWAP_MASK != Mode)
    {
        Status = FMSH_FAILURE ;
    }
    
    return Status;
    
}

void FmshFsbl_ByteSwap(u8* KeyPtr)
{
  
    u8 Temp1;
    Temp1 = * KeyPtr;
    *KeyPtr = *(KeyPtr+3U);
    *(KeyPtr+3U) = Temp1;
    Temp1 = * (KeyPtr+1U);
    * (KeyPtr+1U) = *(KeyPtr+2U);
    *(KeyPtr+2U) = Temp1;

}

void FmshFsbl_SacAesSwitch(u32 Flag)
{

  u32 CfgReg = ReadReg(FPS_CSU_BASEADDR + SAC_CTRL_REG_OFFSET);
  CfgReg &= ~SAC_AES_EN_MASK;
  if(Flag == SAC_AES_EN_FLAG)
  { 
    CfgReg |= SAC_AES_EN;
  }
  WriteReg(FPS_CSU_BASEADDR + SAC_CTRL_REG_OFFSET, CfgReg); 

}

void FmshFsbl_OpenCfgLevelShifter(void)
{
   FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);// unlock SLCR
   FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x83C, 0xF);//open the cfg-level-shifter
   FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x004, 0xDF0D767B);/* relock SLCR */
}

void FmshFsbl_CloseUsrLevelShifter(void)
{
   FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);// unlock SLCR
   FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x838, 0x0);//close the usr-level-shifter
   FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x004, 0xDF0D767B);/* relock SLCR */
}

u8 FmshFsbl_IsBitDone(void)
{
   u32 Reg = ReadReg(FPS_CSU_BASEADDR + 0x18);
   if( (Reg&0x0200) == 0x0)
     return FALSE;
   else
     return TRUE;
}








