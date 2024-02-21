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
* @file fmsh_nfcps_sw.h
* @addtogroup nfcps_v1_0
* @{
*
* This header file contains the basic functions (or macros)
* that can be used to access the device.
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
#ifndef _FMSH_NFCPS_SW_H_   /* prevent circular inclusions */
#define _FMSH_NFCPS_SW_H_

/**********************************Include File*********************************/

/**********************************Type Definition******************************/

/**********************************Macro (inline function) Definition***********/

/**********************************Function Prototype***************************/
int FNfcPs_Initialize(FNfcPs_T* nfc, u16 deviceId);
int FNfcPs_InitController(FNfcPs_T* nfc, u32 ctrl);
int FNfcPs_EccMode(FNfcPs_T* nfc);
int FNfcPs_EraseBlock(FNfcPs_T* nfc, u64 DestAddr, u32 BlockCount);
int FNfcPs_Write(FNfcPs_T* nfc, u64 destAddr, u32 byteCount, u8* srcPtr, u8* userSparePtr);
int FNfcPs_WriteSpareData(FNfcPs_T* nfc, u32 block, u32 page, u8* userSparePtr);
int FNfcPs_Read(FNfcPs_T* nfc, u64 srcAddr, u32 byteCount, u8* destPtr, u8* userSparePtr);
int FNfcPs_ReadSpareData(FNfcPs_T* nfc, u32 block, u32 page, u8* userSparePtr);

/**********************************Constant Definition**************************/

/**********************************Variable Definition**************************/



#endif	/* prevent circular inclusions */