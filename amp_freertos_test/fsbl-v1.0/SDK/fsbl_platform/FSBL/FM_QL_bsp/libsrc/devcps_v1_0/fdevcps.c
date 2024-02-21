/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_devc.c
*
* This file contains all c function code for devc(device configure) usind.
*
* @note		None.
*
* MODIFICATION HISTORY:
*
*<pre>
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------
* 0.01   wfb  11/23/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdlib.h>
#include "fdevcps_public.h"
#include "fdevcps_private.h"
#include "fmsh_aes_sm4.h"
#include "fmsh_ps_parameters.h"
#include "ps_init.h"
/************************** Constant Definitions *****************************/
#define CSU_FREQ 35 //Mhz uint
#define DELAY_FOR_PLCLK_MS 5
#define DUMMY_CLOCK_COUNT 1
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern unsigned long ps_pll_init_data[];

/************************** Function Prototypes ******************************/
#ifdef FSBL_PL_DDR
extern u32 AesSm4GenTag(u8 Alg, const u8* In, u32 InByteLen, u32 KeySel, u8*Iv, u8* Tag);
#endif

/*****************************************************************************/
/**
* This function unlocks CSU module
*
* @param	devcDev is devc handle.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_unLockCSU(FDevcPs_T *devcDev)
{ 
    FMSH_devc_portmap_TypeDef *portmap;
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    DEVC_OUT32P(0x757bdf0d, portmap->csu_unlock); 
    
    return 0;
}

unsigned long temp_ps_pll_init_data[32]=
{
    // ARM_PLL_BYPASS_FORCE = 1
    EMIT_MASKWRITE(FPS_SLCR_BASEADDR + 0x100, 0x00000008U, 0x00000008U),
    // ARM_PLL_RESET = 1
    EMIT_MASKWRITE(FPS_SLCR_BASEADDR + 0x100, 0x00000001U, 0x00000001U),
    // ARM_PLL_FDIV = 13
    // [22:16]
    EMIT_MASKWRITE(FPS_SLCR_BASEADDR + 0x100, 0x007f0000U, 0x000c0000U),
    // ARM_PLL_CLKOUT0_DIVISOR = 2
    // [6:0]
    EMIT_MASKWRITE(FPS_SLCR_BASEADDR + 0x104, 0x0000007fU, 0x00000002U),
    // ENABLE ARM PLL LATCH
    EMIT_WRITE(FPS_SLCR_BASEADDR + 0x180, 0x00000001U), 
    // ARM_PLL_RESET = 0
    EMIT_MASKWRITE(FPS_SLCR_BASEADDR + 0x100, 0x00000001U, 0x00000000U),
    // CHECK ARM PLL STATUS
    EMIT_MASKPOLL(FPS_SLCR_BASEADDR + 0x170, 0x00000001U),
    // ARM_PLL_BYPASS_FORCE = 0
    EMIT_MASKWRITE(FPS_SLCR_BASEADDR + 0x100, 0x00000008U, 0x00000000U),

    EMIT_EXIT(),
};

/*****************************************************************************/
/**
* This function reduce the cpu freq.
*
* @param	
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful

*
* @note		None
*
****************************************************************************/
u8 FDevcPs_adjustFreq(u8* flag)
{
    u32 clk_div=1;
    u8 ret=FMSH_SUCCESS;
    u32 pll_multi=8;
    u32 pll_div=2;
    
    FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);/* unlock SLCR */
    if(FMSH_ReadReg(FPS_SLCR_BASEADDR , 0x20C)&0x1==0x1) 
      clk_div=12; //6:2:1
    else
      clk_div=8; //4:2:1
    
    pll_div=FMSH_ReadReg(FPS_SLCR_BASEADDR , 0x104)&0x7f;
    if((APU_FREQ/clk_div)>(CSU_FREQ*1000000))
    {
        pll_multi=(CSU_FREQ*clk_div*pll_div*10)/(PS_CLK_FREQ/100000);
        if( (pll_multi==0) || (pll_multi>0x7f) )
        {
          ret=FMSH_FAILURE;
          goto END;
        }
        temp_ps_pll_init_data[11]=(pll_multi&0x7f)<<16;
        temp_ps_pll_init_data[15]=pll_div&0x7f;
        // PLL init 
        ret = ps_config(temp_ps_pll_init_data);
        if (ret != FMSH_SUCCESS) 
        {
          ret=FMSH_FAILURE;
          goto END;
        }
        *flag=1;
    }
END:    
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x004, 0xDF0D767B);/* relock SLCR */
    return ret;
}
/*****************************************************************************/
/**
* This function recover the cpu freq.
*
* @param	
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_recoverFreq(void)
{ 
    u32 idx;
    u8 ret=FMSH_SUCCESS;
    unsigned long pll_data[256]={0};
    for(idx=0;idx<256;idx++)
    {
        pll_data[idx]=ps_pll_init_data[idx];
        //arm pll force
        if(idx>3)
        {
          if(pll_data[idx-3]==( (OPCODE_MASKWRITE << 4 ) | 3 ) &&
                pll_data[idx-2]==SLCR_REG_BASE + 0x100 &&
                    pll_data[idx-1]== 0x00000008U &&
                       pll_data[idx]== 0x00000000U)
                break;
        }
    }
    if(idx==256)
	return FMSH_FAILURE;
    idx++;
    pll_data[idx++]=( (OPCODE_WRITE  << 4 ) | 2 );
    pll_data[idx++]=FPS_SLCR_BASEADDR+0x004;
    pll_data[idx++]=0xDF0D767BU;
    pll_data[idx]= EMIT_EXIT();
    // PLL init 
    ret = ps_config(pll_data);
    if (ret != FMSH_SUCCESS) 
  	return ret;
    
    return FMSH_SUCCESS;
}

/*****************************************************************************/
/**
* This function initializes devc device 
*
* @param	devcDev is devc handle.
* @param	addr is the base address of CSU.
*
* @return
*		- 0 if successful
*		- 1 not support
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_init(FDevcPs_T *devcDev, u32 addr)
{
    devcDev->base_address = (void*)addr;

    return 0;
}

/*****************************************************************************/
/**
* This function initializes Device Configuration module, configure pcap interface work parameters
*
* @param	TransferType.
*		- FMSH_PCAP_READBACK
*		- FMSH_NON_SECURE_PCAP_WRITE
*		- FMSH_PCAP_LOOPBACK
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_fabricInit(FDevcPs_T *devcDev, u32 TransferType)
{        

    FDevcPs_unLockCSU(devcDev);
 
    FDevcPs_smap32Swap(devcDev, smap32_swap_enable);
    FDevcPs_rclk_edge(devcDev, falling_edge);//default failing_edge , rising_edge will failed
    FDevcPs_wclk_edge(devcDev, falling_edge);//default failing_edge
    
    FDevcPs_CSI_B_HIGH(devcDev);
    FDevcPs_RDWR_B_HIGH(devcDev);

    if(TransferType == FMSH_PCAP_READBACK) 
    {
        FDevcPs_rxDataSwap(devcDev, byte_swap);
        FDevcPs_txDataSwap(devcDev, byte_swap); 

        //default 3 , for FMSH 325t the value must be set 4,
        //for fmsh 325t the value must be set 3 for readback reg, but the value must be set 4 for readback bitstream
        FDevcPs_readbackDummyCount(devcDev, dummy_4);
        
        FDevcPs_downloadMode(devcDev, DOWNLOAD_BITSTREAM);
        FDevcPs_readFifoThre(devcDev, readFifoThre_hex_0x40);
    } 
    else if(TransferType ==FMSH_NON_SECURE_PCAP_WRITE)
    {
        if(FMSH_ReadReg(FPS_CSU_BASEADDR,0x18)&0x200)
            FDevcPs_Prog_B(devcDev);
        //FDevcPs_rxDataSwap(devcDev, byte_swap); 
        FDevcPs_rxDataSwap(devcDev, none_swap); 
        FDevcPs_downloadMode(devcDev, DOWNLOAD_BITSTREAM);
        FDevcPs_writeFifoThre(devcDev, writeFifoThre_hex_0x80);
    } 
    else if(TransferType == FMSH_PCAP_LOOPBACK) 
    {
        FDevcPs_rxDataSwap(devcDev, byte_swap);
        FDevcPs_txDataSwap(devcDev, byte_swap); 
        FDevcPs_readbackDummyCount(devcDev, dummy_4);//default 3
        FDevcPs_downloadMode(devcDev, DATA_LOOPBACK);      
    }
    else
        return FMSH_FAILURE;

    return FMSH_SUCCESS;
}

/*****************************************************************************/
/**
* This function pull down Prog_B, then pull up
*
* @param	devcDev is devc handle.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_Prog_B(FDevcPs_T *devcDev)
{
    u32 reg;

    FMSH_devc_portmap_TypeDef *portmap;
    
    FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);// unlock SLCR
    FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x838, 0x0);//close the usr-level-shifter
    FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x004, 0xDF0D767B);/* relock SLCR */
   
    portmap = (FMSH_devc_portmap_TypeDef *) devcDev->base_address; 
    
    //Setting PCFG_PROG_B signal to high
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_program_b) != 1) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_program_b, 1U);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    //Setting PCFG_PROG_B signal to low
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_program_b) != 0)
    {
        FMSH_BIT_SET(reg, DEVC_CFG_program_b, 0);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }

    //Polling the PCAP_INIT sts for Reset  
    reg = DEVC_IN32P(portmap->sac_status); 
    while(FMSH_BIT_GET(reg, DEVC_STATUS_pcfg_init) != 0) 
    {
        reg = DEVC_IN32P(portmap->sac_status); 
    }
    
    //Setting PCFG_PROG_B signal to high
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_program_b) != 1) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_program_b, 1U);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }

    //Polling the PCAP_INIT sts for set  
    reg = DEVC_IN32P(portmap->sac_status); 
    while(FMSH_BIT_GET(reg, DEVC_STATUS_pcfg_init) != 1) 
    {
        reg = DEVC_IN32P(portmap->sac_status); 
    }

    return 0;
}

/*****************************************************************************/
/**
* This function pull high CSI_B
*
* @param	devcDev is devc handle.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_CSI_B_HIGH(FDevcPs_T *devcDev)
{
    u32 reg;
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *) devcDev->base_address; 
    
    //Setting PCFG_CSI_B signal to HIGH
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_CSI_B) != 1) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_CSI_B, 1);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function pull high RDWR_B
*
* @param	devcDev is devc handle.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_RDWR_B_HIGH(FDevcPs_T *devcDev)
{  
    u32 reg;
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *) devcDev->base_address;         
    //Setting PCFG_RDWR_B signal to HIGH
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_RDWR_B ) != 1) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_RDWR_B, 1);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function pull low CSI_B
*
* @param	devcDev is devc handle.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_CSI_B_LOW(FDevcPs_T *devcDev)
{
    u32 reg;
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *) devcDev->base_address;         
    //Setting PCFG_CSI_B signal to LOW
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_CSI_B) != 0)
    {
        FMSH_BIT_SET(reg, DEVC_CFG_CSI_B, 0);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function pull low RDWR_B
*
* @param	devcDev is devc handle.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_RDWR_B_LOW(FDevcPs_T *devcDev)
{  
    u32 reg;
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *) devcDev->base_address; 
    //Setting PCFG_RDWR_B signal to LOW
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_RDWR_B ) != 0) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_RDWR_B, 0);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function configures KUPKEY
* this key value maybe changed by different bitstream
*
* @param	devcDev is devc handle.
* @param	p is pointer to KEY buffer.
* @param	len is length of KEY, this should be fix as 8.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_KUPKEY(FDevcPs_T *devcDev, u32 *p, u32 len)
{       
    u32 *des;
    FMSH_devc_portmap_TypeDef *portmap;
    
    FMSH_ASSERT(len == 8);
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    des = malloc(50);
    devc_byte_swap_todes(p, len, des);
    
    DEVC_OUT32P(*(des + 0), portmap->KUP7); 
    DEVC_OUT32P(*(des + 1), portmap->KUP6);         
    DEVC_OUT32P(*(des + 2), portmap->KUP5);         
    DEVC_OUT32P(*(des + 3), portmap->KUP4);         
    DEVC_OUT32P(*(des + 4), portmap->KUP3);         
    DEVC_OUT32P(*(des + 5), portmap->KUP2);         
    DEVC_OUT32P(*(des + 6), portmap->KUP1);         
    DEVC_OUT32P(*(des + 7), portmap->KUP0);        

    free(des);
    
    return 0;
}

/*****************************************************************************/
/**
* This function configures DEVKEY
* this key value maybe changed by different bitstream
*
* @param	devcDev is devc handle.
* @param	p is pointer to KEY buffer.
* @param	len is length of KEY, this should be fix as 8.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_DEVKEY(FDevcPs_T *devcDev, u32 *p, u32 len)
{ 
    u32 *des;
    FMSH_devc_portmap_TypeDef *portmap;
    
    FMSH_ASSERT(len == 8);
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    des = malloc(50);
    devc_byte_swap_todes(p, len, des);

    DEVC_OUT32P(*(des + 0), portmap->dev_key7); 
    DEVC_OUT32P(*(des + 1), portmap->dev_key6);        
    DEVC_OUT32P(*(des + 2), portmap->dev_key5);         
    DEVC_OUT32P(*(des + 3), portmap->dev_key4);       
    DEVC_OUT32P(*(des + 4), portmap->dev_key3);       
    DEVC_OUT32P(*(des + 5), portmap->dev_key2);        
    DEVC_OUT32P(*(des + 6), portmap->dev_key1); 
    DEVC_OUT32P(*(des + 7), portmap->dev_key0); 

    free(des);
    
    return 0;

}

/*****************************************************************************/
/**
* This function configures fixed IV
* this IV value maybe changed by different bitstream
*
* @param	devcDev is devc handle.
* @param	p is pointer to IV buffer.
* @param	len is length of IV, this should be fix as 4.
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_IV(FDevcPs_T *devcDev, u32 *p, u32 len)
{
    u32 *des;
    FMSH_devc_portmap_TypeDef *portmap;
            
    FMSH_ASSERT(len == 3);
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    des = malloc(50);
    devc_byte_swap_todes(p, len, des);
    
    //set iv_reg             
    DEVC_OUT32P(0x2, portmap->IVUP0); 
    DEVC_OUT32P(*(des), portmap->IVUP3); 
    DEVC_OUT32P(*(des + 1), portmap->IVUP2); 
    DEVC_OUT32P(*(des + 2), portmap->IVUP1); 

    free(des);
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets secure download data byteswap
*
* @param	devcDev is devc handle.
* @param	mode
*              none_swap = 0x0,
*              half_word_swap = 0x1,   
*              byte_swap = 0x2,
*              bit_swap = 0x3  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_secDownDataByteSwap(FDevcPs_T *devcDev, enum data_swap mode)
{
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_sec_down_data_byte_swap) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_sec_down_data_byte_swap, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
               
    return 0;
}

/*****************************************************************************/
/**
* This function sets tx data byte swap.
*
* @param	devcDev is devc handle.
* @param	mode
*       none_swap = 0x0,
*       half_word_swap = 0x1,   
*       byte_swap = 0x2,
*       bit_swap = 0x3  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_txDataSwap(FDevcPs_T *devcDev, enum data_swap mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_txfifo_do_data_swap) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_txfifo_do_data_swap, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets bitstream download mode.
*
* @param	devcDev is devc handle.
* @param	mode
*       DOWNLOAD_BITSTREAM = 0x0,
*       READBACK_BITSTREAM = 0x2, 
*       DATA_LOOPBACK = 0x3,     
*       SECURE_DOWNLOAD_BITSTREAM = 0x08  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_downloadMode(FDevcPs_T *devcDev, enum download_mode mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_CSU_DMA_SWITCH) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_CSU_DMA_SWITCH, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}
/*****************************************************************************/
/**
* This function sets rx data byte swap.
*
* @param	devcDev is devc handle.
* @param	mode
*       none_swap = 0x0,
*       half_word_swap = 0x1,   
*       byte_swap = 0x2,
*       bit_swap = 0x3  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_rxDataSwap(FDevcPs_T *devcDev, enum data_swap mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_rxfifo_di_data_swap) != mode)
    {
        FMSH_BIT_SET(reg, DEVC_CFG_rxfifo_di_data_swap, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets swap32 data byte swap.
*
* @param	devcDev is devc handle.
* @param	mode
*        smap32_swap_disable = 0x0,
*        smap32_swap_enable = 0x1  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_smap32Swap(FDevcPs_T *devcDev, enum smap32_swap mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_smap32_swap_ctrl) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_smap32_swap_ctrl, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets readback dummmy counter, that is where data is sampled 
*
* @param	devcDev is devc handle.
* @param	mode
*        smap32_swap_disable = 0x0,
*        smap32_swap_enable = 0x1  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_readbackDummyCount(FDevcPs_T *devcDev, enum dummy_num mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_READBACK_DUMMY_NUM) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_READBACK_DUMMY_NUM, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets read FIFO threshold
*
* @param	devcDev is devc handle.
* @param	mode
*       readFifoThre_hex_0x40 = 0x0,//default
*       readFifoThre_hex_0x80 = 0x1,
*       readFifoThre_hex_0xc0 = 0x2,
*       readFifoThre_hex_0x100 = 0x3
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_readFifoThre(FDevcPs_T *devcDev, enum readFifoThre mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_RFIFO_TH) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_RFIFO_TH, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets write FIFO threshold
*
* @param	devcDev is devc handle.
* @param	mode
*       writeFifoThre_hex_0x80 = 0x0,//default
*       writeFifoThre_hex_0x60 = 0x1,
*       writeFifoThre_hex_0x40 = 0x2,
*       writeFifoThre_hex_0x10 = 0x3
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_writeFifoThre(FDevcPs_T *devcDev, enum writeFifoThre mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_WFIFO_TH) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_WFIFO_TH, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets read clock edge
*
* @param	devcDev is devc handle.
* @param	mode
*       rising_edge = 0x1,
*       failing_edge = 0x0   
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_rclk_edge(FDevcPs_T *devcDev, enum clk_edge mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_RCLK_EDGE) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_RCLK_EDGE, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets write clock edge
*
* @param	devcDev is devc handle.
* @param	mode
*       rising_edge = 0x1,
*       failing_edge = 0x0   
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_wclk_edge(FDevcPs_T *devcDev, enum clk_edge mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_cfg); 
    if(FMSH_BIT_GET(reg, DEVC_CFG_WCLK_EDGE) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_CFG_WCLK_EDGE, mode);
        DEVC_OUT32P(reg, portmap->sac_cfg); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function disable AES
*
* @param	devcDev is devc handle.
* @param	None 
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_CR_AESENDisable(FDevcPs_T *devcDev)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->gcm_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_GCM_CTRL_gcm_en) != 0) 
    {
        FMSH_BIT_SET(reg, DEVC_GCM_CTRL_gcm_en, 0);
        DEVC_OUT32P(reg, portmap->gcm_ctrl); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function enable AES
*
* @param	devcDev is devc handle.
* @param	None 
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_CR_AESENEnable(FDevcPs_T *devcDev)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->gcm_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_GCM_CTRL_gcm_en) != 1) 
    {
        FMSH_BIT_SET(reg, DEVC_GCM_CTRL_gcm_en, 1);
        DEVC_OUT32P(reg, portmap->gcm_ctrl); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function selects algorithm
*
* @param	devcDev is devc handle.
* @param	mode 
*               AES = 0x0,
*               SM4 = 0x1,
*               NONE = 0x2
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_CR_ALG_SEL(FDevcPs_T *devcDev, enum ALG mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->gcm_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_GCM_CTRL_alg_sel) != mode)
    {
        FMSH_BIT_SET(reg, DEVC_GCM_CTRL_alg_sel, mode);
        DEVC_OUT32P(reg, portmap->gcm_ctrl); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function selects encrypt or decrypt
*
* @param	devcDev is devc handle.
* @param	mode 
*               ENCODE = 0x0,
*               DCODE = 0x3 
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_CR_MODE(FDevcPs_T *devcDev, enum MODE mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->gcm_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_GCM_CTRL_mode) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_GCM_CTRL_mode, mode);
        DEVC_OUT32P(reg, portmap->gcm_ctrl); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function selects data stream handle mode
*
* @param	devcDev is devc handle.
* @param	mode 
*               ECB = 0x0,
*               CTR = 0x2, 
*               MULTH = 0x3
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_CR_CHMOD(FDevcPs_T *devcDev, enum CHMOD mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->gcm_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_GCM_CTRL_chmode) != mode) 
    {
        FMSH_BIT_SET(reg, DEVC_GCM_CTRL_chmode, mode);
        DEVC_OUT32P(reg, portmap->gcm_ctrl); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function selects KEY source
*
* @param	devcDev is devc handle.
* @param	mode 
*              DEV_KEY = 0x1,
*              KUP = 0x2,  
*              MULTH_H = 0x3  
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_KEY_SOURCE(FDevcPs_T *devcDev, enum KEYSRC mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->key_src); 
    if(FMSH_BIT_GET(reg, KEY_SRC_key_src ) != mode)
    {
        FMSH_BIT_SET(reg, KEY_SRC_key_src, mode);
        DEVC_OUT32P(reg, portmap->key_src); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function loads AES's KEY & IV
*
* @param	devcDev is devc handle.
* @param	None 
*
* @return
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_KEY_IV_LOAD(FDevcPs_T *devcDev)
{  
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    DEVC_OUT32P(0x3, portmap->key_iv_load); 
    
    return 0;
}

/*****************************************************************************/
/**
* This function sets DECFLAG
*
* @param	devcDev is devc handle.
* @param	mode 
*               use_opkey = 0xe,  
*               no_opkey = 0xa ,
*               ivup_kup_wr_en = 0x3 ,
*               clear = 0x0 
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_AES_DEC_FLAG(FDevcPs_T *devcDev, enum DECFLAG mode)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->dec_flag); 
    if(FMSH_BIT_GET(reg, DEC_FLAG_flag) != mode)
    {
        FMSH_BIT_SET(reg, DEC_FLAG_flag, mode);
        DEVC_OUT32P(reg, portmap->dec_flag); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function enables encrypt moudle (AES/SM4)
*
* @param	devcDev is devc handle.
* @param	None 
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_CTRL_AESenable(FDevcPs_T *devcDev)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_CTRL_encrypted_en) != 0x7) 
    {
        FMSH_BIT_SET(reg, DEVC_CTRL_encrypted_en, 0x7);
        DEVC_OUT32P(reg, portmap->sac_ctrl); 
    }
    
    return 0;
}

/*****************************************************************************/
/**
* This function disables encrypt moudle (AES/SM4)
*
* @param	devcDev is devc handle.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 FDevcPs_CTRL_AESdisable(FDevcPs_T *devcDev)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    reg = DEVC_IN32P(portmap->sac_ctrl); 
    if(FMSH_BIT_GET(reg, DEVC_CTRL_encrypted_en) != 0x0) 
    {
        FMSH_BIT_SET(reg, DEVC_CTRL_encrypted_en, 0x0);
        DEVC_OUT32P(reg, portmap->sac_ctrl); 
    }
    
    return 0;
}

/******************************common data swap*****************************/

/*****************************************************************************/
/**
* This function byte swap data
*
* @param	srcPtr is pointer of data buffer
*               len is length to be swap
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 devc_byte_swap(u32 *srcPtr, u32 len)
{
    u32 i;
    u32 tmp;

    u32 byte_higher, byte_high, byte_low, byte_lower;

    for(i = 0; i < len; i++)
    {
        tmp = *(srcPtr + i);
        byte_higher = tmp & 0x000000ff;
        byte_high = tmp & 0x0000ff00;  
        byte_low = tmp & 0x00ff0000;
        byte_lower = tmp & 0xff000000;          
        
        *(srcPtr + i) = (byte_higher << 24) | ((byte_high >> 8) << 16) | ((byte_low >> 16) << 8) | ((byte_lower >> 24) << 0);
    }
    return 0;
}

/*****************************************************************************/
/**
* This function byte swap data, swap data save in destination address
*
* @param	srcPtr is pointer of data buffer
*               len is length to be swap
*               desPtr is pointer of data buffer saveing data after swap
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None
*
****************************************************************************/
u8 devc_byte_swap_todes(u32 *srcPtr, u32 len, u32 *desPtr)
{
    u32 i;
    u32 tmp;

    u32 byte_higher, byte_high, byte_low, byte_lower;

    for(i = 0; i < len; i++)
    {
        tmp = *(srcPtr + i);
        byte_higher = tmp & 0x000000ff;
        byte_high = tmp & 0x0000ff00;  
        byte_low = tmp & 0x00ff0000;
        byte_lower = tmp & 0xff000000;          
        
        *(desPtr + i) = (byte_higher << 24) | ((byte_high >> 8) << 16) | ((byte_low >> 16) << 8) | ((byte_lower >> 24) << 0);
    }
    return 0;
}

/******************************bitstream configure*****************************/

/****************************************************************************/
/**
*
* This function clears the specified interrupts in the Interrupt Status
* Register.
*
* @param	devcDev is devc handle.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
*****************************************************************************/
u8 FDevcPs_clearPcapStatus(FDevcPs_T *devcDev)
{  
    u32 reg; 
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
    // Clear it all, so if Boot ROM comes back, it can proceed
    // write 0xffffffff to INT_STS
    DEVC_OUT32P(0xffffffff, portmap->sac_int_sts);
    
    //Get PCAP Interrupt Status Register
    reg = DEVC_IN32P(portmap->sac_int_sts);
    if (reg & FMSH_DEVC_ERROR_FLAGS_MASK)
        return FMSH_FAILURE;

    //Read the PCAP sts register for DMA sts
    reg = DEVC_IN32P(portmap->sac_status);
    //If the queue is full(busy), return 1
    if(FMSH_BIT_GET(reg, DEVC_STATUS_dma_busy) == 1)
        return FMSH_FAILURE;
    
    return FMSH_SUCCESS;
}

/******************************************************************************/
/**
*
* This function initiates the DMA transfer.
*
* @param	devcDev is devc handle.
* @param	InstancePtr is a pointer to the XDcfg instance.
* @param	SourcePtr contains a pointer to the source memory where the data
*		is to be transferred from.
* @param	SrcWordLength is the number of words (32 bit) to be transferred
*		for the source transfer.
* @param	DestPtr contains a pointer to the destination memory
*		where the data is to be transferred to.
* @param	DestWordLength is the number of words (32 bit) to be transferred
*		for the Destination transfer.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
****************************************************************************/
u8 FDevcPs_initiateDma(FDevcPs_T *devcDev, u32 SourcePtr, u32 DestPtr, u32 SrcWordLength, u32 DestWordLength)
{
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
 
#if 0
    u64 count0, count1;
    double time;
    //global_timer_enable();  
    count0 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
#endif  
    
    DEVC_OUT32P(SourcePtr, portmap->csu_dma_src_addr);
    DEVC_OUT32P(DestPtr, portmap->csu_dma_dest_addr);
    DEVC_OUT32P(SrcWordLength, portmap->csu_dma_src_len);
    DEVC_OUT32P(DestWordLength, portmap->csu_dma_dest_len);   
   
    // Poll for the DMA done
    //Poll for the D_P done
    FDevcPs_pollDmaDone(devcDev, DEVC_POLL_DONE_MS);
    FDevcPs_pollDPDone(devcDev, DEVC_POLL_DONE_MS);
    
#if 0
    count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    time = (count1 - count0) / (float)( GTC_CLK_FREQ * 1000 );
    LOG_OUT(1,"dma carry time is %fms\r\n",time);
#endif 
    
    return 0;
}

/******************************************************************************/
/**
*
* This function initiates the DMA transfer for readback.
*
* @param	devcDev is devc handle.
* @param	InstancePtr is a pointer to the XDcfg instance.
* @param	SourcePtr contains a pointer to the source memory where the data
*		is to be transferred from.
* @param	SrcWordLength is the number of words (32 bit) to be transferred
*		for the source transfer.
* @param	DestPtr contains a pointer to the destination memory
*		where the data is to be transferred to.
* @param	DestWordLength is the number of words (32 bit) to be transferred
*		for the Destination transfer.
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
****************************************************************************/
u8 FDevcPs_initiateDma_readback(FDevcPs_T *devcDev, u32 SourcePtr, u32 DestPtr, u32 SrcWordLength, u32 DestWordLength)
{
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
        
#ifdef TEST_TIME
    u64 count0, count1;
    double time;
    char s[10];
    global_timer_enable();  
    count0 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
#endif     
        
    DEVC_OUT32P(SourcePtr, portmap->csu_dma_src_addr);
    DEVC_OUT32P(DestPtr, portmap->csu_dma_dest_addr);
    DEVC_OUT32P(SrcWordLength, portmap->csu_dma_src_len);
    DEVC_OUT32P(DestWordLength, portmap->csu_dma_dest_len);   
    
    FDevcPs_RDWR_B_HIGH(devcDev);  
    FDevcPs_CSI_B_LOW(devcDev); 

    //Poll for the DMA done
    //Poll for the D_P done
    FDevcPs_pollDmaDone(devcDev, DEVC_POLL_DONE_MS);
    FDevcPs_pollDPDone(devcDev, DEVC_POLL_DONE_MS);
        
#ifdef TEST_TIME
    count1 = *(u64*)(FPS_GTC_BASEADDR + 0x8);
    time = (count1 - count0) / 25000.0;//(count1 - count0) * 1000 / 25000000;
    sprintf(s, "%f", time);
    uart_logout("dma carry time is /ms");
    uart_logout(s);
#endif   
        
    return 0;
}

/******************************************************************************/
/**
*
* This function polls DMA done
*
* @param	devcDev is devc handle.
* @param	maxcount is the max poll times
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
****************************************************************************/
u8 FDevcPs_pollDmaDone(FDevcPs_T *devcDev, u32 MaxCount)
{
    FMSH_devc_portmap_TypeDef *portmap;

    int count = MaxCount;
    u32 reg = 0;
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 

	
    reg = DEVC_IN32P(portmap->sac_int_sts);
    while (FMSH_BIT_GET(reg, DEVC_INT_STATUS_dma_done_int) != 1)
    {
        reg = DEVC_IN32P(portmap->sac_int_sts);
                
	count -= 1;

        if (reg & FMSH_DEVC_ERROR_FLAGS_MASK)
            return 1;

	if (!count)
            return 1;
        
        delay_ms(1);
    }
    reg = 0;
    FMSH_BIT_SET(reg, DEVC_INT_STATUS_dma_done_int, 1);//clear dma done int status.
    DEVC_OUT32P(reg, portmap->sac_int_sts);
    
    return 0;
}

/******************************************************************************/
/**
*
* This function polls D_P done
*
* @param	devcDev is devc handle.
* @param	maxcount is the max poll times
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
****************************************************************************/
u8 FDevcPs_pollDPDone(FDevcPs_T *devcDev, u32 MaxCount)
{
    FMSH_devc_portmap_TypeDef *portmap;

    int count = MaxCount;
    u32 reg = 0;
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 


    reg = DEVC_IN32P(portmap->sac_int_sts);
    while (FMSH_BIT_GET(reg, DEVC_INT_STATUS_dp_done_int) != 1) 
    {
        reg = DEVC_IN32P(portmap->sac_int_sts);

        count -= 1;

        if (reg & FMSH_DEVC_ERROR_FLAGS_MASK)
            return 1;

        if (!count)
            return 1;
        
        delay_ms(1);
    }

    reg = 0;
    FMSH_BIT_SET(reg, DEVC_INT_STATUS_dp_done_int, 1);//clear dma done int status.
    DEVC_OUT32P(reg, portmap->sac_int_sts);

    return 0;
}
/******************************************************************************/
/**
*
* This function polls FPGA done
*
* @param	devcDev is devc handle.
* @param	maxcount is the max poll times
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
****************************************************************************/
u8 FDevcPs_pollFpgaDone(FDevcPs_T *devcDev, u32 maxcount)
{
    FMSH_devc_portmap_TypeDef *portmap;

    int count = maxcount;
    u32 reg = 0;
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    u32 i=0;
    
#if 1
    u32 Dummy[12]={0};
    for(i=0;i<12;i++)
    {
        Dummy[i++] = 0x20000000;
    }
        
    for(i=0;i<DUMMY_CLOCK_COUNT;i++)
         FDevcPs_transfer(devcDev, Dummy, 12, (u8 *)FMSH_DMA_INVALID_ADDRESS, 0, FMSH_NON_SECURE_PCAP_WRITE_DUMMMY); 
    
#endif
    
    reg = DEVC_IN32P(portmap->sac_status);

    while (FMSH_BIT_GET(reg, DEVC_STATUS_pcfg_done) != 1)
    {
        reg = DEVC_IN32P(portmap->sac_status);
        count -= 1;

        if (count == 0)
            return FMSH_FAILURE;
        
        delay_ms(1);
    }
       
    FMSH_WriteReg(FPS_SLCR_BASEADDR, 0x008, 0xDF0D767B);/* unlock SLCR */
    FMSH_WriteReg(FPS_SLCR_BASEADDR , 0x838, 0xF);

#ifdef DDRPS_0_DEVICE_ID    
    delay_ms(DELAY_FOR_PLCLK_MS);
    
    /*enable HP interface*/
    reg=FMSH_ReadReg(DDR_UMC_REG_BASE,0x490);
    if(0x1==reg)
        FMSH_WriteReg(DDR_UMC_REG_BASE , 0x490, 0x1); 
    reg=FMSH_ReadReg(DDR_UMC_REG_BASE,0x540);
    if(0x1==reg)
        FMSH_WriteReg(DDR_UMC_REG_BASE , 0x540, 0x1); 
    reg=FMSH_ReadReg(DDR_UMC_REG_BASE,0x5f0);
    if(0x1==reg)
        FMSH_WriteReg(DDR_UMC_REG_BASE , 0x5f0, 0x1); 
    reg=FMSH_ReadReg(DDR_UMC_REG_BASE,0x6a0);
    if(0x1==reg)
        FMSH_WriteReg(DDR_UMC_REG_BASE , 0x6a0, 0x1); 
#endif
    
    FMSH_WriteReg(FPS_SLCR_BASEADDR,0x004, 0xDF0D767B);/* relock SLCR */
    
    
    return FMSH_SUCCESS;
}

/****************************************************************************/
/**
*
* Generates a Type 1 packet head that reads back the requested Configuration
* register.
*
* @param	Register is the address of the register to be read back.
* @param	OpCode is the read/write operation code.
* @param	Size is the size of the word to be read.
*
* @return	Type 1 packet head to read the specified register
*
* @note		None.
*
*****************************************************************************/
unsigned long FDevcPs_regAddr(u8 Register, u8 OpCode, u8 Size)
{
	/*
	 * Type 1 Packet Header Format
	 * The head section is always a 32-bit word.
	 *
	 * HeaderType | Opcode | Register Address | Reserved | Word count
	 * [31:29]	[28:27]		[26:13]	     [12:11]     [10:0]
	 * --------------------------------------------------------------
	 *   001 	  xx 	  RRRRRRRRRxxxxx	RR	xxxxxxxxxxx
	 *
	 * ?R? means the bit is not used and reserved for future use.
	 * The reserved bits should be written as 0s.
	 *
	 * Generating the Type 1 packet head which involves sifting of Type 1
	 * Header Mask, Register value and the OpCode which is 01 in this case
	 * as only read operation is to be carried out and then performing OR
	 * operation with the Word Length.
	 */
	return ( ((XDC_TYPE_1 << XDC_TYPE_SHIFT) |
		(Register << XDC_REGISTER_SHIFT) |
		(OpCode << XDC_OP_SHIFT)) | Size);
}

/****************************************************************************/
/**
*
* This function starts the DMA transfer. This function only starts the
* operation and returns before the operation may be completed.
* If the interrupt is enabled, an interrupt will be generated when the
* operation is completed, otherwise it is necessary to poll the Status register
* to determine when it is completed. It is the responsibility of the caller to
* determine when the operation is completed by handling the generated interrupt
* or polling the Status Register.
*
* @param	devcDev is devc handle.
* @param	SourcePtr contains a pointer to the source memory where the data
*		is to be transferred from.
* @param	SrcWordLength is the number of words (32 bit) to be transferred
*		for the source transfer.
* @param	DestPtr contains a pointer to the destination memory
*		where the data is to be transferred to.
* @param	DestWordLength is the number of words (32 bit) to be transferred
*		for the Destination transfer.
* @param	TransferType 
*           FMSH_NON_SECURE_PCAP_WRITE		0
*           FMSH_SECURE_PCAP_WRITE			1
*           FMSH_PCAP_READBACK			2
*           FMSH_NON_SECURE_PCAP_WRITE_DUMMMY	4
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
*****************************************************************************/
u8 FDevcPs_transfer(FDevcPs_T *devcDev,
                      void *SourcePtr, 
                      u32 SrcWordLength, 
                      void *DestPtr, 
                      u32 DestWordLength, 
                      u32 TransferType)
{ 
    u32 reg = 0;
    FMSH_devc_portmap_TypeDef *portmap;

    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 

    //check if DMA command queue is full
    //Read the PCAP sts register for DMA sts
    reg = DEVC_IN32P(portmap->sac_status);
    if(FMSH_BIT_GET(reg, DEVC_STATUS_dma_busy) == 1)
        return FMSH_FAILURE;

    //Check whether the fabric is in initialized state
    reg = DEVC_IN32P(portmap->sac_status);
    if(FMSH_BIT_GET(reg, DEVC_STATUS_pcfg_init) != 1)
        return FMSH_FAILURE;
        
        //We don't need to check PCFG_INIT to be high for non-encrypted loopback transfers

	if ((TransferType == FMSH_SECURE_PCAP_WRITE) ||
		(TransferType == FMSH_NON_SECURE_PCAP_WRITE)) 
    {  
        //Check for valid source pointer and length
        if ((!SourcePtr) || (SrcWordLength == 0))
                return FMSH_FAILURE;
        
        if (TransferType == FMSH_NON_SECURE_PCAP_WRITE)
                FDevcPs_downloadMode(devcDev, DOWNLOAD_BITSTREAM);//set the mode:download bitstream

        if (TransferType == FMSH_SECURE_PCAP_WRITE)      
                FDevcPs_downloadMode(devcDev, SECURE_DOWNLOAD_BITSTREAM);//set sec_down mode

        FDevcPs_RDWR_B_LOW(devcDev);
        FDevcPs_CSI_B_LOW(devcDev);
        
        FDevcPs_initiateDma(devcDev, (u32)SourcePtr, (u32)DestPtr, SrcWordLength, DestWordLength);
    }
    else if (TransferType == FMSH_PCAP_READBACK)
    {
        if ((!DestPtr) || (DestWordLength == 0)) 
            return FMSH_FAILURE;

        //Send READ Frame command to FPGA
        //set the mode:download 
        FDevcPs_downloadMode(devcDev, DOWNLOAD_BITSTREAM);//set the mode:download bitstream

        FDevcPs_RDWR_B_LOW(devcDev);
        FDevcPs_CSI_B_LOW(devcDev);

        FDevcPs_initiateDma(devcDev, (u32)SourcePtr, PCAP_RD_DATA_ADDR, SrcWordLength, 0);
        
        FDevcPs_CSI_B_HIGH(devcDev);         

        FDevcPs_downloadMode(devcDev, READBACK_BITSTREAM);//set the mode:readback bitstream mode
        
        //Initiate the DMA write command.
        FDevcPs_initiateDma_readback(devcDev, PCAP_WR_DATA_ADDR, (u32)DestPtr, 0, DestWordLength);
    }
    else if(TransferType == FMSH_NON_SECURE_PCAP_WRITE_DUMMMY) 
    {
        if ((!SourcePtr) || (SrcWordLength == 0))
            return FMSH_FAILURE;
        
        FDevcPs_downloadMode(devcDev, DOWNLOAD_BITSTREAM);//set the mode:download bitstream

        FDevcPs_CSI_B_HIGH(devcDev);
        FDevcPs_RDWR_B_LOW(devcDev);
        FDevcPs_initiateDma(devcDev, (u32)SourcePtr, (u32)DestPtr, SrcWordLength, DestWordLength);
   }

    return FMSH_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function returns the value of the specified configuration register or bitstream.
*
* @param	devcDev is devc handle.
* @param	DestinationDataPtr contains a pointer to the destination memory
*		    where the data is to be transferred to.
* @param	DestinationLength is the number of words (32 bit) to be transferred
*		    for the Destination transfer.
* @param	addr is the value of the specified configuration
*			register.
* @param	ConfigReg  is a constant which represents the configuration
*			register value to be returned.
*           if(ConfigReg == 0xaa55) radback bitstream
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note	None.
*
****************************************************************************/
u8 FDevcPs_getConfigdata(FDevcPs_T *devcDev, 
                           u32 *DestinationDataPtr, 
                           u32 DestinationLength, 
                           u32 addr, 
                           u32 ConfigReg)
{
    u32 len;
    u32 *p;
    u32 *src_addr = NULL;

    u32 reg = 0;
    FMSH_devc_portmap_TypeDef *portmap;

    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 

    src_addr = (u32*)malloc(500);
       
#ifdef FMSH_325T
        p = (u32*)src_addr;
        *p++ = 0x20000000;
        *p++ = 0x20000000;
        *p++ = 0x20000000;   //Type 1 Write 1 Word to CMD
        *p++ = 0x20000000;   //DESYNC Command
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p = 0x20000000;     //NOOP
        len = 12;
        FDevcPs_transfer(devcDev, (u8*)src_addr, len, (u8 *)FMSH_DMA_INVALID_ADDRESS, 0, FMSH_NON_SECURE_PCAP_WRITE_DUMMMY);
       
        /*for fmsh PL the dummy value must be set 3 for readback reg, but the value must be set 4 for readback bitstream*/
        if(ConfigReg == 0xaa55) 
            FDevcPs_readbackDummyCount(devcDev, dummy_3);
#endif  
                
    p = (u32*)src_addr;
    
    //Clear the interrupt sts bits
    reg = DEVC_IN32P(portmap->sac_int_sts); 
    FMSH_BIT_SET(reg, DEVC_INT_STATUS_dp_done_int, 1);
    FMSH_BIT_SET(reg, DEVC_INT_STATUS_dma_done_int, 1);
    FMSH_BIT_SET(reg, DEVC_INT_STATUS_pcfg_done_int, 1);
    DEVC_OUT32P(reg, portmap->sac_int_sts); 

	/* Check if DMA command queue is full */
    reg = DEVC_IN32P(portmap->sac_status); 
    if(FMSH_BIT_GET(reg, DEVC_STATUS_dma_busy) == 1)
            return 1;
        
	/*
	 * Register Readback in non secure mode
	 * Create the data to be written to read back the
	 * Configuration Registers from PL Region.
	 */
    
    if(ConfigReg == 0xaa55) 
    {	
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0x000000BB; 	/* Bus Width Sync Word */
        *p++ = 0x11220044; 	/* Bus Width Detect */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xAA995566; 	/* Sync Word */      
        *p++ = 0x02000000; 	/* Type 1 NOOP Word 0 */     
        //  *p++ = 0x30008001;  //Type 1 Write 1 Word to CMD
        //  *p++ = 0x0000000B;  //SHUTDOWN Command
        //  *p++ = 0x02000000;  //Type 1 NOOP Word 0            
        *p++ = 0x30008001;  //Type 1 Write 1 Word to CMD    
        *p++ = 0x00000007;  //RCRC Command
        *p++ = 0x20000000;  //Type 1 NOOP Word 0    
        *p++ = 0x20000000;  //Type 1 NOOP Word 0
        *p++ = 0x20000000;  //Type 1 NOOP Word 0
        *p++ = 0x20000000;  //Type 1 NOOP Word 0
        *p++ = 0x20000000;  //Type 1 NOOP Word 0
        *p++ = 0x20000000;  //Type 1 NOOP Word 0     
        *p++ = 0x30008001;  //Type 1 Write 1 Word to CMD
        *p++ = 0x00000004;  //RCFG Command
        *p++ = 0x20000000;  //Type 1 NOOP Word 0 
        *p++ = 0x30002001;  //Type 1 Write 1 Word to FAR
        *p++ = addr; 	//FAR Address = 00000000     
        *p++ = 0x28006000;  //Type 1 Read 0 Words from FDRO     
        *p++ = 0x48000000 | DestinationLength;  //Type 2 Read 202 Words from FDRO(for 7K325T)
        *p++ = 0x20000000;  //Type 1 NOOP Word 0
        *p++ = 0x20000000;  //Type 1 31 More NOOPs Word 0     
        *p++ = 0x20000000;
        *p++ = 0x20000000;
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000;
        *p++ = 0x20000000; 
        *p++ = 0x20000000;        
        *p++ = 0x20000000; 
        *p++ = 0x20000000;    
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000;        
        *p++ = 0x20000000; 
        *p++ = 0x20000000;    
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000;        
        *p++ = 0x20000000; 
        *p++ = 0x20000000;    
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p = 0x20000000; 
                
        len = 64;
    }
    else
    {
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */     
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */    
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */     
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */  
        *p++ = 0x000000BB; 	/* Bus Width Sync Word */
        *p++ = 0x11220044; 	/* Bus Width Detect */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xFFFFFFFF; 	/* Dummy Word */
        *p++ = 0xAA995566; 	/* Sync Word */
        *p++ = 0x20000000; 	/* Type 1 NOOP Word 0 */ 
        *p++ = 0x30008001; 
        *p++ = 0x00000007; 
        *p++ = FDevcPs_regAddr(addr,OPCODE_READ,0x1);    //addr
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000;
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000;
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p++ = 0x20000000;
        *p++ = 0x20000000; 
        *p++ = 0x20000000; 
        *p = 0x20000000; 

        len = 32;
    }

    devc_byte_swap((u32*)src_addr, len);
	FDevcPs_transfer(devcDev, (u8*)src_addr, len, (u8 *)DestinationDataPtr, DestinationLength, FMSH_PCAP_READBACK);

    FDevcPs_CSI_B_HIGH(devcDev);   

    if( ConfigReg == 0xaa55 )
    {	
        p = (u32*)src_addr;
        *p++ = 0x20000000;//Type 1 NOOP Word 0
        *p++ = 0x30008001;//Type 1 Write 1 Word to CMD
        *p++ = 0x0000000D;//START Command
        *p++ = 0x20000000;//Type 1 NOOP Word 0
        *p++ = 0x20000000;//Type 1 Write 1 Word to CMD
        *p++ = 0x20000000;//RCRC Command
        *p++ = 0x20000000;//Type 1 NOOP Word 0
        *p++ = 0x20000000;//Type 1 Write 1 Word to CMD
        *p++ = 0x20000000;//DESYNC Command
        *p++ = 0x20000000;//Type 1 NOOP Word 0
        *p = 0x20000000;  //Type 1 NOOP Word 0
        len = 11;    
    } 
    else 
    {
        p = (u32*)src_addr;
        *p++ = 0x20000000;
        *p++ = 0x20000000;
        *p++ = 0x30008001;   //Type 1 Write 1 Word to CMD
        *p++ = 0x0000000D;   //DESYNC Command
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p++ = 0x20000000;   //NOOP
        *p = 0x20000000;     //NOOP
        len = 12;
    }
    devc_byte_swap((u32*)src_addr, len);
	FDevcPs_transfer(devcDev, (u8*)src_addr, len, (u8 *)FMSH_DMA_INVALID_ADDRESS, 0, FMSH_NON_SECURE_PCAP_WRITE);

    FDevcPs_CSI_B_HIGH(devcDev);    
    FDevcPs_RDWR_B_HIGH(devcDev);   
    
    free(src_addr);
    src_addr = NULL;
    
    return 0;
}
/****************************************************************************/
/**
*
* This function starts the DMA transfer. This function only starts the
* operation and returns before the operation may be completed.
* If the interrupt is enabled, an interrupt will be generated when the
* operation is completed, otherwise it is necessary to poll the Status register
* to determine when it is completed. It is the responsibility of the caller to
* determine when the operation is completed by handling the generated interrupt
* or polling the Status Register.
*
* @param	devcDev is devc handle.
* @param	SourceDataPtr contains a pointer to the source memory where the data
*		is to be transferred from.
* @param	SourceLength is the number of words (32 bit) to be transferred
*		for the source transfer.
* @param	DestinationDataPtr contains a pointer to the destination memory
*		where the data is to be transferred to.
* @param	DestinationLength is the number of words (32 bit) to be transferred
*		for the Destination transfer.
* @param	SecureTransfer 
*           FMSH_NON_SECURE_PCAP_WRITE		0
*           FMSH_SECURE_PCAP_WRITE			1
*           FMSH_PCAP_READBACK			2
*
* @return              
*		- 0 if successful
*		- 1 if unsuccessful
*
* @note		None.
*
*****************************************************************************/
u8 FDevcPs_pcapLoadPartition(FDevcPs_T *devcDev, 
                               u32 *SourceDataPtr, 
                               u32 *DestinationDataPtr, 
                               u32 SourceLength, 
                               u32 DestinationLength, 
                               u32 SecureTransfer)
{
    u32 sts;
    u32 reg = 0;
    FMSH_devc_portmap_TypeDef *portmap;

    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 

    sts = FDevcPs_clearPcapStatus(devcDev);

    if(FMSH_SUCCESS != sts)
        return FMSH_FAILURE;

    sts = FDevcPs_transfer(devcDev, (u8*)SourceDataPtr,
    SourceLength,
    (u8 *)DestinationDataPtr,
    DestinationLength, SecureTransfer);
    

    if(FMSH_SUCCESS != sts)
        return FMSH_FAILURE;

    FDevcPs_CSI_B_HIGH(devcDev);     
    FDevcPs_RDWR_B_HIGH(devcDev);

    //Check for errors
    reg = DEVC_IN32P(portmap->sac_int_sts);
    if (reg & FMSH_DEVC_ERROR_FLAGS_MASK)
        return FMSH_FAILURE;

    return FMSH_SUCCESS;
}

u32 GcmDecTag(u8 Alg, const u8* In, u32 InByteLen, u32 KeySel, u8* Iv,const u8* Tag)
{
    u8  Status;
    u32 cnt; 
    
    u8 GenTag[SECURE_GCM_TAG_SIZE];
  
    FmshFsbl_SacInit(SAC_AES_EN_FLAG);
#ifdef FSBL_PL_DDR
    Status = AesSm4GenTag(Alg, In, InByteLen, KeySel, Iv, GenTag);
#else
    if(BootInstance.BootMode==NAND_FLASH ||  \
               BootInstance.BootMode==SD_CARD )    
    {    
       u8  tmpTag[16]={0};
       BootInstance.DeviceOps.DeviceCopy((u32)Tag,(u32)tmpTag,16 );
       Tag=tmpTag;
       Status =PlCalcTagNoLinear(Alg, In, InByteLen, KeySel, Iv, GenTag);
    }
#endif
    if(Status != FMSH_SUCCESS)
    {
      return Status;
    } 
    
    LOG_OUT (DEBUG_INFO, "Ciphertext GCM Tag is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, Tag, 16);
    LOG_OUT (DEBUG_INFO, "Calculated Ciphertext GCM Tag is :\r\n");
    PRINT_ARRAY (DEBUG_INFO, GenTag, 16);
     
    /*compare generated tag and received tag */
    for(cnt=0;cnt<SECURE_GCM_TAG_SIZE;cnt++)
    {  
        if (GenTag[cnt] != *Tag++)
        {
           
          LOG_OUT(DEBUG_INFO,"GCM Tag is not Matched \r\n");
          
          Status = GCM_ERROR;
           return Status;
        }
    }
    LOG_OUT(DEBUG_INFO,"GCM Tag is Matched, Starting Decryption...... \r\n");

    return Status;

}
/****************************************************************************/
/**
*
* This function will handle the AES/SM4-GCM Decryption.
*
* The Multiple key(a.k.a Key Rolling) or Single key
* Encrypted images will have the same format,
* such that it will have the following:
* 
* Secure head -->	Dummy AES/SM4 Key of 32byte +
* 					Block 0 IV of 12byte +
* 					DLC for Block 0 of 4byte +
* 					GCM tag of 16byte(Un-Enc).
* Block N --> Boot Image Data for Block N of n size +
* 			Block N+1 AES key of 32byte +
* 			Block N+1 IV of 12byte +
* 			GCM tag for Block N of 16byte(Un-Enc).
* 
* The Secure head and Block 0 will be decrypted using
* Device key or user provide key.
* If more than 1 blocks are found then the key and IV
* obtained from previous block will be used for decryption
* 
* 
* 1> Read the 1st 64bytes and decrypt 48 bytes using
* 	the selected Device key.
* 2> Decrypt the 0th block using the IV + Size from step 2
* 	and selected device key.
* 3> After decryption we will get decrypted data+KEY+IV+Blk
* 	Size so store the KEY/IV into KUP/IV registers.
* 4> Using Blk size, IV and Next Block Key information
* 	start decrypting the next block.
* 5> if the Current Image size > Total image length,
* 	go to next step 8. Else go back to step 5
* 6> If there are failures, return error code
* 7> If we have reached this step means the decryption is SUCCESS.
*
* @para£ºalg AES / SM4
* @para£ºno opkey / use opkey
* @para£ºbitlen -- bitsteam length
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/

u8 tmpPlBuffer[TMP_PL_BUF_LEN]={0};
u8 keyRolling_download(FDevcPs_T *devcDev, enum ALG alg, enum DECFLAG decflag, u32 srcPtr, u32 bitlen)
{
    u8 ret;
    u32 block_no;
    u32 head;
    u32 next_len;
    u32 current_totalLen;
    
    u8 Y1[16];
    
    u8* In=(u8*)(srcPtr);
    u8* Tag=(u8*)(srcPtr+48);
    u8 Iv[12];
    u8 Alg=0;
    
    FMSH_devc_portmap_TypeDef *portmap;
    
    portmap = (FMSH_devc_portmap_TypeDef *)devcDev->base_address; 
    
     if(alg==AES)
       Alg=ALG_AES;
     else
       Alg=ALG_SM4;
     
    /*pull down Prog_B, then pull up*/
    FDevcPs_Prog_B(devcDev);
    
    FDevcPs_smap32Swap(devcDev, smap32_swap_enable);
    FDevcPs_secDownDataByteSwap(devcDev, byte_swap);
    FDevcPs_txDataSwap(devcDev, byte_swap); 
    FDevcPs_rxDataSwap(devcDev, byte_swap);
    
    FDevcPs_RDWR_B_LOW(devcDev);
    FDevcPs_CSI_B_LOW(devcDev);

    /*Verify Secure Header Tag*/
    FmshFsbl_SacInit(SAC_AES_EN_FLAG);
    FDevcPs_AES_KEY_SOURCE(devcDev,DEV_KEY);
    FDevcPs_AES_KEY_IV_LOAD(devcDev);

    /*Geg IV from SAC IV register*/
    GetSacIv(Iv);
    ret=GcmDecTag(Alg,In,48,SECURE_CSU_AES_KEY_SRC_DEV,Iv,Tag);
    if(ret != FMSH_SUCCESS)
    {
       return ret;
    } 
    
    /*GCM_EN = 0*/
    FDevcPs_AES_CR_AESENDisable(devcDev);
    FDevcPs_downloadMode(devcDev, SECURE_DOWNLOAD_BITSTREAM);
    FDevcPs_AES_CR_CHMOD(devcDev, CTR);
    FDevcPs_AES_CR_ALG_SEL(devcDev, alg);
    FDevcPs_AES_CR_MODE(devcDev, DCODE);
    FDevcPs_AES_KEY_SOURCE(devcDev, DEV_KEY);
    IvToY1(Iv,Y1);
    SetSacIv(Y1);   
    DEVC_OUT32P(0x2, portmap->IVUP0);  
    FDevcPs_AES_KEY_IV_LOAD(devcDev);
    
    /*set dec flag = secure head,only iv_write_en,no use op_key*/
    FDevcPs_AES_DEC_FLAG(devcDev, decflag);
   
    /*GCM_EN = 1*/
    FDevcPs_AES_CR_AESENEnable(devcDev);
    /***********************HEADER********************************/
    head = 0;

    //head is 12 word
#ifdef FSBL_PL_DDR
    FDevcPs_initiateDma(devcDev, (u32)srcPtr, PCAP_WR_DATA_ADDR, 12, 12);
#else
    if(BootInstance.BootMode==NAND_FLASH ||  \
               BootInstance.BootMode==SD_CARD )    
    { 
        BootInstance.DeviceOps.DeviceCopy((u32)srcPtr,(u32)tmpPlBuffer,48);
        FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR, 12, 12);
    }
    else
        FDevcPs_initiateDma(devcDev, (u32)srcPtr, PCAP_WR_DATA_ADDR, 12, 12);
#endif       
    
    /*GCM_EN = 0*/
    FDevcPs_AES_CR_AESENDisable(devcDev);
    /***********************BLOCK0 - N********************************/ 
    block_no = 0;
    head += 16 * 4;
    current_totalLen = 16;
    next_len =  DEVC_IN32P(portmap->IVUP0); 

    devc_byte_swap((u32*)&next_len, 1);//get next block length
        
    while((current_totalLen < bitlen) && (next_len != 0)) 
    {
        FDevcPs_AES_DEC_FLAG(devcDev, clear);                 
        if(block_no == 0) 
        {
            /*Verify Tag 0*/ 
            FmshFsbl_SacInit(SAC_AES_EN_FLAG);
            GetSacIv(Iv);
            In=(u8*)(srcPtr + head);
            Tag=(u8*)(srcPtr + head + next_len*4 + 48);
            ret=GcmDecTag(Alg,In,next_len*4+48 ,SECURE_CSU_AES_KEY_SRC_DEV,Iv,Tag);
            if(ret != FMSH_SUCCESS)
            {
                return ret;
            }  
            /*GCM_EN = 0*/
            FDevcPs_AES_CR_AESENDisable(devcDev);
            FDevcPs_downloadMode(devcDev, SECURE_DOWNLOAD_BITSTREAM);
            FDevcPs_AES_CR_CHMOD(devcDev, CTR);
            FDevcPs_AES_CR_ALG_SEL(devcDev, alg);
            FDevcPs_AES_CR_MODE(devcDev, DCODE);
            
            /***********************BLOCK0********************************/
            //set key_src=DEV 4'b0001
            FDevcPs_AES_KEY_SOURCE(devcDev, DEV_KEY);
            IvToY1(Iv,Y1);
            SetSacIv(Y1);
            //load the key and iv                         
            FDevcPs_AES_KEY_IV_LOAD(devcDev);  
            //set dec flag = blk0
            FDevcPs_AES_DEC_FLAG(devcDev, ivup_kup_wr_en);   
            //set en=1
            FDevcPs_AES_CR_AESENEnable(devcDev);  
        
#ifdef FSBL_PL_DDR
            FDevcPs_initiateDma(devcDev, (u32)srcPtr + head, PCAP_WR_DATA_ADDR, next_len + 12, next_len + 12);
#else 
            if(BootInstance.BootMode==NAND_FLASH  ||  \
                  BootInstance.BootMode==SD_CARD )    
            { 
              u32 i=0;
              FDevcPs_AES_DEC_FLAG(devcDev, clear);   
              for(i=0;i<next_len*4/TMP_PL_BUF_LEN;i++)
              {
                  BootInstance.DeviceOps.DeviceCopy((u32)srcPtr+head+TMP_PL_BUF_LEN*i,(u32)tmpPlBuffer,TMP_PL_BUF_LEN );
                  FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR,TMP_PL_BUF_LEN/4, TMP_PL_BUF_LEN/4);
              }
              if(next_len*4%TMP_PL_BUF_LEN!=0)
              {
                  BootInstance.DeviceOps.DeviceCopy((u32)srcPtr+head+TMP_PL_BUF_LEN*i,(u32)tmpPlBuffer,next_len*4%TMP_PL_BUF_LEN );
                  FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR,next_len%(TMP_PL_BUF_LEN/4), next_len%(TMP_PL_BUF_LEN/4));
              }
               //KEY & IV
              BootInstance.DeviceOps.DeviceCopy((u32)srcPtr+head+next_len*4,(u32)tmpPlBuffer,48);
              FDevcPs_AES_DEC_FLAG(devcDev, ivup_kup_wr_en);  
              FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR,12, 12);
            }
            else
              FDevcPs_initiateDma(devcDev, (u32)srcPtr + head, PCAP_WR_DATA_ADDR, next_len + 12, next_len + 12);
#endif            
            //set en=0
            FDevcPs_AES_CR_AESENDisable(devcDev);        
        }
        else 
        {
            /*Verify Tag */ 
           FmshFsbl_SacInit(SAC_AES_EN_FLAG);
           GetSacIv(Iv);
           In=(u8*)(srcPtr + head);
           Tag=(u8*)(srcPtr + head + next_len*4 + 48);

            ret=GcmDecTag(Alg,In,next_len*4+48 ,SECURE_CSU_AES_KEY_SRC_KUP,Iv,Tag);
            //ret=AesSm4GcmDec(Alg,AES_KUP_IV_WRITE_FLAG,In,next_len*4+48 ,Out,SECURE_CSU_AES_KEY_SRC_DEV,Iv,Tag);
            if(ret != FMSH_SUCCESS)
            {
                return ret;
            }  
            /*GCM_EN = 0*/
            FDevcPs_AES_CR_AESENDisable(devcDev);
            FDevcPs_downloadMode(devcDev, SECURE_DOWNLOAD_BITSTREAM);
            FDevcPs_AES_CR_CHMOD(devcDev, CTR);
            FDevcPs_AES_CR_ALG_SEL(devcDev, alg);
            FDevcPs_AES_CR_MODE(devcDev, DCODE);
            
            /***********************BLOCK0********************************/
            //set key_src=DEV 4'b0001
            FDevcPs_AES_KEY_SOURCE(devcDev, KUP);   
            IvToY1(Iv,Y1);
            SetSacIv(Y1);
            
            //load the key and iv                         
            FDevcPs_AES_KEY_IV_LOAD(devcDev);  
            //set dec flag = blk0
            FDevcPs_AES_DEC_FLAG(devcDev, ivup_kup_wr_en);   
            /*GCM_EN = 1*/
            FDevcPs_AES_CR_AESENEnable(devcDev);  
            
            /***********************BLOCK1-N********************************/
            //set key_src=DEV 4'b0001
            //FDevcPs_AES_KEY_SOURCE(devcDev, KUP);   
            
            //DEVC_OUT32P(0x2, portmap->IVUP0);         
            //load the key and iv                         
            //FDevcPs_AES_KEY_IV_LOAD(devcDev);
            
            //set en=1
           //FDevcPs_AES_CR_AESENEnable(devcDev);
            //set dec flag = blk1
           //FDevcPs_AES_DEC_FLAG(devcDev, ivup_kup_wr_en);  
           
#ifdef FSBL_PL_DDR
             FDevcPs_initiateDma(devcDev, (u32)srcPtr + head, PCAP_WR_DATA_ADDR, next_len + 12, next_len + 12);
#else
            if(BootInstance.BootMode==NAND_FLASH  ||  \
                 BootInstance.BootMode==SD_CARD )    
            { 
              u32 j=0;
              FDevcPs_AES_DEC_FLAG(devcDev, clear);   
              for(j=0;j<next_len*4/TMP_PL_BUF_LEN;j++)
              {
                  BootInstance.DeviceOps.DeviceCopy((u32)srcPtr+head+TMP_PL_BUF_LEN*j,(u32)tmpPlBuffer,TMP_PL_BUF_LEN );
                  FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR,TMP_PL_BUF_LEN/4, TMP_PL_BUF_LEN/4);
              }
              if(next_len*4%TMP_PL_BUF_LEN!=0)
              {
                  BootInstance.DeviceOps.DeviceCopy((u32)srcPtr+head+TMP_PL_BUF_LEN*j,(u32)tmpPlBuffer,next_len*4%TMP_PL_BUF_LEN );
                  FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR,next_len%(TMP_PL_BUF_LEN/4), next_len%(TMP_PL_BUF_LEN/4));   
              }
              //key & IV
              BootInstance.DeviceOps.DeviceCopy((u32)srcPtr+head+next_len*4,(u32)tmpPlBuffer,48 );
              FDevcPs_AES_DEC_FLAG(devcDev, ivup_kup_wr_en);
              FDevcPs_initiateDma(devcDev, (u32)tmpPlBuffer, PCAP_WR_DATA_ADDR,12, 12);
            }
            else
              FDevcPs_initiateDma(devcDev, (u32)srcPtr + head, PCAP_WR_DATA_ADDR, next_len + 12, next_len + 12);
#endif   
            //set en=0
            FDevcPs_AES_CR_AESENDisable(devcDev);   
        }
        block_no++;
        
        head += (next_len + 16) * 4;
        current_totalLen += (next_len + 16);
                        
        next_len =  DEVC_IN32P(portmap->IVUP0);
        devc_byte_swap((u32*)&next_len, 1);//get next block length
    }
    
    FDevcPs_AES_DEC_FLAG(devcDev, clear);/*clear DEC flag*/
    
    ret = FDevcPs_pollFpgaDone(devcDev, DEVC_POLL_DONE_MS);
           
    return ret;
}

/****************************************************************************/
/**
*
* This function is used to secure download different BITSTREAM
*
* @param   alg_flag£º 0 -- AES 1 -- SM4
* @param   opkey_flag£º 0 -- no opkey 1 -- use opkey
* @param   bitlen: PL BITSTREAM LENGTH
*
* @return   0 if successful, otherwise 1.
*
* @note     None.
*
****************************************************************************/
u8 FDevcPs_keyRollingDownload(FDevcPs_T *dev, u8 alg_flag, u8 opkey_flag, u32 srcPtr, u32 bitlen)
{
    u8 ret = 0;

    if((alg_flag == 0) && (opkey_flag == 0))
        ret |= keyRolling_download(dev, AES, no_opkey, srcPtr, bitlen);
    else if((alg_flag == 0) && (opkey_flag == 1)) 
        ret |= keyRolling_download(dev, AES, use_opkey, srcPtr, bitlen);
    else if((alg_flag == 1) && (opkey_flag == 0)) 
        ret |= keyRolling_download(dev, SM4, no_opkey, srcPtr, bitlen);
    else if((alg_flag == 1) && (opkey_flag == 1)) 
        ret |= keyRolling_download(dev, SM4, use_opkey, srcPtr, bitlen);
    else
        return 1;
    
    return ret;        
}

u8 FDevcPs_devcInitDownload()
{
    u8 ret=FMSH_SUCCESS;
    FDevcPs_T devcDev;
    FDevcPs_init(&devcDev, 0xE0040000);    
    ret=FDevcPs_Prog_B(&devcDev);
    return ret;
}

u8 FDevcPs_download_non_secure(u32 srcAddress,u32 len)
{
    u8 ret=FMSH_SUCCESS;
    FDevcPs_T devcDev;
    u8 *srcPtr;
    srcPtr = (u8*)srcAddress;
    FDevcPs_init(&devcDev, 0xE0040000);
    
    FDevcPs_readFifoThre(&devcDev, (enum readFifoThre)0);
    FDevcPs_fabricInit(&devcDev, FMSH_NON_SECURE_PCAP_WRITE);
     
#ifdef	FSBL_PL_DDR 
    ret=FDevcPs_pcapLoadPartition(&devcDev, (u32*)srcPtr, (u32*)PCAP_WR_DATA_ADDR, 
                              len, len, DOWNLOAD_BITSTREAM);
    if(FMSH_SUCCESS!=ret)
    {
        return ret;
    }
#else
    u32 i=0;
     if(BootInstance.BootMode==NAND_FLASH  ||  \
               BootInstance.BootMode==SD_CARD )     
     {
           for(i=0;i<len*4/TMP_PL_BUF_LEN;i++)
           {
               BootInstance.DeviceOps.DeviceCopy((u32)srcAddress+TMP_PL_BUF_LEN*i,(u32)tmpPlBuffer,TMP_PL_BUF_LEN );
               ret=FDevcPs_pcapLoadPartition(&devcDev, (u32*)tmpPlBuffer, (u32*)PCAP_WR_DATA_ADDR, 
                              TMP_PL_BUF_LEN/4, TMP_PL_BUF_LEN/4, DOWNLOAD_BITSTREAM);
               if(FMSH_SUCCESS!=ret)
               {
                    return ret;
               }
           }
           if(len*4%TMP_PL_BUF_LEN!=0)
           {
               BootInstance.DeviceOps.DeviceCopy((u32)srcAddress+TMP_PL_BUF_LEN*i,(u32)tmpPlBuffer,TMP_PL_BUF_LEN );
               ret=FDevcPs_pcapLoadPartition(&devcDev, (u32*)tmpPlBuffer, (u32*)PCAP_WR_DATA_ADDR, 
                              len%(TMP_PL_BUF_LEN/4), len%(TMP_PL_BUF_LEN/4), DOWNLOAD_BITSTREAM);
               if(FMSH_SUCCESS!=ret)
               {
                  return ret;
               }
           }
     }
     else
     {
        ret=FDevcPs_pcapLoadPartition(&devcDev, (u32*)srcPtr, (u32*)PCAP_WR_DATA_ADDR, 
                              len, len, DOWNLOAD_BITSTREAM);
        if(FMSH_SUCCESS!=ret)
        {
            return ret;
        }
     }
#endif    

    
    ret=FDevcPs_pollFpgaDone(&devcDev, DEVC_POLL_DONE_MS); 
    if(FMSH_SUCCESS!=ret)
    {
       return ret;
    }
    return FMSH_SUCCESS;
}

//#define TEST
u8 FDevcPs_encryptDownload_AES_NoOp(u32* devc_iv, u32 srcPtr,u32 bitlen)
{
    u8 ret=FMSH_SUCCESS;
    FDevcPs_T devcDev;
    
    FDevcPs_init(&devcDev, 0xE0040000);
    FDevcPs_IV(&devcDev, devc_iv, 3);
    ret=keyRolling_download(&devcDev, AES, no_opkey, srcPtr, bitlen);
    return ret;
}

u8 FDevcPs_encryptDownload_AES_UseOp(u32* devc_iv, u32 srcPtr,u32 bitlen)
{
    u8 ret=FMSH_SUCCESS;
    FDevcPs_T devcDev;
    
    FDevcPs_init(&devcDev, 0xE0040000);
    FDevcPs_IV(&devcDev, devc_iv, 3);
    ret=keyRolling_download(&devcDev, AES, use_opkey, srcPtr, bitlen);
    return ret;
}

u8 FDevcPs_encryptDownload_SM4_NoOp(u32* devc_iv, u32 srcPtr,u32 bitlen)
{
    u8 ret=FMSH_SUCCESS;
    FDevcPs_T devcDev;
           
    FDevcPs_init(&devcDev, 0xE0040000);
    FDevcPs_IV(&devcDev, devc_iv, 3);
    ret=keyRolling_download(&devcDev, SM4, no_opkey, srcPtr, bitlen); 
    return ret;
}

u8 FDevcPs_encryptDownload_SM4_UseOp(u32* devc_iv, u32 srcPtr,u32 bitlen)
{
    u8 ret=FMSH_SUCCESS;
    FDevcPs_T devcDev;
           
    FDevcPs_init(&devcDev, 0xE0040000);
    FDevcPs_IV(&devcDev, devc_iv, 3);
    ret=keyRolling_download(&devcDev, SM4, use_opkey, srcPtr, bitlen);
    return ret;
}


