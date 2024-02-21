/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : otg_dev.h
*  
*	File Description : This file declares prototypes of USB OTG API funcions and defines some values.
*
*	Author : Haksoo,Kim
*	Dept. : AP Development Team
*	Created Date : 2007/01/23
*	Version : 0.1 
* 
*	History
*	- Created(Haksoo,Kim 2007/01/23)
*  
**************************************************************************************/
 
#ifndef __OTGDEV_H__
#define __OTGDEV_H__



#include "fmsh_ps_parameters.h"
#include "fmsh_common.h"
#include "fmsh_usb_lib.h"
//=====================================================================================
//
#define CONTROL_EP						0
#define BULK_IN_EP						1
#define BULK_OUT_EP						2

#define FULL_SPEED_CONTROL_PKT_SIZE		64
#define FULL_SPEED_BULK_PKT_SIZE		64

#define HIGH_SPEED_CONTROL_PKT_SIZE		64
#define HIGH_SPEED_BULK_PKT_SIZE		512

#define RX_FIFO_SIZE					16 //512
#define NPTX_FIFO_START_ADDR			RX_FIFO_SIZE
#define NPTX_FIFO_SIZE					16 //512
#define PTX_FIFO_SIZE					512

// string descriptor
#define LANGID_US_L                 	(0x09)
#define LANGID_US_H                 	(0x04)

// Feature Selectors
#define EP_STALL          		0
#define DEVICE_REMOTE_WAKEUP    1
#define TEST_MODE				2

/* Test Mode Selector*/
#define TEST_J					1
#define TEST_K					2
#define TEST_SE0_NAK			3
#define TEST_PACKET				4
#define TEST_FORCE_ENABLE		5

/* DFU status */
#define DFU_STATUS_OK               0x00U
/* DFU alternate setting value when in run-time mode */
#define DFU_ALT_SETTING 			0x1U


enum BULK_IN_EP_CSR
{
	bulkIn_DIEPCTL		= (DIEPCTL0 + 0x20*BULK_IN_EP),
	bulkIn_DIEPINT		= (DIEPINT0 + 0x20*BULK_IN_EP),
	bulkIn_DIEPTSIZ		= (DIEPTSIZ0 + 0x20*BULK_IN_EP),
	bulkIn_DIEPDMA		= (DIEPDMA0 + 0x20*BULK_IN_EP)
};

enum BULK_OUT_EP_CSR
{
	bulkOut_DOEPCTL		= (DOEPCTL0 + 0x20*BULK_OUT_EP),
	bulkOut_DOEPINT		= (DOEPINT0 + 0x20*BULK_OUT_EP),
	bulkOut_DOEPTSIZ	= (DOEPTSIZ0 + 0x20*BULK_OUT_EP),
	bulkOut_DOEPDMA		= (DOEPDMA0 + 0x20*BULK_OUT_EP)
};

enum EP_FIFO_ADDR
{
	control_EP_FIFO		= (EP0_FIFO + 0x700*CONTROL_EP),
	bulkIn_EP_FIFO		= (EP0_FIFO + 0x700*BULK_IN_EP),
	bulkOut_EP_FIFO		= (EP0_FIFO + 0x700*BULK_OUT_EP)
};

enum DFU_STATES
{
        STATE_APP_IDLE=0x00U,
        STATE_APP_DETACH= 0x01U,
        STATE_DFU_IDLE=0x02U,
        STATE_DFU_DOWNLOAD_SYNC=0x03U,
        STATE_DFU_DOWNLOAD_BUSY=0x04U,
        STATE_DFU_DOWNLOAD_IDLE=0x05U,
        STATE_DFU_ERROR=0x0AU
};


//==============================================================================================
// definitions related to Standard Device Requests
enum EP_INDEX
{
	EP0, EP1, EP2, EP3, EP4
};

//------------------------------------------------
// EP0 state
enum EP0_STATE
{
	EP0_STATE_INIT              	= 0,
	EP0_STATE_GD_DEV_0				= 11,
	EP0_STATE_GD_DEV_1				= 12,
	EP0_STATE_GD_DEV_2				= 13,
	EP0_STATE_GD_CFG_0          	= 21,
	EP0_STATE_GD_CFG_1          	= 22,
	EP0_STATE_GD_CFG_2          	= 23,
	EP0_STATE_GD_CFG_3          	= 24,
	EP0_STATE_GD_CFG_4          	= 25,
	EP0_STATE_CONFIGURATION_GET		= 26,
	EP0_STATE_GD_STR_I0         	= 27,
	EP0_STATE_GD_STR_I1         	= 28,
	EP0_STATE_GD_STR_I2         	= 29,
        EP0_STATE_GD_STR_I3        	= 30,
        EP0_STATE_GD_STR_I4         	= 31,
        EP0_STATE_GD_STR_I5         	= 32,
	EP0_STATE_GD_DEV_QUALIFIER  	= 33,
	EP0_STATE_INTERFACE_GET			= 34,	
	EP0_STATE_GET_STATUS0			= 35,
	EP0_STATE_GET_STATUS1			= 36,
	EP0_STATE_GET_STATUS2			= 37,
	EP0_STATE_GET_STATUS3			= 38,
	EP0_STATE_GET_STATUS4			= 39,
	EP0_STATE_GD_OTHER_SPEED		= 40,
	EP0_STATE_GD_CFG_ONLY_0     	= 41,
	EP0_STATE_GD_CFG_ONLY_1     	= 42,
	EP0_STATE_GD_IF_ONLY_0      	= 44,
	EP0_STATE_GD_IF_ONLY_1      	= 45,
	EP0_STATE_GD_EP0_ONLY_0     	= 46,
	EP0_STATE_GD_EP1_ONLY_0     	= 47,
	EP0_STATE_GD_EP2_ONLY_0     	= 48,
	EP0_STATE_GD_EP3_ONLY_0     	= 49,
	EP0_STATE_GD_OTHER_SPEED_HIGH_1	= 51,
	EP0_STATE_GD_OTHER_SPEED_HIGH_2	= 52,
	EP0_STATE_GD_OTHER_SPEED_HIGH_3	= 53,
        EP0_STATE_DFU_DOWNLOAD=60,
        EP0_STATE_DFU_GD_STATUS=61,       
	EP0_STATE_TEST_MODE				= 70
};

// SPEC1.1

// Standard bmRequestType (direction)
// #define DEVICE_bmREQUEST_TYPE(oDeviceRequest)  ((m_poDeviceRequest->bmRequestType) & 0x80)
enum DEV_REQUEST_DIRECTION
{
	HOST_TO_DEVICE				= 0x00,
	DEVICE_TO_HOST				= 0x80
};

// Standard bmRequestType (Type)
// #define DEVICE_bmREQUEST_TYPE(oDeviceRequest)  ((m_poDeviceRequest->bmRequestType) & 0x60)
enum DEV_REQUEST_TYPE
{
	STANDARD_TYPE               = 0x00,
	CLASS_TYPE                  = 0x20,
	VENDOR_TYPE                 = 0x40,
	RESERVED_TYPE               = 0x60
};

// Standard bmRequestType (Recipient)
// #define DEVICE_bmREQUEST_RECIPIENT(oDeviceRequest)  ((m_poDeviceRequest->bmRequestType) & 0x07)
enum DEV_REQUEST_RECIPIENT
{
	DEVICE_RECIPIENT			= 0,
	INTERFACE_RECIPIENT			= 1,
	ENDPOINT_RECIPIENT			= 2,
	OTHER_RECIPIENT				= 3
};

// Standard bRequest codes
enum STANDARD_REQUEST_CODE
{
	STANDARD_GET_STATUS         = 0,
	STANDARD_CLEAR_FEATURE      = 1,
	STANDARD_RESERVED_1         = 2,
	STANDARD_SET_FEATURE        = 3,
	STANDARD_RESERVED_2         = 4,
	STANDARD_SET_ADDRESS        = 5,
	STANDARD_GET_DESCRIPTOR     = 6,
	STANDARD_SET_DESCRIPTOR     = 7,
	STANDARD_GET_CONFIGURATION  = 8,
	STANDARD_SET_CONFIGURATION  = 9,
	STANDARD_GET_INTERFACE      = 10,
	STANDARD_SET_INTERFACE      = 11,
	STANDARD_SYNCH_FRAME        = 12
};

enum DFU_REQUEST_CODE
{
	DFU_DETACH      = 0,
	DFU_DNLOAD      = 1,
	DFU_GETSTATUS   = 3
};

// Descriptor types
enum DESCRIPTOR_TYPE
{
	DEVICE_DESCRIPTOR           = 1,
	CONFIGURATION_DESCRIPTOR    = 2,
	STRING_DESCRIPTOR           = 3,
	INTERFACE_DESCRIPTOR        = 4,
	ENDPOINT_DESCRIPTOR         = 5,
	DEVICE_QUALIFIER            = 6,
	OTHER_SPEED_CONFIGURATION   = 7,
	INTERFACE_POWER	            = 8,
        DFUFUNC_DESCRIPTOR          = 0x21
};

// configuration descriptor: bmAttributes
enum CONFIG_ATTRIBUTES
{
	CONF_ATTR_DEFAULT       	= 0x80, // in Spec 1.0, it was BUSPOWERED bit.
	CONF_ATTR_REMOTE_WAKEUP 	= 0x20,
	CONF_ATTR_SELFPOWERED   	= 0x40
};

// endpoint descriptor
enum ENDPOINT_ATTRIBUTES
{
	EP_ADDR_IN              = 0x80,
	EP_ADDR_OUT             = 0x00,

	EP_ATTR_CONTROL         = 0x0,
	EP_ATTR_ISOCHRONOUS     = 0x1,
	EP_ATTR_BULK            = 0x2,
	EP_ATTR_INTERRUPT       = 0x3
};

// Descriptor size
enum DESCRIPTOR_SIZE
{
	DEVICE_DESC_SIZE            = 18,
	STRING_DESC0_SIZE           = 4,
	STRING_DESC1_SIZE           = 22,
	STRING_DESC2_SIZE           = 44,
        STRING_DESC3_SIZE           = 44,
        STRING_DESC4_SIZE           = 44,
        STRING_DESC5_SIZE           = 44,
	CONFIG_DESC_SIZE            = 9,
	INTERFACE_DESC_SIZE         = 9,
	ENDPOINT_DESC_SIZE          = 7,
        DFU_DESCFUNC_SIZE           = 9,
	DEVICE_QUALIFIER_SIZE       = 10,
	OTHER_SPEED_CFG_SIZE 	    = 9
};


typedef enum
{
	USB_CPU, USB_DMA
} USB_OPMODE;

typedef enum
{
	USB_HIGH, USB_FULL, USB_LOW
} USB_SPEED;

typedef enum
{
	EP_TYPE_CONTROL, EP_TYPE_ISOCHRONOUS, EP_TYPE_BULK, EP_TYPE_INTERRUPT
}EP_TYPE;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bcdUSBL;
	u8 bcdUSBH;
	u8 bDeviceClass;
	u8 bDeviceSubClass;
	u8 bDeviceProtocol;
	u8 bMaxPacketSize0;
	u8 idVendorL;
	u8 idVendorH;
	u8 idProductL;
	u8 idProductH;
	u8 bcdDeviceL;
	u8 bcdDeviceH;
	u8 iManufacturer;
	u8 iProduct;
	u8 iSerialNumber;
	u8 bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 wTotalLengthL;
	u8 wTotalLengthH;
	u8 bNumInterfaces;
	u8 bConfigurationValue;
	u8 iConfiguration;
	u8 bmAttributes;
	u8 maxPower;
} USB_CONFIGURATION_DESCRIPTOR;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bInterfaceNumber;
	u8 bAlternateSetting;
	u8 bNumEndpoints;
	u8 bInterfaceClass;
	u8 bInterfaceSubClass;
	u8 bInterfaceProtocol;
	u8 iInterface;
} USB_INTERFACE_DESCRIPTOR;

typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bEndpointAddress;
	u8 bmAttributes;
	u8 wMaxPacketSizeL;
	u8 wMaxPacketSizeH;
	u8 bInterval;
} USB_ENDPOINT_DESCRIPTOR;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bAttributes;
	u8 bDetachTimeOutL;
        u8 bDetachTimeOutH;
	u8 bTransferSizeL;
        u8 bTransferSizeH;
	u8 bBcdDfuVersionL;
        u8 bBcdDfuVersionH;
}  __attribute__((__packed__))USB_DFUFUNC_DESCRIPTOR;

typedef struct
{
	USB_DEVICE_DESCRIPTOR oDescDevice;
	USB_CONFIGURATION_DESCRIPTOR oDescConfig;
	USB_INTERFACE_DESCRIPTOR oDescInterface;
	USB_ENDPOINT_DESCRIPTOR oDescEndpt1;
	USB_ENDPOINT_DESCRIPTOR oDescEndpt2;
	USB_INTERFACE_DESCRIPTOR oDescDfuInterface;
	USB_DFUFUNC_DESCRIPTOR oDescDfuFunc;
} USB_DESCRIPTORS;

typedef struct
{
	u8 bmRequestType;  
	u8 bRequest;       
	u8 wValue_L;       
	u8 wValue_H;       
	u8 wIndex_L;       
	u8 wIndex_H;       
	u8 wLength_L;      
	u8 wLength_H;      
} DEVICE_REQUEST;

typedef struct{
	u8 CurrState;
	u8 NextState;
	u8 CurrStatus;
	u8 GotReset;
	u32 CurrentInf; /* current interface */
	u8 GotDnloadRqst;
	u32 TotalBytesDnloaded;
	u8 DfuWaitForInterrupt;
	u8 RuntimeToDfu;
}DfuIf;

typedef struct FUsbDevPs
{
	USB_DESCRIPTORS m_oDesc;
	DEVICE_REQUEST m_oDeviceRequest;

	u32  m_uEp0State;
	u32  m_uEp0SubState;
	USB_OPMODE m_eOpMode;
	USB_SPEED m_eSpeed;
        u32  m_uDfuDownloadLen;
        u32  m_uDfuDownloadDone;
	u32  m_uControlEPMaxPktSize;
	u32  m_uBulkInEPMaxPktSize;
	u32  m_uBulkOutEPMaxPktSize;
	u32  m_uDownloadAddress;
	u32  m_uDownloadFileSize;
	u32  m_uUploadAddr;
	u32  m_uUploadSize;
	u8*  m_pDownPt;
	u8*  m_pUpPt;
	u32  m_uIsUsbOtgSetConfiguration;
	u32  m_uDeviceRequestLength;
} FUsbDevPs_T;


typedef struct
{
	u8 ConfigurationValue;
} USB_CONFIGURATION_SET;

typedef struct
{
	u8 Device;
	u8 Interface;
	u8 EndpointCtrl;
	u8 EndpointIn;
	u8 EndpointOut;
} USB_GET_STATUS;

typedef struct
{
	u8 AlternateSetting;
} USB_INTERFACE_GET;


//=====================================================================================
// prototypes of API functions
void FUsbPs_InitOtg(FUsbPs_T *usbDev,USB_SPEED eSpeed);

void FUsbPs_HandleEvent(FUsbPs_T *usbDev);
void FUsbPs_HandleEvent_EP0(FUsbPs_T *usbDev);
void FUsbPs_HandleEvent_Ep0Out(FUsbPs_T *usbDev,u32 fifoCntByte);
void FUsbPs_TransferEp0(FUsbPs_T *usbDev);
void FUsbPs_HandleEvent_BulkIn(FUsbPs_T *usbDev);
void FUsbPs_HandleEvent_BulkOut(FUsbPs_T *usbDev,u32 fifoCntByte);

//-------------------------------------------------------------------------------------
// prototypes of sub functions
void FUsbPs_SoftResetCore(FUsbPs_T *usbDev);
void FUsbPs_WaitCableInsertion(FUsbPs_T *usbDev);
void FUsbPs_InitCore(FUsbPs_T *usbDev);
void FUsbPs_CheckCurrentMode(FUsbPs_T *usbDev,u8 *pucMode);
void FUsbPs_SetSoftDisconnect(FUsbPs_T *usbDev);
void FUsbPs_ClearSoftDisconnect(FUsbPs_T *usbDev);
void FUsbPs_InitDevice(FUsbPs_T *usbDev);


void FUsbPs_SetAllOutEpNak(FUsbPs_T *usbDev);
void FUsbPs_ClearAllOutEpNak(FUsbPs_T *usbDev);
void FUsbPs_SetMaxPktSizes(USB_SPEED eSpeed);
void FUsbPs_SetOtherSpeedConfDescTable(FUsbPs_T *usbDev,u32 length);
void FUsbPs_SetEndpoint(FUsbPs_T *usbDev);
void FUsbPs_SetDescriptorTable(void);

void FUsbPs_CheckEnumeratedSpeed(FUsbPs_T *usbDev,USB_SPEED *eSpeed);

void FUsbPs_SetInEpXferSize(FUsbPs_T *usbDev,EP_TYPE eType, u32 uPktCnt, u32 uXferSize);
void FUsbPs_SetOutEpXferSize(FUsbPs_T *usbDev,EP_TYPE eType, u32 uPktCnt, u32 uXferSize);

void FUsbPs_WrPktEp0(FUsbPs_T *usbDev,u8 *buf, int num);
void FUsbPs_PrintEp0Pkt(u8 *pt, u8 count);
void FUsbPs_WrPktBulkInEp(FUsbPs_T *usbDev,u8 *buf, int num);
void FUsbPs_RdPktBulkOutEp(FUsbPs_T *usbDev,u8 *buf, int num);

//=====================================================================================
// prototypes of API functions
void FUsbPs_ClearDownFileInfo(void);
u8 FUsbPs_IsUsbOtgSetConfiguration(void);
void FUsbPs_SetOpMode(FUsbPs_T *usbDev,USB_OPMODE eMode);
void FUsbPs_SetDownLoadAddr(u32 addr);
u8 FUsbPs_GetDownLoadComplete();




#endif 
