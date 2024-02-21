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
* @file fmsh_qspips_flash.c
* @addtogroup qspips_v1_0
* @{
*
* Contains implements the spi nor flash interface functions of 
* the FQspiPs driver.
* See fmsh_qspips_flash.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.10  hzq 7/1/19 
* 		     First release
* 1.01  hzq 12/1/20
*			 The FQspiFlash_WREN API has been modified to increase poll time
*			 The FQspiFlash_WRDI API has been modified to increase poll time
*			 The FQspiFlash_GetFlagStatus API has been increased to support 
*			 micron device larger than 256Mb
*			 The FQspiFlash_SetReg16 API has been modified to support 
*			 micron device larger than 256Mb
*			 The FQspiFlash_SetReg8 API has been modified to support 
*			 micron device larger than 256Mb
*			 The FQspiFlash_WaitForFSR API has been increased to support 
*			 micron device larger than 256Mb
*			 The FQspiFlash_SectorErase API has been modified to support 
*			 micron device larger than 256Mb
*			 The FQspiFlash_ChipErase API has been modified to support 
*			 micron device larger than 256Mb
* </pre>
*
******************************************************************************/
#include "fmsh_qspips_lib.h"

/*****************************************************************************
* This function executes WREN.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FQspiFlash_WREN(FQspiPs_T* qspi) 
{
    int error = 0;
    u8 status;
    u32 timeout = 0; 
    // Send WREN(0x06) Command
    error = FQspiPs_CmdExecute(qspi, 0x06000001);    
    if(error){
        return FMSH_FAILURE;
    }
    while(1){     
        // poll status.wel value
        status = FQspiFlash_GetStatus1(qspi);
        if(status & WEL_MASK){
            return FMSH_SUCCESS;
        }
        delay_1ms();
        timeout++;
        if(timeout > 1500){
            return FMSH_FAILURE;
        }
    }
}

/*****************************************************************************
* This function executes WRDI.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FQspiFlash_WRDI(FQspiPs_T* qspi)
{ 
    int error = 0;
    u8 status;
    u32 timeout = 0;
    // Send WREN(0x04) Command
    error = FQspiPs_CmdExecute(qspi, 0x04000001);
    if(error){
        return FMSH_FAILURE;
    }
    while(1){
        // poll status.wel value   
        status = FQspiFlash_GetStatus1(qspi);
        status = ~status;
        if(status & WEL_MASK){
            return FMSH_SUCCESS;
        }
        delay_1ms();
        timeout++;
        if(timeout > 1500){
            return FMSH_FAILURE;
        }
    }
}

/*****************************************************************************
* This function executes READ STATUS1.
*
* @param
*
* @return
*		- Status value.
*
* @note		
*
******************************************************************************/
u8 FQspiFlash_GetStatus1(FQspiPs_T* qspi)
{
    //Send Read Status1 Register command(0x05) to device
    FQspiPs_CmdExecute(qspi, 0x05800001);
    
    return FMSH_ReadReg(qspi->config.baseAddress, QSPIPS_FCRDRL_OFFSET) & 0xff;
} 

u8 FQspiFlash_GetFlagStatus(FQspiPs_T* qspiPtr)
{
    //Send Read Flag Status Register command(0x70) to device
    FQspiPs_CmdExecute(qspiPtr, 0x70800001);
    return FMSH_ReadReg(qspiPtr->config.baseAddress, QSPIPS_FCRDRL_OFFSET) & 0xff;
} 

/*****************************************************************************
* This function executes READ 16 bits register.
*
* @param
*
* @return
*		- Register value.
*
* @note		
*
******************************************************************************/
u16 FQspiFlash_GetReg16(FQspiPs_T *qspi, u8 cmd)
{
    u32 reg;
    u16 value;
    
    reg = 0x00900001 | (cmd << 24);
    //Read Register command from device (2B)
    FQspiPs_CmdExecute(qspi, reg);    
    value = FMSH_ReadReg(qspi->config.baseAddress, QSPIPS_FCRDRL_OFFSET) & 0xffff;
    return value;
} 

/*****************************************************************************
* This function executes READ 8 bits register.
*
* @param
*
* @return
*		- Register value.
*
* @note		
*
******************************************************************************/
u8 FQspiFlash_GetReg8(FQspiPs_T *qspi, u8 cmd)
{
    u32 reg;
    u8 value;
    
    reg = 0x00800001 | (cmd << 24);
    //Read Register command from device (1B)
    FQspiPs_CmdExecute(qspi, reg);    
    value = FMSH_ReadReg(qspi->config.baseAddress, QSPIPS_FCRDRL_OFFSET) & 0xff;
    return value;
} 

/*****************************************************************************
* This function executes WRITE 16 bits register.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FQspiFlash_SetReg16(FQspiPs_T* qspi, u8 cmd, u8 high_value, u8 low_value)
{
    int error = FMSH_SUCCESS;
    u32 reg;
    
    reg = 0x00009001 | (cmd << 24);
    // Set value to be sent
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_FCWDRL_OFFSET, ((high_value << 8) | low_value));  
    error = FQspiFlash_WREN(qspi);
    if(error){
        return FMSH_FAILURE;
    }
    //Send command to device
    FQspiPs_CmdExecute(qspi, reg);
    
    error |= FQspiFlash_WaitForWIP(qspi);
    if(qspi->flag & QSPI_FSR)
    {
        error |= FQspiFlash_WaitForFSR(qspi);
    }
    
    return error;
}

/*****************************************************************************
* This function executes WRITE 8 bits register.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FQspiFlash_SetReg8(FQspiPs_T* qspi, u8 cmd, u8 value)
{
    int error = FMSH_SUCCESS;
    u32 reg;
    
    reg = 0x00008001 | (cmd << 24);
    // Set value to be sent
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_FCWDRL_OFFSET, (u32)value);
    
    error = FQspiFlash_WREN(qspi);
    if(error){
        return FMSH_FAILURE;
    }
    
    //Send command to device
    FQspiPs_CmdExecute(qspi, reg);
    
    error |= FQspiFlash_WaitForWIP(qspi);    
    if(qspi->flag & QSPI_FSR)
    {
        error |= FQspiFlash_WaitForFSR(qspi);
    }
    
    return error;
}

/*****************************************************************************
* This function executes READ ID.
*
* @param
*
* @return
*		- ID value.
*
* @note		
*
******************************************************************************/
u32 FQspiFlash_ReadId(FQspiPs_T* qspi)
{
    //Send RDID Command (4 bytes)
    FQspiPs_CmdExecute(qspi, 0x9FD00001);
    //Get ID
    return FMSH_ReadReg(qspi->config.baseAddress, QSPIPS_FCRDRL_OFFSET);
}

/*****************************************************************************
* This function executes READ STATUS1 and wait for WIP.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if overtime.
*
* @note		
*
******************************************************************************/
int FQspiFlash_WaitForWIP(FQspiPs_T* qspi)
{ 
    u8 status;
    u32 timeout = 0;
    
    // Poll Status Register1
    while(1){
        status = FQspiFlash_GetStatus1(qspi);
        status = ~status;
        status &= BUSY_MASK;
        if(status == BUSY_MASK){
            return FMSH_SUCCESS;
        }    
        delay_1ms();
        timeout++;
        if(timeout > 1500){
            return FMSH_FAILURE;
        }
    }
}

int FQspiFlash_WaitForFSR(FQspiPs_T* qspiPtr)
{ 
    u8 status;
    int det = 2;
    u32 timeout = 0;
    
    while(1){
        status = FQspiFlash_GetFlagStatus(qspiPtr);
        if((status & 0x80)== 0x80){
            det--;
            if(det == 0)
                return FMSH_SUCCESS;
        }    
        delay_1ms();
        timeout++;
        if(timeout > 1500){
            return FMSH_FAILURE;
        }
    }
}

/*****************************************************************************
* This function executes SECTOR ERASE.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FQspiFlash_SectorErase(FQspiPs_T* qspi, u32 start_offset, u8 cmd)
{
    int error = FMSH_SUCCESS;
    u32 reg;
    
    //Set Flash Command Address
    reg = qspi->config.dataBaseAddress + start_offset;
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_FCAR_OFFSET, reg);
    
    error = FQspiFlash_WREN(qspi);
    if(error){
        return FMSH_FAILURE;
    }
    
    //Send Erase Command
    reg = 0x000A0001 | (cmd << 24);
    FQspiPs_CmdExecute(qspi, reg);

    // Poll Status Register1
    u8 status;
    u32 timeout = 0;
    while(1){
        status = FQspiFlash_GetStatus1(qspi);
        status = ~status;
        status &= BUSY_MASK;
        if(status == BUSY_MASK){
            break;
        }    
        delay_1ms();
        timeout++;
        if(timeout > 5000){
            error |= FMSH_FAILURE;
            break;
        }
    }
    
    if(qspi->flag & QSPI_FSR)
    {
        error |= FQspiFlash_WaitForFSR(qspi);
    }
    return error;
}

/*****************************************************************************
* This function executes CHIP ERASE.
*
* @param
*
* @return
*		- FMSH_SUCCESS if success.
*		- FMSH_FAILURE if failure.
*
* @note		
*
******************************************************************************/
int FQspiFlash_ChipErase(FQspiPs_T* qspi, u8 cmd)
{
    int error = FMSH_SUCCESS;
    u32 reg;
    
    error = FQspiFlash_WREN(qspi);
    if(error){
        return FMSH_FAILURE;
    }
    
    //Send Erase Command
    reg = 0x00000001 | (cmd << 24);
    FQspiPs_CmdExecute(qspi, reg);

    // Poll Status Register1
    u8 status;
    u32 timeout = 0;
    while(1){
        status = FQspiFlash_GetStatus1(qspi);
        status = ~status;
        status &= BUSY_MASK;
        if(status == BUSY_MASK){
            break;
        }    
        delay_1ms();
        timeout++;
        if(timeout > 600000){
            error |= FMSH_FAILURE;
            break;
        }
    }
    
    if(qspi->flag & QSPI_FSR)
    {
        error |= FQspiFlash_WaitForFSR(qspi);
    }
    return error;
}
