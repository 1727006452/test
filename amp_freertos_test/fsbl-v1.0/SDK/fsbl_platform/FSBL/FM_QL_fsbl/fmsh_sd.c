/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_sd.c
*
* This file contains fmsh_sd.h.
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   xxx  11/23/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include "boot_main.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
u32 FlashReadBaseAddress;

static FIL fil;		/* File object */
static FATFS fatfs;
static char buffer[32];
static char *boot_file = buffer;

/************************** Function Prototypes ******************************/

/*******************************************************************************
*
* This function copies a string to another, the source string must be null-
* terminated.
*
* @param 	Dest is starting address for the destination string
* @param 	Src is starting address for the source string
*
* @return	Starting address for the destination string
*
****************************************************************************/
char *strcpy_rom(char *Dest, const char *Src)
{
	unsigned i;
	for (i=0; Src[i] != '\0'; ++i)
		Dest[i] = Src[i];
	Dest[i] = '\0';
	return Dest;
}

/*******************************************************************************
*
* This function initializes the controller for the SD FLASH interface.
*
* @param	filename of the file that is to be used
*
* @return
*		- XST_SUCCESS if the controller initializes correctly
*		- XST_FAILURE if the controller fails to initializes correctly
*
* @note		None.
*
****************************************************************************/
u32 InitSD(const char *filename)
{
	FRESULT rc;
	TCHAR *path = "0:/"; /* Logical drive number is 0 */

	/* Register volume work area, initialize device */
	rc = f_mount(&fatfs, path, 0);
	LOG_OUT(DEBUG_INFO,"SD: rc= %.8x\n\r", rc);

	if (rc != FR_OK) {
		return FMSH_FAILURE;
	}

	strcpy_rom(buffer, filename);
	boot_file = (char *)buffer;
	FlashReadBaseAddress = FPS_SDMMC0_BASEADDR;

	rc = f_open(&fil, boot_file, FA_READ);
	if (rc) {
		LOG_OUT(DEBUG_INFO,"SD: Unable to open file %s: %d\n", boot_file, rc);
		return FMSH_FAILURE;
	}

	return FMSH_SUCCESS;
}

u32 FmshFsbl_SdInit()
{
    u32 Status; 
#if MMC_SUPPORT    
    u32 MultiBootReg = ReadReg(MULTI_BOOT_REG);
    if((MultiBootReg&MULTIBOOT_ADDR_MASK)==0)
    {
      LOG_OUT(DEBUG_INFO,"Preparing initialize file BOOT.bin......\r\n");
      Status = InitSD("BOOT.bin");
      if(Status != FMSH_SUCCESS)
      {
          return Status;
      }
    }
    else
    {
      LOG_OUT(DEBUG_INFO,"Preparing initialize file BOOT0000.bin......\r\n");
      Status = InitSD("BOOT0000.bin");
      if(Status != FMSH_SUCCESS)
      {
          return Status;
      }
    }
#else
     LOG_OUT(DEBUG_INFO,"Preparing initialize file BOOT.bin......\r\n");
     Status = InitSD("BOOT.bin");
     if(Status != FMSH_SUCCESS)
     {
         return Status;
     }
#endif    
    return FMSH_SUCCESS;
}
/******************************************************************************/
/**
*
* This function provides the SD FLASH interface for the Simplified header
* functionality.
*
* @param	SourceAddress is address in FLASH data space
* @param	DestinationAddress is address in OCM data space
* @param	LengthBytes is the number of bytes to move
*
* @return
*		- XST_SUCCESS if the write completes correctly
*		- XST_FAILURE if the write fails to completes correctly
*
* @note		None.
*
****************************************************************************/
u32 FmshFsbl_SdCopy( u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
	FRESULT rc;	 /* Result code */
	UINT br;

	rc = f_lseek(&fil, SourceAddress);
	if (rc) {
		LOG_OUT(DEBUG_INFO,"SD: Unable to seek to %lx\n", SourceAddress);
		return FMSH_FAILURE;
	}

	rc = f_read(&fil, (void*)DestinationAddress, LengthBytes, &br);

	if (rc) {
		LOG_OUT(DEBUG_INFO,"*** ERROR: f_read returned %d\r\n", rc);
	}

	return FMSH_SUCCESS;
} /* End of SDAccess */


/******************************************************************************/
/**
*
* This function closes the file object
*
* @param	None
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void FmshFsbl_ReleaseSD(void)
{
	f_close(&fil);
	return;
}


