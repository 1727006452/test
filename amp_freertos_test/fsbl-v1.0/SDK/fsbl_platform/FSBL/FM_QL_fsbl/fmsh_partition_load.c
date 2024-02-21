/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_partition_load.c
*
* This file contains .
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
extern FQspiPs_T* QspiInstancePtr;
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern u32 BootHeaderSize;  //header.c
extern u32 FlashReadBaseAddress;
extern BootPs BootInstance;
extern u8 *BootHdr;

u8 AuthBuffer[RSA_SIGNATURE_SIZE]__attribute__ ((aligned (4))) = {0};
EncData  EncryptionData;


/************************** Function Prototypes ******************************/

/**
 * This function validates the partition
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @param	LoadAddress Load address of partition
 *
 * @param	PartitionNum is the partition number to calculate checksum
 *
 * @param	ShaType is either SHA2/SHA3
 *
 * @return	returns FMSH_SUCCESS on success
 * 			returns FMSH_FAILURE on failure
 *
 *****************************************************************************/
static u32 FmshFsbl_CalcualteSHA(const BootPs * BootInstancePtr, UINTPTR LoadAddress,
						u32 PartitionNum)
{
	u8 PartitionHash[HASH_TYPE_SHA2] __attribute__ ((aligned (4))) = {0};
	u8 Hash[HASH_TYPE_SHA2] __attribute__ ((aligned (4))) = {0};
	u32 HashOffset;
	u32 Index;
	u32 Length;
	u32 Status;
	const Ps_PartitionHeader * PartitionHeader;

	/**
	 * Update the variables
	 */
	PartitionHeader = &BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum];
	Length = PartitionHeader->TotalDataWordLength * 4U;
        
	HashOffset = PartitionHeader->ChecksumWordOffset * 4U;

	/* Start the SHA engine */
	FmshFsbl_sha256((u8*)LoadAddress, Length, PartitionHash);
	Status = BootInstancePtr->DeviceOps.DeviceCopy(HashOffset,
			(UINTPTR) Hash, HASH_TYPE_SHA2);

	if (Status != FMSH_SUCCESS) {
		LOG_OUT(DEBUG_INFO,"FSBL_ERROR_HASH_COPY_FAILED \r\n");
		return FMSH_FAILURE;
	}

	for (Index = 0U; Index < HASH_TYPE_SHA2; Index++)
	{
		if(PartitionHash[Index]!= Hash[Index])
		{
			LOG_OUT(DEBUG_INFO,"FSBL_ERROR_HASH_FAILED \r\n");
			return FMSH_FAILURE;
		}
	}
	return Status;
}
/****************************************************************************/
/**
 * This function is used to check whether cpu has handoff address stored
 * in the handoff structure
 *
 * @param FsblInstancePtr is pointer to the XFsbl Instance
 *
 * @param DestinationCpu is the cpu which needs to be checked
 *
 * @return
 * 		- FMSH_SUCCESS if cpu handoff address is not present
 * 		- FMSH_FAILURE if cpu handoff address is present
 *
 * @note
 *
 *****************************************************************************/
static u32 FmshFsbl_CheckHandoffCpu (BootPs * BootInstancePtr, u32 DestinationCpu)
{
	u32 ValidHandoffCpuNo=0U;
	u32 Status=FMSH_SUCCESS;
	u32 Index=0U;
	u32 CpuId=0U;


	ValidHandoffCpuNo = BootInstancePtr->HandoffCpuNo;

	for (Index=0U;Index<ValidHandoffCpuNo;Index++)
	{
		CpuId = BootInstancePtr->HandoffValues[Index].CpuSettings &
			IH_PH_ATTRB_DEST_CPU_MASK;
		if (CpuId == DestinationCpu)
		{
			Status = FMSH_FAILURE;
			break;
		}
	}
	return Status;
}

/****************************************************************************/
/**
 * This function copies the partition to specified destination
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @param	PartitionNum is the partition number in the image to be loaded
 *
 * @return	returns the error codes described in error.h on any error
 * 			returns SUCCESS on success
 *****************************************************************************/
static u32 FmshFsbl_PartitionCopy(BootPs * BootInstancePtr,u32 PartitionNum)
{
	u32 Status=FMSH_SUCCESS;
	Ps_PartitionHeader * PartitionHeaderInfo;
	u32 SrcAddress;
	UINTPTR LoadAddress;
	u32 Length;
        
        u32 DestinationCpu=0U;
	u32 CpuNo=0U;
	u32 DestinationDevice=0U;
	u32 ExecState=0U;
        
	/**
	 * Assign the partition header to local variable
	 */
	PartitionHeaderInfo =&(BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum]);

	/**
	 * Check for XIP image
	 * No need to copy for XIP image
	 */
        DestinationCpu = FmshFsbl_GetDestinationCpu(PartitionHeaderInfo);
        /**
	 * if destination cpu is not present, it means it is for same cpu
	 */
	if (DestinationCpu == IH_PH_ATTRB_DEST_CPU_NONE)
	{
            DestinationCpu = BootInstancePtr->ProcessorID;
	}
        
        /**
	 * Get the execution state
	 */
	ExecState = FmshFsbl_GetExecState(PartitionHeaderInfo);
        
	if (PartitionHeaderInfo->UnEncryptedDataWordLength == 0U)
	{
	     /**
	      * Update the Handoff address only for the first application
	      * of that cpu
	      * This is for XIP image. For other partitions it handoff
	      * address is updated after partition validation
	      */
              CpuNo = BootInstancePtr->HandoffCpuNo;
              if (FmshFsbl_CheckHandoffCpu(BootInstancePtr,DestinationCpu) == FMSH_SUCCESS)
	      {
			BootInstancePtr->HandoffValues[CpuNo].CpuSettings =
			        DestinationCpu | ExecState;
			BootInstancePtr->HandoffValues[CpuNo].HandoffAddress =
				PartitionHeaderInfo->DestinationExecutionAddress;
			BootInstancePtr->HandoffCpuNo += 1U;
	      }
              else 
              {
			/**
			 *
			 * if two partitions has same destination cpu, error can
			 * be triggered here
			 */
	      }
	      Status = FMSH_SUCCESS;
	      goto END;
	}

	/**
	 * Get the source(flash offset) address where it needs to copy
	 */
	SrcAddress = BootInstancePtr->ImageOffsetAddress +((PartitionHeaderInfo->DataWordOffset) *
					IH_PARTITION_WORD_LENGTH);

	/**
	 * Length of the partition to be copied
	 */
	Length  = (PartitionHeaderInfo->TotalDataWordLength) * IH_PARTITION_WORD_LENGTH;

	LoadAddress = (UINTPTR)PartitionHeaderInfo->DestinationLoadAddress ;

        DestinationDevice = FmshFsbl_GetDestinationDevice(PartitionHeaderInfo);
     
        /**
	 * Copy the PL to temporary DDR Address
	 */
	if (DestinationDevice == IH_PH_ATTRB_DEST_DEVICE_PL)
	{
		/**
		 *  if pl has bitstream, skip copying the PL bitstream
		 */
                FmshFsbl_OpenCfgLevelShifter();
#if !DOWNLOAD_BIT_FORCE
                if(FMSH_ReadReg(FPS_CSU_BASEADDR,0x18)&(1<<9))
                {

                      LOG_OUT(DEBUG_INFO, "Bitstream is done , skip copying Partition!\r\n");
                      goto END;
                }
#endif
		if (LoadAddress == 0xFFFFFFFFU)
		{
			LoadAddress = FSBL_PL_DDRADDR;

#ifndef  FSBL_PL_DDR
			/* In case of DDR less system, skip copying */
		        goto END;
#endif
		}
	}
     
	/* Copy the partition to OCM */
       LoadAddress += 0;
       
       if((LoadAddress>FPS_QSPI0_D_BASEADDR && LoadAddress<0xE8FFFFFF)||(LoadAddress>FPS_SMC_NORSRAM0_BASEADDR && LoadAddress<0xE3FFFFFF))
       {
          LOG_OUT(DEBUG_INFO, "LoadAddress in the linear flash,skip copy!!\r\n");
          goto END;
       }
      
       Status = BootInstancePtr->DeviceOps.DeviceCopy(SrcAddress,LoadAddress, Length);
       if (FMSH_SUCCESS != Status)
       {
          LOG_OUT(DEBUG_INFO, "Copy Partition failure!!\r\n");
          goto END;
       }
       LOG_OUT(DEBUG_INFO, "Copy Partition success!!\r\n");

END:
	return Status;
}

/****************************************************************************/
/**
 * This function validates the partition
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @param	PartitionNum is the partition number in the image to be loaded
 *
 * @return	returns the error codes described in error.h on any error
 * 			returns SUCCESS on success
 *
 *****************************************************************************/  
static u32 FmshFsbl_PartitionValidation(BootPs * BootInstancePtr,u32 PartitionNum)
{
  u32 Status = FMSH_SUCCESS;
  u32 LoadAddress;
  u32 Size;
  u32 AcOffset;
  
  u32 EncryptedKeyIv[3];
  u8 IsEncrypted=FALSE;

  u32 DestinationCpu=0U;
  u32 ExecState=0U;
  u32 CpuNo=0U;
  u32 ImageHeaderAddr;
  
  u8 IsPlPartition=0;
  
  Ps_PartitionHeader * PartitionHeader;
  
  u32 counter=10000000;//1s
  
  u32 Cpu_Id=0;
  
  PartitionHeader=&BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum];

  u32 DestinationDevice = FmshFsbl_GetDestinationDevice(PartitionHeader);
  DestinationCpu = FmshFsbl_GetDestinationCpu(PartitionHeader);
  /**
   * if destination cpu is not present, it means it is for same cpu
   */
  if (DestinationCpu == IH_PH_ATTRB_DEST_CPU_NONE)
  {
      DestinationCpu = BootInstancePtr->ProcessorID;
  }

  /**
   *  if pl has bitstream, skip downloading the PL bitstream
   */
#if !DOWNLOAD_BIT_FORCE
   if(DestinationDevice == IH_PH_ATTRB_DEST_DEVICE_PL)
   {
         if(FMSH_ReadReg(FPS_CSU_BASEADDR,0x18)&(1<<9))
         {
             LOG_OUT(DEBUG_INFO, "Bitstream is done , skip downloading!\r\n");
             goto END;
         }
   }
#endif  
  
 /**
  * check the authentication status
  */  
  if(FmshFsbl_IsRsaSignaturePresent(PartitionHeader) == IH_PH_ATTRB_RSA_SIGNATURE)
  {
      LoadAddress = BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum].DestinationLoadAddress;
      
      if(DestinationDevice == IH_PH_ATTRB_DEST_DEVICE_PL || LoadAddress==0xFFFFFFFF)
      {
          LoadAddress=FSBL_PL_DDRADDR;
      }
      Size =(BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum].AuthCertificateOffset-BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum].DataWordOffset)*4U;
      AcOffset = LoadAddress + Size;
      
     /**
      * Read the Image Header Table offset from Boot Header
      */
      Status = BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress + IH_BH_IH_TABLE_OFFSET, 
                                                    (u32)&ImageHeaderAddr,  4);
      if (Status != FMSH_SUCCESS) 
      {
          LOG_OUT(DEBUG_INFO,"Copy Image Header Table Offset Word failed\r\n");
          goto END;
      }
         
      /*Calculate Boot Header Size */
       BootHeaderSize = ImageHeaderAddr;
         
      /* Copy the Boot header to OCM */
      Status = BootInstance.DeviceOps.DeviceCopy(BootInstancePtr->ImageOffsetAddress,(u32)BootHdr, BootHeaderSize);
      if (Status != FMSH_SUCCESS) 
      {
            LOG_OUT(DEBUG_INFO,"Copy Boot Header failed\r\n");
            goto END;
      }
         
#ifdef	FSBL_PL_DDR     
      LOG_OUT(DEBUG_INFO,"Partition Authentication Enabled \r\n");
      Status = FmshFsbl_AuthenticatePartition(LoadAddress,Size,AcOffset,IsPlPartition);
      if (Status != FMSH_SUCCESS)
      {
          BootInstancePtr->ErrorCode = PARTITION_AUTHENTICATE_FAILURE;
          LOG_OUT(DEBUG_INFO,"Authencicatie Partition failed \r\n");          
          goto END;
      }   
#else
      if(DestinationDevice == IH_PH_ATTRB_DEST_DEVICE_PL || LoadAddress==0xFFFFFFFF)
      {
         IsPlPartition=1;
         LoadAddress=BootInstancePtr->ImageOffsetAddress +((PartitionHeader->DataWordOffset) *
					IH_PARTITION_WORD_LENGTH);
         if( BootInstancePtr->BootMode==QSPI_FLASH )
         {
              LoadAddress+=FPS_QSPI0_D_BASEADDR;
              AcOffset = LoadAddress + Size;   
         }
         else if( BootInstancePtr->BootMode==NOR_FLASH )
         {
              LoadAddress+=FPS_SMC_NORSRAM0_BASEADDR;
              AcOffset = LoadAddress + Size;
         }
         else if( BootInstancePtr->BootMode==NAND_FLASH || BootInstancePtr->BootMode==SD_CARD )
         {
              AcOffset = LoadAddress + Size;
         }
      }
      else
        IsPlPartition=0;

      LOG_OUT(DEBUG_INFO,"Partition Authentication Enabled \r\n");
      Status = FmshFsbl_AuthenticatePartition(LoadAddress,Size,AcOffset,IsPlPartition);
      if (Status != FMSH_SUCCESS)
      {
           BootInstancePtr->ErrorCode = PARTITION_AUTHENTICATE_FAILURE;
           LOG_OUT(DEBUG_INFO,"Authencicatie Partition failed \r\n");          
           goto END;
      }
 
#endif
      
#if DEBUG_PERF
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Authencicatie Partition is done at the %f ms\r\n",gtc_time);
#endif    
    
    LOG_OUT(DEBUG_INFO,"Authencicatie Partition success!! \r\n");  
    
   }
    
  /**
   *check encryption status
   */
  if (FmshFsbl_IsEncryptedPresent(PartitionHeader) == IH_PH_ATTRB_ENCRYPTION ) 
  {
      IsEncrypted=TRUE;
        
      LOG_OUT(DEBUG_INFO, "Encryption Enabled\r\n"); 
      
      EncryptionData.Alg = BootInstancePtr->EncrytionAlgorithm;
      EncryptionData.SrcAddr = BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum].DestinationLoadAddress ;
      EncryptionData.LoadAddr = BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum].DestinationLoadAddress;
      EncryptionData.UnencryptedLength = (BootInstancePtr->ImageHeader.PartitionHeader[PartitionNum].UnEncryptedDataWordLength) * IH_PARTITION_WORD_LENGTH;

      if(EncryptionData.Alg == ALG_AES)
      {  
         LOG_OUT(DEBUG_INFO,"Encryption Algorithm is AES256 !! \r\n"); 
      }else
      {
         LOG_OUT(DEBUG_INFO,"Encryption Algorithm is SM4 !! \r\n"); 
      }
   }
  
  /**
   *use SAC_RED KEY to decrypt image
   */
   if((BootInstance.BootHdrAttributes&IH_BH_IMAGE_ATTRB_OPKEY_MASK) == IH_BH_IMAGE_ATTRB_OPKEY_MASK)
   {
      LOG_OUT(DEBUG_INFO,"Optional Key is  used !! \r\n"); 
      EncryptionData.OpKeyUsing = 1;
   }
     

   if (DestinationDevice == IH_PH_ATTRB_DEST_DEVICE_PL)
   {
        FmshFsbl_OpenCfgLevelShifter();
        u8 freqFlag=0;
        /*use dma for copying*/
        if(g_QSPI_DMA_Flag)
        {
          g_QSPI_DMA_Flag=FALSE;
          FDevcPs_devcInitDownload();
          while(counter!=0)
          {
              delay_us(1);
              if(s_DMA_tfrFlag==TRUE)
                break;
              counter--;
          }	// transfer is in progress
          Status=FQspiPs_WaitForIndacDone(QspiInstancePtr, 1);
          FDmaPs_disable(&g_DMA_dmac);
          if(counter==0)
          {
              LOG_OUT(DEBUG_INFO,"DMA COPY the Bitstream is failure !! \r\n"); 
              return FMSH_FAILURE;
          }
        }
        
        Size=PartitionHeader->UnEncryptedDataWordLength;
        LoadAddress=BootInstancePtr->ImageOffsetAddress +((PartitionHeader->DataWordOffset) *
					IH_PARTITION_WORD_LENGTH);

#ifdef	FSBL_PL_DDR
        LoadAddress = FSBL_PL_DDRADDR;     
#else
        if( BootInstancePtr->BootMode==QSPI_FLASH)
        {
           if(LoadAddress>=0x1000000)  //>16MByte
           {
              LoadAddress=FQspiPs_AddressMap(QspiInstancePtr,LoadAddress);
              g_QSPI_High_Addr_Flag=1;
           }
           LoadAddress+=FPS_QSPI0_D_BASEADDR;
        }
        else if(BootInstancePtr->BootMode==NOR_FLASH)
             {
                   LoadAddress+=FPS_SMC_NORSRAM0_BASEADDR;   
             }
#endif

       /*
        * sha256 verification
        */ 
       if ((PartitionHeader->PartitionAttributes & IH_PH_ATTRB_CHECKSUM_MASK) != IH_PH_ATTRB_NOCHECKSUM )
       {       
            if( (LoadAddress&FSBL_PL_DDRADDR)==FSBL_PL_DDRADDR )
            {
                  Status = FmshFsbl_CalcualteSHA(BootInstancePtr, LoadAddress,PartitionNum);
                  if (Status != FMSH_SUCCESS) {
                      BootInstancePtr->ErrorCode = PARTITION_CHECKSUM_ERROR;    
                      TRACE_OUT(DEBUG_OUT,"FSBL_ERROR_PARTITION_CHECKSUM_FAILED \r\n");       
                      Status = PARTITION_CHECKSUM_ERROR;
                      goto END;
		}
                LOG_OUT(DEBUG_INFO,"PL partition SHA verify success!!! \r\n");
            }
            else
            {
                LOG_OUT(DEBUG_INFO,"LoadAddress is 0x%x,verification is not supported !!! \r\n");
            }  
       }
        
        LOG_OUT(DEBUG_INFO,"Prepare downloading bitstream.....\r\n");
        Cpu_Id=ReadReg(PSS_IDCODE_REG);
        if((Cpu_Id&0x10000000)==0)
        {
            Status=FDevcPs_adjustFreq(&freqFlag);
            if(Status!=FMSH_SUCCESS)
            {
                LOG_OUT(DEBUG_INFO,"Frequency reduction failed!!! \r\n"); 
                goto END;
            }
        }
        
        if(IsEncrypted == TRUE)
        {
            EncryptionData.SrcAddr=LoadAddress;
            EncryptionData.UnencryptedLength=Size;
            
            LOG_OUT(DEBUG_INFO,"Get Secure Header IV from  Boot Header!!! \r\n");
            BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress + IH_BH_SECURE_HEADER_IV_OFFSET, (u32)EncryptedKeyIv,  12U);
            LOG_OUT (DEBUG_INFO, "Secure Header IV is :\r\n");
            
            if(EncryptionData.Alg == ALG_AES)
            {
                if(EncryptionData.OpKeyUsing == 1)
                {
                   Status = FDevcPs_encryptDownload_AES_UseOp(EncryptedKeyIv,LoadAddress,Size);
                }
                else
                {
                   Status = FDevcPs_encryptDownload_AES_NoOp(EncryptedKeyIv,LoadAddress,Size);
                }
            }
           else 
           {
                if(EncryptionData.OpKeyUsing == 1)
                {
                    Status = FDevcPs_encryptDownload_SM4_UseOp(EncryptedKeyIv,LoadAddress,Size);
                }
                else
                {
                    Status = FDevcPs_encryptDownload_SM4_NoOp(EncryptedKeyIv,LoadAddress,Size);
                }
            }
        }
        else
        {
            Status = FDevcPs_download_non_secure(LoadAddress,Size);
        }
        
        if(Status != FMSH_SUCCESS)
        {
           LOG_OUT(DEBUG_INFO,"Bitstream download failed!!! \r\n"); 
           if(freqFlag!=0)
              FDevcPs_recoverFreq();
           goto END;
        }
     
        if(freqFlag!=0)
        {
           Status=FDevcPs_recoverFreq();
           if(Status != FMSH_SUCCESS)
           {
              LOG_OUT(DEBUG_INFO,"Frequency recover failed!!! \r\n"); 
              goto END;
           }
        }
        
#if DEBUG_PERF
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(1,"Download the PL bitstream is done at the %f ms\r\n",gtc_time);
#endif   

        goto END;
        
   }
   else
   {  
       /*
        * sha256 verification
        */ 
       if ((PartitionHeader->PartitionAttributes & IH_PH_ATTRB_CHECKSUM_MASK) != IH_PH_ATTRB_NOCHECKSUM )
       {       
           LoadAddress = PartitionHeader->DestinationLoadAddress;
           Status = FmshFsbl_CalcualteSHA(BootInstancePtr, LoadAddress,PartitionNum);
           if (Status != FMSH_SUCCESS) {
                BootInstancePtr->ErrorCode = PARTITION_CHECKSUM_ERROR;    
                TRACE_OUT(DEBUG_OUT,"FSBL_ERROR_PARTITION_CHECKSUM_FAILED \r\n");       
                Status = PARTITION_CHECKSUM_ERROR;
                goto END;
	   }
           LOG_OUT(DEBUG_INFO,"PS partition SHA verify success!!! \r\n");

       } 
       
       /*trigger KEY rolling feature */
        if (IsEncrypted == TRUE) 
        {
            LOG_OUT(DEBUG_INFO,"Preparing Key Rolling Decryption !! \r\n"); 
            Status =  FmshFsbl_Secure_AesDecrypt(&EncryptionData);
            if(Status !=FMSH_SUCCESS)
            {
                BootInstancePtr->ErrorCode = DECYPTION_FAILURE;
                LOG_OUT(DEBUG_INFO,"FSBL Decryption failed!!! \r\n"); 
                goto END;
            }
            
            LOG_OUT(DEBUG_INFO,"FSBL Decryption success!!! \r\n");
        }
        CpuNo = BootInstancePtr->HandoffCpuNo;
        if (FmshFsbl_CheckHandoffCpu(BootInstancePtr,DestinationCpu) == FMSH_SUCCESS)
	{
            BootInstancePtr->HandoffValues[CpuNo].CpuSettings = DestinationCpu | ExecState;
	    BootInstancePtr->HandoffValues[CpuNo].HandoffAddress = PartitionHeader->DestinationExecutionAddress;
	    BootInstancePtr->HandoffCpuNo += 1U;
	}
#if DEBUG_PERF 
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Download the Application is done at the %f ms\r\n",gtc_time);
#endif
   }

END:
  
	return Status;
	

}


/******************************************************************************
 * This function loads the partition
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @param	PartitionNum is the partition number in the image to be loaded
 *
 * @return	returns the error codes described in error.h on any error
 * 			returns SUCCESS on success
 *
 *****************************************************************************/
u32 FmshFsbl_PartitionLoad(BootPs * BootInstancePtr,u32 PartitionNum)
{
    u32 Status;

   /**
    * Partition Header Validation
    */
    
    LOG_OUT(DEBUG_INFO,"Partition header validate......\r\n"); 
    Status = FmshFsbl_PartitionHeaderValidation(BootInstancePtr,PartitionNum);
    if (FMSH_SUCCESS != Status)
    {
        LOG_OUT(DEBUG_INFO,"Partition header validate failure!!\r\n"); 
        goto END;
    }
    LOG_OUT(DEBUG_INFO,"Partition header validate SUCCESS!!\r\n"); 

#if DEBUG_PERF
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Partition header validate is done at the %f ms\r\n",gtc_time);
#endif
    
   /**
    * Partition Copy
    */ 
     LOG_OUT(DEBUG_INFO,"Prepare Copy Partition......\r\n"); 
     Status = FmshFsbl_PartitionCopy(BootInstancePtr,PartitionNum);
     if (FMSH_SUCCESS != Status){
        goto END;
     }
     
#if DEBUG_PERF 
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Copy Partition is done at the %f ms\r\n",gtc_time);
#endif
    
   /**
    * Partition Validation
    */
    Status = FmshFsbl_PartitionValidation(BootInstancePtr,PartitionNum);
    if (FMSH_SUCCESS != Status)
    {
	goto END;
    }
    
#if FSBL_WDT_ENABLE
    FWdtPs_restart(&g_WDT);
#endif
    
END:
	return Status;
}
