/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_ttc_g.c
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
#include "fmsh_ttc_lib.h"

/*
* The configuration table for devices
*/

FTtcPs_Config FTtcPs_ConfigTable[] =
{
	{
		FPAR_TTCPS_0_DEVICE_ID,
		FPAR_TTCPS_0_BASEADDR
	},
        {
		FPAR_TTCPS_1_DEVICE_ID,
		FPAR_TTCPS_1_BASEADDR
	}
};