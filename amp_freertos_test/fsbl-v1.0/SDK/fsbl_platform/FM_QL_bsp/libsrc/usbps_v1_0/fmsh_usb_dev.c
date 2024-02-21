/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_usb_dev.c
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
* 0.01   lq  1/26/2021  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include "fmsh_usb_dev.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
#define CONFIG_DESC_TOTAL_SIZE   	(CONFIG_DESC_SIZE+INTERFACE_DESC_SIZE+ENDPOINT_DESC_SIZE*2+INTERFACE_DESC_SIZE+DFU_DESCFUNC_SIZE)
#define TEST_PKT_SIZE 53

/************************** Variable Definitions *****************************/
u8 TestPkt [TEST_PKT_SIZE] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,					//JKJKJKJK x 9
    0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,						//JJKKJJKK x 8
    0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,						//JJJJKKKK x 8
    0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	                //JJJJJJJKKKKKKK x8 - '1'
    0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,							//'1' + JJJJJJJK x 8
    0xFC,0x7E,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0x7E					//{JKKKKKKK x 10},JK
};

//=====================================================================
// setting the device qualifier descriptor and a string descriptor
const u8 aDeviceQualifierDescriptor[] =
{
	0x0a,                   //  0 desc size
	0x06,                   //  1 desc type (DEVICE_QUALIFIER)
	0x00,                   //  2 USB release
	0x02,                   //  3 => 2.00
	0xFF,                   //  4 class
	0x00,                   //  5 subclass
	0x00,                   //  6 protocol
	64,          			//  7 max pack size
	0x00,                   //  8 number of other-speed configuration
	0x00,                   //  9 reserved
};

const u8 aOtherSpeedConfiguration_full[] =
{
	0x09,                   //  0 desc size
	0x07,                   //  1 desc type (other speed)
	0x20,                   //  2 Total length of data returned
	0x00,                   //  3 
	0x01,                   //  4 Number of interfaces supported by this speed configuration
	0x01,                   //  5 value to use to select configuration
	0x00,                   //  6 index of string desc
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,   //  7 same as configuration desc
	0x19,                   //  8 same as configuration desc
	
};

const u8 aOtherSpeedConfiguration_fullTotal[] =
{
  0x09, 0x07 ,0x20 ,0x00 ,0x01 ,0x01 ,0x00 ,0xC0 ,0x19,
  0x09 ,0x04 ,0x00 ,0x00 ,0x02 ,0xff ,0x00 ,0x00 ,0x00,
  0x07 ,0x05 ,0x83 ,0x02 ,0x40 ,0x00 ,0x00,
  0x07 ,0x05 ,0x04 ,0x02 ,0x40 ,0x00 ,0x00
};

const u8 aOtherSpeedConfiguration_high[] =
{
	0x09,                   //  0 desc size
	0x07,                   //  1 desc type (other speed)
	0x20,                   //  2 Total length of data returned
	0x00,                   //  3 
	0x01,                   //  4 Number of interfaces supported by this speed configuration
	0x01,                   //  5 value to use to select configuration
	0x00,                   //  6 index of string desc
	CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED,   //  7 same as configuration desc
	0x19,                   //  8 same as configuration desc
	
};

const u8 aOtherSpeedConfiguration_highTotal[] =
{
  0x09, 0x07 ,0x20 ,0x00 ,0x01 ,0x01 ,0x00 ,0xC0 ,0x19,
  0x09 ,0x04 ,0x00 ,0x00 ,0x02 ,0xff ,0x00 ,0x00 ,0x00,
  0x07 ,0x05 ,0x81 ,0x02 ,0x00 ,0x02 ,0x00,
  0x07 ,0x05 ,0x02 ,0x02 ,0x00 ,0x02 ,0x00
};


const u8 aDescStr0[]=
{
	4, STRING_DESCRIPTOR, LANGID_US_L, LANGID_US_H, // codes representing languages
};

const u8 aDescStr1[]= // Manufacturer
{
	(0x12+2), STRING_DESCRIPTOR,
	'F', 0x0, 'M', 0x0, 'S', 0x0, 'H', 0x0, '-', 0x0, 'P', 0x0, 'S', 0x0, 'O', 0x0,
	'C', 0x0,
};

const u8 aDescStr2[]= // Product
{
	(0x2e+2), STRING_DESCRIPTOR,
	'F', 0x0, 'M', 0x0, 'S', 0x0, 'H', 0x0, '-', 0x0, 'F', 0x0, 'M', 0x0, 'Q', 0x0,
	'L', 0x0, '4', 0x0, '5', 0x0, 'T', 0x0, '9', 0x0, '0', 0x0, '0', 0x0, 'D', 0x0,
	'e', 0x0, 'm', 0x0, 'o', 0x0, ' ', 0x0, 'D', 0x0, 'F', 0x0, 'U', 0x0,
};

const u8 aDescStr3[]=
{
	(0x2c+2), STRING_DESCRIPTOR,
	'D', 0x0, 'F', 0x0, 'U', 0x0, ' ', 0x0, '2', 0x0, '.', 0x0, '0', 0x0, ' ', 0x0,
	'e', 0x0, 'm', 0x0, 'u', 0x0, 'l', 0x0, 'a', 0x0, 't', 0x0, 'i', 0x0, 'o', 0x0,
	'n', 0x0, ' ', 0x0, 'v', 0x0, '1', 0x0, '.', 0x0, '1', 0x0
};

const u8 aDescStr4[]= // Manufacturer
{
	(0x1E+2), STRING_DESCRIPTOR,
	'2', 0x0, 'A', 0x0, '4', 0x0, '9', 0x0, '8', 0x0, '7', 0x0, '6', 0x0, 'D', 0x0,
	'9', 0x0, 'C', 0x0, 'C', 0x0, '1', 0x0, 'A', 0x0, 'A', 0x0, '4', 0x0, 
};

const u8 aDescStr5[]= // Product
{
	(0x30+2), STRING_DESCRIPTOR,
	'7', 0x0, 'A', 0x0, 'B', 0x0, 'C', 0x0, '7', 0x0, 'A', 0x0, 'B', 0x0, 'C', 0x0,
	'7', 0x0, 'A', 0x0, 'B', 0x0, 'C', 0x0, '7', 0x0, 'A', 0x0, 'B', 0x0, 'C', 0x0,
	'7', 0x0, 'A', 0x0, 'B', 0x0, 'C', 0x0, '7', 0x0, 'A', 0x0, 'B', 0x0, 'C', 0x0
};

FUsbDevPs_T g_UsbDev;
USB_GET_STATUS oStatusGet;
USB_INTERFACE_GET oInterfaceGet;

u16 g_usConfig;
u16 g_usUploadPktLength=0;
u8 g_bTransferEp0 = FALSE;

DfuIf DfuObj;
u32 DownloadDone = 0U;
u8* DfuVirtFlash = (u8*)0x100000;

/************************** Function Prototypes ******************************/

/*****************************************************************************
 * This function handles setting of DFU state.
 *
 * @param	dfu_state is a value of the DFU state to be set
 *
 * @return
 *				None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void DfuSetState(u32 DfuState) 
{
	switch (DfuState) {

		case STATE_APP_IDLE:
		{
			DfuObj.CurrState = STATE_APP_IDLE;
			DfuObj.NextState = STATE_APP_DETACH;
			DfuObj.CurrStatus = DFU_STATUS_OK;
			DfuObj.RuntimeToDfu = 0U;
		}
		break;

		case STATE_APP_DETACH:
		{
			if (DfuObj.CurrState == STATE_APP_IDLE) {

				DfuObj.CurrState = STATE_APP_DETACH;
				DfuObj.NextState = STATE_DFU_IDLE;

				/* Set this flag to indicate we are going from runtime to dfu mode */
				DfuObj.RuntimeToDfu = 1U;

				/* fall through */
			} else if (DfuObj.CurrState == STATE_DFU_IDLE) {
				/* Wait For USB Reset to happen */
				DfuObj.CurrState = STATE_APP_IDLE;
				DfuObj.NextState = STATE_APP_DETACH;
				DfuObj.CurrStatus = DFU_STATUS_OK;
				break;
			} else {
				goto stall;
			}
		}

		case STATE_DFU_IDLE:
		{
			DfuObj.CurrState = STATE_DFU_IDLE;
			DfuObj.NextState = STATE_DFU_DOWNLOAD_SYNC;
		}
		break;
		case STATE_DFU_DOWNLOAD_SYNC:
		{
			DfuObj.CurrState = STATE_DFU_DOWNLOAD_SYNC;
		}
		break;

		case STATE_DFU_DOWNLOAD_BUSY:
		case STATE_DFU_DOWNLOAD_IDLE:
		case STATE_DFU_ERROR:
		default:
		{
		stall:
			 /* Unsupported command. Stall the end point.*/
			DfuObj.CurrState = STATE_DFU_ERROR;
		}
	}

}

/*****************************************************************************
 * This function set dfu download address.
 *
 * @param	addr is a value of download address.
 *
 * @return
 *				None.
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetDownLoadAddr(u32 addr)
{
    DfuVirtFlash = (u8*)addr;
    return ;
}

/*****************************************************************************
 * This function get dfu download completed flag.
 *
 * @param	
 *
 * @return
 *		        download completed flag.
 *
 * @note		None.
 *
 ******************************************************************************/
u8 FUsbPs_GetDownLoadComplete()
{
   return g_UsbDev.m_uDfuDownloadDone;
}

void DfuSetIntf(DEVICE_REQUEST *SetupData)
{
	/* Setting the alternate setting requested */
	DfuObj.CurrentInf = SetupData->wValue_H*256+SetupData->wValue_L;
	if ((DfuObj.CurrentInf >= DFU_ALT_SETTING) || (DfuObj.RuntimeToDfu == 1U)) {

		/* Clear the flag , before entering into DFU mode from runtime mode */
		if (DfuObj.RuntimeToDfu == 1U)
			DfuObj.RuntimeToDfu = 0U;

		/* Entering DFU_IDLE state */
		DfuSetState(STATE_DFU_IDLE);
	} else {
		/* Entering APP_IDLE state */
		DfuSetState(STATE_APP_IDLE);
	}
}

/*****************************************************************************
 *  This function reads data from EP FIFO .
 *
 * @param
 *   usbDev is usb handle
 *   buf, address of the data buffer to write
 *   num, size of the data to read from Bulk Out EP FIFO(byte count.
 *
 * @return
 *		        download completed flag.
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_RdPktEp0(FUsbPs_T *usbDev,u8 *buf, int num)
{
	int i;
	u32 Rdata;

	for (i=0;i<num;i+=4)
	{
		Rdata = FMSH_ReadReg(usbDev->base_address,(control_EP_FIFO));

		buf[i] = (u8)Rdata;
		buf[i+1] = (u8)(Rdata>>8);
		buf[i+2] = (u8)(Rdata>>16);
		buf[i+3] = (u8)(Rdata>>24);
	}
}

/*****************************************************************************
 *  This function handles EP0 control out transfer.
 *
 * @param
 *   usbDev is usb handle
 *
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_HandleEvent_Ep0Out(FUsbPs_T *usbDev,u32 fifoCntByte)
{
        
        u32 RxBytesLeft = fifoCntByte;
	
	if(RxBytesLeft > 0U) 
        {
                FUsbPs_RdPktEp0(usbDev,&DfuVirtFlash[DfuObj.TotalBytesDnloaded],fifoCntByte);
                FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uBulkOutEPMaxPktSize);
		DfuObj.TotalBytesDnloaded += fifoCntByte;
		DfuObj.CurrState = STATE_DFU_DOWNLOAD_IDLE;
		DfuObj.GotDnloadRqst = 0U;
        } else {/*if (RxBytesLeft == 0U)*/
		DfuObj.CurrState = STATE_DFU_IDLE;
		DfuObj.GotDnloadRqst = 0U;
		//Result = FMSH_FAILURE;
	}

	
}

/*****************************************************************************
 *  This function initializes Otg.
 *
 * @param
 *   usbDev is usb handle
 *   eSpeed, USB Speed (high or full)
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_InitOtg(FUsbPs_T *usbDev,USB_SPEED eSpeed)
{
	u8 ucMode;
	g_UsbDev.m_eSpeed = eSpeed;
	g_UsbDev.m_uIsUsbOtgSetConfiguration = 0;
	g_UsbDev.m_uEp0State = EP0_STATE_INIT;
	g_UsbDev.m_uEp0SubState = 0;
	FUsbPs_SoftResetCore(usbDev);
	FUsbPs_WaitCableInsertion(usbDev);
	FUsbPs_InitCore(usbDev);
	FUsbPs_CheckCurrentMode(usbDev,&ucMode);
	if (ucMode == INT_DEV_MODE)
	{
		FUsbPs_SetSoftDisconnect(usbDev);
		delay_ms(10);
		FUsbPs_ClearSoftDisconnect(usbDev);
		FUsbPs_InitDevice(usbDev);
	}
	else
	{
		TRACE_OUT(DEBUG_OUT, "Error : Current Mode is Host\n");
		return;
	}
}

/*****************************************************************************
 *  This function handles various OTG interrupts of Device mode.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_HandleEvent(FUsbPs_T *usbDev)
{
	u32 uGIntStatus, uDStatus;
	u32 ep_int_status, ep_int;

	uGIntStatus = FMSH_ReadReg(usbDev->base_address,GINTSTS); // System status read
	FMSH_WriteReg(usbDev->base_address,GINTSTS, uGIntStatus); // Interrupt Clear		
	TRACE_OUT(DEBUG_OUT, "GINTSTS : %x \n", uGIntStatus);

	if (uGIntStatus & INT_RESET) // Reset interrupt
	{
		FMSH_WriteReg(usbDev->base_address,DCTL,FMSH_ReadReg(usbDev->base_address,DCTL) & ~(TEST_CONTROL_FIELD));
		FUsbPs_SetAllOutEpNak(usbDev);
		g_UsbDev.m_uEp0State = EP0_STATE_INIT;
		FMSH_WriteReg(usbDev->base_address,DAINTMSK,((1<<BULK_OUT_EP)|(1<<CONTROL_EP))<<16|((1<<BULK_IN_EP)|(1<<CONTROL_EP)));
		FMSH_WriteReg(usbDev->base_address,DOEPMSK, CTRL_OUT_EP_SETUP_PHASE_DONE|AHB_ERROR|TRANSFER_DONE);
		FMSH_WriteReg(usbDev->base_address,DIEPMSK, INTKN_TXFEMP|NON_ISO_IN_EP_TIMEOUT|AHB_ERROR|TRANSFER_DONE);
		FUsbPs_ClearAllOutEpNak(usbDev);
		FMSH_WriteReg(usbDev->base_address,DCFG, FMSH_ReadReg(usbDev->base_address,DCFG)&~(0x7f<<4));	//clear device address
	}

	if (uGIntStatus & INT_ENUMDONE) // Device Speed Detection interrupt
	{
		TRACE_OUT(DEBUG_OUT, "\n [USB_Diag_Log]  : Speed Detection interrupt \n");
		
		uDStatus = FMSH_ReadReg(usbDev->base_address,DSTS); // System status read

		if (((uDStatus&0x6) >>1) == USB_HIGH) 			// Set if Device is High speed or Full speed
		{
			FUsbPs_SetMaxPktSizes(USB_HIGH);
		}
		else if(((uDStatus&0x6) >>1) == USB_FULL)
		{
			FUsbPs_SetMaxPktSizes(USB_FULL);
		}
		else
		{
			TRACE_OUT(DEBUG_OUT, "\n Error:Neither High_Speed nor Full_Speed\n");
			return;
		}
		
		FUsbPs_SetEndpoint(usbDev);
		FUsbPs_SetDescriptorTable();
	}
	
	if (uGIntStatus & INT_RESUME)
	{

	}

	if (uGIntStatus & INT_SUSPEND)
	{
                
	}

	if(uGIntStatus & INT_RX_FIFO_NOT_EMPTY)
	{
		u32 GrxStatus;
		u32 fifoCntByte;

		TRACE_OUT(DEBUG_OUT, "INT_RX_FIFO_NOT_EMPTY\n");
		FMSH_WriteReg(usbDev->base_address,GINTMSK,  INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND);
		
		GrxStatus = FMSH_ReadReg(usbDev->base_address,GRXSTSP);
		TRACE_OUT(DEBUG_OUT, "GRXSTSP : %x \n", GrxStatus);
	
		if ((GrxStatus & SETUP_PKT_RECEIVED) == SETUP_PKT_RECEIVED) 
		{
			FUsbPs_HandleEvent_EP0(usbDev);
			g_bTransferEp0=TRUE;
		}
		else if ((GrxStatus & OUT_PKT_RECEIVED) == OUT_PKT_RECEIVED)
		{
			fifoCntByte = (GrxStatus & 0x7ff0)>>4;
                        
                        if( (g_UsbDev.m_uEp0State == EP0_STATE_DFU_DOWNLOAD) && (fifoCntByte>0) )
                        { 		
                                
                                FUsbPs_HandleEvent_Ep0Out(usbDev,fifoCntByte);
                                FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
                                FMSH_WriteReg(usbDev->base_address,DOEPCTL0, 1u<<31|1<<26);		//ep0 enable, clear nak
                                FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY);
                                g_UsbDev.m_uDfuDownloadLen+=fifoCntByte;
                                if(g_UsbDev.m_uDfuDownloadLen==g_UsbDev.m_uDeviceRequestLength)
                                {    
                                    g_UsbDev.m_uDfuDownloadLen=0;
                                    g_UsbDev.m_uEp0State = EP0_STATE_INIT;
                                }
                                return;
                        }
			if(((GrxStatus&0xf)==BULK_OUT_EP)&&(fifoCntByte))
			{				
				TRACE_OUT(DEBUG_OUT, "Bulk Out : OUT_PKT_RECEIVED\n");
				FUsbPs_HandleEvent_BulkOut(usbDev,fifoCntByte);
				if( g_UsbDev.m_eOpMode == USB_CPU )
					FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY);
				return;
			}
		}
		FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET
			|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY); //gint unmask
	}

	if ((uGIntStatus & INT_IN_EP) || (uGIntStatus & INT_OUT_EP))
	{
		u32 uDmaEnCheck;
		s32 uRemainCnt; 
		
		ep_int = FMSH_ReadReg(usbDev->base_address,DAINT);
		TRACE_OUT(DEBUG_OUT, "DAINT : %x \n", ep_int);
		
		if (ep_int & (1<<CONTROL_EP))
		{
			ep_int_status = FMSH_ReadReg(usbDev->base_address,DIEPINT0);
			TRACE_OUT(DEBUG_OUT, "DIEPINT0 : %x \n", ep_int_status);
			
			if (ep_int_status & INTKN_TXFEMP)
			{
				if (g_bTransferEp0==TRUE)
				{
					FUsbPs_TransferEp0(usbDev);
					g_bTransferEp0 = FALSE;
				}
			}
			
			FMSH_WriteReg(usbDev->base_address,DIEPINT0, ep_int_status); // Interrupt Clear
		}
		
		if (ep_int & ((1<<CONTROL_EP)<<16))
		{
			ep_int_status = FMSH_ReadReg(usbDev->base_address,DOEPINT0);
			TRACE_OUT(DEBUG_OUT, "DOEPINT0 : %x \n", ep_int_status);

			FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
			FMSH_WriteReg(usbDev->base_address,DOEPCTL0, 1u<<31|1<<26);		//ep0 enable, clear nak
			
			FMSH_WriteReg(usbDev->base_address,DOEPINT0, ep_int_status); 		// Interrupt Clear
		}

		if(ep_int & (1<<BULK_IN_EP))
		{
			ep_int_status = FMSH_ReadReg(usbDev->base_address,bulkIn_DIEPINT);
 			TRACE_OUT(DEBUG_OUT, "bulkIn_DIEPINT : %x \n", ep_int_status);
			FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPINT, ep_int_status); // Interrupt Clear	
			
			uDmaEnCheck = FMSH_ReadReg(usbDev->base_address,GAHBCFG);
			
			if (g_UsbDev.m_eOpMode == USB_CPU)
			{
				if (ep_int_status&TRANSFER_DONE)
				{
					FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPINT, TRANSFER_DONE); // Interrupt Clear
					
					g_UsbDev.m_pUpPt += g_usUploadPktLength;
					FUsbPs_HandleEvent_BulkIn(usbDev);
				}
			}
			
			else if ((uDmaEnCheck&MODE_DMA)&&(ep_int_status&TRANSFER_DONE))
			{
				TRACE_OUT(DEBUG_OUT, "DMA IN : Transfer Done\n");

				g_UsbDev.m_pUpPt = (u8 *)FMSH_ReadReg(usbDev->base_address,bulkIn_DIEPDMA);
				uRemainCnt = g_UsbDev.m_uUploadSize- ((u32)g_UsbDev.m_pUpPt - g_UsbDev.m_uUploadAddr);
				
				if (uRemainCnt>0) 
				{
					u32 uPktCnt, uRemainder;
					uPktCnt = (u32)(uRemainCnt/g_UsbDev.m_uBulkInEPMaxPktSize);
					uRemainder = (u32)(uRemainCnt%g_UsbDev.m_uBulkInEPMaxPktSize);
					if(uRemainder != 0)
					{
						uPktCnt += 1;
					}
					TRACE_OUT(DEBUG_OUT, "uRemainCnt : %d \n", uRemainCnt);
					if (uPktCnt> 1023)
					{
						FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, 1023, (g_UsbDev.m_uBulkInEPMaxPktSize*1023));
					}
					else
					{
						FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, uPktCnt, uRemainCnt);
					}
					FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|BULK_IN_EP<<11|g_UsbDev.m_uBulkInEPMaxPktSize<<0);		//ep1 enable, clear nak, bulk, usb active, next ep1, max pkt
				}
				else
				{
					TRACE_OUT(DEBUG_OUT, "DMA IN : Transfer Complete\n");
				}
			}
		}

		if (ep_int & ((1<<BULK_OUT_EP)<<16))
		{
			ep_int_status = FMSH_ReadReg(usbDev->base_address,bulkOut_DOEPINT);
			FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPINT, ep_int_status); 		// Interrupt Clear
			TRACE_OUT(DEBUG_OUT, "bulkOut_DOEPINT : %x \n", ep_int_status);
			uDmaEnCheck = FMSH_ReadReg(usbDev->base_address,GAHBCFG);
			if ((uDmaEnCheck&MODE_DMA)&&(ep_int_status&TRANSFER_DONE))
			{
				TRACE_OUT(DEBUG_OUT, "DMA OUT : Transfer Done\n");
				g_UsbDev.m_pDownPt = (u8 *)FMSH_ReadReg(usbDev->base_address,bulkOut_DOEPDMA);

				uRemainCnt = g_UsbDev.m_uDownloadFileSize - ((u32)g_UsbDev.m_pDownPt - g_UsbDev.m_uDownloadAddress + 8);
				
				if (uRemainCnt>0) 
				{
					u32 uPktCnt, uRemainder;
					uPktCnt = (u32)(uRemainCnt/g_UsbDev.m_uBulkOutEPMaxPktSize);
					uRemainder = (u32)(uRemainCnt%g_UsbDev.m_uBulkOutEPMaxPktSize);
					if(uRemainder != 0)
					{
						uPktCnt += 1;
					}
					TRACE_OUT(DEBUG_OUT, "uRemainCnt : %d \n", uRemainCnt);
					if (uPktCnt> 1023)
					{
						FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, 1023, (g_UsbDev.m_uBulkOutEPMaxPktSize*1023));
					}
					else
					{
						FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, uPktCnt, uRemainCnt);
					}
					FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
				}
				else
				{
					TRACE_OUT(DEBUG_OUT, "DMA OUT : Transfer Complete\n");
				}
			}
		}
	}
}


/*****************************************************************************
 *  This function is called when Setup packet is received.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_HandleEvent_EP0(FUsbPs_T *usbDev)
{
	u16 i;
	u32 ReadBuf[64]={0x0000, };
	u16 setaddress;
	u32 uRemoteWakeUp=FALSE;
	
	for(i=0;i<2;i++)
	{
		ReadBuf[i] = FMSH_ReadReg(usbDev->base_address,EP0_FIFO);
	}

	g_UsbDev.m_oDeviceRequest.bmRequestType=ReadBuf[0];
	g_UsbDev.m_oDeviceRequest.bRequest=ReadBuf[0]>>8;
	g_UsbDev.m_oDeviceRequest.wValue_L=ReadBuf[0]>>16;
	g_UsbDev.m_oDeviceRequest.wValue_H=ReadBuf[0]>>24;
	g_UsbDev.m_oDeviceRequest.wIndex_L=ReadBuf[1];
	g_UsbDev.m_oDeviceRequest.wIndex_H=ReadBuf[1]>>8;
	g_UsbDev.m_oDeviceRequest.wLength_L=ReadBuf[1]>>16;
	g_UsbDev.m_oDeviceRequest.wLength_H=ReadBuf[1]>>24;

	FUsbPs_PrintEp0Pkt((u8 *)&g_UsbDev.m_oDeviceRequest, 8);
        if((g_UsbDev.m_oDeviceRequest.bmRequestType&0x60)==0x00){
	switch (g_UsbDev.m_oDeviceRequest.bRequest)
	{
		case STANDARD_SET_ADDRESS:
			setaddress = (g_UsbDev.m_oDeviceRequest.wValue_L); // Set Address Update bit
			FMSH_WriteReg(usbDev->base_address,DCFG, 1<<18|setaddress<<4|g_UsbDev.m_eSpeed<<0);
			TRACE_OUT(DEBUG_OUT, "\n MCU >> Set Address : %d \n", setaddress);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_SET_DESCRIPTOR:
			TRACE_OUT(DEBUG_OUT, "\n MCU >> Set Descriptor \n");
			break;

		case STANDARD_SET_CONFIGURATION:
			TRACE_OUT(DEBUG_OUT, "\n MCU >> Set Configuration \n");
			g_usConfig = g_UsbDev.m_oDeviceRequest.wValue_L; // Configuration value in configuration descriptor
			g_UsbDev.m_uIsUsbOtgSetConfiguration = 1;
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_GET_CONFIGURATION:
			g_UsbDev.m_uEp0State = EP0_STATE_CONFIGURATION_GET;
			break;

		case STANDARD_GET_DESCRIPTOR:
			switch (g_UsbDev.m_oDeviceRequest.wValue_H)
			{
				case DEVICE_DESCRIPTOR:
					g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) |
						g_UsbDev.m_oDeviceRequest.wLength_L);
					TRACE_OUT(DEBUG_OUT, "\n MCU >> Get Device Descriptor = 0x%x \n",g_UsbDev.m_uDeviceRequestLength);
					g_UsbDev.m_uEp0State = EP0_STATE_GD_DEV_0;
					break;

				case CONFIGURATION_DESCRIPTOR:
					g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) |
						g_UsbDev.m_oDeviceRequest.wLength_L);
					TRACE_OUT(DEBUG_OUT, "\n MCU >> Get Configuration Descriptor = 0x%x \n",g_UsbDev.m_uDeviceRequestLength);

					if (g_UsbDev.m_uDeviceRequestLength > CONFIG_DESC_SIZE){
					// === GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
					// Windows98 gets these 4 descriptors all together by issuing only a request.
					// Windows2000 gets each descriptor seperately.
					// g_UsbDev.m_uEpZeroTransferLength = CONFIG_DESC_TOTAL_SIZE;
						g_UsbDev.m_uEp0State = EP0_STATE_GD_CFG_0;
					}
					else
						g_UsbDev.m_uEp0State = EP0_STATE_GD_CFG_ONLY_0; // for win2k
					break;

				case STRING_DESCRIPTOR :
					TRACE_OUT(DEBUG_OUT, "\n MCU >> Get String Descriptor \n");
					switch(g_UsbDev.m_oDeviceRequest.wValue_L)
					{
						case 0:
							g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I0;
							break;
						case 1:
							g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I1;
							break;
						case 2:
							g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I2;
							break;
                                                case 3:
							g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I3;
							break;
						case 4:
							g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I4;
							break;
						case 5:
							g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I5;
							break;
						default:
								break;
					}
					break;
				case ENDPOINT_DESCRIPTOR:
					TRACE_OUT(DEBUG_OUT, "\n MCU >> Get Endpoint Descriptor \n");
					switch(g_UsbDev.m_oDeviceRequest.wValue_L&0xf)
					{
					case 0:
						g_UsbDev.m_uEp0State=EP0_STATE_GD_EP0_ONLY_0;
						break;
					case 1:
						g_UsbDev.m_uEp0State=EP0_STATE_GD_EP1_ONLY_0;
						break;
					default:
						break;
					}
					break;

				case DEVICE_QUALIFIER:
					g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) |
						g_UsbDev.m_oDeviceRequest.wLength_L);
					TRACE_OUT(DEBUG_OUT, "\n MCU >> Get Device Qualifier Descriptor = 0x%x \n",g_UsbDev.m_uDeviceRequestLength);
					g_UsbDev.m_uEp0State = EP0_STATE_GD_DEV_QUALIFIER;
					break;
					
			   	case OTHER_SPEED_CONFIGURATION :
					TRACE_OUT(DEBUG_OUT, ("\n MCU >> Get OTHER_SPEED_CONFIGURATION \n"));
					g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) |
						g_UsbDev.m_oDeviceRequest.wLength_L);
					g_UsbDev.m_uEp0State = EP0_STATE_GD_OTHER_SPEED;
					break;
					
			}
			break;

		case STANDARD_CLEAR_FEATURE:
			TRACE_OUT(DEBUG_OUT, "\n MCU >> Clear Feature \n");
			switch (g_UsbDev.m_oDeviceRequest.bmRequestType)
			{
				case DEVICE_RECIPIENT:
					if (g_UsbDev.m_oDeviceRequest.wValue_L == 1)
						uRemoteWakeUp = FALSE;
					break;

				case ENDPOINT_RECIPIENT:
					if (g_UsbDev.m_oDeviceRequest.wValue_L == 0)
					{
						if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == CONTROL_EP)
							oStatusGet.EndpointCtrl= 0;

						if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_IN_EP) // IN  Endpoint
							oStatusGet.EndpointIn= 0;

						if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_OUT_EP) // OUT Endpoint
							oStatusGet.EndpointOut= 0;
					}
					break;

				default:
					break;
			}
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_SET_FEATURE:
			TRACE_OUT(DEBUG_OUT, "\n MCU >> Set Feature \n");
			switch (g_UsbDev.m_oDeviceRequest.bmRequestType)
			{
				case DEVICE_RECIPIENT:
					if (g_UsbDev.m_oDeviceRequest.wValue_L == 1)
						uRemoteWakeUp = TRUE;
						break;

				case ENDPOINT_RECIPIENT:
					if (g_UsbDev.m_oDeviceRequest.wValue_L == 0)
					{
						if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == CONTROL_EP)
							oStatusGet.EndpointCtrl= 1;

						if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_IN_EP)
							oStatusGet.EndpointIn= 1;

						if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_OUT_EP)
							oStatusGet.EndpointOut= 1;
					}
					break;

				default:
					break;
			}

			//=======================================================
			
			switch (g_UsbDev.m_oDeviceRequest.wValue_L) {

				case EP_STALL:
					// TBD: additional processing if required
					break;

				
				case TEST_MODE:						
					if ((0 != g_UsbDev.m_oDeviceRequest.wIndex_L ) ||(0 != g_UsbDev.m_oDeviceRequest.bmRequestType)) 
						break;

					g_UsbDev.m_uEp0State = EP0_STATE_TEST_MODE;

					break;

				default:
					g_UsbDev.m_uEp0State = EP0_STATE_INIT;
					break;
			}
			//=======================================================
			break;

		case STANDARD_GET_STATUS:
			switch(g_UsbDev.m_oDeviceRequest.bmRequestType)
			{
				case  (0x80):	//device
					oStatusGet.Device=((u8)uRemoteWakeUp<<1)|0x1;		// SelfPowered
					g_UsbDev.m_uEp0State = EP0_STATE_GET_STATUS0;
					break;

				case  (0x81):	//interface
					oStatusGet.Interface=0;
					g_UsbDev.m_uEp0State = EP0_STATE_GET_STATUS1;
					break;

				case  (0x82):	//endpoint
					if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == CONTROL_EP)
						g_UsbDev.m_uEp0State = EP0_STATE_GET_STATUS2;

					if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_IN_EP)
						g_UsbDev.m_uEp0State = EP0_STATE_GET_STATUS3;

					if ((g_UsbDev.m_oDeviceRequest.wIndex_L & 0x7f) == BULK_OUT_EP)
						g_UsbDev.m_uEp0State = EP0_STATE_GET_STATUS4;
					break;

				default:
					break;
			}
			break;

		case STANDARD_GET_INTERFACE:
			g_UsbDev.m_uEp0State = EP0_STATE_INTERFACE_GET;
			break;

		case STANDARD_SET_INTERFACE:
			oInterfaceGet.AlternateSetting= g_UsbDev.m_oDeviceRequest.wValue_L;
                        DfuSetIntf(&g_UsbDev.m_oDeviceRequest);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case STANDARD_SYNCH_FRAME:
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		default:
			break;
	}
	}
        else if((g_UsbDev.m_oDeviceRequest.bmRequestType&0x60)==0x20)
        {
            switch (g_UsbDev.m_oDeviceRequest.bRequest)
            {
                case DFU_DETACH:
                  g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) |
						g_UsbDev.m_oDeviceRequest.wLength_L);
		  g_UsbDev.m_uEp0State = EP0_STATE_GD_DEV_0;
                  break;
                case DFU_DNLOAD:
                  g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) | g_UsbDev.m_oDeviceRequest.wLength_L);
                  if( (DfuObj.TotalBytesDnloaded>0) && (g_UsbDev.m_uDeviceRequestLength==0) )
                  {  
                      DfuObj.TotalBytesDnloaded=0;
                      g_UsbDev.m_uDfuDownloadDone=TRUE;
                      g_UsbDev.m_uEp0State = EP0_STATE_INIT;
                  }
                  else
                      g_UsbDev.m_uEp0State = EP0_STATE_DFU_DOWNLOAD;
                  break;
                case DFU_GETSTATUS:
                  g_UsbDev.m_uDeviceRequestLength = (u32)((g_UsbDev.m_oDeviceRequest.wLength_H << 8) |
						g_UsbDev.m_oDeviceRequest.wLength_L);
                  g_UsbDev.m_uEp0State = EP0_STATE_DFU_GD_STATUS;
                  break;
                default:
                  break;
            
            }
        }
	FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
	
	FMSH_WriteReg(usbDev->base_address,DIEPCTL0, ((1<<26)|(CONTROL_EP<<11)|(0<<0)));	//clear nak, next ep0, 64byte
		
}

/*****************************************************************************
 *  This function is called during data phase of control transfer.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_TransferEp0(FUsbPs_T *usbDev)
{

	TRACE_OUT(DEBUG_OUT, "TransferEp0 , g_UsbDev.m_uEp0State == %d\n", g_UsbDev.m_uEp0State);
        static u8 DfuReply[6]={0,};
        
	switch (g_UsbDev.m_uEp0State)
	{
		case EP0_STATE_INIT:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 0);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(BULK_IN_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			TRACE_OUT(DEBUG_OUT, "EndpointZeroTransfer(EP0_STATE_INIT)\n");
			break;

		// ===DFU_GET_STATUS  ===
		case EP0_STATE_DFU_GD_STATUS:
                        
                        if(DfuObj.CurrState == STATE_DFU_IDLE )
			{
				DfuObj.CurrState = STATE_DFU_DOWNLOAD_SYNC;
				++DownloadDone;
			}
			else if (DfuObj.CurrState == STATE_DFU_DOWNLOAD_SYNC)
			{
				DfuObj.CurrState = STATE_DFU_DOWNLOAD_BUSY;
			}
			else
			{
				/*Misra C compliance*/
			}
			DfuReply[0] = DfuObj.CurrStatus;
			DfuReply[4] = DfuObj.CurrState;

			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 6);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			FUsbPs_WrPktEp0(usbDev,DfuReply, 6); // EP0_PKT_SIZE
			
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			TRACE_OUT(DEBUG_OUT, "EndpointZeroTransfer(EP0_STATE_GD_DEV_0)\n");
			break;
                case DFU_DNLOAD:
                        
                        break;
                        
                // === GET_DESCRIPTOR:DEVICE ===
		case EP0_STATE_GD_DEV_0:
			if (g_UsbDev.m_uDeviceRequestLength<=DEVICE_DESC_SIZE)
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uDeviceRequestLength);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescDevice))+0, g_UsbDev.m_uDeviceRequestLength); // EP0_PKT_SIZE
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, DEVICE_DESC_SIZE);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescDevice))+0, DEVICE_DESC_SIZE); // EP0_PKT_SIZE
			}
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			TRACE_OUT(DEBUG_OUT, "EndpointZeroTransfer(EP0_STATE_GD_DEV_0)\n");
			break;
                        
		// === GET_DESCRIPTOR:CONFIGURATION+INTERFACE+ENDPOINT0+ENDPOINT1 ===
		// Windows98 gets these 4 descriptors all together by issuing only a request.
		// Windows2000 gets each descriptor seperately.
		// === GET_DESCRIPTOR:CONFIGURATION ONLY for WIN2K===
		case EP0_STATE_GD_CFG_0:
			if(g_UsbDev.m_uDeviceRequestLength<=CONFIG_DESC_TOTAL_SIZE)
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uDeviceRequestLength);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescConfig))+0, g_UsbDev.m_uDeviceRequestLength); // EP0_PKT_SIZE
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, CONFIG_DESC_TOTAL_SIZE);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescConfig))+0, CONFIG_DESC_TOTAL_SIZE); // EP0_PKT_SIZE
			}
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			TRACE_OUT(DEBUG_OUT, "EndpointZeroTransfer(EP0_STATE_GD_CFG)\n");
			break;

		case EP0_STATE_GD_DEV_QUALIFIER:	//only supported in USB 2.0
			if(g_UsbDev.m_uDeviceRequestLength<=10)
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uDeviceRequestLength);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDeviceQualifierDescriptor+0, g_UsbDev.m_uDeviceRequestLength);
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 10);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDeviceQualifierDescriptor+0, 10);
			}
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;		

                case EP0_STATE_GD_OTHER_SPEED:
			FUsbPs_SetOtherSpeedConfDescTable(usbDev,g_UsbDev.m_uDeviceRequestLength);
			break;	

		// === GET_DESCRIPTOR:CONFIGURATION ONLY===
		case EP0_STATE_GD_CFG_ONLY_0:
			TRACE_OUT(DEBUG_OUT, "[DBG : EP0_STATE_GD_CFG_ONLY]\n");
			if(g_UsbDev.m_uDeviceRequestLength<=CONFIG_DESC_SIZE)
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uDeviceRequestLength);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescConfig))+0, g_UsbDev.m_uDeviceRequestLength); // EP0_PKT_SIZE
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, CONFIG_DESC_SIZE);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescConfig))+0, CONFIG_DESC_SIZE); // EP0_PKT_SIZE
			}
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:INTERFACE ONLY===

		case EP0_STATE_GD_IF_ONLY_0:
			if(g_UsbDev.m_uDeviceRequestLength<=INTERFACE_DESC_SIZE)
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uDeviceRequestLength);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescInterface))+0, g_UsbDev.m_uDeviceRequestLength);	// INTERFACE_DESC_SIZE
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, INTERFACE_DESC_SIZE);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
				FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescInterface))+0, INTERFACE_DESC_SIZE);	// INTERFACE_DESC_SIZE
			}
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:ENDPOINT 1 ONLY===
		case EP0_STATE_GD_EP0_ONLY_0:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, ENDPOINT_DESC_SIZE);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescEndpt1))+0, ENDPOINT_DESC_SIZE);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:ENDPOINT 2 ONLY===
		case EP0_STATE_GD_EP1_ONLY_0:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, ENDPOINT_DESC_SIZE);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,((u8 *)&(g_UsbDev.m_oDesc.oDescEndpt2))+0, ENDPOINT_DESC_SIZE);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		// === GET_DESCRIPTOR:STRING ===
		case EP0_STATE_GD_STR_I0:
			TRACE_OUT(DEBUG_OUT, "[GDS0_0]");
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, STRING_DESC0_SIZE);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr0, STRING_DESC0_SIZE);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;
		case EP0_STATE_GD_STR_I1:
			TRACE_OUT(DEBUG_OUT, "[GDS1_%d]", g_UsbDev.m_uEp0SubState);
			if ((g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize+g_UsbDev.m_uControlEPMaxPktSize)<sizeof(aDescStr1))
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr1+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), g_UsbDev.m_uControlEPMaxPktSize);
				g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I1;
				g_UsbDev.m_uEp0SubState++;
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, sizeof(aDescStr1)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr1+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), sizeof(aDescStr1)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				g_UsbDev.m_uEp0State = EP0_STATE_INIT;
				g_UsbDev.m_uEp0SubState = 0;
			}
			break;

		case EP0_STATE_GD_STR_I2:
			TRACE_OUT(DEBUG_OUT, "[GDS2_%d]", g_UsbDev.m_uEp0SubState);
			if ((g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize+g_UsbDev.m_uControlEPMaxPktSize)<sizeof(aDescStr2))
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr2+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), g_UsbDev.m_uControlEPMaxPktSize);
				g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I2;
				g_UsbDev.m_uEp0SubState++;
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, sizeof(aDescStr2)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				TRACE_OUT(DEBUG_OUT, "[E]");
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr2+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), sizeof(aDescStr2)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				g_UsbDev.m_uEp0State = EP0_STATE_INIT;
				g_UsbDev.m_uEp0SubState = 0;
			}
			break;
                        
                case EP0_STATE_GD_STR_I3:
			TRACE_OUT(DEBUG_OUT, "[GDS2_%d]", g_UsbDev.m_uEp0SubState);
			if ((g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize+g_UsbDev.m_uControlEPMaxPktSize)<sizeof(aDescStr3))
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr3+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), g_UsbDev.m_uControlEPMaxPktSize);
				g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I3;
				g_UsbDev.m_uEp0SubState++;
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, sizeof(aDescStr3)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				TRACE_OUT(DEBUG_OUT, "[E]");
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr3+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), sizeof(aDescStr3)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				g_UsbDev.m_uEp0State = EP0_STATE_INIT;
				g_UsbDev.m_uEp0SubState = 0;
			}
			break;
                   case EP0_STATE_GD_STR_I4:
			TRACE_OUT(DEBUG_OUT, "[GDS2_%d]", g_UsbDev.m_uEp0SubState);
			if ((g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize+g_UsbDev.m_uControlEPMaxPktSize)<sizeof(aDescStr4))
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr4+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), g_UsbDev.m_uControlEPMaxPktSize);
				g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I4;
				g_UsbDev.m_uEp0SubState++;
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, sizeof(aDescStr4)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				TRACE_OUT(DEBUG_OUT, "[E]");
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr4+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), sizeof(aDescStr4)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				g_UsbDev.m_uEp0State = EP0_STATE_INIT;
				g_UsbDev.m_uEp0SubState = 0;
			}
			break;
                        
                 case EP0_STATE_GD_STR_I5:
			TRACE_OUT(DEBUG_OUT, "[GDS2_%d]", g_UsbDev.m_uEp0SubState);
			if ((g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize+g_UsbDev.m_uControlEPMaxPktSize)<sizeof(aDescStr5))
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, g_UsbDev.m_uControlEPMaxPktSize);
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr5+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), g_UsbDev.m_uControlEPMaxPktSize);
				g_UsbDev.m_uEp0State = EP0_STATE_GD_STR_I5;
				g_UsbDev.m_uEp0SubState++;
			}
			else
			{
				FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, sizeof(aDescStr5)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
				TRACE_OUT(DEBUG_OUT, "[E]");
				FUsbPs_WrPktEp0(usbDev,(u8 *)aDescStr5+(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize), sizeof(aDescStr5)-(g_UsbDev.m_uEp0SubState*g_UsbDev.m_uControlEPMaxPktSize));
				g_UsbDev.m_uEp0State = EP0_STATE_INIT;
				g_UsbDev.m_uEp0SubState = 0;
			}
			break;
                        
		case EP0_STATE_CONFIGURATION_GET:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FMSH_WriteReg(usbDev->base_address,EP0_FIFO, g_usConfig);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_INTERFACE_GET:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)&oInterfaceGet+0, 1);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;


		case EP0_STATE_GET_STATUS0:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)&oStatusGet+0, 1);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS1:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)&oStatusGet+1, 1);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS2:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)&oStatusGet+2, 1);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS3:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)&oStatusGet+3, 1);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_GET_STATUS4:
			FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 1);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
			FUsbPs_WrPktEp0(usbDev,(u8 *)&oStatusGet+4, 1);
			g_UsbDev.m_uEp0State = EP0_STATE_INIT;
			break;

		case EP0_STATE_TEST_MODE:					
			switch(g_UsbDev.m_oDeviceRequest.wIndex_H)
			{	
				u32 uTemp;
				
				case TEST_J:
					//Set Test J
					FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 0);
					FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					TRACE_OUT(DEBUG_OUT, "Test_J\n");
					uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_J_MODE);						
					FMSH_WriteReg(usbDev->base_address,DCTL,uTemp);
					g_UsbDev.m_uEp0State = EP0_STATE_INIT;
					break;

				case TEST_K:
					//Set Test K
					FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 0);
					FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					TRACE_OUT(DEBUG_OUT, "Test_K\n");
					uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_K_MODE);						
					FMSH_WriteReg(usbDev->base_address,DCTL,uTemp);
					g_UsbDev.m_uEp0State = EP0_STATE_INIT;
					break;

				case TEST_SE0_NAK:
					//Set Test SE0_NAK
					FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 0);
					FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					TRACE_OUT(DEBUG_OUT, "Test_SE0_NAK\n");
					uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_SE0_NAK_MODE);						
					FMSH_WriteReg(usbDev->base_address,DCTL,uTemp);
					g_UsbDev.m_uEp0State = EP0_STATE_INIT;
					break;

				case TEST_PACKET:
					//Set Test Packet
					FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 0);
					FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, TEST_PKT_SIZE);
					FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					FUsbPs_WrPktEp0(usbDev,TestPkt, TEST_PKT_SIZE);
					TRACE_OUT(DEBUG_OUT, "Test_Packet\n");
					uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_PACKET_MODE);						
					FMSH_WriteReg(usbDev->base_address,DCTL,uTemp);
					g_UsbDev.m_uEp0State = EP0_STATE_INIT;
					break;	
					
				case TEST_FORCE_ENABLE:
					//Set Test Force Enable
					FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 0);
					FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, 64byte
					TRACE_OUT(DEBUG_OUT, "Test_Force_Enable\n");
					uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
					uTemp = uTemp & ~(TEST_CONTROL_FIELD) | (TEST_FORCE_ENABLE_MODE);						
					FMSH_WriteReg(usbDev->base_address,DCTL,uTemp);
					g_UsbDev.m_uEp0State = EP0_STATE_INIT;
					break;	
			}

		default:
			break;
	}
}

/*****************************************************************************
 *  This function handles bulk in transfer in CPU mode.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_HandleEvent_BulkIn(FUsbPs_T *usbDev)
{
	u8* BulkInBuf;
	u32 uRemainCnt;

	TRACE_OUT(DEBUG_OUT, "CPU_MODE Bulk In Function\n");
	
	BulkInBuf = g_UsbDev.m_pUpPt;
	
	uRemainCnt = g_UsbDev.m_uUploadSize- ((u32)g_UsbDev.m_pUpPt - g_UsbDev.m_uUploadAddr);

	if (uRemainCnt > g_UsbDev.m_uBulkInEPMaxPktSize)
	{
		g_usUploadPktLength = g_UsbDev.m_uBulkInEPMaxPktSize;
		FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, 1, g_usUploadPktLength);
		FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkInEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
		FUsbPs_WrPktBulkInEp(usbDev,BulkInBuf, g_usUploadPktLength);
	}	
	else if(uRemainCnt > 0)
	{		
		g_usUploadPktLength = uRemainCnt;
		FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, 1, g_usUploadPktLength);
		FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkInEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
		FUsbPs_WrPktBulkInEp(usbDev,BulkInBuf, g_usUploadPktLength);
	}
	else
	{
		g_usUploadPktLength = 0;
	}
}

/*****************************************************************************
 *  This function handles bulk out transfer.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_HandleEvent_BulkOut(FUsbPs_T *usbDev,u32 fifoCntByte)
{
	u8 TempBuf[16];
	u32 uCheck;

	if (g_UsbDev.m_uDownloadFileSize==0)
	{
		if (fifoCntByte == 10)
		{		
			FUsbPs_RdPktBulkOutEp(usbDev,(u8 *)TempBuf, 10);
			uCheck = *((u8 *)(TempBuf+8)) + (*((u8 *)(TempBuf+9))<<8);

			if (uCheck==0x1)
			{
				g_UsbDev.m_uUploadAddr =
					*((u8 *)(TempBuf+0))+
					(*((u8 *)(TempBuf+1))<<8)+
					(*((u8 *)(TempBuf+2))<<16)+
					(*((u8 *)(TempBuf+3))<<24);

				g_UsbDev.m_uUploadSize =
					*((u8 *)(TempBuf+4))+
					(*((u8 *)(TempBuf+5))<<8)+
					(*((u8 *)(TempBuf+6))<<16)+
					(*((u8 *)(TempBuf+7))<<24);
				g_UsbDev.m_pUpPt=(u8 *)g_UsbDev.m_uUploadAddr;
				TRACE_OUT(DEBUG_OUT, "UploadAddress : 0x%x, UploadSize: %d\n", g_UsbDev.m_uUploadAddr, g_UsbDev.m_uUploadSize);
 	
				if (g_UsbDev.m_eOpMode == USB_DMA)
				{
					if (g_UsbDev.m_uUploadSize>0)
					{
						u32 uPktCnt, uRemainder;
						
						TRACE_OUT(DEBUG_OUT, "Dma Start for IN PKT \n");

						FMSH_WriteReg(usbDev->base_address,GAHBCFG, MODE_DMA|BURST_INCR4|GBL_INT_UNMASK);
						FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND); //gint unmask

						FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPDMA, (u32)g_UsbDev.m_pUpPt);

						uPktCnt = (u32)(g_UsbDev.m_uUploadSize/g_UsbDev.m_uBulkInEPMaxPktSize);
						uRemainder = (u32)(g_UsbDev.m_uUploadSize%g_UsbDev.m_uBulkInEPMaxPktSize);
						if(uRemainder != 0)
						{
							uPktCnt += 1;
						}
						if (uPktCnt > 1023)
						{
							FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, 1023, (g_UsbDev.m_uBulkInEPMaxPktSize*1023));							
						}
						else
						{
							FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, uPktCnt, g_UsbDev.m_uUploadSize);
						}
					
						FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPCTL, 1u<<31|1<<26|2<<18|1<<15|BULK_IN_EP<<11|g_UsbDev.m_uBulkInEPMaxPktSize<<0);		//ep1 enable, clear nak, bulk, usb active, next ep1, max pkt						
					}		
				}
				else	//CPU_MODE
				{
					FUsbPs_HandleEvent_BulkIn(usbDev);
				}  
			}
			g_UsbDev.m_uDownloadFileSize=0;
			return;
		}
		else
		{
			FUsbPs_RdPktBulkOutEp(usbDev,(u8 *)TempBuf, 8);
			TRACE_OUT(DEBUG_OUT, "downloadFileSize==0, 1'st BYTE_READ_CNT_REG : %x\n", fifoCntByte);
			
			
				g_UsbDev.m_uDownloadAddress=
					*((u8 *)(TempBuf+0))+
					(*((u8 *)(TempBuf+1))<<8)+
					(*((u8 *)(TempBuf+2))<<16)+
					(*((u8 *)(TempBuf+3))<<24);
			

			g_UsbDev.m_uDownloadFileSize=
				*((u8 *)(TempBuf+4))+
				(*((u8 *)(TempBuf+5))<<8)+
				(*((u8 *)(TempBuf+6))<<16)+
				(*((u8 *)(TempBuf+7))<<24);

			g_UsbDev.m_pDownPt=(u8 *)g_UsbDev.m_uDownloadAddress;
			TRACE_OUT(DEBUG_OUT, "downloadAddress : 0x%x, downloadFileSize: %d\n", g_UsbDev.m_uDownloadAddress, g_UsbDev.m_uDownloadFileSize);

			FUsbPs_RdPktBulkOutEp(usbDev,(u8 *)g_UsbDev.m_pDownPt, fifoCntByte-8); // The first 8-bytes are deleted.
			g_UsbDev.m_pDownPt += fifoCntByte-8;
			
			if (g_UsbDev.m_eOpMode == USB_CPU)
			{
				FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, 1, g_UsbDev.m_uBulkOutEPMaxPktSize);
				FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
			}
			else
			{
				if (g_UsbDev.m_uDownloadFileSize>g_UsbDev.m_uBulkOutEPMaxPktSize)
				{
					u32 uPktCnt, uRemainder;
					
					TRACE_OUT(DEBUG_OUT, "downloadFileSize!=0, Dma Start for 2nd OUT PKT \n");
					FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND); //gint unmask
					FMSH_WriteReg(usbDev->base_address,GAHBCFG, MODE_DMA|BURST_INCR4|GBL_INT_UNMASK);
					FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPDMA, (u32)g_UsbDev.m_pDownPt);
					uPktCnt = (u32)(g_UsbDev.m_uDownloadFileSize-g_UsbDev.m_uBulkOutEPMaxPktSize)/g_UsbDev.m_uBulkOutEPMaxPktSize;
					uRemainder = (u32)((g_UsbDev.m_uDownloadFileSize-g_UsbDev.m_uBulkOutEPMaxPktSize)%g_UsbDev.m_uBulkOutEPMaxPktSize);
					if(uRemainder != 0)
					{
						uPktCnt += 1;
					}
					if (uPktCnt > 1023)
					{
						FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, 1023, (g_UsbDev.m_uBulkOutEPMaxPktSize*1023));
					}
					else
					{
						FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, uPktCnt, (g_UsbDev.m_uDownloadFileSize-g_UsbDev.m_uBulkOutEPMaxPktSize));
					}
					FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
				}		
			}
		}
	}

	else
	{
		if (g_UsbDev.m_eOpMode == USB_CPU)
		{
			TRACE_OUT(DEBUG_OUT, "downloadFileSize!=0, 2nd BYTE_READ_CNT_REG : %x\n", fifoCntByte);
			FUsbPs_RdPktBulkOutEp(usbDev,(u8 *)g_UsbDev.m_pDownPt, fifoCntByte);
			g_UsbDev.m_pDownPt += fifoCntByte;
			
			FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, 1, g_UsbDev.m_uBulkOutEPMaxPktSize);
			FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkOutEPMaxPktSize<<0);		//ep3 enable, clear nak, bulk, usb active, next ep3, max pkt 64
		}
	}
}

/*****************************************************************************
 *  This function soft-resets OTG Core and then unresets again.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SoftResetCore(FUsbPs_T *usbDev)
{
	u32 uTemp;

	FMSH_WriteReg(usbDev->base_address,GRSTCTL, CORE_SOFT_RESET);

	do
	{
		uTemp = FMSH_ReadReg(usbDev->base_address,GRSTCTL);
	}while(!(uTemp & AHB_MASTER_IDLE));
	
}

/*****************************************************************************
 *  This function checks if the cable is inserted.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_WaitCableInsertion(FUsbPs_T *usbDev)
{
	u32 uTemp, i=0;
	u8 ucFirst=1;

	do
	{
		delay_ms(10);
		
		uTemp = FMSH_ReadReg(usbDev->base_address,GOTGCTL);

		if (uTemp & (B_SESSION_VALID|A_SESSION_VALID))
		{
			break;
		}
		else if(ucFirst == 1)
		{
			TRACE_OUT(DEBUG_OUT, "\nInsert an OTG cable into the connector!\n");
			ucFirst = 0;
		}
		
		
		i++;

	}while(1);
}

/*****************************************************************************
 *  This function initializes OTG Link Core.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_InitCore(FUsbPs_T *usbDev)
{	
	FMSH_WriteReg(usbDev->base_address,GAHBCFG, PTXFE_HALF|NPTXFE_HALF|MODE_SLAVE|BURST_SINGLE|GBL_INT_UNMASK);
	
	FMSH_WriteReg(usbDev->base_address,GUSBCFG, 0<<15			// PHY Low Power Clock sel
					|1<<14			// Non-Periodic TxFIFO Rewind Enable
					|0x5<<10		// Turnaround time
					|1<<9|1<<8		// [0:HNP disable, 1:HNP enable][ 0:SRP disable, 1:SRP enable] H1= 1,1							
					|0<<7			// Ulpi DDR sel
					|0<<6			// 0: high speed utmi+, 1: full speed serial
					|1<<4			// 0: utmi+, 1:ulpi
					|0<<3			// phy i/f  0:8bit, 1:16bit
					|0x7<<0			// HS/FS Timeout*
					);
}

/*****************************************************************************
 *  This function checks the current mode.
 *
 * @param
 *   usbDev is usb handle
 *   pucMode, current mode(device or host)
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_CheckCurrentMode(FUsbPs_T *usbDev,u8 *pucMode)
{
	u32 uTemp;

	uTemp = FMSH_ReadReg(usbDev->base_address,GINTSTS);
	*pucMode = uTemp & 0x1;
}

/*****************************************************************************
 *  This function puts the OTG device core in the disconnected state.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetSoftDisconnect(FUsbPs_T *usbDev)
{
	u32 uTemp;

	uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
	uTemp |= SOFT_DISCONNECT;
	FMSH_WriteReg(usbDev->base_address,DCTL, uTemp);
}

/*****************************************************************************
 *  This function makes the OTG device core to exit from the disconnected state.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_ClearSoftDisconnect(FUsbPs_T *usbDev)
{
	u32 uTemp;

	uTemp = FMSH_ReadReg(usbDev->base_address,DCTL);
	uTemp = uTemp & ~SOFT_DISCONNECT;
	FMSH_WriteReg(usbDev->base_address,DCTL, uTemp);
}

/*****************************************************************************
 *  This function configures OTG Core to initial settings of device mode.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_InitDevice(FUsbPs_T *usbDev)
{
	FMSH_WriteReg(usbDev->base_address,DCFG, 1<<18|g_UsbDev.m_eSpeed<<0);				// [][1: full speed(30Mhz) 0:high speed]

	FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET
					|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY);	//gint unmask	
}


/*****************************************************************************
 *  This function sets NAK bit of all EPs.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetAllOutEpNak(FUsbPs_T *usbDev)
{
	u8 i;
	u32 uTemp;
	
	for(i=0;i<16;i++)
	{
		uTemp = FMSH_ReadReg(usbDev->base_address,DOEPCTL0+0x20*i);
		uTemp |= DEPCTL_SNAK;
		FMSH_WriteReg(usbDev->base_address,DOEPCTL0+0x20*i, uTemp);
	}
}

/*****************************************************************************
 *  This function clears NAK bit of all EPs.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_ClearAllOutEpNak(FUsbPs_T *usbDev)
{
	u8 i;
	u32 uTemp;
	
	for(i=0;i<16;i++)
	{
		uTemp = FMSH_ReadReg(usbDev->base_address,DOEPCTL0+0x20*i);
		uTemp |= (DEPCTL_EPENA|DEPCTL_CNAK);
		FMSH_WriteReg(usbDev->base_address,DOEPCTL0+0x20*i, uTemp);
	}
}

/*****************************************************************************
 *  This function sets the max packet sizes of USB transfer types according to the speed.
 *
 * @param
 *   usbDev is usb handle
 *   eSpeed, usb speed(high or full)
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetMaxPktSizes(USB_SPEED eSpeed)
{
	if (eSpeed == USB_HIGH)
	{
		g_UsbDev.m_eSpeed = USB_HIGH;
		g_UsbDev.m_uControlEPMaxPktSize = HIGH_SPEED_CONTROL_PKT_SIZE;
		g_UsbDev.m_uBulkInEPMaxPktSize = HIGH_SPEED_BULK_PKT_SIZE;
		g_UsbDev.m_uBulkOutEPMaxPktSize = HIGH_SPEED_BULK_PKT_SIZE;
	}
	else
	{
		g_UsbDev.m_eSpeed = USB_FULL;
		g_UsbDev.m_uControlEPMaxPktSize = FULL_SPEED_CONTROL_PKT_SIZE;
		g_UsbDev.m_uBulkInEPMaxPktSize = FULL_SPEED_BULK_PKT_SIZE;
		g_UsbDev.m_uBulkOutEPMaxPktSize = FULL_SPEED_BULK_PKT_SIZE;
	}
}

/*****************************************************************************
 *  This function sets the endpoint-specific CSRs.
 *
 * @param
 *   usbDev is usb handle
 *   eSpeed, usb speed(high or full)
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetOtherSpeedConfDescTable(FUsbPs_T *usbDev,u32 length)
{
	// Standard device descriptor
	if (g_UsbDev.m_eSpeed == USB_HIGH) 
	{	
	    if (length ==9)
	    {	    
	    	FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 9);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			FUsbPs_WrPktEp0(usbDev,((u8 *)&aOtherSpeedConfiguration_full)+0, 9); // EP0_PKT_SIZE
		}
	    else if(length ==32)
		{	    
	    	FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 32);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			FUsbPs_WrPktEp0(usbDev,((u8 *)&aOtherSpeedConfiguration_fullTotal)+0, 32); // EP0_PKT_SIZE			
	    }
	}
	else
	{
	    if (length ==9)
	    {	    
	    	FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 9);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			FUsbPs_WrPktEp0(usbDev,((u8 *)&aOtherSpeedConfiguration_high)+0, 9); // EP0_PKT_SIZE
		}
	    else if(length ==32)
		{	    
	    	FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_CONTROL, 1, 32);
			FMSH_WriteReg(usbDev->base_address,DIEPCTL0, (1u<<31)|(1<<26)|(CONTROL_EP<<11)|(0<<0));	//ep0 enable, clear nak, next ep0, max 64byte
			FUsbPs_WrPktEp0(usbDev,((u8 *)&aOtherSpeedConfiguration_highTotal)+0, 32); // EP0_PKT_SIZE
	    }	 			
	}
	g_UsbDev.m_uEp0State = EP0_STATE_INIT;
}

/*****************************************************************************
 *  This function sets the endpoint-specific CSRs.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetEndpoint(FUsbPs_T *usbDev)
{
	// Unmask DAINT source
	FMSH_WriteReg(usbDev->base_address,DIEPINT0, 0xff);
	FMSH_WriteReg(usbDev->base_address,DOEPINT0, 0xff);	
	FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPINT, 0xff);
	FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPINT, 0xff);
	
	// Init For Ep0
	FMSH_WriteReg(usbDev->base_address,DIEPCTL0, ((1<<26)|(CONTROL_EP<<11)|(0<<0)));	//MPS:64bytes
	FMSH_WriteReg(usbDev->base_address,DOEPCTL0, (1u<<31)|(1<<26)|(0<<0));		//ep0 enable, clear nak
}

/*****************************************************************************
 *  This function sets the standard descriptors.
 *
 * @param
 *   usbDev is usb handle
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetDescriptorTable(void)
{
	// Standard device descriptor
	g_UsbDev.m_oDesc.oDescDevice.bLength=DEVICE_DESC_SIZE;	//0x12 
	g_UsbDev.m_oDesc.oDescDevice.bDescriptorType=DEVICE_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescDevice.bDeviceClass=0x0; // 0x0
	g_UsbDev.m_oDesc.oDescDevice.bDeviceSubClass=0x0;
	g_UsbDev.m_oDesc.oDescDevice.bDeviceProtocol=0x0;
	g_UsbDev.m_oDesc.oDescDevice.bMaxPacketSize0=g_UsbDev.m_uControlEPMaxPktSize;


        g_UsbDev.m_oDesc.oDescDevice.idVendorL=0xE8;	//0x45;
	g_UsbDev.m_oDesc.oDescDevice.idVendorH=0x04;	//0x53;
	g_UsbDev.m_oDesc.oDescDevice.idProductL=0x34;   //0x00
	g_UsbDev.m_oDesc.oDescDevice.idProductH=0x12;   //0x64

	g_UsbDev.m_oDesc.oDescDevice.bcdDeviceL=0x00;
	g_UsbDev.m_oDesc.oDescDevice.bcdDeviceH=0x01;
	g_UsbDev.m_oDesc.oDescDevice.iManufacturer=0x1; // index of string descriptor
	g_UsbDev.m_oDesc.oDescDevice.iProduct=0x2;	// index of string descriptor
	g_UsbDev.m_oDesc.oDescDevice.iSerialNumber=0x03;
	g_UsbDev.m_oDesc.oDescDevice.bNumConfigurations=0x1;
	
	g_UsbDev.m_oDesc.oDescDevice.bcdUSBL=0x00;
	g_UsbDev.m_oDesc.oDescDevice.bcdUSBH=0x02; 	// Ver 2.0

	// Standard configuration descriptor
	g_UsbDev.m_oDesc.oDescConfig.bLength=CONFIG_DESC_SIZE; // 0x9 bytes
	g_UsbDev.m_oDesc.oDescConfig.bDescriptorType=CONFIGURATION_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescConfig.wTotalLengthL=CONFIG_DESC_TOTAL_SIZE; 
	g_UsbDev.m_oDesc.oDescConfig.wTotalLengthH=0;
	g_UsbDev.m_oDesc.oDescConfig.bNumInterfaces=1;
// dbg    descConf.bConfigurationValue=2; // why 2? There's no reason.
	g_UsbDev.m_oDesc.oDescConfig.bConfigurationValue=1;
	g_UsbDev.m_oDesc.oDescConfig.iConfiguration=0;
	g_UsbDev.m_oDesc.oDescConfig.bmAttributes=CONF_ATTR_DEFAULT|CONF_ATTR_SELFPOWERED; // bus powered only.
	g_UsbDev.m_oDesc.oDescConfig.maxPower=0x0; // draws 50mA current from the USB bus.

	// Standard interface descriptor
	g_UsbDev.m_oDesc.oDescInterface.bLength=INTERFACE_DESC_SIZE; // 9
	g_UsbDev.m_oDesc.oDescInterface.bDescriptorType=INTERFACE_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescInterface.bInterfaceNumber=0x0;
	g_UsbDev.m_oDesc.oDescInterface.bAlternateSetting=0x0; // ?
	g_UsbDev.m_oDesc.oDescInterface.bNumEndpoints = 2;	// # of endpoints except EP0
	g_UsbDev.m_oDesc.oDescInterface.bInterfaceClass=0xff; // 0x0 ?
	g_UsbDev.m_oDesc.oDescInterface.bInterfaceSubClass=0xff;
	g_UsbDev.m_oDesc.oDescInterface.bInterfaceProtocol=0xff;
	g_UsbDev.m_oDesc.oDescInterface.iInterface=0x04;

	// Standard endpoint0 descriptor
	g_UsbDev.m_oDesc.oDescEndpt1.bLength=ENDPOINT_DESC_SIZE;
	g_UsbDev.m_oDesc.oDescEndpt1.bDescriptorType=ENDPOINT_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescEndpt1.bEndpointAddress=BULK_IN_EP|EP_ADDR_IN; // 2400Xendpoint 1 is IN endpoint.
	g_UsbDev.m_oDesc.oDescEndpt1.bmAttributes=EP_ATTR_BULK;
	g_UsbDev.m_oDesc.oDescEndpt1.wMaxPacketSizeL=(u8)g_UsbDev.m_uBulkInEPMaxPktSize; // 64
	g_UsbDev.m_oDesc.oDescEndpt1.wMaxPacketSizeH=(u8)(g_UsbDev.m_uBulkInEPMaxPktSize>>8);
	g_UsbDev.m_oDesc.oDescEndpt1.bInterval=0x0; // not used

        // Standard endpoint1 descriptor
	g_UsbDev.m_oDesc.oDescEndpt2.bLength=ENDPOINT_DESC_SIZE;
	g_UsbDev.m_oDesc.oDescEndpt2.bDescriptorType=ENDPOINT_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescEndpt2.bEndpointAddress=BULK_OUT_EP|EP_ADDR_OUT; // 2400X endpoint 3 is OUT endpoint.
	g_UsbDev.m_oDesc.oDescEndpt2.bmAttributes=EP_ATTR_BULK;
	g_UsbDev.m_oDesc.oDescEndpt2.wMaxPacketSizeL=(u8)g_UsbDev.m_uBulkOutEPMaxPktSize; // 64
	g_UsbDev.m_oDesc.oDescEndpt2.wMaxPacketSizeH=(u8)(g_UsbDev.m_uBulkOutEPMaxPktSize>>8);
	g_UsbDev.m_oDesc.oDescEndpt2.bInterval=0x0; // not used
        
	// DFU Interface Descriptor1
	g_UsbDev.m_oDesc.oDescDfuInterface.bLength=INTERFACE_DESC_SIZE; // 9
	g_UsbDev.m_oDesc.oDescDfuInterface.bDescriptorType=INTERFACE_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescDfuInterface.bInterfaceNumber=0x0;
	g_UsbDev.m_oDesc.oDescDfuInterface.bAlternateSetting=0x01; // ?
	g_UsbDev.m_oDesc.oDescDfuInterface.bNumEndpoints = 0x00;	// 
	g_UsbDev.m_oDesc.oDescDfuInterface.bInterfaceClass=0xFE; // 0x0 ?
        g_UsbDev.m_oDesc.oDescDfuInterface.bInterfaceSubClass=0x01;
        g_UsbDev.m_oDesc.oDescDfuInterface.bInterfaceProtocol=0x02;
        g_UsbDev.m_oDesc.oDescDfuInterface.iInterface=0x04;
        
        // DFU Interface Descriptor2
	g_UsbDev.m_oDesc.oDescDfuFunc.bLength=DFU_DESCFUNC_SIZE;
	g_UsbDev.m_oDesc.oDescDfuFunc.bDescriptorType=DFUFUNC_DESCRIPTOR;
	g_UsbDev.m_oDesc.oDescDfuFunc.bAttributes=0x3;
	g_UsbDev.m_oDesc.oDescDfuFunc.bDetachTimeOutL=0x00;
	g_UsbDev.m_oDesc.oDescDfuFunc.bDetachTimeOutH=0x20;
	g_UsbDev.m_oDesc.oDescDfuFunc.bTransferSizeL=0x00;
        g_UsbDev.m_oDesc.oDescDfuFunc.bTransferSizeH=0x04;
        g_UsbDev.m_oDesc.oDescDfuFunc.bBcdDfuVersionL=0x10;
        g_UsbDev.m_oDesc.oDescDfuFunc.bBcdDfuVersionH=0x01;
        
}

/*****************************************************************************
 *  This function checks the current usb speed.
 *
 * @param
 *   usbDev is usb handle
 *   eSpeed, usb speed(high or full)
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_CheckEnumeratedSpeed(FUsbPs_T *usbDev,USB_SPEED *eSpeed)
{
	u32 uDStatus;
	
	uDStatus = FMSH_ReadReg(usbDev->base_address,DSTS); // System status read

	*eSpeed = (USB_SPEED)((uDStatus&0x6) >>1);
}

/*****************************************************************************
 *  This function sets DIEPTSIZn CSR according to input parameters.
 *
 * @param
 *   usbDev is usb handle
 *   eType, transfer type
 *   uPktCnt, packet count to transfer
 *   uXferSize, transfer size
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetInEpXferSize(FUsbPs_T *usbDev,EP_TYPE eType, u32 uPktCnt, u32 uXferSize)
{	
	if(eType == EP_TYPE_CONTROL)
	{
		FMSH_WriteReg(usbDev->base_address,DIEPTSIZ0, (uPktCnt<<19)|(uXferSize<<0));
	}
	else if(eType == EP_TYPE_BULK)
	{
		FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPTSIZ, (1<<29)|(uPktCnt<<19)|(uXferSize<<0));
	}
}

/*****************************************************************************
 *  This function sets DOEPTSIZn CSR according to input parameters.
 *
 * @param
 *   usbDev is usb handle
 *   eType, transfer type
 *   uPktCnt, packet count to transfer
 *   uXferSize, transfer size
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetOutEpXferSize(FUsbPs_T *usbDev,EP_TYPE eType, u32 uPktCnt, u32 uXferSize)
{	
	if(eType == EP_TYPE_CONTROL)
	{
		FMSH_WriteReg(usbDev->base_address,DOEPTSIZ0, (3<<29)|(uPktCnt<<19)|(uXferSize<<0));
	}
	else if(eType == EP_TYPE_BULK)
	{
		FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPTSIZ, (uPktCnt<<19)|(uXferSize<<0));
	}
}

/*****************************************************************************
 *  This function reads data from the buffer and writes the data on EP0 FIFO.
 *
 * @param
 *   usbDev is usb handle
 *   buf, address of the data buffer to write on Control EP FIFO
 *   num, size of the data to write on Control EP FIFO(byte count)
 *  
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_WrPktEp0(FUsbPs_T *usbDev,u8 *buf, int num)
{
	int i;
	u32 Wr_Data=0;

	for(i=0;i<num;i+=4)
	{
		//rb1004...080309 : Generated the "alignment error" when the compile option is -O3(best code)
		//Wr_Data = ((*(buf+3))<<24)|((*(buf+2))<<16)|((*(buf+1))<<8)|*buf;
		Wr_Data = 0;
		Wr_Data |= ((*(buf+3))<<24);
		Wr_Data |= ((*(buf+2))<<16);
		Wr_Data |= ((*(buf+1))<<8);
		Wr_Data |= *buf;
		FMSH_WriteReg(usbDev->base_address,control_EP_FIFO, Wr_Data);
		buf += 4;
	}
}


/*****************************************************************************
 *  This function reads data from the buffer and displays the data.
 *
 * @param
 *   pt, address of the data buffer to read
 *   count, size of the data to read(byte count)
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_PrintEp0Pkt(u8 *pt, u8 count)
{
	int i;
	TRACE_OUT(DEBUG_OUT, "[DBG:");
	for(i=0;i<count;i++)
		TRACE_OUT(DEBUG_OUT, "%x,", pt[i]);
	TRACE_OUT(DEBUG_OUT, "]\n");
}

/*****************************************************************************
 *  This function reads data from the buffer and writes the data on Bulk In EP FIFO.
 *
 * @param
 *   usbDev is usb handle
 *   buf, address of the data buffer to write on Bulk In EP FIFO
 *   num, size of the data to write on Bulk In EP FIFO(byte count)
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_WrPktBulkInEp(FUsbPs_T *usbDev,u8 *buf, int num)
{
	int i;
	u32 Wr_Data=0;

	for(i=0;i<num;i+=4)
	{
		Wr_Data=((*(buf+3))<<24)|((*(buf+2))<<16)|((*(buf+1))<<8)|*buf;
		FMSH_WriteReg(usbDev->base_address,bulkIn_EP_FIFO, Wr_Data);
		buf += 4;
	}
}

/*****************************************************************************
 *  This function reads data from Bulk Out EP FIFO and writes the data on the buffer.
 *
 * @param
 *   usbDev is usb handle
 *   buf, address of the data buffer to write
 *   num, size of the data to read from Bulk Out EP FIFO(byte count)
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_RdPktBulkOutEp(FUsbPs_T *usbDev,u8 *buf, int num)
{
	int i;
	u32 Rdata;

	for (i=0;i<num;i+=4)
	{
		Rdata = FMSH_ReadReg(usbDev->base_address,bulkOut_EP_FIFO);

		buf[i] = (u8)Rdata;
		buf[i+1] = (u8)(Rdata>>8);
		buf[i+2] = (u8)(Rdata>>16);
		buf[i+3] = (u8)(Rdata>>24);
	}
}

/*****************************************************************************
 *  This function clears variables related to file-downloading.
 *
 * @param
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_ClearDownFileInfo(void)
{
	g_UsbDev.m_uDownloadAddress = 0;
	g_UsbDev.m_uDownloadFileSize = 0;
	g_UsbDev.m_pDownPt = 0;
}

/*****************************************************************************
 *  This function checks if Set Configuration is received from the USB Host.
 *
 * @param
 *   
 * @return
 *     configuration result
 * @note		None.
 *
 ******************************************************************************/
u8 FUsbPs_IsUsbOtgSetConfiguration(void)
{
	if (g_UsbDev.m_uIsUsbOtgSetConfiguration == 0)
		return FALSE;
	else
		return TRUE;
}

/*****************************************************************************
 *  This function sets CSRs related to the operation mode.
 *
 * @param
 *   usbDev is usb handle
 *   eMode, operation mode(cpu or dma)
 *   
 * @return
 *
 * @note		None.
 *
 ******************************************************************************/
void FUsbPs_SetOpMode(FUsbPs_T *usbDev,USB_OPMODE eMode)
{
	g_UsbDev.m_eOpMode = eMode;

	FMSH_WriteReg(usbDev->base_address,GINTMSK, INT_RESUME|INT_OUT_EP|INT_IN_EP|INT_ENUMDONE|INT_RESET|INT_SUSPEND|INT_RX_FIFO_NOT_EMPTY); //gint unmask
	FMSH_WriteReg(usbDev->base_address,GAHBCFG, MODE_SLAVE|BURST_SINGLE|GBL_INT_UNMASK);

	FUsbPs_SetOutEpXferSize(usbDev,EP_TYPE_BULK, 1, g_UsbDev.m_uBulkOutEPMaxPktSize);
	FUsbPs_SetInEpXferSize(usbDev,EP_TYPE_BULK, 1, 0);

	FMSH_WriteReg(usbDev->base_address,bulkOut_DOEPCTL, 1u<<31|1<<26|2<<18|1<<15|g_UsbDev.m_uBulkOutEPMaxPktSize<<0);		//bulk out ep enable, clear nak, bulk, usb active, max pkt
	FMSH_WriteReg(usbDev->base_address,bulkIn_DIEPCTL, 0u<<31|1<<27|2<<18|1<<15|g_UsbDev.m_uBulkInEPMaxPktSize<<0);			//bulk in ep enable, clear nak, bulk, usb active, max pkt
}
