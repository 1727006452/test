/******************************************************************************
*
* Copyright (C) 2018 - 2028 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file  fmsh_common.c
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
* 0.01   yl  12/20/2018  First Release
*</pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "fmsh_common.h"
#include "stdlib.h"
#include "fmsh_ps_parameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

#ifndef FMSH_NASSERT
void onAssert__(char const *file, unsigned line)
{
	PRINTF("[ERR]: Error Report: %s, line: %u\r\n", file, line);
	abort();
}
#endif

/*bit_no 1-32 
*set the 0 to bit_no bits 1
*/
u32 mask_generate(u32 bit_no)
{
    u32 i, temp = 0;
    for(i = 0; i< bit_no; i++)
    {
        temp |= (0x01 << i);
    }
    return temp;
}    

void global_timer_enable()
{
 	FMSH_WriteReg(FPS_GTC_BASEADDR, 0, 1);
}

void global_timer_disable()
{
	FMSH_WriteReg(FPS_GTC_BASEADDR, 0, 0);
}

u64 get_current_time()   
{
        u32 low;
        u32 high;
        u64 ret;

        /* Reading Global Timer Counter Register */
        do
        {
                high = FMSH_ReadReg(FPS_GTC_BASEADDR, 0xc);
                low = FMSH_ReadReg(FPS_GTC_BASEADDR, 0x8);
        } while(FMSH_ReadReg(FPS_GTC_BASEADDR, 0xc) != high);

	ret = (((u64) high) << 32U) | (u64) low;
        
        return ret;
}

void delay_ms(double time_ms)
{
        double counts;
        u64 tCur;
        u64 tEnd;
        
        counts = time_ms * 1000 * GTC_CLK_FREQ;
  
  	global_timer_enable();
  	tCur = get_current_time();
        
        while(1)
  	{
		tEnd = get_current_time();
		if((tEnd - tCur) > (u64)counts)
	  	break;
  	}
}

void delay_us(u32 time_us)
{
        u64 counts;
        u64 tCur;
        u64 tEnd;
        
        counts = time_us * GTC_CLK_FREQ;

        global_timer_enable();
        tCur = get_current_time();
        
        while(1)
        {
                tEnd = get_current_time();
                if((tEnd - tCur) > counts)
                break;
        }
}

void delay_1ms()
{
        double counts;
        u64 tCur;
        u64 tEnd;
        
        counts = 1000 * GTC_CLK_FREQ;
  
  	global_timer_enable();
  	tCur = get_current_time();
        
        while(1)
  	{
		tEnd = get_current_time();
		if((tEnd - tCur) > counts)
	  	break;
  	}
}

void delay_1us()
{
        double counts;
        u64 tCur;
        u64 tEnd;
        
        counts = GTC_CLK_FREQ;

        global_timer_enable();
        tCur = get_current_time();
        
        while(1)
        {
                tEnd = get_current_time();
                if((tEnd - tCur) > counts)
                break;
        }
}