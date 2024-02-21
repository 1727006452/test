/******************************************************************************
******************************************************************************/
/*****************************************************************************/
/**
*
* @file FGicPs_selftest.c
* @addtogroup scugic_v3_1
* @{
*
* Contains diagnostic self-test functions for the FGicPs driver.
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a drg  01/19/10 First release
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "fmsh_gic.h"
#include "fmsh_gic_hw.h"
#include "fmsh_common_types.h"
#include "fmsh_common.h"


/************************** Constant Definitions *****************************/


#define SGI_ID          0U
#define CPU_ID          1U

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
*
* Run a self-test on the driver/device. This test reads the ID registers and
* compares them.
*
* @param	InstancePtr is a pointer to the FGicPs instance.
*
* @return
*
* 		- XST_SUCCESS if self-test is successful.
* 		- XST_FAILURE if the self-test is not successful.
*
* @note		None.
*
******************************************************************************/
u32  gicTestFlag = GIC_FAILURE;
void SGI0_hanlder (void *InstancePtr);
u32  FGicPs_SelfTest(FGicPs *InstancePtr)
{    
	    
    u32 Status;
    Status =  FGicPs_SetupInterruptSystem(InstancePtr);
    if(Status!=GIC_SUCCESS)
      {
      return GIC_FAILURE ;
      }
       
       Status = FGicPs_Connect(InstancePtr,SGI_ID,
                  (FMSH_InterruptHandler)SGI0_hanlder,InstancePtr );
            if (Status != GIC_SUCCESS) {
              return GIC_FAILURE;
             }      
            
       FMSH_ExceptionRegisterHandler(FMSH_EXCEPTION_ID_IRQ_INT,
          (FMSH_ExceptionHandler)FGicPs_InterruptHandler_IRQ,
                InstancePtr);      
                        
        FGicPs_Enable(InstancePtr, SGI_ID);        
        FGicPs_SoftwareIntr(InstancePtr,SGI_ID,CPU_ID);
        return gicTestFlag;
}


void SGI0_hanlder (void *InstancePtr)
{
    
    gicTestFlag = GIC_SUCCESS;

  
}


/** @} */
