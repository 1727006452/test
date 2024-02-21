/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/******************************************************************************
*
* @file  fmsh_smc_cfi.h
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
#ifndef _FMSH_SMC_CFI_H_
#define _FMSH_SMC_CFI_H_

/***************************** Include Files *********************************/

#include "fmsh_common.h"

/************************** Constant Definitions *****************************/

#define CFI_MAX_ERASE_REGION    4

/** Common flash interface query command. */
#define CFI_QUERY_COMMAND     0x98
#define CFI_QUERY_ADDRESS     0x55
#define CFI_QUERY_OFFSET      0x10

/** Vendor command set control interface ID code . */
#define CMD_SET_NULL          0x0000
#define CMD_SET_INTEL_EXT     0x0001 
#define CMD_SET_AMD           0x0002
#define CMD_SET_INTEL         0x0003
#define CMD_SET_AMD_EXT       0x0004
#define CMD_SET_MISUBISHI     0x0100
#define CMD_SET_MISUBISHI_EXT 0x0101
#define CMD_SET_SST           0x0102


/** Indicates the maximum region for norflash device. */
#define NORFLASH_MAXNUMRIGONS 4
/** Indicates the NorFlash uses an 8-bit address bus. */
#define FLASH_CHIP_WIDTH_8BITS  0x01
/** Indicates the NorFlash uses an 16-bit address bus. */
#define FLASH_CHIP_WIDTH_16BITS 0x02
/** Indicates the NorFlash uses an 32-bit address bus. */
#define FLASH_CHIP_WIDTH_32BITS 0x04
/** Indicates the NorFlash uses an 64-bit address bus. */
#define FLASH_CHIP_WIDTH_64BITS 0x08

/**************************** Type Definitions *******************************/

/*****************************************************************************
* DESCRIPTION
*  This is a Norflash CFI query system interface information.
*
*****************************************************************************/
struct NorFlashCfiQueryInfo {
    uint8_t reserved;
    /** Query Unique String "QRY". */
    uint8_t queryUniqueString[3];
    /** Primary vendor command set and control interface ID .*/
    uint16_t primaryCode;
    /** Address for primary extended query table.*/
    uint16_t primaryAddr;
    /** Alternate vendor command set and control interface ID .*/
    uint16_t alternateCode;
    /** Address for alternate extended query table.*/
    uint16_t alternateAddr;
    /** Vcc logic supply minimum write/erase voltage.*/
    uint8_t minVcc;
    /** Vcc logic supply maximum write/erase voltage.*/
    uint8_t maxVcc;
    /** Vpp logic supply minimum write/erase voltage.*/
    uint8_t minVpp;
    /** Vpp logic supply maximum write/erase voltage.*/
    uint8_t maxVpp;
    /** Timeout per single write (2<<n) in microsecond.*/
    uint8_t minTimeOutWrite;
    /** Timeout for minimum-size buffer write (2<<n) in microsecond.*/
    uint8_t minTimeOutBuffer;
    /** Timeout for block erase (2<<n) in microsecond.*/
    uint8_t minTimeOutBlockErase;
    /** Timeout for chip erase (2<<n) in microsecond.*/
    uint8_t minTimeOutChipErase;
    /** Maximum timeout per write (2<<n) in microsecond.*/
    uint8_t maxTimeOutWrite;
    /** Maximum timeout for buffer write (2<<n) in microsecond.*/
    uint8_t maxTimeOutBuffer;
    /** Maximum timeout for block erase (2<<n) in microsecond.*/
    uint8_t maxTimeOutBlockErase;
    /** Maximum timeout for chip erase (2<<n) in microsecond.*/
    uint8_t maxTimeOutChipErase;
};

/*****************************************************************************
* DESCRIPTION
*  This is a Norflash CFI Erase block Region information.
*
*****************************************************************************/
struct EraseRegionInfo {
    /** Number of erase blocks within the region.*/
    uint16_t Y;
    /** Size within the region.*/
    uint16_t Z;
};

/*****************************************************************************
* DESCRIPTION
*  This is a Norflash CFI critical details of device geometry.
*
*****************************************************************************/
struct NorFlashCfiDeviceGeometry {
    uint8_t reserved1;
    /** Size of Device (2<<n) in number of bytes.*/
    uint8_t deviceSize;
    /** Flash device interface description.*/
    uint16_t deviceInterface;
    /** Maximum number of bytes in multi-byte write (2<<n).*/
    uint16_t numMultiWrite;
    /** Number of erase block regions.*/
    uint8_t numEraseRegion;
    uint8_t reserved2;
    /** Erase block Region information. */
    struct EraseRegionInfo eraseRegionInfo[16];
};

/*****************************************************************************
* DESCRIPTION
*  This is a Norflash Common Flash Interface information.
*
*****************************************************************************/
typedef struct _norflash_cfi {
    /** CFI query system interface information. */
    struct NorFlashCfiQueryInfo norFlashCfiQueryInfo;
    /** CFI critical details of device geometry. */
    struct NorFlashCfiDeviceGeometry norFlashCfiDeviceGeometry;
}NorFlashCFI_T;

/*****************************************************************************
* DESCRIPTION
*  This is a particular model of NorFlash device.
*
*****************************************************************************/
typedef struct _norflash {
    /* amd/intel cmd identify*/
	uint32_t cmdSet;
	/* Base address */
	uint32_t baseAddress;
	/* device ID */
	u16 manufacture_id;
	u16 device_id1;
	/* Address bus using giving by CFI detection.
	    It can not retrieve info directly from  the NorFlashCFI, it depend on hardware connection. */
	uint8_t deviceChipWidth;
	/* Indicate the decive CFI is compatible */
	uint8_t cfiCompatible;
	/* Norflash Common Flash Interface information. */
	NorFlashCFI_T cfiDescription;
}NorFlash_T;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

uint8_t NorFlash_CFI_Detect(NorFlash_T *pNorFlash, uint8_t hardwareBusWidth);
uint32_t NorFlash_GetDeviceNumOfBlocks(NorFlash_T *pNorFlash);
uint32_t NorFlash_GetDeviceMinBlockSize(NorFlash_T *pNorFlash);
uint32_t NorFlash_GetDeviceMaxBlockSize(    NorFlash_T *pNorFlash);
uint32_t NorFlash_GetDeviceBlockSize(    NorFlash_T *pNorFlash, uint32_t sector);
uint16_t NorFlash_GetDeviceSectorInRegion(    NorFlash_T *pNorFlash, uint32_t memoryOffset);
uint32_t NorFlash_GetDeviceSectorAddress(    NorFlash_T *pNorFlash, uint32_t sector);
uint32_t NorFlash_GetByteAddress(NorFlash_T *pNorFlash, uint32_t offset);
uint32_t NorFlash_GetByteAddressInChip(NorFlash_T *pNorFlash, uint32_t offset);
uint32_t NorFlash_GetAddressInChip(NorFlash_T *pNorFlash, uint32_t offset);
uint8_t NorFlash_GetDataBusWidth(NorFlash_T *pNorFlash);
unsigned long  NorFlash_GetDeviceSizeInBytes(NorFlash_T *pNorFlash);

#endif /* #ifndef _FMSH_SMC_CFI_H_ */

