/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_gtc_g.c
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
#include "fmsh_gtc_lib.h"

/*
* The configuration table for devices
*/

FGtcPs_Config FGtcPs_ConfigTable[] =
{
	{
		FPAR_GTCPS_DEVICE_ID,
		FPAR_GTCPS_BASEADDR
	}
};