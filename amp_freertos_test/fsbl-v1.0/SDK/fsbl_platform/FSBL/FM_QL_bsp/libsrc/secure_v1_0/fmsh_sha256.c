/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sha256.c
*
* This file contains fmsh_sha.h.
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
static void ShaReset()
{
    u32 RstReg = ReadReg(FPS_CSU_BASEADDR + SHA2_COMPUTE_RST_REG); 
    WriteReg(FPS_CSU_BASEADDR + SHA2_COMPUTE_RST_REG, RstReg & ~0x00000001);
    WriteReg(FPS_CSU_BASEADDR + SHA2_COMPUTE_RST_REG, RstReg | 0x00000001);
    WriteReg(FPS_CSU_BASEADDR + SHA2_COMPUTE_RST_REG, RstReg & ~0x00000001);
}

u8 FmshFsbl_sha256(u8* Message, u32 MessageByteLen, u8* Digest)
{
    u32 Status;
    u32 PaddingByteLen;
   
    u8 BackUpBuff[72];
//    u8* MessagePtr = (u8*)(A7_SECURITY_RAM_BASE);
   
    FmshFsbl_SacInit(0);
    FmshFsbl_SetSacMode(SAC_SHA256_MODE);
  
    Status = FmshFsbl_SetSacDataSwap(SAC_RX_FIFO_DATA_BYTE_SWAP );
    if (Status != FMSH_SUCCESS)
    {
       return Status;
    }
    ShaReset();
   
    PaddingByteLen = MessageByteLen % SHA_BLOCK_SIZE < SHA_PADDING_BOUNDARY_SIZE? \
        SHA_BLOCK_SIZE - MessageByteLen % SHA_BLOCK_SIZE :\
          SHA_BLOCK_SIZE*2 - MessageByteLen % SHA_BLOCK_SIZE;
   

    /*copy in information to A7 sram*/
   // memcpy(MessagePtr,Message,MessageByteLen);
    
    /*save data zone */
    memcpy(BackUpBuff,(u8*)((u32)Message + MessageByteLen),PaddingByteLen);
    memset((u8*)((u32)Message + MessageByteLen),0U,PaddingByteLen );

    Fmsh_Out8((u32)Message  + MessageByteLen, SHA_FILL_DATA);
     
    WriteReg((u32)Message + MessageByteLen + PaddingByteLen -4U, MessageByteLen*8U);
   
    FmshFsbl_ByteSwap((u8*)((u32)Message + MessageByteLen + PaddingByteLen -4U ));
      
    // CsuInitiateDma((u32)0x20000, (u32) Digest, MessageByteLen/4U, MessageByteLen/4U);
    FmshFsbl_CsuInitiateDma((u32)Message - SPU_APU_ADDRESS_OFFSET, (u32) Digest, (MessageByteLen + PaddingByteLen)/4U, NULL);
     
    FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
    FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    

    *(u32*) Digest = ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG0_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG1_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG2_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG3_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG4_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG5_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG6_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG7_OFFSET );
    FmshFsbl_ByteSwap(Digest);

    memcpy((u8*)((u32)Message + MessageByteLen),BackUpBuff,PaddingByteLen); 

    return PUF_SHA_OK;
}    

u8 FmshFsbl_part_sha256(u8* Message, u32 MessageByteLen, u8* Digest)
{
   
    FmshFsbl_CsuInitiateDma((u32)Message - SPU_APU_ADDRESS_OFFSET, (u32) Digest, MessageByteLen/4U, NULL);
     
    FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
    FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    
    return PUF_SHA_OK;
}

u8 tmpMessage[1024]={0};
u8 padMessage[128]={0};
u8 FmshFsbl_LinearBurstSha256(u8* Message, u32 MessageByteLen, u8* Digest)
{
    u32 i=0;
    u32 indx=0;
    u32 Status;
    u32 PaddingByteLen;
    
    
    PaddingByteLen = MessageByteLen % SHA_BLOCK_SIZE < SHA_PADDING_BOUNDARY_SIZE? \
        SHA_BLOCK_SIZE - MessageByteLen % SHA_BLOCK_SIZE :\
          SHA_BLOCK_SIZE*2 - MessageByteLen % SHA_BLOCK_SIZE;
    
    FmshFsbl_SacInit(0);
    FmshFsbl_SetSacMode(SAC_SHA256_MODE);
  
    Status = FmshFsbl_SetSacDataSwap(SAC_RX_FIFO_DATA_BYTE_SWAP);
    if (Status != FMSH_SUCCESS)
    {
       return Status;
    }
    ShaReset();
    
   for(i=0;i<MessageByteLen;i++)
   {
      tmpMessage[indx++]=*(Message+i);
      if(indx==1024)
      {  
        indx=0;
        FmshFsbl_part_sha256(tmpMessage,1024,Digest);
      }
   }
   FmshFsbl_part_sha256(tmpMessage,indx,Digest);
   
   indx=0;
   padMessage[indx++]=SHA_FILL_DATA;
   for(i=1;i<PaddingByteLen-4;i++)
   {
      padMessage[indx++]=0;
   }
   padMessage[indx++]=((MessageByteLen*8U)>>24)&0xff;
   padMessage[indx++]=((MessageByteLen*8U)>>16)&0xff;
   padMessage[indx++]=((MessageByteLen*8U)>>8)&0xff;
   padMessage[indx++]=(MessageByteLen*8U)&0xff;
   
   FmshFsbl_part_sha256(padMessage,indx,Digest);
   
   *(u32*) Digest = ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG0_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG1_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG2_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG3_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG4_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG5_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG6_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG7_OFFSET );
    FmshFsbl_ByteSwap(Digest);
   
   return 0;
  
}
/*
 *for nand sd read use copy func..
*/
u32 s32_tmpMessage[1024]={0};
u8 FmshFsbl_NoneLinearBurstSha256(u8* Message, u32 MessageByteLen, u8* Digest)
{
    u32 i=0;
    u32 indx=0;
    u32 Status=FMSH_SUCCESS;
    u32 PaddingByteLen;
    u32 ScrAddr=(u32)(Message);
    //u32 *DestPtr=s32_tmpMessage;
    
    PaddingByteLen = MessageByteLen % SHA_BLOCK_SIZE < SHA_PADDING_BOUNDARY_SIZE? \
        SHA_BLOCK_SIZE - MessageByteLen % SHA_BLOCK_SIZE :\
          SHA_BLOCK_SIZE*2 - MessageByteLen % SHA_BLOCK_SIZE;
    
    FmshFsbl_SacInit(0);
    FmshFsbl_SetSacMode(SAC_SHA256_MODE);
  
    Status = FmshFsbl_SetSacDataSwap(SAC_RX_FIFO_DATA_BYTE_SWAP);
    if (Status != FMSH_SUCCESS)
    {
       return Status;
    }
    ShaReset();
      
   for(i=0;i<(MessageByteLen/1024);i++)
   {
      Status = BootInstance.DeviceOps.DeviceCopy(ScrAddr+1024*i,(u32)tmpMessage, 1024);
      FmshFsbl_part_sha256(tmpMessage,1024,Digest);
   }
   if(MessageByteLen%1024!=0)
   {
      Status = BootInstance.DeviceOps.DeviceCopy(ScrAddr+1024*i,(u32)tmpMessage, (MessageByteLen%1024));
      FmshFsbl_part_sha256(tmpMessage,MessageByteLen%1024,Digest);
   }
   
   indx=0;
   padMessage[indx++]=SHA_FILL_DATA;
   for(i=1;i<PaddingByteLen-4;i++)
   {
      padMessage[indx++]=0;
   }
   padMessage[indx++]=((MessageByteLen*8U)>>24)&0xff;
   padMessage[indx++]=((MessageByteLen*8U)>>16)&0xff;
   padMessage[indx++]=((MessageByteLen*8U)>>8)&0xff;
   padMessage[indx++]=(MessageByteLen*8U)&0xff;
   
   FmshFsbl_part_sha256(padMessage,indx,Digest);
   
   *(u32*) Digest = ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG0_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG1_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG2_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG3_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG4_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG5_OFFSET );
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG6_OFFSET ); 
    FmshFsbl_ByteSwap(Digest);
      Digest+=4;
    *(u32*)(Digest)= ReadReg(FPS_CSU_BASEADDR + SHA2_DIGEST_REG7_OFFSET );
    FmshFsbl_ByteSwap(Digest);
  
    return 0;
}

u8 FmshFsbl_SubgroupSha256(u8* Message, u32 MessageByteLen, u8* Digest)
{
    u8 Status=0;
    //QSPI NOR
    if( (BootInstance.BootMode==QSPI_FLASH) ||   \
              (BootInstance.BootMode==NOR_FLASH) )
    {
      Status=FmshFsbl_LinearBurstSha256(Message, MessageByteLen, Digest);
    }
    //SD NAND
    else 
    {
      Status=FmshFsbl_NoneLinearBurstSha256(Message, MessageByteLen, Digest);
    }  
 
    
    return Status;

}
