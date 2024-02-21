/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_uart_lib.c
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
* 0.01   wfb  11/23/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include "fmsh_usb_lib.h"
#include "fmsh_ps_parameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
FUsbPs_Config FUsbPs_ConfigTable[] =
{
	{
		FPAR_USBPS_0_DEVICE_ID,
		FPAR_USBPS_0_BASEADDR,
		FPAR_USBPS_1_USB_CLK_FREQ_HZ
	},
        {
		FPAR_USBPS_1_DEVICE_ID,
		FPAR_USBPS_0_BASEADDR,
		FPAR_USBPS_1_USB_CLK_FREQ_HZ
	}
};
/************************** Function Prototypes ******************************/
