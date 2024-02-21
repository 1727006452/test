/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_header.c
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

/************************** Variable Definitions *****************************/
extern BootPs BootInstance;
extern FQspiPs_T* QspiInstancePtr;

Ps_BootHeader BootHeaderInfo;
u8 ImageHeaderBuffer[8*1024]__attribute__ ((aligned (4)));
u8 BootHeaderBuffer[8*1024]__attribute__ ((aligned (4)));
u8 *ImageHdr = ImageHeaderBuffer;
u8 *BootHdr = BootHeaderBuffer;
u32 BootHeaderSize;

 /***************** Macros (Inline Functions) Definitions *********************/
 /*This function validates the image identification in boot header.*/
static u32 ValidateImageID(Ps_BootHeader *Header)
{
    if ( (Header->ImageId != XLNX_IMAGE_ID) && (Header->ImageId != FMSH_IMAGE_ID ))
     /*0x584c4e58*//*0x464d5348*/{
        LOG_OUT(DEBUG_INFO,"Error: Image Identification 0x%8.8x != 0x%8.8x\r\n",
			Header->ImageId ,"XLNX");
        return FMSH_FAILURE; 
    }
    LOG_OUT(DEBUG_INFO,"Image ID verified success!!!\r\n");
    
    return FMSH_SUCCESS; 
}

/**
 *  This function is used to validate the word checksum for the image header
 *  table and partition headers.
 */
static u32 ValidateChecksum(u32 Buffer[], u32 Length)
{
	u32 Status;
	u32 Checksum=0U;
	u32 Count;

	/**
	 * Length has to be atleast equal to 2,
	 */
	if (Length < 2U)
	{
              Status = FMSH_FAILURE;
              goto END;
	}

	/**
	 * Checksum = ~(X1 + X2 + X3 + .... + Xn)
	 * Calculate the checksum
	 */
	for (Count = 0U; Count < (Length-1U); Count++) 
        {
	        /**
		 * Read the word from the header
		 */
              Checksum += Buffer[Count];
	}

	/**
	 * Invert checksum
	 */
	Checksum ^= 0xFFFFFFFF;

	/**
	 * Validate the checksum
	 */
	if (Buffer[Length-1U] != Checksum) 
        {
             LOG_OUT(DEBUG_INFO,"Error: Calculated Checksum 0x%0lx !=  %0lx located in BootROM Header\r\n",Checksum, Buffer[Length-1U]);
             Status =  FMSH_FAILURE;
	}
	else
	{
            LOG_OUT(DEBUG_INFO,"Checksum verified success!!!\r\n");
            Status = FMSH_SUCCESS;
	}

END:
	return Status;
}
/**
*  This function checks the fields of the image header table and validates
*  them. Image header table contains the fields that common across the
*  partitions and for image
*
* @param ImageHeaderTable pointer to the image header table.
*
* @return
* 	- SUCCESS on successful image header table validation
* 	- errors as mentioned in error.h
*
* @note
*
*****************************************************************************/
static u32 ValidateImageHeaderTable(Ps_ImageHeaderTable * ImageHeaderTable)
{
  
    u32 Status=FMSH_SUCCESS;
    u32 EfuseCtrl = 0;
    u32 AcOffset=0U;
    u32 Size=0;
    u32 ImageHeaderAddr=0;
   
   /**
    * Check the check sum of the image header table
    */
    Status = ValidateChecksum((u32 *)ImageHeaderTable,IH_IHT_LEN/4U);
    if (FMSH_SUCCESS != Status)
    {
        BootInstance.ErrorCode = IMG_HEADER_CHECKSUM_ERROR;
	LOG_OUT(DEBUG_INFO,"Checksum validate failed!!!\r\n");
	goto END;
    }
    LOG_OUT(DEBUG_INFO,"Checksum validate success!!!\r\n");

   /**
    * Read the Image Header Table offset from Boot Header
    */
    Status = BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress + IH_BH_IH_TABLE_OFFSET, 
                                                    (u32)&ImageHeaderAddr,  4U);
    if (Status != FMSH_SUCCESS) 
    {
        LOG_OUT(DEBUG_INFO,"Copy Image Header Table Offset Word failed\r\n");
        goto END;
    }
    /**
     * Read Efuse bit and check Boot Header for Authentication
     */
    EfuseCtrl = ReadReg(EFUSE_SEC_CTRL);
    if (((FmshFsbl_FindOneInNumber(EfuseCtrl)&0x7fff) > 0x8) || ((BootHeaderInfo.ImageAttr & IH_BH_IMAGE_ATTRB_RSA_MASK) == IH_BH_IMAGE_ATTRB_RSA_MASK) )
    {
         LOG_OUT(DEBUG_INFO,"RSA Authentication Enabled,Preparing Boot Image Header Authentication...... \r\n");
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
         
         /** 
          *  Authenticate the image header , Read AC offset from Image header table 
          */ 
         Status = BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress + ImageHeaderAddr +
                IH_IHT_AC_OFFSET, (u32)(&AcOffset), IH_FIELD_LEN);
         if (FMSH_SUCCESS != Status)
         {
           LOG_OUT(DEBUG_INFO,"Device Copy Image Header AC Offset Word Failed \n\r");
           goto END;
         } 
         /*Check if AC offset equal zero*/
         if(AcOffset == 0)
         {
             //BootInstance.ErrorCode = NO_IMG_HEADER_AC_ERROR;
             LOG_OUT(DEBUG_INFO,"ERROR: RSA authentication certificate dose not exist in boot image...... \r\n");
             Status = FMSH_FAILURE;
             goto END;
         }
         /*Calculate Boot Header Size */
         BootHeaderSize = ImageHeaderAddr;
         
         /* Copy the Boot header to OCM */
         Status = BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress,(UINTPTR)BootHdr, BootHeaderSize);
         if (Status != FMSH_SUCCESS) 
         {
            LOG_OUT(DEBUG_INFO,"Copy Boot Header failed\r\n");
            goto END;
         }
         
        /*Total size of Image header may vary depending on padding so  size = AC address - Start address;*/
         //imageHeader+rsa
         Size = (AcOffset * IH_PARTITION_WORD_LENGTH)-ImageHeaderAddr + RSA_CERTIFICATE_SIZE;
         
         /* Copy the Image header and AC to OCM */
         Status = BootInstance.DeviceOps.DeviceCopy(BootInstance.ImageOffsetAddress +\
                    ImageHeaderAddr,(UINTPTR)ImageHdr, Size);
         if (Status != FMSH_SUCCESS) 
         {
             LOG_OUT(DEBUG_INFO,"Copy Image Header and AC  Failed\r\n");
             goto END;
         }
         //imageHeader
         Size -=  RSA_CERTIFICATE_SIZE;
         
         AcOffset =(u32)ImageHdr + Size;
         //SIZE: imageHeader- 0xA40-0X8C0=0X180
         //AcOffset: Ptr->imageHeader RSA
         Status = FmshFsbl_AuthenticateHeader((u32)ImageHdr,Size,AcOffset);    
         if (Status != FMSH_SUCCESS)
         {
           BootInstance.ErrorCode = BOOT_HEADER_SIGNATURE_VERIFY_FAILED;
           goto END;
         }               
     }
           

	/**
	 * Print the Image header table details
	 * Print the Bootgen version
	 */
	LOG_OUT(DEBUG_INFO,"Image Header Table Details\r\n");
	LOG_OUT(DEBUG_INFO,"Boot Gen Ver: 0x%0lx \r\n",ImageHeaderTable->Version);
	LOG_OUT(DEBUG_INFO,"Number of Partitions: 0x%0lx \r\n",ImageHeaderTable->NoOfPartitions);
	LOG_OUT(DEBUG_INFO,"Partition Header Address: 0x%0lx \r\n",ImageHeaderTable->PartitionHeaderAddress);
	LOG_OUT(DEBUG_INFO,"Partition Present Device: 0x%0lx \r\n",ImageHeaderTable->PartitionPresentDevice);

END:
	return Status;
}

static u32 CheckValidMemoryAddress(u32 Address, u32 CpuId, u32 DevId)
{
	u32 Status = FMSH_SUCCESS;

	/* Check if Address is in the range of DDR */
#ifdef FSBL_PS_DDR
	/**
	 * Check if Address is in the range of PS DDR
	 */
	if ((Address >= FSBL_PS_DDR_START_ADDRESS) &&
	     (Address <= FSBL_PS_DDR_END_ADDRESS) )
	{
		goto END;
	}  
#endif

#ifdef FSBL_PL_DDR
	/**
	 * Check if Address is in the range of PS DDR
	 */
	if ((Address >= FSBL_PL_DDR_START_ADDRESS) &&
	     (Address < FSBL_PL_DDR_END_ADDRESS) )
	{
		goto END;
	}
#endif

	 /**
         * Check if Address is in the range of last bank of AHB
         */
        if ((DevId == IH_PH_ATTRB_DEST_DEVICE_PS) &&
              (Address >= FPS_AHB_SRAM_BASEADDR) )
        {
                goto END;
        }
        
        /**
         * Check if Address is in the range of last bank of OCM
         */
        if ((DevId == IH_PH_ATTRB_DEST_DEVICE_PS) &&
              (Address >= PS_OCM_START_ADDRESS) &&
                  (Address < PS_OCM_END_ADDRESS) )
        {
                goto END;
        }

        /**
         * If destination device is PL and load address is not configured,
         * don't consider this as error as we will use temp load address
         * to load PL bitstream
         */
        if ((DevId == IH_PH_ATTRB_DEST_DEVICE_PL) &&
			(Address == 0xffffffff))
        {
		goto END;
        }

	/**
	 * Not a valid address
	 */
	Status = ERROR_INVALID_EXCUTION_ADDRESS;
        BootInstance.ErrorCode = ERROR_INVALID_EXCUTION_ADDRESS;
	LOG_OUT(DEBUG_INFO,
		"FMSH_FSBL_ERROR_ADDRESS: %llx\n\r", Address);
END:
	return Status;
}


/**
 * This function reads the image header from flash device. Image header
 * reads the image header table and partition headers
 *
 * @param ImageHeader pointer to the image header at which image header to
 * be stored
 *
 * @param DeviceOps pointer deviceops structure which contains the
 * function pointer to flash read function
 * 		 If it is NULL then image header will be copied from OCM buffer,
 * 		 which has image header copied from flash device.
 *
 * @param FlashImageOffsetAddress base offset to the image in the flash
 *
 * @return
 * 	- SUCCESS on successful reading image header
 * 	- errors as mentioned in fmsh_error.h
 *
 * @note
 *****************************************************************************/
static u32 ValidatePartitionHeader(Ps_PartitionHeader * PartitionHeader,u32 RunningCpu)
{
    u32 Status = FMSH_SUCCESS;
    u32 BootMode;
    u8 IsEncrypted=FALSE;
    u8 IsAuthenticated=FALSE;
    u32 DestinationCpu=0U;
    u32 DestinationDevice=0;
    
    if (FmshFsbl_IsEncryptedPresent(PartitionHeader) == IH_PH_ATTRB_ENCRYPTION )
    {
	IsEncrypted = TRUE;
        LOG_OUT(DEBUG_INFO,"Partition is encrypted, enter secure mode ......\r\n");
    }
    else
    {
	IsEncrypted = FALSE;
        LOG_OUT(DEBUG_INFO,"Partition is unencrypted......\r\n");
    }

    if (FmshFsbl_IsRsaSignaturePresent(PartitionHeader) == IH_PH_ATTRB_RSA_SIGNATURE )
    {
	IsAuthenticated = TRUE;
        LOG_OUT(DEBUG_INFO,"Partition is Authenticated, enter secure mode......\r\n");
    }
    else
    {
	IsAuthenticated = FALSE;
        LOG_OUT(DEBUG_INFO,"Partition is unauthenticated......\r\n");
    }
    
    DestinationCpu = FmshFsbl_GetDestinationCpu(PartitionHeader);
    /* if destination cpu is not present, it means it is for same cpu */
    if (DestinationCpu == IH_PH_ATTRB_DEST_CPU_NONE)
    {
	DestinationCpu = RunningCpu;
    }
    DestinationDevice = FmshFsbl_GetDestinationDevice(PartitionHeader);
    
    /**
     * check for XIP image - partition lengths should be zero
     * execution address should be in QSPI
     */
    if (PartitionHeader->UnEncryptedDataWordLength == 0U)
    {
        LOG_OUT(DEBUG_INFO,"FSBL will execute in place\r\n");
	if ((IsAuthenticated ==   TRUE ) || (IsEncrypted ==	TRUE ))
	{
            Status = ERROR_XIP_AUTH_ENC_PRESENT;
            BootInstance.ErrorCode = ERROR_XIP_AUTH_ENC_PRESENT;
	    LOG_OUT(DEBUG_INFO,"ERROR_XIP_AUTH_ENC_PRESENT\r\n");
	    goto END;
	}
        BootMode =  BootInstance.BootMode;      
        if(BootMode == QSPI_BOOT_MODE)
        {
            if ((PartitionHeader->DestinationExecutionAddress
				< QSPI_LINEAR_BASE_ADDRESS_START) ||
				(PartitionHeader->DestinationExecutionAddress
					> QSPI_LINEAR_BASE_ADDRESS_END))
	    {
		Status = ERROR_APU_XIP_EXCUTION_ADDRESS;
                BootInstance.ErrorCode = ERROR_APU_XIP_EXCUTION_ADDRESS;
		LOG_OUT(DEBUG_INFO,"ERROR_APU_XIP_EXCUTION_ADDRESS\r\n");
		goto END;
	    }
            WriteReg( CSU_EXCUTION_MODE_ADDR,NON_SECURE_APU_EXECUTE_MODE);
            WriteReg( APU_EXCUTION_ADDR,PartitionHeader->DestinationExecutionAddress);
            
            /* Re-initialize QSPI xip MODE */     
            FQspiPs_EnterXIP(QspiInstancePtr,DIOR_CMD);
        }
        if(BootMode == NOR_BOOT_MODE)
        {
            if ((PartitionHeader->DestinationExecutionAddress
				< NOR_LINEAR_BASE_ADDRESS_START) ||(PartitionHeader->DestinationExecutionAddress
					> NOR_LINEAR_BASE_ADDRESS_END))
            {
                Status = ERROR_APU_XIP_EXCUTION_ADDRESS;
                BootInstance.ErrorCode = ERROR_APU_XIP_EXCUTION_ADDRESS;
                LOG_OUT(DEBUG_INFO,"ERROR_APU_XIP_EXCUTION_ADDRESS\r\n");
                goto END;
            }
 
        }
        
    }
    /**
     * check for authentication and encryption length
     */
     if ((IsAuthenticated == FALSE ) && (IsEncrypted == FALSE ))
     {
	/**
	 * all lengths should be equal
	 */
	 if ((PartitionHeader->UnEncryptedDataWordLength !=
			PartitionHeader->EncryptedDataWordLength) ||
				(PartitionHeader->EncryptedDataWordLength !=
					PartitionHeader->TotalDataWordLength))
	{
            Status = MISMATCH_FSBL_LENGTH;
            BootInstance.ErrorCode = MISMATCH_FSBL_LENGTH;
	    LOG_OUT(DEBUG_INFO,"ERROR_PARTITION_LENGTH\r\n");
	    goto END;
	}
        if((PartitionHeader->DestinationLoadAddress==0) && (DestinationDevice == IH_PH_ATTRB_DEST_DEVICE_PS))
        {
            Status = PARTITION_SKIP_LOAD;
	    LOG_OUT(DEBUG_INFO,"Skip load this partition!!\r\n");
	    goto END;
        }
     } 
     else if ((IsAuthenticated == TRUE ) && (IsEncrypted == FALSE ))
     {
	/**
         * TotalDataWordLength should be more
	 */
	if ((PartitionHeader->UnEncryptedDataWordLength !=
		       PartitionHeader->EncryptedDataWordLength) ||
				(PartitionHeader->EncryptedDataWordLength >=
					PartitionHeader->TotalDataWordLength))
	{
             Status = MISMATCH_FSBL_LENGTH;
             BootInstance.ErrorCode = MISMATCH_FSBL_LENGTH;
	     LOG_OUT(DEBUG_INFO,"ERROR_PARTITION_LENGTH\r\n");
	     goto END;
	}
     } 
     else if ((IsAuthenticated == FALSE ) &&(IsEncrypted == TRUE ))
     {
	/**
	 * EncryptedDataWordLength should be more
	 */
	 if ((PartitionHeader->UnEncryptedDataWordLength >=
			PartitionHeader->EncryptedDataWordLength) ||
				(PartitionHeader->EncryptedDataWordLength !=
					PartitionHeader->TotalDataWordLength))
	{
             Status = MISMATCH_FSBL_LENGTH;
             BootInstance.ErrorCode = MISMATCH_FSBL_LENGTH;
	     LOG_OUT(DEBUG_INFO,"ERROR_PARTITION_LENGTH\r\n");
	     goto END;
	}
     } 
     else /* Authenticated and Encrypted */
     {
	/**
	 * TotalDataWordLength should be more
	 */
	 if ((PartitionHeader->UnEncryptedDataWordLength >=
			PartitionHeader->EncryptedDataWordLength) ||
				(PartitionHeader->EncryptedDataWordLength >=
					PartitionHeader->TotalDataWordLength))
	{
             Status = MISMATCH_FSBL_LENGTH;
             BootInstance.ErrorCode = MISMATCH_FSBL_LENGTH;
	     LOG_OUT(DEBUG_INFO,"ERROR_PARTITION_LENGTH\r\n");
	     goto END;
	}
    }
 
   Status=CheckValidMemoryAddress(PartitionHeader->DestinationLoadAddress,DestinationCpu, DestinationDevice);
   if(FMSH_SUCCESS==Status)
   {
      goto END;
   }
   
END:  
       /**
        * Print Partition Header Details
        */
        LOG_OUT(DEBUG_INFO,"UnEncrypted data Length: 0x%0lx \r\n",
				PartitionHeader->UnEncryptedDataWordLength);
	LOG_OUT(DEBUG_INFO,"Data word offset: 0x%0lx \r\n",
				PartitionHeader->EncryptedDataWordLength);
	LOG_OUT(DEBUG_INFO,"Total Data word length: 0x%0lx \r\n",
				PartitionHeader->TotalDataWordLength);
	LOG_OUT(DEBUG_INFO,"Destination Load Address: 0x%0lx \r\n",
			(UINTPTR)PartitionHeader->DestinationLoadAddress);
	LOG_OUT(DEBUG_INFO,"Execution Address: 0x%0lx \r\n",
			(UINTPTR)PartitionHeader->DestinationExecutionAddress);
	LOG_OUT(DEBUG_INFO,"Data word offset: 0x%0lx \r\n",
				PartitionHeader->DataWordOffset);
	LOG_OUT(DEBUG_INFO,"Partition Attributes: 0x%0lx \r\n",
				PartitionHeader->PartitionAttributes);
    
        return Status;
    
}
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
u32 FmshFsbl_GetPartitionOwner(const Ps_PartitionHeader * PartitionHeader)
{
        return PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_PART_OWNER_MASK;

}
 
u32 FmshFsbl_IsRsaSignaturePresent(const Ps_PartitionHeader * PartitionHeader)
{
        return  PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_RSA_SIGNATURE_MASK;
}

u32 FmshFsbl_GetChecksumType(Ps_PartitionHeader * PartitionHeader)
{
        return PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_CHECKSUM_MASK;
}

u32 FmshFsbl_GetDestinationCpu(Ps_PartitionHeader * PartitionHeader)
{
        return PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_DEST_CPU_MASK;
}

u32 FmshFsbl_IsEncryptedPresent(const Ps_PartitionHeader * PartitionHeader)
{
        return PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_ENCRYPTION_MASK;
}

u32 FmshFsbl_GetDestinationDevice(const Ps_PartitionHeader * PartitionHeader)
{
        return PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_DEST_DEVICE_MASK;
}

u32 FmshFsbl_GetExecState(const Ps_PartitionHeader * PartitionHeader)
{
        return PartitionHeader->PartitionAttributes &
                                IH_PH_ATTRB_EXEC_ST_MASK;
}


u32 FmshFsbl_ValidataBootHeader(Ps_BootHeader *Header)
{
    u32 Status = FMSH_SUCCESS;
   
    Status = ValidateImageID (Header);
    if (Status != FMSH_SUCCESS) 
    {
        return FMSH_FAILURE;
    } 

    Status =  ValidateChecksum ((u32*)Header, sizeof(Ps_BootHeader)/4U); 
    if (Status != FMSH_SUCCESS) 
    {
        return FMSH_FAILURE;
    }

    return Status;
  
}

u32 FmshFsbl_PartitionHeaderValidation(BootPs * BootInstance,u32 PartitionNum)
{
      u32 Status;
      Ps_PartitionHeader * PartitionHeader;
      
     /**
      * Assign the partition header to local variable
      */
      PartitionHeader = &(BootInstance->ImageHeader.PartitionHeader[PartitionNum]);

     /**
      * Check the check sum of the partition header
      */
      Status = ValidateChecksum( (u32 *)PartitionHeader, IH_PH_LEN/4U);
      if (FMSH_SUCCESS != Status)
      {
          BootInstance->ErrorCode = ERROR_PH_CHECKSUM_FAILED;
          LOG_OUT(DEBUG_INFO,"Partition Header Checksum Verify Failure!\r\n");
          goto END;
      }
   
      
     /**
      * Check if partition belongs to FSBL
      */
      if (FmshFsbl_GetPartitionOwner(PartitionHeader) !=
			IH_PH_ATTRB_PART_OWNER_FSBL)
     {
	/**
	 * If the partition doesn't belong to FSBL, skip the partition
	 */
	 LOG_OUT(DEBUG_INFO,"Skipping the Partition 0x%0lx\n",
				PartitionNum);
	 Status = ERROR_NOT_PARTITION_OWNER;
         BootInstance->ErrorCode = ERROR_NOT_PARTITION_OWNER;
	 goto END;
      }
     
	/**
	 * Validate the fields of partition
	 */
	Status = ValidatePartitionHeader(PartitionHeader,BootInstance->ProcessorID);
	if (FMSH_SUCCESS != Status)
	{
		goto END;
	}

END:
	return Status;
}


/****************************************************************************
 * This function reads the image header from flash device. Image header
 * reads the image header table and partition headers
 *
 * @param ImageHeader pointer to the image header at which image header to
 * be stored
 *
 * @param DeviceOps pointer deviceops structure which contains the
 * function pointer to flash read function
 *
 * @param FlashImageOffsetAddress base offset to the image in the flash
 *
 * @return
 * 	- FMSH_SUCCESS on successful reading image header
 * 	- errors as mentioned in fmsh_error.h
 *
 * @note
 *****************************************************************************/
u32 FmshFsbl_ReadImageHeader(Ps_ImageHeader* ImageHeader,Ps_DeviceOps* DeviceOps,u32 FlashImageOffsetAddress)
{
	u32 Status = FMSH_SUCCESS;
	u32 ImageHeaderTableAddressOffset=0U;
	u32 PartitionHeaderAddress=0U;
	u32 PartitionIndex=0U;

	/**
	 * Read the Image Header Table offset from
	 * Boot Header
	 */
	Status = DeviceOps->DeviceCopy(FlashImageOffsetAddress
	            + 0x98,
		   (u32) &ImageHeaderTableAddressOffset, 4);
	if (FMSH_SUCCESS != Status)
	{
		LOG_OUT(DEBUG_INFO,"Device Copy Failed \n\r");
		goto END;
	}
	LOG_OUT(DEBUG_INFO,"Image Header Table Offset 0x%0lx \n\r",
			ImageHeaderTableAddressOffset);

	/**
	 * Read the Image header table of 64 bytes
	 * and update the image header table structure
	 */
	Status = DeviceOps->DeviceCopy(FlashImageOffsetAddress +
				    ImageHeaderTableAddressOffset,
				    (u32 ) &(ImageHeader->ImageHeaderTable),
				    64);
	if (FMSH_SUCCESS != Status)
	{
		LOG_OUT(DEBUG_INFO,"Device Copy Failed \n\r");
		goto END;
	}


	/**
	 * Check the validity of Image Header Table
	 */
	Status = ValidateImageHeaderTable(&(ImageHeader->ImageHeaderTable));
	if (FMSH_SUCCESS != Status)
	{
		LOG_OUT(DEBUG_INFO,"Image Header Table "
				"Validation failed \n\r");
		goto END;
	}

	/**
	 * Update the first partition address
	 */
	PartitionHeaderAddress =
		(ImageHeader->ImageHeaderTable.PartitionHeaderAddress)
			* 4;

	/**
	 * Read the partitions based on the partition offset
	 * and update the partition header structure
	 */
	for (PartitionIndex=0U;
                PartitionIndex<ImageHeader->ImageHeaderTable.NoOfPartitions;
                    PartitionIndex++)
	{

		/**
		 * Read the Image header table of 64 bytes
		 * and update the image header table structure
		 */
		Status =  DeviceOps->DeviceCopy(
	                FlashImageOffsetAddress + PartitionHeaderAddress,
		(u32 ) &(ImageHeader->PartitionHeader[PartitionIndex]),
			64);
		if (FMSH_SUCCESS != Status)
		{
			LOG_OUT(DEBUG_INFO,"Device Copy Failed \n\r");
			goto END;
		}

		/**
		 * Update the next partition present address
		 */
		PartitionHeaderAddress =
		 (ImageHeader->PartitionHeader[PartitionIndex].NextPartitionOffset)
			  * 4;
	}


END:
	return Status;
}
