/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_uart_sint.c
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
* 0.01   lq  08/27/2019  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "fmsh_uart_lib.h"
#include "fmsh_ps_parameters.h"
#include "fmsh_usb_common.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern FUsbPs_Config FUsbPs_ConfigTable[];

/************************** Function Prototypes ******************************/

/****************************************************************************
*
*  This function look up the uart by device id. 
*
* @param   DeviceId is device id. 
*
* @return  
* - point to the corresponding device
* - NULL invalid device id
*
* @note    none
*
****************************************************************************/
FUsbPs_Config *FUsbPs_LookupConfig(u16 DeviceId)
{
    FUsbPs_Config *CfgPtr=NULL;
    
    u32 Index;
    
    for(Index=0;Index<FPAR_USBPS_NUM_INSTANCES;Index++)
    {
        if(FUsbPs_ConfigTable[Index].DeviceId==DeviceId)
        {
            CfgPtr=&FUsbPs_ConfigTable[Index];
            break;
        }
    }
    
    return (FUsbPs_Config *)CfgPtr;
}

/****************************************************************************/
/**
*
*  This function initializes a uart. It disables all interrupts and
*  resets the Tx and Rx FIFOs.  It also initializes the driver's
*  internal data structures.
*
* @param   uartDev is uart handle. 
*
* @return  
*  0           -- if successful
*  -FMSH_ENOSYS  -- hardware parameters for the device could not be
*              automatically determined
*
* @note    
*  If the FMSH_apb_uart is independently reset, it must be re-initialized
*  by calling this function, in order to keep the device and driver in
*  sync.
*
****************************************************************************/
int FUsbPs_init(FUsbPs_T *usbDev, FUsbPs_Config *cfg)
{
        int retval = FMSH_SUCCESS;
        
        usbDev->base_address = cfg->BaseAddress;
        usbDev->id =cfg->DeviceId;
        usbDev->input_clock = cfg->InputClockHz;
        
        return retval;
}