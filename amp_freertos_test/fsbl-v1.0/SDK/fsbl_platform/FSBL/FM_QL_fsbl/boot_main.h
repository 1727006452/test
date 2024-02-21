#ifndef BOOT_MAIN_H
#define BOOT_MAIN_H

#include "ps_init.h"
#include "fmsh_ps_parameters.h"
#include "fmsh_common.h"
#include "cache.h"
#include "fdevcps_public.h"

#include "fmsh_aes_sm4.h"
#include "fmsh_authentication.h"
#include "fmsh_csudma.h"
#include "fmsh_efuse.h"
#include "fmsh_sha.h"

#include "fmsh_error.h"
#include "fmsh_header.h"
#include "fmsh_sac.h"
#include "fmsh_fnand.h"
#include "fmsh_nor.h"
#include "fmsh_qspi.h"
#include "fmsh_sd.h"
#include "fmsh_dma.h"
#include "fmsh_uart_logout.h"
#include "fmsh_watchdog.h"
#include "fmsh_gic.h"

#ifdef DDRPS_0_DEVICE_ID
#define  FSBL_PL_DDR  
#define  FSBL_PS_DDR 
#endif

#define  FSBL_PL_DDRADDR  FPS_DDR3MEM_BASEADDR  
#define  FSBL_PL_DDR_START_ADDRESS FPS_DDR3MEM_BASEADDR
#define  FSBL_PL_DDR_END_ADDRESS   (FPS_DDR3MEM_BASEADDR+0x40000000) 
#define  FSBL_PS_DDR_START_ADDRESS  FPS_DDR3MEM_BASEADDR
#define  FSBL_PS_DDR_END_ADDRESS   (FPS_DDR3MEM_BASEADDR+0x40000000)

/* Reset Reason */
#define PS_ONLY_RESET		0x1U

#define SLCR_LOCK_OFFSET 0x4
#define SLCR_UNLOCK_OFFSET 0x8

#define REBOOT_STATUS_OFFSET 0x0258
#define REBOOT_POR 0x00400000
#define REBOOT_NON_POR 0x00600000

#define REBOOT_STATUS_REG		(FPS_SLCR_BASEADDR + 0x400)
#define BOOT_MODE_REG			(FPS_SLCR_BASEADDR + 0x404U)
#define MULTI_BOOT_REG                  (FPS_CSU_BASEADDR + 0x38)

#define APU_RVBARADDR0  (FPS_SLCR_BASEADDR + 0x438)
#define APU_RVBARADDR1  (FPS_SLCR_BASEADDR + 0x440)
#define APU_RVBARADDR2  (FPS_SLCR_BASEADDR + 0x448)
#define APU_RVBARADDR3  (FPS_SLCR_BASEADDR + 0x450)

#define PSS_IDCODE_REG (FPS_SLCR_BASEADDR + 0x40CU)
#define PSS_IDCODE_DEVICE_MASK 0x1f000
#define PSS_IDCODE_DEVICE_7045 (0x11<<12)
#define PSS_IDCODE_DEVICE_7010 (0x2<<12)
#define PSS_IDCODE_DEVICE_7020 (0x7<<12)
#define PSS_IDCODE_DEVICE_7030 (0xC<<12)
#define PSS_IDCODE_DEVICE_7045AI (0x14<<12)

#define ATTRB_DEST_CPU_A7_0 0x100

#define PCAP_UNLOCK_OFFSET           0x30


#define    PCAP_CTRL_REG   FPS_CSU_BASEADDR + 0x8
#define    SECURE_MODE_MASK      0x80
#define    MULTIBOOT_EN_MASK    0x40000000

#define NON_SECURE  0x3000000

 /* PS reset control register define*/
#define PS_RST_REASON_MASK			0x00600000	/**< PS software reset */

/*
 * SLCR BOOT Mode Register defines
 */
#define BOOT_MODES_MASK			0x00000007 /**< FLASH types */
 
/* Boot Modes */
#define JTAG_BOOT_MODE			0x00000000 /**< JTAG Boot Mode */
#define QSPI_BOOT_MODE			0x00000001 /**< QSPI Boot Mode */
#define NOR_BOOT_MODE		        0x00000002 /**< NOR Boot Mode */
#define NAND_BOOT_MODE		        0x00000004 /**< NAND Boot Mode */
#define NAND_BOOT_MODE_16BIT            0x00000007 
#define SD_BOOT_MODE	                0x00000005 /**< SD Boot Mode */
#define MMC_BOOT_MODE			0x00000005 /**< MMC Boot Device */
#define UART_DEVICE                     0x00000008

/*Boot Device*/
#define JTAG			0x00000000 /**< JTAG Boot Mode */
#define QSPI_FLASH		0x00000001 /**< QSPI Boot Mode */
#define NOR_FLASH		0x00000002 /**< NOR Boot Mode */
#define NAND_FLASH		0x00000003 /**< NAND Boot Mode */
#define SD_CARD			0x00000004 /**< SD Boot Mode */

#define BOOT_STATUS_JTAG 0x3U

 /* These are the SLCR lock and unlock macros*/
#define SlcrUnlock()	     *(volatile u32 *)(FPS_SLCR_BASEADDR + SLCR_UNLOCK_OFFSET)=0xDF0D767B
#define SlcrLock()           *(volatile u32 *)(FPS_SLCR_BASEADDR + SLCR_LOCK_OFFSET)= 0x767B767B

#define CsuUnlock()          *(volatile u32 *)(PCAP_REG_BASE_ADDR + PCAP_UNLOCK_OFFSET)=0x757bdf0d 

#define GOLDEN_IMAGE_OFFSET		0x8000

#define MULTIBOOT_ADDR_MASK 0x1FFF

#define Fmsh_In8(Addr) *(volatile u8 *)(Addr)	
#define Fmsh_Out8(Addr,Value)  *(volatile u8 *)(Addr)=Value

#define Fmsh_In32(Addr)  *(volatile u32 *)(Addr)
#define Fmsh_Out32(Addr,Value)  *(volatile u32 *)(Addr)=Value
   
#define ReadReg(Addr)    *(volatile u32 *)(Addr)
#define WriteReg(Addr,Value)   *(volatile u32 *)(Addr)=Value

typedef struct {
	u32 DeviceBaseAddress; /**< Flash device base address */
	u32 (*DeviceInit) ();
		/**< Function pointer for Device initialization code */
	u32 (*DeviceCopy) (u32 SrcAddress, u32 DestAddress, u32 Length);
		/**< Function pointer for device copy */

		/**< Function pointer for device release */
} Ps_DeviceOps;

/**
 * This stores the handoff Address of the different cpu's
 */
typedef struct {
	u32 CpuSettings;
	u32 HandoffAddress;
} Ps_HandoffValues;

/**
 * This is FSBL instance pointer. This stores all the information
 * required for FSBL
 */
typedef struct {
      float Version; /**<  Version */
      u32 ProcessorID; /**< One of A7-0 */
      u32 HandoffCpuNo; /**< Number of CPU's FSBL will handoff to */
      u8  SecureModeFlag;
      u32 ResetReason;
      u32 SearchRange;
      u32 SystemClk;
      u32 XipMode;
      u32 EncryptionStatus; 
      u32 EncrytionAlgorithm;
      u32 BootHdrAttributes; /**< Boot Header attributes */
      u32 ImageOffsetAddress; /**< Flash offset address */
      Ps_ImageHeader ImageHeader; /** Image header */
      u32 ErrorCode; /**< Error code during FSBL failure */
      u32 BootMode; /**< Primary boot device used for booting  */
      Ps_DeviceOps DeviceOps; /**< Device operations for bootmodes */
      Ps_HandoffValues HandoffValues[10];/**< Handoff address for different CPU's  */
} BootPs;

extern BootPs BootInstance;

/**
 * BOOT stages definition
 */
#define BOOT_STAGE0		(0x0U)
#define BOOT_STAGE1		(0x1U)
#define BOOT_STAGE2		(0x2U)
#define BOOT_STAGE3		(0x3U)
#define BOOT_STAGE4		(0x4U)
#define BOOT_STAGE_ERR		(0x5U)
#define BOOT_STAGE_DEFAULT	(0x6U)

/*Boot header research */
/*Research boundry of boot devices*/
#define NAND_LIMITATION (128*1024*1024)
#define NOR_LIMITATION  (32*1024*1024)
#define QSPI_4BIT_LIMITATION (16*1024*1024)
#define QSPI_8BIT_LIMITATION (32*1024*1024)
#define SD_LIMITATION  0

/* FSBL exit definition */
#define FSBL_NO_HANDOFFEXIT 0
#define FSBL_HANDOFFEXIT    1
//main.c
void PrintArray_1 (const unsigned char Buf[], unsigned int Len);

//fmsh_initialization.c
u32 FmshFsbl_BootInitialize(BootPs * BootInstancePtr);
u32 FmshFsbl_BootDeviceInitAndValidate(BootPs * FsblInstancePtr);
u32 BootHeaderSearchAndValidate(BootPs * BootInstancePtr);

//fmsh_partition_load.c
u32 FmshFsbl_PartitionLoad(BootPs * BootInstancePtr,u32 PartitionNum);

//boothandoff.c
void ErrorLockDown(void);
u32  BootHandoff (const BootPs * BootInstancePtr);
void DefaultHandoffExit();
void FmshFsbl_HandoffExit(u32 HandOffAddr,u32 Flag);

//fmsh_header.c
u32 FmshFsbl_PartitionHeaderValidation(BootPs * BootInstance,u32 PartitionNum);
u32 FmshFsbl_ReadImageHeader(Ps_ImageHeader* ImageHeader,Ps_DeviceOps* DeviceOps,u32 FlashImageOffsetAddress);

extern u64 gtc_count0, gtc_count1;
extern double gtc_time;

#endif
