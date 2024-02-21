/*****************************************************************************/
/**
*
* @file main.c
*
* This is the main file which contains code for the FSBL.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 3.0   lq   19/10/24 Added the modification log.
* 3.1   lq   19/10/25 Fix for qspips_v1_0
*                     Added slcrps_v1_0.   
* 3.2   lq   19/10/28 Fix the function CheckValidMemoryAddress in the fmsh_header.c. 
* 3.3   lq   19/11/1  Update NFC driver.   
* 3.4   lq   19/11/7  Update Uart driver.    
* 3.5   lq   19/12/13  Fix QSPI dummy_clk.    
*                      Delete the define of FSBL_PERF.
*                      Fix FDevcPs_recoverFreq() function.
* 3.6   lq   19/12/13  Fix division factor to 4 in the FQspiPs_Initialize() function.    
*                      Fix FmshFsbl_PartitionValidation() function.
* 3.7   lq   20/3/25   Added XIP MODE support.    
*                      Fix some bug.
* 3.8   lq   20/9/27   Fix adjust frequency function.
*                      Add partition detail log.
* 3.9   lq   20/10/29  Fix disable cache of the fmash_exit.s.
*                      Add some error code.
*                      Fix AesSm4() function.
* 3.10  lq   20/12/15  Fix qspi x4 init funcion.
*                      Update NFC driver.
* 3.11  lq   21/03/29  Update QSPI driver.
*                      Add DUMMY_CLOCK_COUNT for poll fpga cfg done.
* 3.12  lq   21/03/29  Update SD driver for EMMC suport.             
*                      Add SHA verify.    
* 3.13  lq   21/12/07  Update QSPI driver for high address suport.             
*                      Full mask revision cancel adjust frequency.        
* 3.14  lq   21/12/16  Fix division factor to 4 in the FQspiPs_Initialize() function.           
*                      Add build data and procise version in the FmshFsbl_SystemInit() function.
* 3.15  lq   22/03/11  Enable i-cache.           
*                      
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "boot_main.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
void PrintArray_1 (const unsigned char Buf[], unsigned int Len)
{
    u32 Index;
  
    for (Index=0U;Index<Len;Index++)
    {
      LOG_OUT(DEBUG_INFO, "%02lx ",Buf[Index]);
      if(((Index+1) %32 == 0)&&(Index != 0))
        LOG_OUT(DEBUG_INFO, "\r\n");
    }
    LOG_OUT(DEBUG_INFO, "\r\n");
}


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
extern u32 FlashReadBaseAddress ;
u64 gtc_count0, gtc_count1;
double gtc_time;
BootPs BootInstance={3.15, 0x100U};

/*****************************************************************************/
/** This is the FSBL main function and is implemented stage wise.
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
u32 main(void)
{
  
  
#if DEBUG_PERF
    global_timer_enable();  
    gtc_count0 = *(u64*)(FPS_GTC_BASEADDR + 0x8); 
#endif

    /**
     * Local variables
     */
    u32 BootStatus = FMSH_SUCCESS;
    u32 BootStage =  BOOT_STAGE1; 
    u32 PartitionNum=0U;

    while (BootStage <= BOOT_STAGE_DEFAULT) 
    {
        switch (BootStage)
        {
          case BOOT_STAGE1:
          {
              /* Initialize the system*/
              BootStatus = FmshFsbl_BootInitialize(&BootInstance);          
              if (BootStatus != FMSH_SUCCESS)
              {
                 BootStage = BOOT_STAGE_ERR;
              } 
              else 
              {
                 BootStage = BOOT_STAGE2;
 
                 /* Set MIO14 output */
                 FMSH_WriteReg(FPS_GPIO_BASEADDR, 0x04, 0x4000);
                 /* Set MIO14 output high to light up the som-led2 */
                 FMSH_WriteReg(FPS_GPIO_BASEADDR, 0x00, 0x4000);
              }
          }break;

          case BOOT_STAGE2:
	  {
             LOG_OUT(DEBUG_INFO,"======= In BootStage 2 ======= \r\n");
             /**
	      *  boot device
	      *  DeviceOps
	      *  image header
	      */
             BootStatus = FmshFsbl_BootDeviceInitAndValidate(&BootInstance);
             
	     if ( (FMSH_SUCCESS != BootStatus) && (BOOT_STATUS_JTAG != BootStatus) )
	     {
                LOG_OUT(DEBUG_INFO,"Boot Device Initialization and validate failed!!!\r\n");
		BootStage = BOOT_STAGE_ERR;
	     }
             else if (BOOT_STATUS_JTAG == BootStatus) 
             {
                /**
		 * This is JTAG boot mode, go to the handoff BootStage
	         */
                BootStage = BOOT_STAGE4;
	     } 
             else 
             {
               /**
		* Start the partition loading from 1
		* 0th partition will be FSBL
		*/
		PartitionNum = 0x1U;
#if MMC_SUPPORT
                PartitionNum = 0x0U;
                BootStage = BOOT_STAGE3;
#else
                if(BootInstance.ImageHeader.ImageHeaderTable.NoOfPartitions==1)
                {
                  LOG_OUT(DEBUG_INFO,"None valid Partition in the BOOT.bin!! \n\r");
                  BootStage = BOOT_STAGE4;
                }
                else
                  BootStage = BOOT_STAGE3;
#endif                
	     }
	} break;

        case BOOT_STAGE3:
	{
             LOG_OUT(DEBUG_INFO,"======= In BootStage 3 ======= \r\n");
             
             /**
	      * Load the partitions
	      *  image header
	      *  partition header
	      *  partition parameters
	      */
	     BootStatus = FmshFsbl_PartitionLoad(&BootInstance,PartitionNum);
	     if (FMSH_SUCCESS != BootStatus)
	     {
               if(PARTITION_SKIP_LOAD == BootStatus)
               {
                  /* skip load*/
                 LOG_OUT(DEBUG_INFO,"Partition Load Skip ,PartitionNum=0x%d!!\r\n",PartitionNum);
                  BootStage = BOOT_STAGE4;
               }
               else
               {
                  /* Error*/
                  LOG_OUT(DEBUG_INFO,"Partition  Load Failed,PartitionNum=0x%d!!\r\n",PartitionNum);
                  BootStage = BOOT_STAGE_ERR;
               }
	     } 
             else 
             {
		LOG_OUT(DEBUG_INFO,"Partition Load Success \r\n");
                /**
		 * Check loading all partitions is completed
		 */
                //BootStatus = Fsbl_CheckEarlyHandoff(&BootInstance, PartitionNum);
                if (PartitionNum <(BootInstance.ImageHeader.ImageHeaderTable.NoOfPartitions-1U))
		{
                    PartitionNum++;
		} else {
		       /**
			* No more partitions present, go to handoff stage
		        */
			LOG_OUT(DEBUG_INFO,"All Partitions Loaded \n\r");
                        BootStage = BOOT_STAGE4;
                }
	     }/* End of else loop for Load Success */
				 
	} break;

	case BOOT_STAGE4:
	{
             LOG_OUT(DEBUG_INFO,"================= In BootStage 4 ============ \r\n");
              /**
              * Handoff to the applications
              * Handoff address
              * xip
              * ps7 post config
              */
             LOG_OUT(DEBUG_INFO,"Handoff control to JTAG or FSBL...... \r\n");
	     BootStatus = BootHandoff(&BootInstance);
             if (FMSH_SUCCESS != BootStatus)
             {
                  BootInstance.ErrorCode = BootStatus;
                  BootStage = BOOT_STAGE_ERR;
             }
	} break;

	case BOOT_STAGE_ERR:
	{
	     LOG_OUT(DEBUG_INFO,"================= In BootStage Error ============ \r\n");
             ErrorLockDown();
	     BootStage = BOOT_STAGE_DEFAULT;
        }break;

	case BOOT_STAGE_DEFAULT:
	default:
	{
	     /** 
	      * we should never be here
	      */
            LOG_OUT(DEBUG_INFO,"In default BootStage: We should never be here \r\n");			
            /* Exit FSBL*/
	    DefaultHandoffExit();
        }break;
            
      }/* End of switch(FsblBootStage) */
      if(BootStage==BOOT_STAGE_DEFAULT) {
          break;
      }
    } /* End of while() */

    /**
     * We should never be here
     */
    LOG_OUT(DEBUG_INFO,"In default BootStage: We should never be here \r\n");
    /**
     * Exit FSBL
     */
    DefaultHandoffExit();

    return 0;
        
}
        
        
       
        





