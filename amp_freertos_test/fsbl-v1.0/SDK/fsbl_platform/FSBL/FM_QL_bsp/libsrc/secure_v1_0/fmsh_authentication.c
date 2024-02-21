/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_authentication.c
*
* This file contains fmsh_authentication.h.
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
extern u8 *BootHdr;
extern u32 BootHeaderSize;
extern u8 BootPerformanceTest;
extern Ps_BootHeader BootHeaderInfo;

u8 EfusePpkHash[HASH_TYPE_SHA2] __attribute__ ((aligned (4)))={0U};
u8 EfuseSpkID[SPKID_AC_ALIGN]={0U};
static u8 *PpkModular;
static u8 *PpkModularEx;
static u32	PpkExp;



/*****************************************************************************/
/*
* This function is used to read PPK0/PPK1 hash from efuse based on PPK
* selection.
*
* @param	PpkHash is a pointer to an array which holds the readback
*		PPK hash in.
* @param	PpkSelect is a u8 variable which has to be provided by user
*		based on this input reading is happens from efuse PPK0 or PPK1
* @param	SpKId is a pointer to an array in which SPKID from eFUSE will
*			stored.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void ReadPpkHashSpkID(u32 *PpkHash, u8 PpkSelect, u32 *SpkId)
{
	s32 RegNum;
	u32 *DataRead = PpkHash;

	if (PpkSelect == 0) {
		for (RegNum = 0; RegNum < 8; RegNum++) {

			*DataRead = Fmsh_In32(EFUSE_PPK0_START + (RegNum * 4));
                   
			DataRead++;
		}
	}
	else {
		for (RegNum = 0; RegNum < 8; RegNum++) {

			*DataRead = Fmsh_In32(EFUSE_PPK1_START + (RegNum * 4));
              
			DataRead++;
		}
	}

	/* Reading SPK ID */
        
	*SpkId = Fmsh_In32(EFUSE_SPKID);
  
}

/*****************************************************************************/
/**
*
* This function is used to set ppk pointer to ppk in OCM
*
* @param	None
*
* @return
*
* @note		None
*
******************************************************************************/

static void SetPpk( u32 AcOffset )
{
	
	u8 *PpkPtr;
	
	/*
	 * Set PPK only if is not already set
	 */
//	if(PpkAlreadySet == 0)
//	{
	
		/*
		 * Set PpkPtr to PPK in OCM
		 */
	 
		/*

		 */
		PpkPtr = (u8 *)(AcOffset);
		PpkPtr += RSA_HEADER_SIZE;

		/*
		 * Increment the pointer by Magic word size
		 */
		PpkPtr += RSA_MAGIC_WORD_SIZE;

		/*
		 * Set pointer to PPK
		 */
		PpkModular = (u8 *)PpkPtr;
		PpkPtr += RSA_PPK_MODULAR_SIZE;

		PpkModularEx = (u8 *)PpkPtr;

		PpkPtr += RSA_PPK_MODULAR_EXT_SIZE;
		PpkExp = *((u32 *)PpkPtr);
	
		/*
		 * Setting variable to avoid resetting PPK pointers
		 */
//		PpkAlreadySet=1;
//	}
	
	return;
}
/******************************************************************************
*
* This function compares the hashs
*
* @param	Hash1 stores the hash to be compared.
* @param	Hash2 stores the hash to be compared.
*
* @return
* 		Error code on failure
* 		SUCESS on success
*
* @note		None.
*
******************************************************************************/
static u32 CompareHashs(u8 *Hash1, u8 *Hash2)
{
	u8 Index;
	u32 *HashOne = (u32 *)Hash1;
	u32 *HashTwo = (u32 *)Hash2;

	for (Index = 0; Index < HASH_TYPE_SHA2/4; Index++) {
		if (HashOne[Index] != HashTwo[Index]) {
			return FMSH_FAILURE;
		}
	}

	return FMSH_SUCCESS;
}
/*****************************************************************************/
/*
* This function is used to verify PPK hash and SPK ID of the partition with
* the values stored on eFUSE.
*
* @param	AcOffset is the Authentication certificate offset which has
*		AC.
* @param	HashLen holds the type of authentication enabled.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static u32 PpkSpkIdVer(u32 AcOffset)
{
	u8 PpkHash[HASH_TYPE_SHA2] __attribute__ ((aligned (4)));
//	void * ShaCtx = (void * )NULL;
	u8 * AcPtr = (u8*) AcOffset;
	u32 Status = FMSH_SUCCESS;
	u8 *SpkId = (u8 *)(AcPtr + SPKID_AC_ALIGN);
	u32 *SpkIdEfuse = (u32 *)EfuseSpkID;

	(void)memset(PpkHash,0U,sizeof(PpkHash));
  
	/* Hash calculation on PPK */
        FmshFsbl_sha256(PpkModular, RSA_PPK_MODULAR_SIZE+RSA_PPK_MODULAR_EXT_SIZE+ RSA_PPK_EXPO_SIZE,PpkHash );
        LOG_OUT(DEBUG_INFO,"Calculated PPK hash:\r\n");
        PRINT_ARRAY (DEBUG_INFO, (u8*)PpkHash, sizeof(PpkHash));
        LOG_OUT(DEBUG_INFO,"Read eFuse PPK hash:\r\n");
        PRINT_ARRAY (DEBUG_INFO, (u8*)EfusePpkHash, sizeof(EfusePpkHash));

	/* Compare hashs */
	Status = CompareHashs(PpkHash, EfusePpkHash);
	if (Status != FMSH_SUCCESS) {
        BootInstance.ErrorCode = PPK_HASH_MISMATCH;
        LOG_OUT(DEBUG_INFO,"PPK hash is not matched and  Verification failed\r\n");
        ErrorLockDown();
		goto END;

	}
        LOG_OUT(DEBUG_INFO,"PPK hash verification success\r\n");
	/* Compare SPK ID */
	if (*SpkIdEfuse !=  *(u32 *)SpkId) {
          
        LOG_OUT(DEBUG_INFO,"SPK ID in eFUSE is:0x%08x,SPK ID in Certificate is:0x%08x. \r\n",*SpkIdEfuse,*(u32 *)SpkId);  
        LOG_OUT(DEBUG_INFO,"SPK ID verification failed\r\n");
        BootInstance.ErrorCode = SPK_ID_MISMATCH;
        ErrorLockDown();
		goto END;
	}
        LOG_OUT(DEBUG_INFO,"SPK ID verification success\r\n");
END:

	return Status;

}

/*****************************************************************************/
/**
*
* This function recreates the and check signature
*
* @param	Partition signature
* @param	Partition hash value which includes boot header, partition data
* @return
*		- SUCCESS if check passed
*		- FAILURE if check failed
*
* @note		None
*
******************************************************************************/
static u32 RecreatePaddingAndCheck(u8 *signature, u8 *hash)
{
    u8 T_padding[] = {0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48,
			0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
    u8 * pad_ptr = signature + 256;
    u32 pad = 256 - 3 - 19 - 32;
    u32 ii;

    /*
    * Re-Create PKCS#1v1.5 Padding
    * MSB  ----------------------------------------------------LSB
    * 0x0 || 0x1 || 0xFF(for 202 bytes) || 0x0 || T_padding || SHA256 Hash
    */
    if (*--pad_ptr != 0x00 || *--pad_ptr != 0x01) 
    {
    	return FMSH_FAILURE;
    }

    for (ii = 0; ii < pad; ii++) 
    {
    	if (*--pad_ptr != 0xFF) 
        {
            return FMSH_FAILURE;
        }
    }

    if (*--pad_ptr != 0x00) 
    {
       	return FMSH_FAILURE;
    }

    for (ii = 0; ii < sizeof(T_padding); ii++) 
    {
    	if (*--pad_ptr != T_padding[ii]) 
        {
            return FMSH_FAILURE;
        }
    }
    
    LOG_OUT (DEBUG_INFO, "Decrypted  Hash is :\r\n");
    PRINT_ARRAY(DEBUG_INFO, pad_ptr-0x20 , 32);
    for (ii = 0; ii < 32; ii++) 
    {
       	if (*--pad_ptr != hash[ii])
        {
       	   LOG_OUT (DEBUG_INFO, "Decrypted  Hash is not matched with calculated Hash.\r\n");	
           return FMSH_FAILURE;
        }
    }

    return FMSH_SUCCESS;
    
}

/************************** Function Prototypes ******************************/
#ifndef FSBL_PL_DDR
u8 AcBuffer[0x7C0]={0};
#endif
u32 FmshFsbl_AuthenticatePartition(u32 PartitionOffset,u32 PartitionLen, u32 AcOffset, u8 PlFlag)
{
    u8 DecryptSignature[256];
    u8 HashSignature[32];
    u8 *SpkModular;

    u8 *SpkModularEx;
    
    u32 SpkExp;
    u32 Status = FMSH_SUCCESS;
    u32 EfuseRsaEn = ReadReg(EFUSE_SEC_CTRL) & EFUSE_SEC_CTRL_RSA_EN_MASK;
    
#ifndef FSBL_PL_DDR
    if(PlFlag==1)
    {
        //SD NAND
        if( (BootInstance.BootMode==SD_CARD) ||   \
              (BootInstance.BootMode==NAND_FLASH) )
        {
          Status = BootInstance.DeviceOps.DeviceCopy(AcOffset,(u32)AcBuffer, 0x7C0);
          AcOffset=(u32)AcBuffer;
        }
    }
#endif  
    
     u8 *SignaturePtr =(u8*)AcOffset;
    
    SetPpk(AcOffset);    
    
    if(((FmshFsbl_FindOneInNumber(EfuseRsaEn)& 0x7fff) > 0x8) || ((BootHeaderInfo.ImageAttr & IH_BH_IMAGE_ATTRB_RSA_MASK) != IH_BH_IMAGE_ATTRB_RSA_MASK)) 
    { 
      if ((*(u32 *)(AcOffset)& IH_AC_ATTRB_PPK_SELECT_MASK) == 0x00U) 
      {
	  /* PPK 0 */
          ReadPpkHashSpkID((u32 *)EfusePpkHash,0U, (u32 *)EfuseSpkID);
      }else 
      {
	  /* PPK 1 */
          ReadPpkHashSpkID((u32 *)EfusePpkHash,1U, (u32 *)EfuseSpkID);
      }
    
      Status = PpkSpkIdVer(AcOffset);
      if (Status != FMSH_SUCCESS) 
      {
	  goto END;
      }
    }
      
    SignaturePtr = (u8*)AcOffset;
    
   /*
    * Increment the pointer by authentication Header size
    */
    SignaturePtr += RSA_HEADER_SIZE;

   /*
    * Increment the pointer by Magic word size
    */
    SignaturePtr += RSA_MAGIC_WORD_SIZE;

   /*
    * Increment the pointer beyond the PPK
    */
    SignaturePtr += RSA_PPK_MODULAR_SIZE;
    SignaturePtr += RSA_PPK_MODULAR_EXT_SIZE;
    SignaturePtr += RSA_PPK_EXPO_SIZE;
     
   /*
    * Calculate SPK Hash
    */
    //FmshFsbl_sha256
#ifdef   FSBL_PL_DDR  
     FmshFsbl_sha256((u8 *)SignaturePtr, (RSA_SPK_MODULAR_EXT_SIZE + \
				RSA_SPK_EXPO_SIZE + RSA_SPK_MODULAR_SIZE),HashSignature); 
    
#else  
    if(PlFlag==1)
    {
        FmshFsbl_LinearBurstSha256((u8 *)SignaturePtr, (RSA_SPK_MODULAR_EXT_SIZE + \
				RSA_SPK_EXPO_SIZE + RSA_SPK_MODULAR_SIZE),HashSignature); 
        //FmshFsbl_SubgroupSha256((u8 *)SignaturePtr, (RSA_SPK_MODULAR_EXT_SIZE + \
	//			RSA_SPK_EXPO_SIZE + RSA_SPK_MODULAR_SIZE),HashSignature); 
    }
    else
        FmshFsbl_sha256((u8 *)SignaturePtr, (RSA_SPK_MODULAR_EXT_SIZE + \
				RSA_SPK_EXPO_SIZE + RSA_SPK_MODULAR_SIZE),HashSignature); 
#endif
    LOG_OUT (DEBUG_INFO, "Calculated SPK Hash is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, HashSignature, sizeof(HashSignature));
        
   /*
    * Extract SPK signature
    */
    SpkModular = (u8 *)SignaturePtr;
     
    SignaturePtr += RSA_SPK_MODULAR_SIZE;

    SpkModularEx = (u8 *)SignaturePtr;

    SignaturePtr += RSA_SPK_MODULAR_EXT_SIZE;
    SpkExp = *((u32 *)SignaturePtr);
    SignaturePtr += RSA_SPK_EXPO_SIZE;

   /*
    * Decrypt SPK Signature
    */
    rsa2048_pubexp((unsigned long*)DecryptSignature,\
			(unsigned long*)SignaturePtr,\
			(u32)PpkExp,\
			(unsigned long*)PpkModular,\
			(unsigned long*)PpkModularEx);
    LOG_OUT (DEBUG_INFO, "Decypted SPK Signature is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, DecryptSignature, sizeof(DecryptSignature));
    
    Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
    if (Status != FMSH_SUCCESS) 
    {
        BootInstance.ErrorCode = SPK_SIGNATURE_VERIFY_FAILED;
        LOG_OUT(DEBUG_INFO,"Partition SPK Signature Authentication failed\r\n");
        ErrorLockDown();
	return FMSH_FAILURE;
    }
     
    LOG_OUT(DEBUG_INFO,"Partition SPK Signature Authentication Success\r\n");
 
    SignaturePtr += RSA_SPK_SIGNATURE_SIZE;
           
   /* 
    * Decrypt Boot Header Signature 
    */
    rsa2048_pubexp((unsigned long*)DecryptSignature,\
			(unsigned long*)SignaturePtr,\
			(u32)SpkExp,\
			(unsigned long*)SpkModular,\
			(unsigned long*)SpkModularEx);

    
    LOG_OUT (DEBUG_INFO, "Decypted Partition Signature is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, DecryptSignature, sizeof(DecryptSignature));
    
    
    /*
     * Calculate BOOT Header Hash
     */      
     FmshFsbl_sha256((u8*)BootHdr, BootHeaderSize,HashSignature);
       
     LOG_OUT (DEBUG_INFO, "Calculated Boot Header Hash is :\r\n");
     PRINT_ARRAY (DEBUG_INFO, HashSignature, sizeof(HashSignature));
     Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
     if (Status != FMSH_SUCCESS) 
     {
         BootInstance.ErrorCode = BOOT_HEADER_SIGNATURE_VERIFY_FAILED;
	 LOG_OUT(DEBUG_INFO,"Boot Header Signature Authentication failed\r\n");
         ErrorLockDown();
	 return FMSH_FAILURE;
     }
      
     LOG_OUT(DEBUG_INFO,"Boot Header Signature Authentication Success\r\n");
 
     SignaturePtr += RSA_BOOTHEADER_SIGNATURE_SIZE;
    
    /*
     * Decrypt Partition Signature
     */
     rsa2048_pubexp((unsigned long*)DecryptSignature,\
			(unsigned long*)SignaturePtr,\
			(u32)SpkExp,\
			(unsigned long*)SpkModular,\
			(unsigned long*)SpkModularEx);

    /*
     * Calculate partition Hash 
     */
#ifdef  FSBL_PL_DDR
     FmshFsbl_sha256((u8*)PartitionOffset, PartitionLen + RSA_CERTIFICATE_SIZE-RSA_PARTITION_SIGNATURE_SIZE,HashSignature);
#else
     if(PlFlag==1)
     {
        FmshFsbl_SubgroupSha256((u8*)PartitionOffset, PartitionLen + RSA_CERTIFICATE_SIZE-RSA_PARTITION_SIGNATURE_SIZE,HashSignature);
     }
     else
     {
        FmshFsbl_sha256((u8*)PartitionOffset, PartitionLen + RSA_CERTIFICATE_SIZE-RSA_PARTITION_SIGNATURE_SIZE,HashSignature);
     }
#endif
     
     LOG_OUT (DEBUG_INFO, "Calculated Partition Hash is :\r\n");
     PRINT_ARRAY (DEBUG_INFO, HashSignature, sizeof(HashSignature));
     Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
     if (Status != FMSH_SUCCESS) 
     {
        BootInstance.ErrorCode = PARTITION_SIGNATURE_VERIFY_FAILED;	
	LOG_OUT(DEBUG_INFO,"Partition  Signature Authentication failed\r\n");
        ErrorLockDown();
	return FMSH_FAILURE;
     }
      
     LOG_OUT(DEBUG_INFO,"Partition  Signature Authentication Success\r\n");
        
END:
	return Status;
}

u32 FmshFsbl_AuthenticateHeader(u32 PartitionOffset,u32 PartitionLen, u32 AcOffset)
{
	u8 DecryptSignature[256];
	u8 HashSignature[32];
        u8 *SpkModular;
	u8 *SpkModularEx;
	u32 SpkExp;
	u8 *SignaturePtr = (u8*)AcOffset;
	u32 Status = FMSH_SUCCESS;
        u32 EfuseRsaEn = ReadReg(EFUSE_SEC_CTRL);
       
        SetPpk(AcOffset);    
        if((FmshFsbl_FindOneInNumber(EfuseRsaEn)&0x7fff) > 0x8) 
        { 
            if ((*(u32 *)(AcOffset)& IH_AC_ATTRB_PPK_SELECT_MASK) == 0x00U) 
            {
                /* PPK 0 */
                ReadPpkHashSpkID((u32 *)EfusePpkHash,0U, (u32 *)EfuseSpkID);
            }else {
                /* PPK 1 */
                ReadPpkHashSpkID((u32 *)EfusePpkHash,1U, (u32 *)EfuseSpkID);
            }

            Status = PpkSpkIdVer(AcOffset);
            if (Status != FMSH_SUCCESS) 
            {
                goto END;
            }
        }
    
        SignaturePtr = (u8*)AcOffset;
        /*
	 * Increment the pointer by authentication Header size
	 */
	SignaturePtr += RSA_HEADER_SIZE;

	/*
	 * Increment the pointer by Magic word size
	 */
	SignaturePtr += RSA_MAGIC_WORD_SIZE;

	/*
	 * Increment the pointer beyond the PPK
	 */
	SignaturePtr += RSA_PPK_MODULAR_SIZE;
	SignaturePtr += RSA_PPK_MODULAR_EXT_SIZE;
	SignaturePtr += RSA_PPK_EXPO_SIZE;
	/*
	 * Calculate SPK Hash
         */
        FmshFsbl_sha256((u8 *)SignaturePtr, (RSA_SPK_MODULAR_EXT_SIZE +
				RSA_SPK_EXPO_SIZE + RSA_SPK_MODULAR_SIZE),
				HashSignature);

        
        LOG_OUT (DEBUG_INFO, "Calculated SPK Hash is :\r\n");
        PRINT_ARRAY (DEBUG_INFO, HashSignature, sizeof(HashSignature));
        
        /*
   	 * Extract SPK signature
   	 */
	SpkModular = (u8 *)SignaturePtr;
	SignaturePtr += RSA_SPK_MODULAR_SIZE;

	SpkModularEx = (u8 *)SignaturePtr;

	SignaturePtr += RSA_SPK_MODULAR_EXT_SIZE;
	SpkExp = *((u32 *)SignaturePtr);
	SignaturePtr += RSA_SPK_EXPO_SIZE;

	/*
	 * Decrypt SPK Signature
	 */
	rsa2048_pubexp((unsigned long*)DecryptSignature,\
			(unsigned long*)SignaturePtr,\
			(u32)PpkExp,\
			(unsigned long*)PpkModular,\
			(unsigned long*)PpkModularEx);


      LOG_OUT (DEBUG_INFO, "Decypted SPK Signature is :\r\n");

        
      Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
      if (Status != FMSH_SUCCESS) 
      {
                BootInstance.ErrorCode = SPK_SIGNATURE_VERIFY_FAILED;
		LOG_OUT(DEBUG_INFO,"Partition SPK Signature Authentication failed\r\n");
                ErrorLockDown();
		return FMSH_FAILURE;
      }
      LOG_OUT(DEBUG_INFO,"SPK Signature Authentication Success\r\n");
    
     
        LOG_OUT(DEBUG_INFO,"Boot Header Signature Authentication Start...\r\n");    
        SignaturePtr += RSA_SPK_SIGNATURE_SIZE;
    
	rsa2048_pubexp((unsigned long*)DecryptSignature,\
			(unsigned long*)SignaturePtr,\
			(u32)SpkExp,\
			(unsigned long*)SpkModular,\
			(unsigned long*)SpkModularEx);

    
    // LOG_OUT (DEBUG_INFO, "Decypted Partition Signature is :\r\n");
    // PRINT_ARRAY (DEBUG_INFO, DecryptSignature, sizeof(DecryptSignature));
    
    
    
     /*
      * Calculate BOOT Header Hash 
      */

       
       FmshFsbl_sha256((u8*)BootHdr, BootHeaderSize,HashSignature);
       

//	sha256((u8*)PartitionOffset, PartitionLen + RSA_CERTIFICATE_SIZE-2*RSA_PARTITION_SIGNATURE_SIZE,HashSignature);
   

        LOG_OUT (DEBUG_INFO, "Calculated Boot Header Hash is :\r\n");
        PRINT_ARRAY (DEBUG_INFO, HashSignature, sizeof(HashSignature));
	Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
	if (Status != FMSH_SUCCESS) {
      
        BootInstance.ErrorCode = BOOT_HEADER_SIGNATURE_VERIFY_FAILED;
		LOG_OUT(DEBUG_INFO,"Boot Header Signature Authentication failed\r\n");
        ErrorLockDown();
		return FMSH_FAILURE;
	}
      
	LOG_OUT(DEBUG_INFO,"Boot Header  Signature Authentication Success\r\n");
    
    
    
        SignaturePtr += RSA_BOOTHEADER_SIGNATURE_SIZE;
    /*
	 * Decrypt Partition Signature
	 */


	rsa2048_pubexp((unsigned long*)DecryptSignature,\
			(unsigned long*)SignaturePtr,\
			(u32)SpkExp,\
			(unsigned long*)SpkModular,\
			(unsigned long*)SpkModularEx);
 
    
//     LOG_OUT (DEBUG_INFO, "Decypted Partition Signature is :\r\n");
//      PRINT_ARRAY (DEBUG_INFO, DecryptSignature, sizeof(DecryptSignature));
    
    
    
	/*
	 * Calculate partition Hash */
       FmshFsbl_sha256((u8*)PartitionOffset, PartitionLen + RSA_CERTIFICATE_SIZE-RSA_PARTITION_SIGNATURE_SIZE,HashSignature);
       

       
        LOG_OUT (DEBUG_INFO, "Calculated Partition Hash is :\r\n");
        PRINT_ARRAY (DEBUG_INFO, HashSignature, sizeof(HashSignature));
	Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
        if(Status != FMSH_SUCCESS)
        {
              BootInstance.ErrorCode = PARTITION_SIGNATURE_VERIFY_FAILED;
              ErrorLockDown();
              LOG_OUT(DEBUG_INFO,"Partition  Signature Authentication failed\r\n");
	
              return FMSH_FAILURE;
	}
      
	LOG_OUT(DEBUG_INFO,"Partition  Signature Authentication Success\r\n");
        
END:
	return Status;
        
}



