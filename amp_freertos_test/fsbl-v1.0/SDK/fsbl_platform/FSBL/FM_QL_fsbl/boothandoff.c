
#include "boot_main.h"

extern u8 SecureFlag;
extern BootPs BootInstance;

#define DECFG_BASEADDR 0xF8007000 
#define ROM_SHADOW_OFFSET 0x28

#define DECFG_BASEADDR 0xF8007000 
#define DECFG_CTRL_OFFSET 0x0

extern void FMSH_EXIT(u32 HandOffAddr,u32 Flag);

void HandoffJtagExit(void)
{
    invalidate_icache_all();
    icache_disable();
    asm(				
       "LOOP:\n\t"
       "wfe\n\t"
        "b LOOP"
      );
}

/*****************************************************************************
*
* This function checks if a given CPU is supported by this variant of Silicon
* Currently it checks if it is CG part and disallows handoff to A53_2/3 cores
*
* @param	none
*
* @return	FMSH_SUCCESS if supported CPU, FMSH_FAILURE if not.
*
******************************************************************************/
static u32 FmshFsbl_CheckSupportedCpu(u32 CpuId)
{
	u32 Status = FMSH_SUCCESS;

	if((CpuId != IH_PH_ATTRB_DEST_CPU_A7_0) && (CpuId != IH_PH_ATTRB_DEST_CPU_A7_1)
                        && (CpuId != IH_PH_ATTRB_DEST_CPU_A7_2) && (CpuId != IH_PH_ATTRB_DEST_CPU_A7_3))
        {
		Status = FMSH_FAILURE;
		goto END;
	}

	/* Add code to check for support of other CPUs/cores in future */

END:
	return Status;
}

/*****************************************************************************
 * FSBL exit function before the assembly code
 *
 * @param HandoffAddress is handoff address for the FSBL running cpu
 *
 * @param Flag is to determine whether to handoff to application or
 * 			to be in wfe state
 *
 * @return None
 *
 *
 *****************************************************************************/
void FmshFsbl_HandoffExit(u32 HandoffAddress,u32 Flag)
{

	LOG_OUT(DEBUG_INFO,"Exit from FSBL \n\r");

	/**
	 * Exit to handoff address
	 * PTRSIZE is used since handoff is in same running cpu
	 * and address is of PTRSIZE
	 */
	FMSH_EXIT(HandoffAddress,Flag);

	/**
	 * should not reach here
	 */
	return ;

}
/****************************************************************************
*
* @param
*
* @return
*
* @note
*
*
*****************************************************************************/
static void FmshFsbl_UpdateResetVector (u32 HandOffAddress, u32 CpuSettings)
{
  u32 AddressReg=0U;
  u32 CpuId;
  SlcrUnlock();
  CpuId = CpuSettings & IH_PH_ATTRB_DEST_CPU_MASK;

  
  
  /**
  * for A53 cpu, write 64bit handoff address
  * to the RVBARADDR in APU
  */
  
  switch (CpuId)
  {
  case IH_PH_ATTRB_DEST_CPU_A7_0:
    AddressReg = APU_RVBARADDR0;
    break;
  case IH_PH_ATTRB_DEST_CPU_A7_1:
    AddressReg = APU_RVBARADDR1;
    break;
  case IH_PH_ATTRB_DEST_CPU_A7_2:
    AddressReg = APU_RVBARADDR2;
    break;
  case IH_PH_ATTRB_DEST_CPU_A7_3:
    AddressReg = APU_RVBARADDR3;
    break;
  default:
    /**
    * error can be triggered here
    */
    break;
  }
  Fmsh_Out32(AddressReg, HandOffAddress);
  SlcrLock();
  return;
}


u32 BootHandoff (const BootPs * BootInstancePtr)
{
  u32 Status=FMSH_SUCCESS;
  u32 CpuIndex=0U;
  u32 CpuId=0U;
  u32 CpuSettings=0U;
  u32 HandoffAddress=0U;
  u32 RunningCpuHandoffAddress=0U;
  s32 RunningCpuHandoffAddressPresent=FALSE;

  while (CpuIndex < BootInstancePtr->HandoffCpuNo)
  {
    CpuSettings = BootInstancePtr->HandoffValues[CpuIndex].CpuSettings;
    
    CpuId = CpuSettings & IH_PH_ATTRB_DEST_CPU_MASK;
    
      /**
      * Check if handoff address is present
      */
      if (CpuId != BootInstancePtr->ProcessorID)
      {
        /* Check if handoff CPU is supported */
        Status = FmshFsbl_CheckSupportedCpu(CpuId);
        if (FMSH_SUCCESS != Status)
        {
          LOG_OUT(DEBUG_INFO,"FSBL_ERROR_UNAVAILABLE_CPU\n\r");
          Status = ERROR_UNAVAILABLE_CPU;
          goto END;
        }
        
        /**
        * Read the handoff address from structure
        */
        HandoffAddress = (u64 )BootInstancePtr->HandoffValues[CpuIndex].HandoffAddress;
        
        /**
        * Update the handoff address at reset vector address
        */
        FmshFsbl_UpdateResetVector(HandoffAddress, CpuSettings);
             
      } 
      else 
      {
        /**
        * Update the running cpu handoff address
        */
        RunningCpuHandoffAddressPresent = TRUE;
        RunningCpuHandoffAddress = BootInstancePtr->HandoffValues[CpuIndex].HandoffAddress;
      }
    /**
    * Go to the next cpu
    */
    CpuIndex++;
  }
  
#if FSBL_WDT_ENABLE  
  FmshFsbl_WdtClose();
#endif
  
  LOG_OUT(DEBUG_INFO,"APU JTAG will be enabled.!!!\r\n");  

  if(FmshFsbl_IsBitDone()==FALSE)
    FmshFsbl_CloseUsrLevelShifter();
  
  /**
  * call to the handoff routine
  * which will never return
  */
  if (RunningCpuHandoffAddressPresent ==  TRUE)
  {
      FmshFsbl_HandoffExit(RunningCpuHandoffAddress,FSBL_HANDOFFEXIT);
  } 
  else 
  {
      FmshFsbl_HandoffExit(0U,FSBL_NO_HANDOFFEXIT);
  }
  
END:
  return Status;
  
}
    
void DefaultHandoffExit()
{
#if FSBL_WDT_ENABLE  
  FmshFsbl_WdtClose();
#endif
  asm(
      "LOOP:\n\t"
       "wfe\n\t"
        "b LOOP"
      );

}

void ErrorLockDown(void)
{
    u32 BootDevice=0;
    
#if FSBL_WDT_ENABLE  
  FmshFsbl_WdtClose();
#endif
      /**
       * Print the FSBL error
       */
    LOG_OUT(DEBUG_INFO,FmshFsbl_GetErrorInfo(BootInstance.ErrorCode));
    BootDevice = ReadReg(BOOT_MODE_REG) & BOOT_MODES_MASK;
    if( (BootDevice==QSPI_BOOT_MODE) || 
       (BootDevice==SD_BOOT_MODE) || 
       (BootDevice==NOR_BOOT_MODE) || 
         (BootDevice==NAND_BOOT_MODE) )
    {
      
#if FSBL_MULTI_BOOT_ENABLE 
      u32 MultiReg=0;
      MultiReg=FMSH_ReadReg(FPS_CSU_BASEADDR,0x38);
      WriteReg(MULTI_BOOT_REG,MultiReg+1);  
      //WriteReg(MULTI_BOOT_REG,GOLDEN_IMG_ADDRESS/GOLDEN_IMAGE_OFFSET);
      FMSH_WriteReg(FPS_CSU_BASEADDR,0x8,FMSH_ReadReg(FPS_CSU_BASEADDR,0x8)|(1<<30));
#endif
        /*Soft reset*/
	FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);
        WriteReg(FPS_SLCR_BASEADDR+0x200,1);
	FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x004, 0xDF0D767B);
       
        while(1);
    }
    else
    {
        HandoffJtagExit();
    }

}

