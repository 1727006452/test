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
* @file fmsh_qspips.h
* @addtogroup qspips_v1_0
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
*                       
* </pre>
*
******************************************************************************/
#ifndef _FMSH_QSPIPS_H_	/* prevent circular inclusions */ 
#define _FMSH_QSPIPS_H_	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/ 

/************************** Constant Definitions *****************************/   

#ifndef FPAR_QSPIPS_0_DEVICE_ID    
    #define FPS_QSPI0_DEVICE_ID          (0)   
#else
    #define FPS_QSPI0_DEVICE_ID          FPAR_QSPIPS_0_DEVICE_ID
#endif

#ifndef  FPS_QSPI0_BASEADDR  
    #define FPS_QSPI0_BASEADDR           (0xe0000000)
#endif
    
#ifndef  FPS_QSPI0_D_BASEADDR  
    #define FPS_QSPI0_D_BASEADDR         (0xe8000000)
#endif
    
#ifndef FPAR_QSPIPS_1_DEVICE_ID    
    #define FPS_QSPI1_DEVICE_ID          (1)        
#else
    #define FPS_QSPI1_DEVICE_ID          FPAR_QSPIPS_1_DEVICE_ID  
#endif

#ifndef  FPS_QSPI1_BASEADDR  
    #define FPS_QSPI1_BASEADDR           (0xe0020000)
#endif
    
#ifndef  FPS_QSPI1_D_BASEADDR  
    #define FPS_QSPI1_D_BASEADDR         (0xe9000000)
#endif
    
#define FPAR_QSPIPS_NUM_INSTANCES               (2) 

/**************************** Type Definitions *******************************/  
    
typedef void (*FQspiPs_StatusHandler)(void *callBackRef, 
                                     u32 statusEvent, u32 byteCount);

typedef struct {
  u16 deviceId;		                /**< Unique ID  of device */
  u32 dataBaseAddress;              /**< Data Base address of the device */
  u32 baseAddress;                  /**< Config Base address of the device */
} FQspiPs_Config_T;

typedef struct FQspiPs_Tag{
  FQspiPs_Config_T config;          /**< Configuration structure */ 

  u32 flag;
  u8 maker;                         /**< Flash maker */
  u32 devSize;                      /**< Flash device size in bytes */
  u32 sectorSize;                   /**< Flash sector size in bytes */ 
  
  int isBusy;             
  
  u8 *sendBufferPtr;	            /**< Buffer to send (state) */
  u8 *recvBufferPtr;	            /**< Buffer to receive (state) */
  int requestedBytes;	            /**< Number of bytes to transfer (state) */
  int remainingBytes;	            /**< Number of bytes left to transfer(state) */
  
  FQspiPs_StatusHandler statusHandler;
  void* statusRef;
} FQspiPs_T;
           
/***************** Macros (Inline Functions) Definitions *********************/  
        
/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

FQspiPs_Config_T* FQspiPs_LookupConfig(u16 deviceId);
int FQspiPs_CfgInitialize(FQspiPs_T* qspi, FQspiPs_Config_T* configPtr);
void FQspiPs_Reset(FQspiPs_T* qspi);
void FQspiPs_SetStatusHandler(FQspiPs_T* qspi, void* callBackRef, 
                              FQspiPs_StatusHandler funcPtr);
int FQspiPs_SelfTest(FQspiPs_T* qspi);
int FQspiPs_SetFlashReadMode(FQspiPs_T* qspi, u8 cmd);
void FQspiPs_InterruptHandler(void* instancePtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* prevent circular inclusions */