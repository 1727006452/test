/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_usb_private.h
*
* This file contains ......
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   lq  1/25/2021  First Release
*</pre>
******************************************************************************/

#ifndef _FMSH_USB_PRIVATE_H_ /* prevent circular inclusions */
#define _FMSH_USB_PRIVATE_H_ /* by using protection macros */

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif
  
/***************************** Include Files *********************************/
#include "fmsh_common.h"
#include "fmsh_usb_common.h"
/************************** Constant Definitions *****************************/  
/*
* OTG CORE REGISTERS OFFSET
*/
enum USBOTG_REGS
{
	//==============================================================================================
	// Core Global Registers
	GOTGCTL		= 0x000,		// OTG Control & Status
	GOTGINT		= 0x004,		// OTG Interrupt
	GAHBCFG		= 0x008,		// Core AHB Configuration
	GUSBCFG		= 0x00C,		// Core USB Configuration
	GRSTCTL		= 0x010,		// Core Reset
	GINTSTS		= 0x014,		// Core Interrupt
	GINTMSK		= 0x018,		// Core Interrupt Mask
	GRXSTSR		= 0x01C,		// Receive Status Debug Read/Status Read
	GRXSTSP		= 0x020,		// Receive Status Debug Pop/Status Pop
	GRXFSIZ		= 0x024,		// Receive FIFO Size
	GNPTXFSIZ	= 0x028,		// Non-Periodic Transmit FIFO Size
	GNPTXSTS	= 0x02C,		// Non-Periodic Transmit FIFO/Queue Status

	HPTXFSIZ	= 0x100,		// Host Periodic Transmit FIFO Size
	DPTXFSIZ1	= 0x104,		// Device Periodic Transmit FIFO-1 Size
	DPTXFSIZ2	= 0x108,		// Device Periodic Transmit FIFO-2 Size
	DPTXFSIZ3	= 0x10C,		// Device Periodic Transmit FIFO-3 Size
	DPTXFSIZ4	= 0x110,		// Device Periodic Transmit FIFO-4 Size
	DPTXFSIZ5	= 0x114,		// Device Periodic Transmit FIFO-5 Size
	DPTXFSIZ6	= 0x118,		// Device Periodic Transmit FIFO-6 Size
	DPTXFSIZ7	= 0x11C,		// Device Periodic Transmit FIFO-7 Size
	DPTXFSIZ8	= 0x120,		// Device Periodic Transmit FIFO-8 Size
	DPTXFSIZ9	= 0x124,		// Device Periodic Transmit FIFO-9 Size
	DPTXFSIZ10	= 0x128,		// Device Periodic Transmit FIFO-10 Size
	DPTXFSIZ11	= 0x12C,		// Device Periodic Transmit FIFO-11 Size
	DPTXFSIZ12	= 0x130,		// Device Periodic Transmit FIFO-12 Size
	DPTXFSIZ13	= 0x134,		// Device Periodic Transmit FIFO-13 Size
	DPTXFSIZ14	= 0x138,		// Device Periodic Transmit FIFO-14 Size
	DPTXFSIZ15	= 0x13C,		// Device Periodic Transmit FIFO-15 Size
	
	//==============================================================================================
	// Host Mode Registers
	//------------------------------------------------
	// Host Global Registers
	HCFG		= 0x400,		// Host Configuration
	HFIR		= 0x404,		// Host Frame Interval
	HFNUM		= 0x408,		// Host Frame Number/Frame Time Remaining
	HPTXSTS		= 0x410,		// Host Periodic Transmit FIFO/Queue Status
	HAINT		= 0x414,		// Host All Channels Interrupt
	HAINTMSK	= 0x418,		// Host All Channels Interrupt Mask

	//------------------------------------------------
	// Host Port Control & Status Registers
	HPRT		= 0x440,		// Host Port Control & Status

	//------------------------------------------------
	// Host Channel-Specific Registers
	HCCHAR0		= 0x500,		// Host Channel-0 Characteristics
	HCSPLT0		= 0x504,		// Host Channel-0 Split Control
	HCINT0		= 0x508,		// Host Channel-0 Interrupt
	HCINTMSK0	= 0x50C,		// Host Channel-0 Interrupt Mask
	HCTSIZ0		= 0x510,		// Host Channel-0 Transfer Size
	HCDMA0		= 0x514,		// Host Channel-0 DMA Address

	
	//==============================================================================================
	// Device Mode Registers
	//------------------------------------------------
	// Device Global Registers
	DCFG 		= 0x800,		// Device Configuration
	DCTL 		= 0x804,		// Device Control
	DSTS 		= 0x808,		// Device Status
	DIEPMSK 	= 0x810,		// Device IN Endpoint Common Interrupt Mask
	DOEPMSK 	= 0x814,		// Device OUT Endpoint Common Interrupt Mask
	DAINT 		= 0x818,		// Device All Endpoints Interrupt
	DAINTMSK 	= 0x81C,		// Device All Endpoints Interrupt Mask
	DTKNQR1 	= 0x820,		// Device IN Token Sequence Learning Queue Read 1
	DTKNQR2 	= 0x824,		// Device IN Token Sequence Learning Queue Read 2
	DVBUSDIS 	= 0x828,		// Device VBUS Discharge Time
	DVBUSPULSE 	= 0x82C,		// Device VBUS Pulsing Time
	DTKNQR3 	= 0x830,		// Device IN Token Sequence Learning Queue Read 3
	DTKNQR4 	= 0x834,		// Device IN Token Sequence Learning Queue Read 4
	
	//------------------------------------------------
	// Device Logical IN Endpoint-Specific Registers
	DIEPCTL0 	= 0x900,		// Device IN Endpoint 0 Control
	DIEPINT0 	= 0x908,		// Device IN Endpoint 0 Interrupt
	DIEPTSIZ0 	= 0x910,		// Device IN Endpoint 0 Transfer Size
	DIEPDMA0 	= 0x914,		// Device IN Endpoint 0 DMA Address
	//------------------------------------------------
	// Device Logical OUT Endpoint-Specific Registers
	DOEPCTL0 	= 0xB00,		// Device OUT Endpoint 0 Control
	DOEPINT0 	= 0xB08,		// Device OUT Endpoint 0 Interrupt
	DOEPTSIZ0 	= 0xB10,		// Device OUT Endpoint 0 Transfer Size
	DOEPDMA0 	= 0xB14,		// Device OUT Endpoint 0 DMA Address
	
	//------------------------------------------------
	// Power & clock gating registers
	PCGCCTRL	= 0xE00,

	//------------------------------------------------
	// Endpoint FIFO address
	EP0_FIFO	= 0x700
	
 };

// GOTGCTL
#define B_SESSION_VALID			(0x1<<19)
#define A_SESSION_VALID			(0x1<<18)

// GAHBCFG
#define PTXFE_HALF			(0<<8)
#define PTXFE_ZERO			(1<<8)
#define NPTXFE_HALF			(0<<7)
#define NPTXFE_ZERO			(1<<7)
#define MODE_SLAVE			(0<<5)
#define MODE_DMA			(1<<5)
#define BURST_SINGLE			(0<<1)
#define BURST_INCR			(1<<1)
#define BURST_INCR4			(3<<1)
#define BURST_INCR8			(5<<1)
#define BURST_INCR16			(7<<1)
#define GBL_INT_UNMASK			(1<<0)
#define GBL_INT_MASK			(0<<0)

// GRSTCTL
#define AHB_MASTER_IDLE			(1u<<31)
#define CORE_SOFT_RESET			(0x1<<0)

// GINTSTS/GINTMSK core interrupt register
#define INT_RESUME              	(1u<<31)
#define INT_DISCONN              	(0x1<<29)
#define INT_CONN_ID_STS_CNG		(0x1<<28)
#define INT_OUT_EP			(0x1<<19)
#define INT_IN_EP			(0x1<<18)
#define INT_ENUMDONE			(0x1<<13)
#define INT_RESET               	(0x1<<12)
#define INT_SUSPEND             	(0x1<<11)
#define INT_TX_FIFO_EMPTY		(0x1<<5)
#define INT_RX_FIFO_NOT_EMPTY		(0x1<<4)
#define INT_SOF				(0x1<<3)
#define INT_DEV_MODE			(0x0<<0)
#define INT_HOST_MODE			(0x1<<1)

// GRXSTSP STATUS
#define OUT_PKT_RECEIVED		(0x2<<17)
#define SETUP_PKT_RECEIVED		(0x6<<17)

// DCTL device control register
#define NORMAL_OPERATION		(0x1<<0)
#define SOFT_DISCONNECT			(0x1<<1)
#define	TEST_J_MODE			(TEST_J<<4)
#define	TEST_K_MODE			(TEST_K<<4)
#define	TEST_SE0_NAK_MODE		(TEST_SE0_NAK<<4)
#define	TEST_PACKET_MODE		(TEST_PACKET<<4)
#define	TEST_FORCE_ENABLE_MODE		(TEST_FORCE_ENABLE<<4)
#define TEST_CONTROL_FIELD		(0x7<<4)

// DAINT device all endpoint interrupt register
#define INT_IN_EP0			(0x1<<0)
#define INT_IN_EP1			(0x1<<1)
#define INT_IN_EP3			(0x1<<3)
#define INT_OUT_EP0			(0x1<<16)
#define INT_OUT_EP2			(0x1<<18)
#define INT_OUT_EP4			(0x1<<20)

// DIEPCTL0/DOEPCTL0 device control IN/OUT endpoint 0 control register
#define DEPCTL_EPENA			(0x1<<31)
#define DEPCTL_EPDIS			(0x1<<30)
#define DEPCTL_SNAK			(0x1<<27)
#define DEPCTL_CNAK			(0x1<<26)
#define DEPCTL_CTRL_TYPE		(EP_TYPE_CONTROL<<18)
#define DEPCTL_ISO_TYPE			(EP_TYPE_ISOCHRONOUS<<18)
#define DEPCTL_BULK_TYPE		(EP_TYPE_BULK<<18)
#define DEPCTL_INTR_TYPE		(EP_TYPE_INTERRUPT<<18)
#define DEPCTL_USBACTEP			(0x1<<15)
#define DEPCTL0_MPS_64			(0x0<<0)
#define DEPCTL0_MPS_32			(0x1<<0)
#define DEPCTL0_MPS_16			(0x2<<0)
#define DEPCTL0_MPS_8			(0x3<<0)

// DIEPCTLn/DOEPCTLn device control IN/OUT endpoint n control register

// DIEPMSK/DOEPMSK device IN/OUT endpoint common interrupt mask register
// DIEPINTn/DOEPINTn device IN/OUT endpoint interrupt register
#define BACK2BACK_SETUP_RECEIVED  		(0x1<<6)
#define INTKN_TXFEMP				(0x1<<4)
#define NON_ISO_IN_EP_TIMEOUT			(0x1<<3)
#define CTRL_OUT_EP_SETUP_PHASE_DONE	        (0x1<<3)
#define AHB_ERROR				(0x1<<2)
#define TRANSFER_DONE				(0x1<<0)

#endif 
