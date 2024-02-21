/******************************************************************************
*
* Copyright (C) FMSH, Corp.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* FMSH BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the FMSH shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from FMSH.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file fmsh_nfcps.h
* @addtogroup nfcps_v1_0
* @{
*
* This header file contains the identifiers and driver
* functions (or macros) that can be used to access the device.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 11/26/20 
* 		     Delete macro of some controller and device infomation.
*            Add type defination FNfcPs_NandCaps which is a struct of 
*            configuration.      
*            Modify FSpiPs_T to increase support to assign data buffer
*            dynamiclly.
*                       
* </pre>
*
******************************************************************************/

#ifndef _FMSH_NFCPS_H_   /* prevent circular inclusions */
#define _FMSH_NFCPS_H_

#ifdef __cplusplus
extern "C" {
#endif
    
/**********************************Include File*********************************/

/**********************************Constant Definition**************************/

#ifndef FPAR_NANDPS_0_DEVICE_ID         
#define FPS_NFC_S_DEVICE_ID             (0) 
#else
#define FPS_NFC_S_DEVICE_ID             FPAR_NANDPS_0_DEVICE_ID     
#endif
    
#ifndef FPS_NFC_S_BASEADDR
#define FPS_NFC_S_BASEADDR              (0xE0042000)   
#endif
    
#define FPS_NFC_S_FLASHWIDTH            (16)
#define FPS_NFC_NUM_INSTANCES           (1)   

#define FPS_NFC_SAMPLE_DELAY          0 // 0 cycles
#define FPS_NFC_TIMING_TWHR           12// 60ns
#define FPS_NFC_TIMING_TRHW           20 // 100ns
#define FPS_NFC_TIMING_TADL           14 // 70ns
#define FPS_NFC_TIMING_TCCS           0 // not defined
#define FPS_NFC_TIMING_TWW            20 // 100ns
#define FPS_NFC_TIMING_TRR            4 // 20ns
#define FPS_NFC_TIMING_TWB            20 // (max)100ns
#define FPS_NFC_TIMING_TRWH           2 // 10ns
#define FPS_NFC_TIMING_TRWP           3 // 12ns

/**********************************Type Definition******************************/
    
typedef void (*Nfc_StatusHandler) (void *callBackRef, u32 statusEvent,
                                   u32 byteCount);

/******************************
* This enum contains ECC Mode
**************/
typedef enum {
	NFCPS_ECC_NONE,	        /**< No ECC */
	NFCPS_ECC_ONDIE,	    /**< On-Die ECC */
    NFCPS_ECC_HW,	        /**< Hardware controller ECC */
    NFCPS_ECC_SW	        /**< Hardware controller ECC */
} FNfcPs_EccMode_E;

/******************************
* Bad block table descriptor
**************/
typedef struct{
    u32 blockOffset;	    /**< Block offset where BBT resides */
    u32 sigOffset;		    /**< Signature offset in Spare area */
    u32 verOffset;		    /**< Offset of BBT version */
    u32 sigLength;		    /**< Length of the signature */
    u32 maxBlocks;		    /**< Max blocks to search for BBT */
    char signature[4];	    /**< BBT signature */
    u8 version;		        /**< BBT version */
    u32 valid;		        /**< BBT descriptor is valid or not */
} FNfcPs_BbtDesc_T;

/****************************
 * Bad block pattern
 ****************/
typedef struct {
	u32 options;		    /**< Options to search the bad block pattern */
	u32 offset;		        /**< Offset to search for specified pattern */
	u32 length;		        /**< Number of bytes to check the pattern */
	u8 pattern[2];		    /**< Pattern format to search for */
} FNfcPs_BbPattern_T;

typedef struct {
    u8 jedec_supp;
    u8 onfi_supp;
} FNfcPs_NandCaps;

/*****************************
 * This typedef contains configuration information for the flash device.
 ****************/
typedef struct {
    u16 deviceId;		    /**< Unique ID  of device */
    intptr_t baseAddress;        /**< AHB Base address of the device */
    u32 flashWidth;
} FNfcPs_Config_T;

typedef struct FNfcPs_Tag{
    FNfcPs_Config_T config;	                    /**< Configuration structure */   
    u32 flag;
    FNfcPs_ParaPage_T ParaPage;
    FNfcPs_Model_T model;                       /**< Part geometry */
    
    FNfcPs_NandCaps cap1;
    FNfcPs_EccMode_E eccMode;                   /**< ECC Mode */
    u8* eccCalc;	                            /**< Buffer for calculated ECC */
    u8* eccCode;	                            /**< Buffer for stored ECC */
    
    u8 isBadBlockWrite;
    FNfcPs_BbtDesc_T bbtDesc;                   /**< Bad block table descriptor */
    FNfcPs_BbtDesc_T bbtMirrorDesc;	            /**< Mirror BBT descriptor */
    FNfcPs_BbPattern_T bbPattern;	            /**< Bad block pattern to search */
    u8* bbInfo;
    u32* bbMap;                                 /**< Nandflash Bad Block Info */
    
    u8* dataBufferPtr;                          /**< Buffer to send (state) */
    u8* spareBufferPtr;	                        /**< Buffer to receive (state) */
    
    int (*Erase) (struct FNfcPs_Tag* nfc, u64 destAddr, u32 blockCount);
    int (*ReadPage) (struct FNfcPs_Tag* nfc, u64 srcAddr, u32 byteCount, u8* destPtr); /**< Read Page routine */
	int (*WritePage) (struct FNfcPs_Tag* nfc, u64 destAddr, u32 byteCount, u8* srcPtr); /**< Write Page routine */
    
    Nfc_StatusHandler statusHandler;
    void* statusRef;
} FNfcPs_T;

/**********************************Macro (inline function) Definition***********/

/**********************************Variable Definition**************************/

/**********************************Function Prototype***************************/
FNfcPs_Config_T* FNfcPs_LookupConfig(u16 deviceId);
int FNfcPs_CfgInitialize(FNfcPs_T* nfc, FNfcPs_Config_T* configPtr);
void FNfcPs_Reset(FNfcPs_T* nfc);
int FNfcPs_SelfTest(FNfcPs_T* nfc);
void FNfcPs_SetStatusHandler(FNfcPs_T* nfc, void* callBackRef, Nfc_StatusHandler funcPtr);
void FNfcPs_InterruptHandler(void *instancePtr);
u32 FNfcPs_FindInstruction(FNfcPs_T* nfc, u32 command);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* prevent circular inclusions */