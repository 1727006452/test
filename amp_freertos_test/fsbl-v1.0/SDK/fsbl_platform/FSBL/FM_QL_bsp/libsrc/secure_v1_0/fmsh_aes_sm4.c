/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_aes_sm4.h
*
* This file contains header fmsh_aes_sm4.h
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

/************************** Variable Definitions *****************************/
extern BootPs BootInstance;
u8 SecureHeaderIv[SECURE_IV_SIZE];
//EncData   EncryptionData;

 
/****************************************************************************
*
* This function is used to set mode of the Encryption engine.
*
* @param   AesCHMode  --ECB --CTR --MULTH
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
static u32 SetAesCHMode(u32 AesCHMode)
{
    u32 CfgReg;
    u32 Status = FMSH_SUCCESS;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET);
    CfgReg &= ~AES_CHMODE_MASK; 
    CfgReg |= AesCHMode;/*config AES-GCM mode*/
    WriteReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET,CfgReg);
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET);
    if ((CfgReg & AES_CHMODE_MASK) != AesCHMode)
    {
        Status = FMSH_FAILURE ;  
    }
    
    return Status;
}

/****************************************************************************
*
* This function is used to set mode of the Encryption engine for encryption 
* and decryption.
*
* @param   Mode  --ENCRYPTION --DECRYPTION
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
static u32 SetEncDecMode(u32 Mode)
{
    u32 CfgReg;
    u32 Status = FMSH_SUCCESS;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET);
    CfgReg &= ~ENC_DEC_MODE_MASK; 
    CfgReg |= Mode;/*config AES-GCM mode*/
    WriteReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET,CfgReg);
    if ((ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET)& ENC_DEC_MODE_MASK) != Mode)
    {
        Status = FMSH_FAILURE ;
    }
    
    return Status;
}
/****************************************************************************
*
* This function is used to set algorithm of the Encryption engine for encryption 
* and decryption.
*
* @param   Mode  --AES_MODE --SM4_MODE
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
static u32 SetAlgMode(u32 Mode)
{
    u32 CfgReg;
    u32 Status = FMSH_SUCCESS;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET);
    CfgReg &= ~ALG_MODE_MASK; 
    CfgReg |= Mode;/*config AES-GCM mode*/
    WriteReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET,CfgReg);
    if ((ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET)& ALG_MODE_MASK) != Mode)
    {
       Status = FMSH_FAILURE ;
    }
    
    return Status;
}
/****************************************************************************
*
* This function is used to load the iv into the Encryption engine.
*
* @param   None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
static void Secure_AesIvLoad(void)
{

	/* Trig loading of key. */
	WriteReg(FPS_CSU_BASEADDR + SECURE_CSU_AES_KEY_IV_LOAD_OFFSET, SECURE_CSU_AES_IV_LOAD);

	/* Wait for AES key loading.*/
//	Secure_AesWaitIvLoad();
}

/****************************************************************************
*
* This function is used to enable the Encryption engine. 
*
* @param   None.
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
static u32 AesEn(void)
{
    u32 CfgReg;
    u32 Status;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET);
    CfgReg |= AES_EN; 
    WriteReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET,CfgReg);
    if((ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET)& AES_EN) !=  AES_EN)
    {
       Status = FMSH_FAILURE ;
       return Status;
    } 
    return FMSH_SUCCESS;
}
/****************************************************************************
*
* This function is used to disable the Encryption engine. 
*
* @param   None.
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
static u32 AesDis(void)
{
    u32 CfgReg;
    u32 Status;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET);
    CfgReg &= AES_DIS; 
    WriteReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET,CfgReg);
    if ((ReadReg(FPS_CSU_BASEADDR + AES_CR_ADDR_OFFSET) & AES_EN) != 0)
    {
        Status = FMSH_FAILURE ;
        return Status;
    }
    return FMSH_SUCCESS;
}
/*****************************************************************************
 *
 * Configures and load AES key from selected key source.
 *
 * @param	KeySel  
 *              --SECURE_CSU_AES_KEY_SRC_DEV
 *              --SECURE_CSU_AES_KEY_SRC_KUP
 *              --SECURE_CSU_AES_KEY_SRC_MULT_H.
 *
 * @return	None.
 *
 * @note	None.
 *
 ******************************************************************************/
static void Secure_AesKeySelNLoad(u32 KeySel)
{
        
  if(KeySel == SECURE_CSU_AES_KEY_SRC_DEV)
  {
    WriteReg(FPS_CSU_BASEADDR + SECURE_CSU_AES_KEY_SRC_OFFSET, SECURE_CSU_AES_KEY_SRC_DEV);
  }
  else if(KeySel == SECURE_CSU_AES_KEY_SRC_KUP)
       {
          WriteReg(FPS_CSU_BASEADDR + SECURE_CSU_AES_KEY_SRC_OFFSET,SECURE_CSU_AES_KEY_SRC_KUP);
       }
       else
       {
          WriteReg(FPS_CSU_BASEADDR + SECURE_CSU_AES_KEY_SRC_OFFSET,SECURE_CSU_AES_KEY_SRC_MULT_H);
       }
   /* Trig loading of key. */
   WriteReg(FPS_CSU_BASEADDR + SECURE_CSU_AES_KEY_IV_LOAD_OFFSET,SECURE_CSU_AES_KEY_LOAD);

	/* Wait for AES key loading.*/
//	Secure_AesWaitKeyLoad();
}

/****************************************************************************
*
* This function is used to set the iv. 
*
* @param   Iv is pointer to IV buffer.
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
u32 SetSacIv(u8* Iv)
{
    u32 * IvPtr = (u32*) (Iv);
    WriteReg(FPS_CSU_BASEADDR + SAC_AES_IVR3_ADDR_OFFSET,IvPtr[3]);
    WriteReg(FPS_CSU_BASEADDR + SAC_AES_IVR2_ADDR_OFFSET,IvPtr[2]);
    WriteReg(FPS_CSU_BASEADDR + SAC_AES_IVR1_ADDR_OFFSET,IvPtr[1]);
    WriteReg(FPS_CSU_BASEADDR + SAC_AES_IVR0_ADDR_OFFSET,IvPtr[0]);
    Secure_AesIvLoad();
    return FMSH_SUCCESS;   
} 

/****************************************************************************
*
* This function is used to get the iv. 
*
* @param   Iv is pointer to IV buffer.
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
u32 GetSacIv(u8* Iv)
{
    
    u32 * IvPtr = (u32*) (Iv);
 //   WriteReg(FPS_CSU_BASEADDR + SAC_AES_IVR3_ADDR_OFFSET,IvPtr[3]);
    *IvPtr = ReadReg(FPS_CSU_BASEADDR + SAC_AES_IVR3_ADDR_OFFSET);
    FmshFsbl_ByteSwap((u8*)IvPtr);
    IvPtr++;
    *IvPtr = ReadReg(FPS_CSU_BASEADDR + SAC_AES_IVR2_ADDR_OFFSET);
     FmshFsbl_ByteSwap((u8*)IvPtr);
    IvPtr++;
    *IvPtr = ReadReg(FPS_CSU_BASEADDR + SAC_AES_IVR1_ADDR_OFFSET);
    FmshFsbl_ByteSwap((u8*)IvPtr);
    
    return FMSH_SUCCESS;   
} 

u32 IvToY1(u8* Iv,u8* Y1)
{
    u32 cnt;  
    for(cnt=0;cnt<SECURE_IV_SIZE;cnt++)
    {
         Y1[15U-cnt] = Iv[cnt];
    }
        
    Y1[0] = 2U;
    Y1[1] = 0U;
    Y1[2] = 0U;
    Y1[3] = 0U;

    return FMSH_SUCCESS;
}

static u32 IvToY0(u8* Iv,u8* Y0)
{
    u32 cnt;  
    for(cnt=0;cnt<SECURE_IV_SIZE;cnt++)
    {
        Y0[cnt] = Iv[cnt];
    }
        
    Y0[12] = 0U;
    Y0[13] = 0U;
    Y0[14] = 0U;
    Y0[15] = 1U;

    return FMSH_SUCCESS;
}


static u32 SetAesH(u8* H)
{
    u32* KeyPtr =(u32*) (H);
      
    WriteReg(FPS_CSU_BASEADDR+SAC_AES_MULTH_H3_ADDR_OFFSET,KeyPtr[0]);
    WriteReg(FPS_CSU_BASEADDR+SAC_AES_MULTH_H2_ADDR_OFFSET,KeyPtr[1]);
    WriteReg(FPS_CSU_BASEADDR+SAC_AES_MULTH_H1_ADDR_OFFSET,KeyPtr[2]);
    WriteReg(FPS_CSU_BASEADDR+SAC_AES_MULTH_H0_ADDR_OFFSET,KeyPtr[3]);
    Secure_AesKeySelNLoad(SECURE_CSU_AES_KEY_SRC_MULT_H);
    
    return FMSH_SUCCESS; 
}
  
/****************************************************************************
*
* This function is used to set dec_flag_reg. 
*
* @param   DecFlag  .
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
static u32 SetDecFlag(u32 DecFlag) 
{

    u32 CfgReg;
    u32 Status;
    CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_DEC_FLAG_ADDR);
    CfgReg &= ~AES_DEC_FLAG_MASK; 
    CfgReg |= DecFlag;/*config AES-GCM mode*/
    WriteReg(FPS_CSU_BASEADDR + AES_DEC_FLAG_ADDR,CfgReg);
    if ((ReadReg(FPS_CSU_BASEADDR + AES_DEC_FLAG_ADDR)& AES_DEC_FLAG_MASK) != DecFlag)
    {
          Status = FMSH_FAILURE ;
          return Status;
    }
    return FMSH_SUCCESS;
}
/****************************************************************************
*
* This function calculate the reslut of input data with the relate algorithm.
*
* @param	Alg is ALG_AES or ALG_SM4.
* @param	In is pointer to the source data.
* @param	InByteLen is the length of source data.
* @param	Out is pointer to the reslut data.
* @param	KeySel is key source mode.
* @param	EncDec is  GCM_ENCRYPT or GCM_DECRYPT.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
*****************************************************************************/
static u32 AesSm4(u8 Alg, u8* In, u32 InByteLen, u8* Out, u32 KeySel, u32 EncDec)
{
    u32 Status = FMSH_SUCCESS;
    u32 PaddingByteLen = 0;
    u8 BackUpBuff[MAXIMUM_GCM_PADDING_SIZE];
    u8 tmp[64];
    u8* InPtr = tmp;
    u8* OutPtr = (u8*)(tmp + InByteLen + MAXIMUM_GCM_PADDING_SIZE);
   
    Status =  FmshFsbl_SetSacMode(SAC_AES_GCM_MODE);
    Status = SetAesCHMode(ECB_MODE);
    if (Status != FMSH_SUCCESS)
    {
        return Status;
    }
    if(Alg == ALG_AES )
    {
        Status = SetAlgMode(AES_MODE);  
        
        if (Status != FMSH_SUCCESS)
        {
            return Status;
        }
    }
    else if(Alg == ALG_SM4)
    {
        Status = SetAlgMode(SM4_MODE);

    }
    else
    {
        Status = ALGORITHMIC_MISMATCH;
        return Status;   
    }
   
    if(EncDec == GCM_ENCRYPT)
    {    
        Status = SetEncDecMode(ENCRYPTION_MODE);
    }
    else if (EncDec == GCM_DECRYPT)
    {
        Status = SetEncDecMode(DECRYPTION_MODE);  
    }
    else
    {
      Status = ENC_DEC_MODE_MISMATCH;
      return Status;
    }   
    
    Status = FmshFsbl_SetSacDataSwap(SAC_RX_FIFO_DATA_BYTE_SWAP);
    if (Status != FMSH_SUCCESS)
    {
        return Status;
    }
    
    memcpy(InPtr,In,InByteLen);
    
    if(InByteLen%GCM_BLK_SIZE != 0)
    {
        PaddingByteLen = GCM_BLK_SIZE - InByteLen%GCM_BLK_SIZE;
        InByteLen = InByteLen + PaddingByteLen;
        memcpy(BackUpBuff,(u8*)((u32)InPtr+InByteLen),PaddingByteLen);
        memset((u8*)((u32)InPtr+InByteLen),0U,PaddingByteLen);
    }  
    
    Secure_AesKeySelNLoad(KeySel);
    
    Status = AesEn();
    if (Status != FMSH_SUCCESS)
    {
        return Status;
    }
  
    FmshFsbl_CsuInitiateDma((u32) InPtr + SPU_APU_ADDRESS_OFFSET, (u32) OutPtr + SPU_APU_ADDRESS_OFFSET, (u32) InByteLen/4, (u32) InByteLen/4);
    
    FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT);  
    FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    
    if(PaddingByteLen != 0)
    {
       memcpy((u8*)((u32)InPtr+InByteLen),BackUpBuff,PaddingByteLen);
    }
    memcpy(Out,OutPtr,InByteLen - PaddingByteLen);
    Status = AesDis();   
    if (Status != FMSH_SUCCESS) 
    {
        return Status;
    }
    return Status; 
 
 }


/****************************************************************************
*
* This function calculate the tag of input data with the relate algorithm 
* for nolinear memory.
*
* @param	Alg is ALG_AES or ALG_SM4.
* @param	In is pointer to the source data.
* @param	InByteLen is the length of source data.
* @param	KeySel is key source mode.
* @param	Iv is pointer to the IV data.
* @param	Tag is pointer to the TAG data.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
*****************************************************************************/
u32 PlCalcTagNoLinear(u8 Alg, const u8* In, u32 InByteLen, u32 KeySel, u8*Iv, u8* Tag)
{
    u32 Status = GCM_OK;
    u8 PlainText[16] ={0};
    u8 H[SECURE_MULTI_H_SIZE];
    u8 Y0[SECURE_Y_SIZE];
    u8 EncY0[SECURE_Y_SIZE];
    u32 cnt;
    u32 PaddingByteLen = 0;
    u32 GHash[4];
    u8 GcmTag[SECURE_GCM_TAG_SIZE];
    u32 *CT_Y0 = (u32 *)EncY0;
    u32 i,j=0;
    
    /*calculate H*/
    Status = AesSm4(Alg, PlainText, sizeof(PlainText), H, KeySel, GCM_ENCRYPT);

    /*calculate encrypted Y0*/
    IvToY0(Iv,Y0);
    Status = AesSm4(Alg, Y0, sizeof(Y0), EncY0, KeySel, GCM_ENCRYPT);

    /* IV register clear */ 
    SetSacIv(PlainText);
    Status =  FmshFsbl_SetSacMode(SAC_MULTH_MODE); 
    if (Status != FMSH_SUCCESS)
    {
        return Status;
    }
    
    Status = SetAesCHMode(MULTIH_MODE);
    if (Status != FMSH_SUCCESS) 
    {
        return Status;
    }
    
    Status = SetAesH(H);
    
    FmshFsbl_SetSacDataSwap(SAC_RX_FIFO_DATA_BYTE_SWAP);

    if(InByteLen % GCM_BLK_SIZE != 0)
    {
        PaddingByteLen= GCM_BLK_SIZE - InByteLen % GCM_BLK_SIZE;
    }  

    u8 PadData[32]={0};
    for(i=0;i<PaddingByteLen+LEN_A + LEN_C-4;i++)
      PadData[i]=0;
    
    PadData[i++]=(InByteLen*8U)/256/256/256;
    PadData[i++]=(InByteLen*8U)/256/256;
    PadData[i++]=(InByteLen*8U)/256;
    PadData[i++]=(InByteLen*8U)%256;
  
    Status = AesEn();
    if (Status != FMSH_SUCCESS) 
    {
        return Status;
    }
    
    //u32 CfgReg = ReadReg(FPS_CSU_BASEADDR + AES_DEC_FLAG_ADDR);
    //SetDecFlag(0);
    for(j=0;j<InByteLen/TMP_PL_BUF_LEN;j++)
    {
         BootInstance.DeviceOps.DeviceCopy((u32)In+TMP_PL_BUF_LEN*j,(u32)tmpPlBuffer,TMP_PL_BUF_LEN );
         FmshFsbl_CsuInitiateDma((u32)tmpPlBuffer, (u32) GHash,TMP_PL_BUF_LEN/4, TMP_PL_BUF_LEN/4);
         FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
         FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    }
    if(InByteLen%TMP_PL_BUF_LEN!=0)
    {
         BootInstance.DeviceOps.DeviceCopy((u32)In+TMP_PL_BUF_LEN*j,(u32)tmpPlBuffer,InByteLen%TMP_PL_BUF_LEN );
         FmshFsbl_CsuInitiateDma((u32)tmpPlBuffer, (u32) GHash,(InByteLen%TMP_PL_BUF_LEN)/4, (InByteLen%TMP_PL_BUF_LEN)/4);
         FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
         FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    }
    //SetDecFlag(CfgReg);
    FmshFsbl_CsuInitiateDma((u32)PadData, (u32) GHash,i/4, i/4);
    
    FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
    FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    
    GHash[0] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG0_ADDR_OFFSET);
    GHash[1] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG1_ADDR_OFFSET);
    GHash[2] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG2_ADDR_OFFSET);
    GHash[3] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG3_ADDR_OFFSET);
    
    WriteReg((u32)GcmTag, GHash[0]^CT_Y0[3]);
    WriteReg((u32)GcmTag + 4U, GHash[1]^CT_Y0[2]);
    WriteReg((u32)GcmTag + 8U,GHash[2]^CT_Y0[1]);
    WriteReg((u32)GcmTag + 12U, GHash[3]^CT_Y0[0]);
     
    for (cnt=0;cnt<SECURE_GCM_TAG_SIZE;cnt++)
    {
        *Tag++ = GcmTag[SECURE_GCM_TAG_SIZE-1U-cnt];
    }

    Status = AesDis();   
    if (Status != FMSH_SUCCESS)
    {
        Status = GCM_ERROR;
        return Status;
    }
     
    return Status; 
 }
/****************************************************************************
*
* This function calculate the tag of input data with the relate algorithm 
* for linear memory.
*
* @param	Alg is ALG_AES or ALG_SM4.
* @param	In is pointer to the source data.
* @param	InByteLen is the length of source data.
* @param	KeySel is key source mode.
* @param	Iv is pointer to the IV data.
* @param	Tag is pointer to the TAG data.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
*****************************************************************************/
u32 AesSm4GenTag(u8 Alg, const u8* In, u32 InByteLen, u32 KeySel, u8*Iv, u8* Tag)
{
    u32 Status = GCM_OK;
    u8 PlainText[16] ={0};
    u8 H[SECURE_MULTI_H_SIZE];
    u8 Y0[SECURE_Y_SIZE];
    u8 EncY0[SECURE_Y_SIZE];
    u32 cnt;
    u32 PaddingByteLen = 0;
    u8 BackUpBuff[32];
    u32 GHash[4];
    u8 GcmTag[SECURE_GCM_TAG_SIZE];
    u32 *CT_Y0 = (u32 *)EncY0;

    /*calculate H*/
    Status = AesSm4(Alg, PlainText, sizeof(PlainText), H, KeySel, GCM_ENCRYPT);

    /*calculate encrypted Y0*/
    IvToY0(Iv,Y0);
    Status = AesSm4(Alg, Y0, sizeof(Y0), EncY0, KeySel, GCM_ENCRYPT);

    /* IV register clear */ 
    SetSacIv(PlainText);
    Status =  FmshFsbl_SetSacMode(SAC_MULTH_MODE); 
  
    if (Status != FMSH_SUCCESS) 
    {
      return Status;
    }
    
    Status = SetAesCHMode(MULTIH_MODE);
    if (Status != FMSH_SUCCESS) 
    {
       return Status;
    }
    
    Status = SetAesH(H);
    
    FmshFsbl_SetSacDataSwap(SAC_RX_FIFO_DATA_BYTE_SWAP);

    if(InByteLen % GCM_BLK_SIZE != 0)
    {
       PaddingByteLen= GCM_BLK_SIZE - InByteLen % GCM_BLK_SIZE;
    }  
    /*save data zone */
    memcpy(BackUpBuff,(u8*)((u32)In+InByteLen),PaddingByteLen + LEN_A + LEN_C);
    memset((u8*)((u32)In+InByteLen),0U,PaddingByteLen + LEN_A + LEN_C);
    
    WriteReg((u32)In+InByteLen+ PaddingByteLen + LEN_A + LEN_C -4U, InByteLen*8U);
    FmshFsbl_ByteSwap((u8*)(u32)In+InByteLen+ PaddingByteLen + LEN_A + LEN_C -4U) ;
    
    Status = AesEn();
    if (Status != FMSH_SUCCESS) 
    {
       return Status;
    }
 
    FmshFsbl_CsuInitiateDma((u32) In - SPU_APU_ADDRESS_OFFSET, (u32) GHash, (InByteLen+ PaddingByteLen + LEN_A + LEN_C)/4, 
                   (InByteLen+ PaddingByteLen + LEN_A + LEN_C)/4 );
     
    FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
    FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    
    memcpy((u8*)((u32)In+InByteLen),BackUpBuff,PaddingByteLen + LEN_A + LEN_C);
     
    GHash[0] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG0_ADDR_OFFSET);
    GHash[1] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG1_ADDR_OFFSET);
    GHash[2] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG2_ADDR_OFFSET);
    GHash[3] = ReadReg(FPS_CSU_BASEADDR + AES_MULTH_TAG3_ADDR_OFFSET);
     
    WriteReg((u32)GcmTag, GHash[0]^CT_Y0[3]);
    WriteReg((u32)GcmTag + 4U, GHash[1]^CT_Y0[2]);
    WriteReg((u32)GcmTag + 8U,GHash[2]^CT_Y0[1]);
    WriteReg((u32)GcmTag + 12U, GHash[3]^CT_Y0[0]);
     
    for (cnt=0;cnt<SECURE_GCM_TAG_SIZE;cnt++)
    {
        *Tag++ = GcmTag[SECURE_GCM_TAG_SIZE-1U-cnt];
    }

    Status = AesDis();   
    if (Status != FMSH_SUCCESS)
    {
        Status = GCM_ERROR;
        return Status;
    }
     
    return Status; 

 }

static u32 AesSm4Cnt(u8 Alg,u32 DecFlag, const u8* In, u32 InByteLen, u8* Out, u32 KeySel, u8* Iv,u32 EncDec)
{
    u32 Status = GCM_OK;
    u32 PaddingByteLen = 0;
    u8 BackUpBuff[16];
    u8 Y1[SECURE_Y_SIZE];
    u32 DstAddr;
    if((u32)Out == SECURE_DESTINATION_PCAP_ADDR)
    {
        DstAddr = (u32)Out;
    
        Status =  FmshFsbl_SetSacMode(SAC_SECURE_BITSTREAM_DOWNLOAD_MODE);

    }
    else
    {
        DstAddr = (u32) Out - SPU_APU_ADDRESS_OFFSET;  
        Status =  FmshFsbl_SetSacMode(SAC_AES_GCM_MODE);
    }
    Status = SetAesCHMode(CTR_MODE);
    
    if(Alg == ALG_AES )
    {
        Status = SetAlgMode(AES_MODE);  
        
    }
    else if(Alg == ALG_SM4)
    {
        Status = SetAlgMode(SM4_MODE);

    }
    else
    {
        Status = ALGORITHMIC_MISMATCH;
   //  printf ("Unsupported Algorithmic \r\n");
        return Status;
    
    }
    
    if(EncDec == GCM_ENCRYPT)
    {    
        Status = SetEncDecMode(ENCRYPTION_MODE);
    }
    else if(EncDec == GCM_DECRYPT)
    {
        Status = SetEncDecMode(DECRYPTION_MODE);  
    
    }else
    {
        Status = FMSH_FAILURE;
    }   
 
   Secure_AesKeySelNLoad(KeySel);
  
   FmshFsbl_SetSacDataSwap(SAC_TX_FIFO_DATA_BYTE_SWAP | SAC_RX_FIFO_DATA_BYTE_SWAP);
    
    if (Status != FMSH_SUCCESS) {
          // printf ("Set AES_GCM key  failure \r\n");;
          return Status;
      }
   

    IvToY1(Iv,Y1);
    Status = SetSacIv(Y1);

    if(InByteLen % GCM_BLK_SIZE != 0)
    {
        PaddingByteLen = GCM_BLK_SIZE - InByteLen%GCM_BLK_SIZE;
        memcpy(BackUpBuff,(u8*)((u32)In+InByteLen),PaddingByteLen);
        memset((u8*)((u32)In+InByteLen),0U,PaddingByteLen);
    
    }
    
    SetDecFlag(DecFlag);
    Status = AesEn();
     if (Status != FMSH_SUCCESS) {
          // printf ("Set AES_GCM enable  failure \r\n");;
          return Status;
      }
 
 
    FmshFsbl_CsuInitiateDma((u32) In - SPU_APU_ADDRESS_OFFSET, DstAddr,\
    (InByteLen+PaddingByteLen)/4, (InByteLen+PaddingByteLen -AES_GCM_KEY_IV_SIZE)/4);
    FmshFsbl_CsuDmaPollDone(IXR_DMA_DONE_MASK, MAX_COUNT); 
    FmshFsbl_CsuDmaPollDone(IXR_D_P_DONE_MASK, MAX_COUNT);
    if(PaddingByteLen != 0)
    {
    memcpy((u8*)((u32)In+InByteLen),BackUpBuff,PaddingByteLen);
    }
    
    
    Status = AesDis();   
     if (Status != FMSH_SUCCESS) {
          Status = GCM_ERROR;
          // printf ("GCM_ERROR \r\n");;
          return Status;
      }
     
 return Status; 
 
 
 }

// GCM����
//   1.����SAC modeΪebc mode
//   2.��KEY ��128 bit 0���ܵõ�H;
// 3����KEY��Y0��IV��||31bit0,1)���ܣ�
//4.����SAC modeΪCTR mode
//5.��Key�����ļ��ܣ�IV��ʼֵΪY1= Y0+1;
//6.����SAC modeΪmulth mode
//7.��H��IV��Ϊ0�����룬ʹ��AES�������İ������Ghash��
//8.GHASH����ܺ��Y0���õ�GCMtag��
//
//
//
//GCM����
//1.����SAC modeΪebc mode
//2.��KEY ��128 bit 0���ܵõ�H;
//3.��KEY��Y0��IV��||31bit0,1)���ܣ�
//
//4.����SAC modeΪmulth mode
//5.��H��IV��Ϊ0�����룬ʹ��AES�������İ������Ghash��
//
//6.GHASH����ܺ��Y0���õ�GCMtag��
//7.�������GHASH��boot image��tag���бȽϡ�
//7.����SAC modeΪCTR mode����
//8.��Key�����Ľ��ܣ�IV��ʼֵΪY1= Y0+1;

 u32 AesSm4GcmDec(u8 Alg,u32 DecFlag, const u8* In, u32 InByteLen, u8* Out, u32 KeySel, u8* Iv,const u8* Tag)
{
    u8  Status;
    u32 cnt;
 //   u8 Y1[16];
    u8 GenTag[SECURE_GCM_TAG_SIZE];
  

    FmshFsbl_SacInit(SAC_AES_EN_FLAG);
    Status = AesSm4GenTag(Alg, In, InByteLen, KeySel, Iv, GenTag);
//    if(Status != GCM_OK)
//    {
//      // printf("AES MULTH MODE GEN TAG FAILURE!\r\n");
//      return Status;
//    } 
    
    LOG_OUT (DEBUG_INFO, "Ciphertext GCM Tag is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, Tag, 16);
    LOG_OUT (DEBUG_INFO, "Calculated Ciphertext GCM Tag is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, GenTag, 16);
     
    /*compare generated tag and received tag */
    for(cnt=0;cnt<SECURE_GCM_TAG_SIZE;cnt++)
    {
      
        if (GenTag[cnt] != *Tag++)
        {
           
          LOG_OUT(DEBUG_INFO,"GCM Tag is not Matched \r\n");
          
          Status = GCM_ERROR;
           return Status;
        }
    }
    
    LOG_OUT(DEBUG_INFO,"GCM Tag is Matched, Starting Decryption...... \r\n");
//    SetDecFlag(DecFlag);
    Status = AesSm4Cnt(Alg,DecFlag, In, InByteLen,Out, KeySel,Iv, GCM_DECRYPT);
    SetDecFlag(0x0);

    return Status;

}








/*

Key rolling ����
1.����key rolling �Ĵ���������ΪKey rolling ģʽ��
1.ѡ��device key source,trigger loading.
2.��boot image�е�secure header IV  copy����,д�� AES iv�Ĵ�����
3.GCM ���ܲ������������ģ����ĳ��ȣ�key��IV��tag�����ȣ���
4.���ܺ�Ӳ���Զ����key �� IV�����롣
*/

s32 Secure_AesDecryptBlk(EncData* EncryptionData, u8 *Dst,const u8 *Src, const u8 *Tag, u32 Len, u32 Flag)
{
  
    volatile s32 Status = FMSH_SUCCESS;
    u8 Iv[SECURE_IV_SIZE];
    u32 DecFlag = AES_KUP_IV_WRITE_FLAG;
    u32 KeySel = SECURE_CSU_AES_KEY_SRC_KUP;
    FmshFsbl_SacInit(SAC_AES_EN_FLAG);
    /*Geg IV from SAC IV register*/
    GetSacIv(Iv);
    Len += AES_GCM_KEY_IV_SIZE;
 //   Len += 48U;
    if(Flag == 0){
      
      LOG_OUT(DEBUG_INFO,"Current Block is Secure Header,Device Key is used to decrypt Secure Header!!! \r\n");
        KeySel = SECURE_CSU_AES_KEY_SRC_DEV;
        if(EncryptionData->OpKeyUsing)

        {
            DecFlag = AES_SECURE_HEADER_OP_KEY_FLAG ;
        }
        else
        {
            DecFlag = AES_SECURE_HEADER_NOT_OP_KEY_FLAG;
         }     
    LOG_OUT(DEBUG_INFO,"Get Secure Header IV from  Boot Header!!! \r\n");
    BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress + IH_BH_SECURE_HEADER_IV_OFFSET, (u32)Iv,  12U);
    LOG_OUT (DEBUG_INFO, "Secure Header IV is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, Iv, sizeof(Iv));
    
//    KeySel = SECURE_CSU_AES_KEY_SRC_DEV;
    }
    
    if(Flag == 1){
       DecFlag = AES_KUP_IV_WRITE_FLAG;
       KeySel = SECURE_CSU_AES_KEY_SRC_DEV;
    }

    
    Status = AesSm4GcmDec(EncryptionData->Alg,DecFlag, Src, Len, Dst, KeySel,Iv, Tag);
    if(Status != FMSH_SUCCESS)
    {
    // printf("AES GCM DECYPTION FAILURE!\r\n");
    return Status;
    }    
    
    
    return Status;
  
  
  
  
  
  
  
  
}


/************************** Function Prototypes ******************************/

s32 FmshFsbl_Secure_AesDecrypt(EncData*  EncryptionData)
{

    volatile s32 Status = FMSH_SUCCESS;
	u32 CurrentImgLen = 0x0U;
	u32 NextBlkLen = 0U;
    u32 PrevBlkLen = 0x0U;
	u8 *DestAddr= 0x0U;
	u8 *SrcAddr = 0x0U;
	u8 *GcmTagAddr = 0x0U;
	u32 BlockCnt = 0x0U;
	u32 ImageLen = 0x0U;

    DestAddr =(u8*) (EncryptionData->LoadAddr);
	ImageLen = EncryptionData->UnencryptedLength;
    
    SrcAddr = (u8*)(EncryptionData->SrcAddr);
	GcmTagAddr = SrcAddr + SECURE_HDR_SIZE;
    
    
    Secure_AesKeySelNLoad(SECURE_CSU_AES_KEY_SRC_DEV);
    
    
    while(CurrentImgLen <= ImageLen)
    {
		PrevBlkLen = NextBlkLen;
                
		/* Start decryption of Secure-Header/Block/Footer. */
                Status = Secure_AesDecryptBlk(EncryptionData, DestAddr,SrcAddr, GcmTagAddr,NextBlkLen,BlockCnt);
        	/* If decryption failed then return error. */
		if(Status != FMSH_SUCCESS)
		{
			goto ENDF;
		}
                
        		/*
		 * Find the size of next block to be decrypted.
		 * Size is in 32-bit words so mul it with 4
		 */
       
        LOG_OUT(DEBUG_INFO,"Block%d is Decypted Success,Preparing Decrypt Block%d\r\n",BlockCnt,BlockCnt+1);        
                
        NextBlkLen = ReadReg(FPS_CSU_BASEADDR + SAC_AES_IVR0_ADDR_OFFSET); 
       
        FmshFsbl_ByteSwap((u8*)&NextBlkLen);
        NextBlkLen*=4U;
        if(NextBlkLen==0)
          break;
        LOG_OUT (DEBUG_INFO, "Next Block Length is : 0x%x\r\n",NextBlkLen);
        /* Update the current image size. */
	CurrentImgLen += NextBlkLen;
        /*this means last block*/
	if(BlockCnt > 0U)
	{
	    /*
	     * Update DestAddr and SrcAddr for next Block decryption.
	     */
            if (EncryptionData->LoadAddr != SECURE_DESTINATION_PCAP_ADDR)
	    {
		DestAddr += PrevBlkLen;
	    }
          
	    SrcAddr = (GcmTagAddr + SECURE_GCM_TAG_SIZE);
			
        } 
        else
	{
	    /* Update SrcAddr for Block-0 */
	    SrcAddr = (SrcAddr + SECURE_HDR_SIZE + SECURE_GCM_TAG_SIZE);
        }

	/* Update the GcmTagAddr to get GCM-TAG for next block. */
	GcmTagAddr = SrcAddr + NextBlkLen + SECURE_HDR_SIZE;
               
	/* Update block count. */
	BlockCnt++;

    };
    ENDF:
	AesDis();
    FmshFsbl_SacAesSwitch(SAC_AES_DIS_FLAG); 
	return Status;
		
    }