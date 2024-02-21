/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_common.h
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
#ifndef _FMSH_SMC_COMMON_H_
#define _FMSH_SMC_COMMON_H_

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/
 
/** The function completed successfully. */
#define NorCommon_ERROR_NONE               0

/** Can not detect common flash infterface.*/
#define NorCommon_ERROR_UNKNOWNMODEL       1

/** A read operation cannot be carried out.*/
#define NorCommon_ERROR_CANNOTREAD         2

/** A write operation cannot be carried out.*/
#define NorCommon_ERROR_CANNOTWRITE        3

/** A erase operation cannot be carried out.*/
#define NorCommon_ERROR_CANNOTERASE        4

/** A locked operation cannot be carried out.*/
#define NorCommon_ERROR_PROTECT            5

/** An operation is time out.*/
#define NorCommon_ERROR_TIMEOUT            6

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#define NOR_WRITE_COMMAND8(commandAddress, command) \
    {*(volatile uint8_t *)(commandAddress) = (uint8_t) command;}
    
#define NOR_WRITE_DATA8(dataAddress, data) \
    {(*((volatile uint8_t *) dataAddress)) = (uint8_t) data;}
    
#define NOR_READ_DATA8(dataAddress) \
    (*((volatile uint8_t *) dataAddress))

/************************** Function Prototypes ******************************/

void WriteCommand(uint32_t commandAddress, uint32_t command);
void ReadRawData(uint32_t dataAddress, uint8_t *buffer);
void WriteRawData(uint32_t dataAddress, uint8_t *buffer);
    
#endif /* #ifndef _FMSH_SMC_COMMON_H_ */
