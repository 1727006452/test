/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_devc_g.c
*
* This file contains global variable which use to config.
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
#include "fmsh_devc_lib.h"
#include "fmsh_ps_parameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
FDevcPs_Config FDevcPs_ConfigTable[] =
{
	{
		FPAR_DEVCPS_DEVICE_ID,
		FPAR_DEVCPS_BASEADDR
	}
};
/************************** Function Prototypes ******************************/
