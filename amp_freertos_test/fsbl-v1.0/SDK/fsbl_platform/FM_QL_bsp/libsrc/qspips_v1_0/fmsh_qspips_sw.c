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
* @file fmsh_qspips_sw.c
* @addtogroup qspips_v1_0
* @{
*
* Contains implements the low level interface functions of the FQspiPs driver.
* See fmsh_qspips_hw.h for a detailed description of the device and driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  hzq 7/1/19 
* 		     First release
* 1.01  hzq 12/1/20
*			 The FQspiPs_Initialize API has been modified to support 
*			 flash reset.
*			 The FQspiPs_GetFlashInfo API has been modified to support 
*			 micron device larger than 256Mb.
*			 The FQspiPs_ResetFlash API has been modified to fix bug.
*			 The FQspiPs_UnlockFlash API has been modified to fix bug.
* </pre>
*
******************************************************************************/
#include <string.h>
#include "fmsh_qspips_lib.h"

static int FQspiPs_Calibration(FQspiPs_T* qspiPtr, u32 baudRate);

/**************************
* qspi struct Initialize
*
**************/

int FQspiPs_Initialize(FQspiPs_T* qspi, u16 deviceId)
{
    int err = 0;
     
    FMSH_ASSERT(qspi != NULL);
    
    FQspiPs_Config_T* cfgPtr;
    cfgPtr = FQspiPs_LookupConfig(deviceId);
    if (cfgPtr == NULL) 
    {
        return FMSH_FAILURE;
    }
    err = FQspiPs_CfgInitialize(qspi, cfgPtr);  
    if(err != FMSH_SUCCESS)
    {
        return FMSH_FAILURE;
    }

    /* reset controller */
    FQspiPs_Reset(qspi); 
    
    /* increase operation speed */
    FQspiPs_SetBaudRate(qspi, QSPIPS_BAUD(8));
    
    /* get maker & device size */
    err = FQspiPs_GetFlashInfo(qspi);
    if(err != FMSH_SUCCESS)
    {
        return FMSH_FAILURE;
    } 	  
    
    FQspiPs_ResetFlash(qspi);
    
    /*set transfer mode */
    if(FQspiPs_IsFlashQuad(qspi))
        err = FQspiPs_SetFlashMode(qspi, QOR_CMD); 
    else
        err = FQspiPs_SetFlashMode(qspi, DOR_CMD); 
    if(err != FMSH_SUCCESS)
    {
        return FMSH_FAILURE;
    } 	
    
    /* set baud rate at 4 */
    FQspiPs_Calibration(qspi, 4);
	
    return FMSH_SUCCESS;
}

static int FQspiPs_Calibration(FQspiPs_T* qspiPtr, u32 baudRate)
{
    int i;
    u32 id = 0, id_temp = 0;
    int range_lo = -1, range_hi = -1;
    
    FQspiPs_Disable(qspiPtr);
    FQspiPs_SetBaudRate(qspiPtr, QSPIPS_BAUD(8));
    FQspiPs_SetCaptureDelay(qspiPtr, 0);
    FQspiPs_Enable(qspiPtr);
    id = FQspiFlash_ReadId(qspiPtr);
    
    FQspiPs_SetBaudRate(qspiPtr, QSPIPS_BAUD(baudRate));
    for(i=0; i<16; i++)
    {
        FQspiPs_Disable(qspiPtr);
        FQspiPs_SetCaptureDelay(qspiPtr, i);
        FQspiPs_Enable(qspiPtr);
        id_temp = FQspiFlash_ReadId(qspiPtr);
        /* search for lowest value of delay */
        if((range_lo == -1) && (id == id_temp))
        {
            range_lo = i;
            continue;
        }
        /* search for highest value of delay */
        if((range_lo != -1) && (id != id_temp))
        {
            range_hi = i-1;
            break;
        }
        range_hi = i;
    }
    
    if(range_lo == -1)
    {
        TRACE_OUT(DEBUG_OUT, "QSPI Calibration failed\n");
        return FMSH_FAILURE;
    }
    
    FQspiPs_Disable(qspiPtr);
    FQspiPs_SetCaptureDelay(qspiPtr, (range_hi + range_lo) / 2);
    FQspiPs_Enable(qspiPtr);
    TRACE_OUT(DEBUG_OUT, "QSPI Read data capture delay calibrate to %i (%i - %i)\n", 
              (range_hi + range_lo) / 2, range_lo, range_hi);
    
    return FMSH_SUCCESS;
}
/*******************************
* check flash info
*
***************/
int FQspiPs_GetFlashInfo(FQspiPs_T* qspi)
{  
    u32 flashId;
    u8 maker, devSize;
    
    FMSH_ASSERT(qspi != NULL);
    
    //Read ID in Auto mode.
    flashId = FQspiFlash_ReadId(qspi);
    maker = flashId & 0xff;
    devSize = (flashId >> 16) & 0xff;
    
    //Deduce flash make
    switch(maker)
    {
    case SPI_MICRON_ID: qspi->maker = SPI_MICRON_ID; break;
    case SPI_SPANSION_ID: qspi->maker = SPI_SPANSION_ID; break;
    case SPI_WINBOND_ID: qspi->maker = SPI_WINBOND_ID; break;
    case SPI_MACRONIX_ID: qspi->maker = SPI_MACRONIX_ID; break;
    case SPI_ISSI_ID: qspi->maker = SPI_ISSI_ID; break;
    case SPI_FMSH_ID: qspi->maker = SPI_FMSH_ID; break;
    case SPI_GD_ID: qspi->maker = SPI_GD_ID; break;
    default: qspi->maker = SPI_UNKNOWN_ID; break;
    }
    
    //Deduce flash Size
    switch(devSize)
    {
    case QSPIFLASH_SIZE_ID_256K: qspi->devSize = QSPIFLASH_SIZE_256K; break;
    case QSPIFLASH_SIZE_ID_512K: qspi->devSize = QSPIFLASH_SIZE_512K; break;
    case QSPIFLASH_SIZE_ID_1M: qspi->devSize = QSPIFLASH_SIZE_1M; break;
    case QSPIFLASH_SIZE_ID_2M: qspi->devSize = QSPIFLASH_SIZE_2M; break;
    case QSPIFLASH_SIZE_ID_4M: qspi->devSize = QSPIFLASH_SIZE_4M; break;
    case QSPIFLASH_SIZE_ID_8M: qspi->devSize = QSPIFLASH_SIZE_8M; break;
    case QSPIFLASH_SIZE_ID_16M: qspi->devSize = QSPIFLASH_SIZE_16M; break;
    case QSPIFLASH_SIZE_ID_32M: qspi->devSize = QSPIFLASH_SIZE_32M; break;
    case QSPIFLASH_SIZE_ID_64M: qspi->devSize = QSPIFLASH_SIZE_64M; break;
    case QSPIFLASH_SIZE_ID_128M: qspi->devSize = QSPIFLASH_SIZE_128M; break;
    case QSPIFLASH_SIZE_ID_256M: qspi->devSize = QSPIFLASH_SIZE_256M; break;
    case QSPIFLASH_SIZE_ID_512M:
    case ISSI_QSPIFLASH_SIZE_ID_512M: qspi->devSize = QSPIFLASH_SIZE_512M; break;
    case QSPIFLASH_SIZE_ID_1G: 
    case MACRONIX_QSPIFLASH_SIZE_ID_1G: qspi->devSize = QSPIFLASH_SIZE_1G; break;
    default: qspi->devSize = SPI_UNKNOWN_SIZE; break;
    }
    
    if((qspi->maker == SPI_MICRON_ID) && (qspi->devSize >= QSPIFLASH_SIZE_512M))
    {
        FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_WCCR_OFFSET, 0x022770);
        qspi->flag |= QSPI_FSR;
    }
    
    return FMSH_SUCCESS;
}

/********************************
* set flash(x1, x2, x4) mode and timing
*
****************/
int FQspiPs_IsFlashQuad(FQspiPs_T* qspi)
{
  
  FMSH_ASSERT(qspi != NULL);
  
  // config qspi flash registers (dummy & quad_en)
  if(qspi->maker == SPI_SPANSION_ID)
  {
    u8 cfg = FQspiFlash_GetReg8(qspi, SPANSION_RDCR_CMD); //0x35
    if((cfg & SPANSION_QUAD_EN_MASK) == 0x00) 
      return 0;
    else
      return 1;
  }
  
  else if(qspi->maker == SPI_MICRON_ID)
  {
      return 1;
  }
  
  else if(qspi->maker == SPI_WINBOND_ID)
  {
    u8 status2 = FQspiFlash_GetReg8(qspi, WINBOND_RDSR2_CMD); //0x35     
    if((status2 & WINBOND_QUAD_EN_MASK) == 0x00)
      return 0;
    else 
      return 1;
  }
  
  else if(qspi->maker == SPI_MACRONIX_ID)
  {
    u8 status = FQspiFlash_GetReg8(qspi, RDSR1_CMD); //0x05
    if((status & MACRONIX_QUAD_EN_MASK) == 0x00)
      return 0;
    else
      return 1;
  }
  
  else if(qspi->maker == SPI_ISSI_ID)
  {
    u8 status = FQspiFlash_GetReg8(qspi, 0x05); //RDSR1_CMD       
    if((status & ISSI_QUAD_EN_MASK) == 0x00)
      return 0;
    else 
      return 1;
  }
  
  else if(qspi->maker == SPI_FMSH_ID)
  {
    u8 status2 = FQspiFlash_GetReg8(qspi, FMSH_RDSR2_CMD); //0x35
    if((status2 & FMSH_QUAD_EN_MASK) == 0x00)
      return 0;
    else
      return 1;
  }
  
  return 0;
}

int FQspiPs_SetFlashMode(FQspiPs_T* qspi, u8 cmd)
{
    u32 configReg;
    
    FMSH_ASSERT(qspi != NULL);
    switch(cmd)
    {
    case READ_CMD: configReg = 0x00000003; break;
    case FAST_READ_CMD: configReg = 0x0800000B; break;
    case DOR_CMD: configReg = 0x0801003B; break;
    case QOR_CMD: configReg = 0x0802006B; break;
    case DIOR_CMD:   
        if(qspi->maker == SPI_MICRON_ID)
            configReg = 0x080110BB;
        else
            configReg = 0x040110BB;   
        break;
    case QIOR_CMD:
        if(qspi->maker == SPI_MICRON_ID)
            configReg = 0x0A0220EB;
        else
            configReg = 0x041220EB;   
        break;
    default: configReg = 0x0800000B; break;
    }
    
    FQspiPs_WaitIdle(qspi);   
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_DRIR_OFFSET, configReg);
    return FMSH_SUCCESS;
}

/****************************
* qspi flash xip
*
***************/
int FQspiPs_EnterXIP(FQspiPs_T* qspi, u8 cmd)
{
    u32 DRIR;
    
    FMSH_ASSERT(qspi != NULL);
    
    FQspiPs_DisableDAC(qspi);
    
    if((qspi->maker == SPI_SPANSION_ID) ||
       (qspi->maker == SPI_ISSI_ID) || 
       (qspi->maker == SPI_MACRONIX_ID)) 
    {
        FQspiPs_SetModeBits(qspi, 0xA5);
        if(cmd == DIOR_CMD)
        {
            DRIR = 0x001110BB;    
        }
        else if(cmd == QIOR_CMD)
        {
            DRIR = 0x041220EB;
        }
    }
    
    else if((qspi->maker == SPI_WINBOND_ID) ||
            (qspi->maker == SPI_FMSH_ID) || 
            (qspi->maker == SPI_GD_ID)) 
    {
        FQspiPs_SetModeBits(qspi, 0x20);
        if(cmd == DIOR_CMD)
        {
            DRIR = 0x001110BB;    
        }
        else if(cmd == QIOR_CMD)
        {
            DRIR = 0x041220EB;
        }
    }
    
    else if(qspi->maker == SPI_MICRON_ID)
    {
        FQspiPs_SetModeBits(qspi, 0x00);
        if(cmd == DIOR_CMD)
        {
            DRIR = 0x041110BB;    
        }
        else if(cmd == QIOR_CMD)
        {
            DRIR = 0x081220EB;
        }
        
        u8 volatile_cfg = FQspiFlash_GetReg8(qspi, MICRON_RDVCR_CMD);
        FQspiFlash_SetReg8(qspi, MICRON_WRVCR_CMD, volatile_cfg & 0xf3);
    } 
    
    else
    {
        FQspiPs_SetModeBits(qspi, 0xA5);
        if(cmd == DIOR_CMD)
        {
            DRIR = 0x001110BB;    
        }
        else if(cmd == QIOR_CMD)
        {
            DRIR = 0x041220EB;
        }
    }
    
    FQspiPs_WaitIdle(qspi);   
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_DRIR_OFFSET, DRIR);
    
    FQspiPs_EnableXip(qspi);
    FQspiPs_EnableDAC(qspi);
    
    return FMSH_SUCCESS;
}

/****************************
* qspi flash reset
*
***************/
int FQspiPs_ResetFlash(FQspiPs_T* qspi)
{
    int err = 0;
    
    FMSH_ASSERT(qspi != NULL);
    
    if(qspi->maker == SPI_SPANSION_ID)
    {
        err = FQspiPs_CmdExecute(qspi, 0xFF000001); 
        if(err != FMSH_SUCCESS) 
        {
            return FMSH_FAILURE;
        }
        err = FQspiPs_CmdExecute(qspi, 0xF0000001); 
        if(err != FMSH_SUCCESS) 
        {
            return FMSH_FAILURE;
        }
    }
    else
    {
        err = FQspiPs_CmdExecute(qspi, 0x66000001); 
        if(err != FMSH_SUCCESS) 
        {
            return FMSH_FAILURE;
        }
        err = FQspiPs_CmdExecute(qspi, 0x99000001); 
        if(err != FMSH_SUCCESS) 
        {
            return FMSH_FAILURE;
        }
    }
    
    delay_1ms();
    
    return FMSH_SUCCESS;
}

/*******************************
* qspi flash enable quad
*
**********************/

int FQspiPs_EnableQuad(FQspiPs_T* qspi)
{
    int err;
    
    FMSH_ASSERT(qspi != NULL);

    // config qspi flash registers (quad_en)
    if(qspi->maker == SPI_SPANSION_ID)
    {
        u8 status1 = FQspiFlash_GetReg8(qspi, RDSR1_CMD); //0x05
        u8 cfg = FQspiFlash_GetReg8(qspi, SPANSION_RDCR_CMD); //0x35
        if((cfg & SPANSION_QUAD_EN_MASK) == 0)
        {
            cfg |= (u8)SPANSION_QUAD_EN_MASK;
            err = FQspiFlash_SetReg16(qspi, WRR_CMD, cfg, status1);//0x01
            if(err)
            {
                return FMSH_FAILURE;
            }  
        }	
    }
    
    else if(qspi->maker == SPI_MICRON_ID)
    {

    }
    
    else if(qspi->maker == SPI_MACRONIX_ID)
    {
        u8 status = FQspiFlash_GetReg8(qspi, RDSR1_CMD); //0x05
        if((status & MACRONIX_QUAD_EN_MASK) == 0)
        {
            status |= (u8)MACRONIX_QUAD_EN_MASK;
            err = FQspiFlash_SetReg8(qspi, WRR_CMD, status);//0x01
            if(err)
            {
                return FMSH_FAILURE;
            }  
        }  
    }
    
    else if(qspi->maker == SPI_ISSI_ID)
    {   
        u8 status = FQspiFlash_GetReg8(qspi, RDSR1_CMD); //0x05   
        if((status & ISSI_QUAD_EN_MASK) == 0)
        {
            status |= (u8)ISSI_QUAD_EN_MASK;
            err = FQspiFlash_SetReg8(qspi, WRR_CMD, status); //0x01
            if(err)
            {
                return FMSH_FAILURE;
            } 
        }    
    }
    
    else if(qspi->maker == SPI_WINBOND_ID)
    {
        u8 status1 = FQspiFlash_GetReg8(qspi, RDSR1_CMD); //0x05
        u8 status2 = FQspiFlash_GetReg8(qspi, WINBOND_RDSR2_CMD); //0x35     
        if((status2 & WINBOND_QUAD_EN_MASK) == 0)
        {
            status2 |= (u8)WINBOND_QUAD_EN_MASK;
            err = FQspiFlash_SetReg16(qspi, WRR_CMD, status2, status1);//0x01
            if(err)
            {
                return FMSH_FAILURE;
            }  
        }
    }
    
    else if(qspi->maker == SPI_FMSH_ID)
    {
        u8 status2 = FQspiFlash_GetReg8(qspi, FMSH_RDSR2_CMD); //0x35
        if((status2 & FMSH_QUAD_EN_MASK) == 0)
        {
            status2 |= (u8)FMSH_QUAD_EN_MASK;
            err = FQspiFlash_SetReg8(qspi, 0x31, status2);//0x31
            if(err)
            {
                return FMSH_FAILURE;
            }
        } 
    }
 
    else if(qspi->maker == SPI_GD_ID)
    {
        u8 status2 = FQspiFlash_GetReg8(qspi, GD_RDSR2_CMD); //0x35
        if((status2 & GD_QUAD_EN_MASK) == 0)
        {
            status2 |= (u8)GD_QUAD_EN_MASK;
            err = FQspiFlash_SetReg8(qspi, 0x31, status2);//0x31
            if(err)
            {
                return FMSH_FAILURE;
            }
        } 
    }
    
    else{
        //do nothing
    }
    
    return FMSH_SUCCESS;
}

/*******************************
* qspi flash disable protect
*
**********************/
int FQspiPs_UnlockFlash(FQspiPs_T* qspi)
{
    int err = 0;
    
    FMSH_ASSERT(qspi != NULL);
       
    if(qspi->maker == SPI_ISSI_ID)
    {
        u8 status = FQspiFlash_GetReg8(qspi, 0x05); //RDSR1_CMD
        status &= ISSI_QUAD_EN_MASK;
        err = FQspiFlash_SetReg8(qspi, WRR_CMD, status);   
    }
    
    else if(qspi->maker == SPI_MACRONIX_ID)
    {
        u8 status = FQspiFlash_GetReg8(qspi, 0x05); //RDSR1_CMD
        status &= MACRONIX_QUAD_EN_MASK;
        err = FQspiFlash_SetReg8(qspi, WRR_CMD, status);   
    }
    else if(qspi->maker == SPI_SPANSION_ID)
    {
        u8 cfg = FQspiFlash_GetReg8(qspi, 0x35); //RDSR1_CMD
        err = FQspiFlash_SetReg16(qspi, WRR_CMD, cfg, 0x00);    
    }
    else
    {
        err = FQspiFlash_SetReg8(qspi, WRR_CMD, 0x00);  
    }

    if(err != FMSH_SUCCESS) 
    {
        return FMSH_FAILURE;
    }
    return FMSH_SUCCESS;
}

/******************************
* remap high address data to 16M AHB area
*
******************/
int FQspiPs_SetFlashSegment(FQspiPs_T* qspi, u8 highAddr)
{
    int err;
    u8 extAddr, extAddrRb;
    
    FMSH_ASSERT(qspi != NULL);
    
    if((qspi->maker == SPI_SPANSION_ID) ||
       (qspi->maker == SPI_ISSI_ID))
    {        
        /* bit7 is extadd id 0 to enable 3B address */
        extAddr = highAddr & 0x7F;
        err = FQspiFlash_SetReg8(qspi, 0x17, extAddr); 
        if(err != 0)
        {
            return FMSH_FAILURE;
        }
        
        /* readback & verify */
        extAddrRb = FQspiFlash_GetReg8(qspi, 0x16); 
        if(extAddrRb != extAddr)
        {
            return FMSH_FAILURE;
        }       
    }
    
    else if((qspi->maker == SPI_MICRON_ID) ||
            (qspi->maker == SPI_MACRONIX_ID) ||
            (qspi->maker == SPI_WINBOND_ID) || 
            (qspi->maker == SPI_FMSH_ID) ||
            (qspi->maker == SPI_GD_ID))
    {        
        extAddr = highAddr;
        
        err = FQspiFlash_SetReg8(qspi, 0xC5, extAddr);
        if(err != 0)
        {
            return FMSH_FAILURE;
        }
        
        /* readback & verify */
        extAddrRb = FQspiFlash_GetReg8(qspi, 0xC8);
        if(extAddrRb != extAddr)
        {
            return FMSH_FAILURE;
        }   
    }

    else
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

/******************************
* erase all data on chip
*
*****************/
int FQspiPs_EraseChip(FQspiPs_T* qspi)
{
    int err;
    
    FMSH_ASSERT(qspi != NULL);
    
    err = FQspiFlash_ChipErase(qspi, CE_CMD);
    if(err != FMSH_SUCCESS)
    {
        return FMSH_FAILURE;
    }
    
    return FMSH_SUCCESS;
}

int FQspiPs_EraseSectors(FQspiPs_T* qspi, u32 offset, u32 byteCount, u32 sectorSize)
{
    int err;
    u32 sectorOffset;
    u32 sectorEnd = offset + byteCount;
    
    FMSH_ASSERT(qspi != NULL);
    
    qspi->sectorSize = sectorSize;
    
    if(byteCount == 0)
    {
        return FMSH_FAILURE; 
    }
    if(qspi->devSize < sectorEnd)
    {
        return FMSH_FAILURE; 
    }   
    if(qspi->isBusy == TRUE)
    {
        return FMSH_FAILURE;
    }
    qspi->isBusy = TRUE;
    
    /* calculate sector start address to be erased */
    sectorOffset = offset & ~(qspi->sectorSize-1);
    /* Erase Flash with required sectors */
    while(sectorOffset < sectorEnd)
    {
        err = FQspiFlash_SectorErase(qspi, sectorOffset, SE_CMD);
        if(err != 0)
        {
            return FMSH_FAILURE;
        }
        sectorOffset += qspi->sectorSize;
    }
    
    qspi->isBusy = FALSE;
    
    return FMSH_SUCCESS;
}

int FQspiPs_SendBytes(FQspiPs_T* qspi, u32 offset, u32 byteCount, u8 *sendBuffer)
{
    int err;
    u32 address;
    
    FMSH_ASSERT(qspi != NULL);
    FMSH_ASSERT(sendBuffer != NULL);
    
    if(byteCount == 0)
    {
        return FMSH_FAILURE; 
    }
    if(qspi->devSize < (offset + byteCount))
    {
        return FMSH_FAILURE; 
    }
    if(qspi->isBusy == TRUE)
    {
        return FMSH_FAILURE;
    }
    qspi->isBusy = TRUE;
    
    /* Write to Flash  */
    address = qspi->config.dataBaseAddress + offset;
    memcpy((void*)address,
           (void*)sendBuffer,
           (size_t)byteCount); 
    
    /* Wait for write operation done */
    err = FQspiFlash_WaitForWIP(qspi);
    if(err)
    {
        return FMSH_FAILURE;
    }
    
    qspi->isBusy = FALSE;
    
    return FMSH_SUCCESS;
}

int FQspiPs_RecvBytes(FQspiPs_T* qspi, u32 offset, u32 byteCount, u8 *recvBuffer)
{
    u32 address;
    
    FMSH_ASSERT(qspi != NULL);
    FMSH_ASSERT(recvBuffer != NULL);
    
    if(byteCount == 0)
    {
        return FMSH_FAILURE; 
    }    
    if(qspi->devSize < (offset + byteCount))
    {
        return FMSH_FAILURE; 
    }
    if(qspi->isBusy == TRUE)
    {
        return FMSH_FAILURE;
    }
    qspi->isBusy = TRUE;
    
    /* Read from flash */
    address = qspi->config.dataBaseAddress + offset;
    memcpy((void*)recvBuffer,
           (void*)address,
           (size_t)byteCount); 
    
    qspi->isBusy = FALSE;
    
    return FMSH_SUCCESS;
}

int FQspiPs_FastSendBytes(FQspiPs_T* qspi, u32 offset, u32 byteCount, u8* sendBuffer)
{
    u32 configReg;
    u32 waterMark = 32;
    
    FMSH_ASSERT(qspi != NULL);
    FMSH_ASSERT(sendBuffer != NULL);
    
    if(qspi->isBusy == TRUE)
    {
        return FMSH_FAILURE;
    }
    qspi->isBusy = TRUE;
    
    qspi->requestedBytes = byteCount;
    qspi->remainingBytes = byteCount;
    qspi->sendBufferPtr = sendBuffer;
    
    /* set flash address where write data (address in flash) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IWTSAR_OFFSET, offset);  
    /* set number of bytes to be transferred */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IWTNBR_OFFSET, qspi->requestedBytes);  
    /* set controller trig adress where write data (adress in controller) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IAATR_OFFSET, qspi->config.dataBaseAddress);    
    /* set Trigger Address Range (2^15) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_ITARR_OFFSET, 0xf);      
    /* set WaterMark Register (between 10~250 if page_size is 256 bytes) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IWTWR_OFFSET, waterMark);   
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IRTWR_OFFSET, 0x0);   
    /* trigger Indirect Write access */
    configReg = 0x1;
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IWTCR_OFFSET, configReg);
    
    return FMSH_SUCCESS;
}

int FQspiPs_FastRecvBytes(FQspiPs_T* qspi, u32 offset, u32 byteCount, u8* recvBuffer)
{
    u32 configReg;
    u32 waterMark = 480;
    
    FMSH_ASSERT(qspi != NULL);
    FMSH_ASSERT(recvBuffer != NULL);
    
    if(qspi->isBusy == TRUE)
    {
        return FMSH_FAILURE;
    }
    qspi->isBusy = TRUE;
    
    qspi->requestedBytes = byteCount;
    qspi->remainingBytes = byteCount;
    qspi->recvBufferPtr = recvBuffer;
    
    /* set flash address where read data (address in flash) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IRTSAR_OFFSET, offset);  
    /* set number of bytes to be transferred */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IRTNBR_OFFSET, qspi->requestedBytes);  
    /* set controller trig adress where read data (adress in controller) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IAATR_OFFSET, qspi->config.dataBaseAddress);    
    /* set Trigger Address Range (2^15) */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_ITARR_OFFSET, 0xf);  
    /* set WaterMark Register */
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IWTWR_OFFSET, 0x0); 
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IRTWR_OFFSET, waterMark);  
    /* trigger Indirect Write access */
    configReg = 0x1;
    FMSH_WriteReg(qspi->config.baseAddress, QSPIPS_IRTCR_OFFSET, configReg);  
    
    return FMSH_SUCCESS;
}

