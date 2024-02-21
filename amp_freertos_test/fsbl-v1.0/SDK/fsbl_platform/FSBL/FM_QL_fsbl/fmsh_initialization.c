/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_initialization.c
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
* 0.01   lq  11/23/2018  First Release
* 0.02   lq  12/04/2020  Add PSS_IDCODE_DEVICE_7045AI IDCode
* 0.03   lq  12/09/2021  Add build data and procise version in the FmshFsbl_SystemInit() function.
* 0.04   lq  03/11/2021  Enable i-cache.
* 0.05   hzq 01/06/2022  Fix FmshFsbl_BootHeaderSearchAndValidate that QSPI encount a problem in XIP
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include "boot_main.h"

/************************** Constant Definitions *****************************/
#define PHY_ITF_GMIIMII  0
#define PHY_ITF_RGMII  1
/**************************** Type Definitions *******************************/
typedef enum _FGmacPs_PathSel{
        gmac_path_gmii=PHY_ITF_GMIIMII,
        gmac_path_rgmii=PHY_ITF_RGMII
 }FGmacPs_ITF_Type;

/***************** Macros (Inline Functions) Definitions *********************/
static u32 FmshFsbl_SystemInit(BootPs * BootInstancePtr);
static u32 FmshFsbl_BootDeviceInit(BootPs * BootInstancePtr);

/************************** Variable Definitions *****************************/
extern FQspiPs_T* QspiInstancePtr;
extern BootPs BootInstance;
extern Ps_BootHeader BootHeaderInfo;

/************************** Function Prototypes ******************************/

static void FGmacPs_InitPhy()
{
    u32 reg;

#ifdef ENETPS_0_DEVICE_ID 
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x008,0xDF0D767BU); /* unlock */
    /* selct phy interface */
    reg=FMSH_ReadReg(FPS_SLCR_BASEADDR,0x414);
    (reg)&=~(7);//RESET_BIT(reg,7);
    reg|=FPAR_GMACPS_0_INTERFACE;  /* gmac 0 */
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x414,reg);
    /* reset */
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x270,0x0fU);
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x270,0x00U);
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x004,0xDF0D767BU); /* lock */
#endif
    
#ifdef ENETPS_1_DEVICE_ID
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x008,0xDF0D767BU); /* unlock */
    /* selct phy interface */
    reg=FMSH_ReadReg(FPS_SLCR_BASEADDR,0x414);
    (reg)&=~(7<<4);//RESET_BIT(reg,7<<4);
    reg|=(FPAR_GMACPS_1_INTERFACE<<4);  /* gmac 1 */
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x414,reg);
    /* reset */
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x270,0xF<<5);
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x270,0x00U);
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x004,0xDF0D767BU); /* lock */
#endif
}
/******************************************************************************

 * This function initializes the system using the psu_init()
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @return	returns the error codes described in error.h on any error
 * 			returns FMSH_SUCCESS on success
 *
 ******************************************************************************/
static u32 FmshFsbl_SystemInit(BootPs * BootInstancePtr)
{
    // Get the PS_VERSION on run time
    int Status=FMSH_SUCCESS;
    u32 BootModeRegister = 0;  
    u32 RebootStatus = 0;
    u32 PreviousSecureMode = 0;
    
    Status=ps_init();
    if(Status!=PS_INIT_SUCCESS)
       return Status; 
    
    FGmacPs_InitPhy();
    
    icache_enable();
    
#if     FSBL_WDT_ENABLE
     FmshFsbl_WdtInit();
#endif     
     
#if    DEBUG_INFO|DEBUG_PERF
     Status=FmshFsbl_UartInit();
     if(Status!=FMSH_SUCCESS)
        return Status;
     LOG_OUT(DEBUG_INFO,"====PSOC FSBL BOOTING ....... ====\r\n");
     LOG_OUT(DEBUG_INFO,"====FSBL Version: %3.2f ====\r\n",BootInstance.Version);
     LOG_OUT(DEBUG_INFO,"====Procise SVN Version: %d  Release Date: %s====\r\n",PROCISE_SVN_VERSION,PROCISE_RELEASE_DATE);
     //LOG_OUT(DEBUG_INFO,"====Build Date: %s  %s ====\r\n",__DATE__,__TIME__);
     LOG_OUT(DEBUG_INFO,"======= In BootStage 1 ======= \r\n");
     LOG_OUT(DEBUG_INFO,"====UART initialized success!!!====\r\n");
#endif

#if     DEVC_QSPI_DMA_ENABLE     
    Status=FGicPs_SelfTest(&IntcInstance);
    if(Status!=FMSH_SUCCESS)
        return Status;
#endif
    
    /* 
     ** Read boot mode register to konow which boot device   
     */ 
    BootModeRegister = ReadReg(BOOT_MODE_REG);
    LOG_OUT(DEBUG_INFO,"BootMode Register is : 0x%08x\r\n",BootModeRegister);
    BootInstancePtr->BootMode = BootModeRegister;

#if MMC_SUPPORT
    if(((BootInstancePtr->BootMode)&BOOT_MODES_MASK)==QSPI_BOOT_MODE)
    {
        BootInstancePtr->BootMode&=(~BOOT_MODES_MASK);
        BootInstancePtr->BootMode|=MMC_BOOT_MODE;
    }
#endif    
    
    /* get PS reset reason via reading reboot status register */  
     RebootStatus = ReadReg(REBOOT_STATUS_REG)&PS_RST_REASON_MASK;
     PreviousSecureMode = (ReadReg (PCAP_CTRL_REG)&SECURE_MODE_MASK) >> 7U;
    
     if (RebootStatus == REBOOT_NON_POR) 
     {
        LOG_OUT(DEBUG_INFO,"PS was reset by Non_POR!!!\r\n");
        BootInstancePtr->ResetReason=REBOOT_NON_POR;
        if (PreviousSecureMode == 0U) 
        {
           LOG_OUT(DEBUG_INFO,"PS was booted nonsecurely last time,Non Secure mode is set!\r\n");
           BootInstancePtr->SecureModeFlag = 0;
        }
     }
     else if (RebootStatus == REBOOT_POR)
     {
        LOG_OUT(DEBUG_INFO,"PS was reset by POR,Non_secure mode is set!\r\n");
        BootInstancePtr->SecureModeFlag = 0;
     }
     else 
     {
        Status = FMSH_FAILURE;
        LOG_OUT(DEBUG_INFO,"RebootStatus Error\r\n");
     }
     
     return Status;
}

/*****************************************************************************
 * This function initializes the processor and updates the cluster id
 * which indicates CPU on which fsbl is running
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @return	returns the error codes described in fmsh_error.h on any error
 * 			returns FMSH_SUCCESS on success
 *
 ******************************************************************************/
static u32 FmshFsbl_ProcessorInit(BootPs * BootInstancePtr)
{
	u32 Status = FMSH_SUCCESS;
	u32 ClusterId=0U;

	/**
	 * Read the cluster ID and Update the Processor ID
	 * Initialize the processor settings that are not done in
	 * BSP startup code
	 */
	ClusterId = ReadReg(PSS_IDCODE_REG);

        LOG_OUT(DEBUG_INFO,"Cluster ID 0x%x\n\r", ClusterId);

	/* store the processor ID based on the cluster ID */
	if( (ClusterId & PSS_IDCODE_DEVICE_MASK) == PSS_IDCODE_DEVICE_7045 || (ClusterId & PSS_IDCODE_DEVICE_MASK) == PSS_IDCODE_DEVICE_7010 \
           || (ClusterId & PSS_IDCODE_DEVICE_MASK) == PSS_IDCODE_DEVICE_7020 || (ClusterId & PSS_IDCODE_DEVICE_MASK) == PSS_IDCODE_DEVICE_7030 \
             || (ClusterId & PSS_IDCODE_DEVICE_MASK) == PSS_IDCODE_DEVICE_7045AI )
        {
            LOG_OUT(DEBUG_INFO,"Running on A7-0\r\n");
            BootInstancePtr->ProcessorID =ATTRB_DEST_CPU_A7_0;
	} 
        else
        {
            LOG_OUT(DEBUG_INFO,"Invalid ID!\r\n");
            BootInstancePtr->ErrorCode=INVALID_ID;
            Status=FMSH_FAILURE;
        }
        
	return Status;
}

/******************************************************************************
 * This function is initializes the processor and system.
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @return
 *          - returns the error codes described in error.h on any error
 * 			- returns SUCCESS on success
 *
 *****************************************************************************/
u32 FmshFsbl_BootInitialize(BootPs * BootInstancePtr)
{
    u32 Status = FMSH_SUCCESS;
    
    /* Configure the system as in PSU */
    Status = FmshFsbl_SystemInit(BootInstancePtr);
    if (FMSH_SUCCESS != Status) 
    {
	goto END;
    }
    
    /* Initialize the processor */
    Status = FmshFsbl_ProcessorInit(BootInstancePtr);
    if (FMSH_SUCCESS != Status) 
    {
	goto END;
    }
    
#if DEBUG_PERF
      gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
      gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
      LOG_OUT(DEBUG_PERF,"Boot Initialize is done at the %f ms\r\n",gtc_time);
#endif      
      
END:
    return Status;

}

/******************************************************************************
 * This function initializes the primary boot device
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @return	returns the error codes described in error.h on any error
 * 			returns SUCCESS on success
 *
 ******************************************************************************/
static u32 FmshFsbl_BootDeviceInit(BootPs * BootInstancePtr)
{
    u32 Status = FMSH_SUCCESS;
    u32 BootDevice;

   /**
    * Read Boot Mode register and update the value
    */
    BootDevice = BootInstancePtr->BootMode & BOOT_MODES_MASK;
    
    /**
     * Enable drivers only if they are device boot modes
     * Not required for JTAG modes
     */
     switch(BootDevice)
     {
        case JTAG_BOOT_MODE:
        {
            LOG_OUT(DEBUG_INFO,"In JTAG Boot Mode \r\n");
            BootInstancePtr->BootMode = JTAG;
            BootInstancePtr->SecureModeFlag =0;
            Status = BOOT_STATUS_JTAG;
         }break;
            
        case NOR_BOOT_MODE:
        {
            LOG_OUT(DEBUG_INFO,"NOR Boot Mode \r\n");
            /* Update the deviceops structure with necessary values */
            BootInstancePtr->DeviceOps.DeviceInit = FmshFsbl_NorInit;
            BootInstancePtr->DeviceOps.DeviceCopy = FmshFsbl_NorCopy;            
            BootInstancePtr->BootMode = NOR_FLASH;
            BootInstancePtr->SearchRange = NOR_LIMITATION;
            Status = FMSH_SUCCESS;
       }break;

       case QSPI_BOOT_MODE:
       {
	    LOG_OUT(DEBUG_INFO,"QSPI Boot Mode \r\n");
	    /* Update the deviceops structure with necessary values*/
	    BootInstancePtr->DeviceOps.DeviceInit = FmshFsbl_InitQspi;
	    BootInstancePtr->DeviceOps.DeviceCopy = FmshFsbl_QspiAccess;
            BootInstancePtr->BootMode = QSPI_FLASH;
            BootInstancePtr->SearchRange = QSPI_4BIT_LIMITATION; 
            Status = FMSH_SUCCESS;
        }
	break;

	case NAND_BOOT_MODE:
	{
	    LOG_OUT(DEBUG_INFO,"NAND Boot Mode \r\n");
	    /* Update the deviceops structure with necessary values */
	    BootInstancePtr->DeviceOps.DeviceInit = FmshFsbl_NandInit;
	    BootInstancePtr->DeviceOps.DeviceCopy = FmshFsbl_NandCopy;
            BootInstancePtr->BootMode = NAND_FLASH;
            BootInstancePtr->SearchRange = NAND_LIMITATION;
	    Status = FMSH_SUCCESS;
	} 
        break;
        
	case SD_BOOT_MODE:
	{	
            LOG_OUT(DEBUG_INFO,"SD Boot Mode \r\n");
            /* Update the deviceops structure with necessary values */
	    BootInstancePtr->DeviceOps.DeviceInit = FmshFsbl_SdInit;
	    BootInstancePtr->DeviceOps.DeviceCopy = FmshFsbl_SdCopy;
            BootInstancePtr->BootMode = SD_CARD;
            BootInstancePtr->SearchRange = SD_LIMITATION;
            Status = FMSH_SUCCESS;
        } 
        break;
        
	default:
        {
            LOG_OUT(DEBUG_INFO,"BOOT ERROR: UNSUPPORTED BOOT MODE\r\n");
            Status = INVALID_BOOT_MODE;
            BootInstancePtr->ErrorCode = INVALID_BOOT_MODE;
	} 
        break;

    }

    /*In case of error or Jtag boot, goto end*/
    if (FMSH_SUCCESS != Status) 
    {
	goto END;
    }
    
    /**
     * Initialize the Device Driver
     */
    Status = BootInstancePtr->DeviceOps.DeviceInit(); 
    if (FMSH_SUCCESS != Status)
    {        
        LOG_OUT(DEBUG_INFO,"Boot device  initialization failed......\r\n");
        goto END;
    }
    
    LOG_OUT(DEBUG_INFO,"Boot device  initialization success......\r\n");
    
END:
    return Status;
    
}

/******************************************************************************
 * This function is used to search valid boot header.
 *
 * @param	BootInstance is pointer to the BootPs Instance
 *
 * @return	returns the error codes described in error.h on any error
 * 			returns FMSH_SUCCESS on success
 *
 ******************************************************************************/
static u32 FmshFsbl_BootHeaderSearchAndValidate(BootPs * BootInstance)
{
    u32 Status = FMSH_SUCCESS;
    u8 ValidFlag = 0;
    u32 MultiBootReg = 0;
    u32 ImageStartAddress;
    u32 EncrytionAlg;
    u32 WidthDetection;
    
    BootInstance->ImageOffsetAddress=0x0U;
   /**
    * Read the Multiboot Register
    */
    if((ReadReg(PCAP_CTRL_REG) & MULTIBOOT_EN_MASK) >> 30 )
    {
        MultiBootReg = ReadReg(MULTI_BOOT_REG);
    }
     
    do
    {
        LOG_OUT(DEBUG_INFO,"Multiboot register: 0x%x\r\n",MultiBootReg);
        ImageStartAddress = (MultiBootReg & MULTIBOOT_ADDR_MASK)* GOLDEN_IMAGE_OFFSET;
        LOG_OUT(DEBUG_INFO,"Image start address: 0x%x\r\n",ImageStartAddress);
    
        /* Load boot header information to global array*/
        Status = BootInstance->DeviceOps.DeviceCopy(ImageStartAddress + IH_BH_OFFSET, \
                (u32)(&BootHeaderInfo), sizeof(Ps_BootHeader));

        if (Status != FMSH_SUCCESS) 
        {
            LOG_OUT(DEBUG_INFO,"Boot header load failed\r\n");
        }
        LOG_OUT(DEBUG_INFO,"Load boot header info (offset:0x20~0x48)success!\r\n");
    
        Status = FmshFsbl_ValidataBootHeader(&BootHeaderInfo);
    
        if (Status != FMSH_SUCCESS) 
        {
            LOG_OUT(DEBUG_INFO,"Boot header validate failed,increment 32KB to search next boot header\r\n");
            MultiBootReg++;    
        } 
        else 
        {
            LOG_OUT(DEBUG_INFO,"Boot header validate success, this is a valid image!!!\r\n");
            ValidFlag = 1;
            BootInstance->ImageOffsetAddress =  ImageStartAddress;
            BootInstance->BootHdrAttributes = BootHeaderInfo.ImageAttr;
            BootInstance->EncryptionStatus = BootHeaderInfo.EncryptionStatus;
            /* QSPI X4 mode DETECT*/
            if(BootInstance->BootMode == QSPI_BOOT_MODE)
            {
#if XIPMODE_SUPPORT == 0
                FQspiPs_SetFlashMode(QspiInstancePtr, QOR_CMD); 
                BootInstance->DeviceOps.DeviceCopy(ImageStartAddress + \
                         IH_BH_WIDTH_DETECTION, (u32)&WidthDetection ,4U);
        
                if(WidthDetection != 0xAA995566)
                {
                    FQspiPs_SetFlashMode(QspiInstancePtr,DOR_CMD); 
                }
#endif
            }
        
            if(BootInstance->EncryptionStatus)
            {       
                BootInstance->SecureModeFlag = 1U;
                WriteReg (PCAP_CTRL_REG ,ReadReg(PCAP_CTRL_REG)| SECURE_MODE_MASK );
                BootInstance->DeviceOps.DeviceCopy(ImageStartAddress + \
                         IH_BH_ENC_ALG_OFFSET, (u32)&EncrytionAlg ,4U);
                BootInstance->EncrytionAlgorithm = EncrytionAlg;

            }
            else
            {
              
                if((FmshFsbl_FindOneInNumber(ReadReg(EFUSE_SECURE_BOOT_EN)&0x7fff) > 0x8))
                {
                    LOG_OUT(DEBUG_INFO,"Secure boot is force!\r\n");
                    BootInstance->ErrorCode = SECURE_BOOT_FORCE_NOT_MATCH;
                    Status = BOOT_STAGE_ERR;
                    BootInstance->SecureModeFlag = 1U;
                    goto END;
                }
          
                if(BootInstance->ResetReason == REBOOT_NON_POR &&  BootInstance->SecureModeFlag == 1U)
                {  
                    BootInstance->ErrorCode = SECURITY_VIOLATION;
                    Status = BOOT_STAGE_ERR;
                    goto END;
                }
                BootInstance->SecureModeFlag = 0U;
            }
            /*if boot header is valid,then write valid multiboot address to MULTIBOOT ADDR REGISTER  */
            if(MultiBootReg != 0)
            {
                WriteReg(PCAP_CTRL_REG, ReadReg(PCAP_CTRL_REG)| MULTIBOOT_EN_MASK);
                WriteReg(MULTI_BOOT_REG,MultiBootReg);
            }
            break;
       }
    } while (ValidFlag == 0 && ImageStartAddress <= BootInstance->SearchRange);
    
    if ((ValidFlag == 0) && (ImageStartAddress > BootInstance->SearchRange))
    {   
        LOG_OUT(DEBUG_INFO,"No valid image was found, system will lockdown!\r\n");
        BootInstance->SecureModeFlag = 0; 
        BootInstance->ErrorCode = NO_VALID_BOOT_IMG_HEADER;
        return FMSH_FAILURE;
    }
    else 
        return FMSH_SUCCESS;
    
END:
   return Status;
}

/******************************************************************************
 * This function validates the image header
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @return	returns the error codes described in fmsh_error.h on any error
 * 			returns FMSH_SUCCESS on success
 *
 ******************************************************************************/
static u32 FmshFsbl_ValidateHeader(BootPs * BootInstancePtr)
{
	u32 Status =  FMSH_SUCCESS;
	u32 BootHdrAttrb=0U;
	u32 FlashImageOffsetAddress=0U;
	
	FlashImageOffsetAddress = BootInstancePtr->ImageOffsetAddress;

	/**
	 * Read Boot Image attributes
	 */
	Status = BootInstancePtr->DeviceOps.DeviceCopy(FlashImageOffsetAddress
                    + 0x44,
                   (u32) &BootHdrAttrb, 4);
        if (FMSH_SUCCESS != Status) 
        {
                LOG_OUT(DEBUG_INFO,"Device Copy Failed \n\r");
                goto END;
        }
	BootInstancePtr->BootHdrAttributes = BootHdrAttrb;

	/**
	 * Read Image Header and validate Image Header Table
	 */
	Status = FmshFsbl_ReadImageHeader(&BootInstancePtr->ImageHeader,
					&BootInstancePtr->DeviceOps,
					FlashImageOffsetAddress);
	if (FMSH_SUCCESS != Status) 
        {
		goto END;
	}

END:
	return Status;
}

/******************************************************************************
 * This function initializes the primary and secondary boot devices
 * and validates the image header
 *
 * @param	BootInstancePtr is pointer to the BootPs Instance
 *
 * @return	returns the error codes described in fmsh_error.h on any error
 * 			returns FMSH_SUCCESS on success
 ******************************************************************************/
u32 FmshFsbl_BootDeviceInitAndValidate(BootPs * BootInstancePtr)
{
    u32 Status = FMSH_SUCCESS;

    /**
     * Configure the  boot device
     */
    LOG_OUT(DEBUG_INFO,"Preparing boot device initialization......\r\n");
    Status = FmshFsbl_BootDeviceInit(BootInstancePtr);        
    if (FMSH_SUCCESS != Status ) 
    {
       goto END;
    }
    
#if DEBUG_PERF  
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Boot device initialization is done at the %f ms\r\n",gtc_time);
#endif
    
    /**
     * Read the Boot header
     */
    LOG_OUT(DEBUG_INFO,"Preparing boot header search and validate......\r\n");
    Status = FmshFsbl_BootHeaderSearchAndValidate(BootInstancePtr);
    if (FMSH_SUCCESS != Status) 
    {
        LOG_OUT(DEBUG_INFO,"No valid image found,Error lockdown......\r\n");
        goto END;
    }
    
#if DEBUG_PERF
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Boot header search is done at the %f ms\r\n",gtc_time);
#endif
    
    /**
     * Validate the header
     */
     Status = FmshFsbl_ValidateHeader(BootInstancePtr);
     if (FMSH_SUCCESS != Status) 
     {
	goto END;
     }

#if DEBUG_PERF     
    gtc_count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    gtc_time = (gtc_count1 - gtc_count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(DEBUG_PERF,"Boot header validate is done at the %f ms\r\n",gtc_time);
#endif

#if FSBL_WDT_ENABLE
    FWdtPs_restart(&g_WDT);
#endif
    
END:
	return Status;
}



