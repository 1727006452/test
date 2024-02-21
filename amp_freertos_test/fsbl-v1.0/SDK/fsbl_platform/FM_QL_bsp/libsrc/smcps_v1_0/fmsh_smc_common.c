/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_common.c
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
* 0.01   yl  12/20/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include <string.h>
#include "fmsh_smc_lib.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*****************************************************************************
*
* @description
* This function writes a command to address.
*
* @param    
*           commandAddress Command address offset.
*           command Command to be send.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void WriteCommand(uint32_t commandAddress, uint32_t command)
{
    NOR_WRITE_COMMAND8(commandAddress, command);
}

/*****************************************************************************
*
* @description
* This function reads data from the NorFlash chip into the provided buffer.
*
* @param    
*           address  Address of data.
*           buffer  Buffer where the data will be stored.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void ReadRawData(uint32_t    address, uint8_t *buffer)
{
    *buffer = NOR_READ_DATA8(address);
}

/*****************************************************************************
*
* @description
* This function writes data to the NorFlash chip from the provided buffer.
*
* @param    
*           address  Address of data.
*           buffer  Buffer where the data will be stored.
*
* @return	
*           NA.
*
* @note		NA.
*
*****************************************************************************/
void WriteRawData(uint32_t  address, uint8_t *buffer)

{
    NOR_WRITE_DATA8(address, *buffer);
}

