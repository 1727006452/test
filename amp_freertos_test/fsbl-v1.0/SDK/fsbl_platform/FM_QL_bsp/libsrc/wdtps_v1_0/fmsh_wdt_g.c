/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_wdt_g.c
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
* 0.01   mtl  08/27/2019  First Release
*</pre>
******************************************************************************/

#include "fmsh_ps_parameters.h"
#include "fmsh_wdt_lib.h"

/*
* The configuration table for devices
*/

FWdtPs_Config FWdtPs_ConfigTable[] =
{
	{
		FPAR_WDTPS_DEVICE_ID,
		FPAR_WDTPS_BASEADDR
	}
};