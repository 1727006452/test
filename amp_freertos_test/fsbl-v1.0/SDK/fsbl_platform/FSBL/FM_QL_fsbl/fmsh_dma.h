
#ifndef _FMSH_DMA_H_
#define _FMSH_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "fmsh_dmac_config.h"
#include "fmsh_dmac_lib.h"
#include "string.h"
#include "stdio.h"
  
// Declare global variables
extern FDmaPs_T g_DMA_dmac;
extern FDmaPs_Param_T g_DMA_param;
extern FDmaPs_Instance_T g_DMA_instance;
extern volatile BOOL s_DMA_tfrFlag;

// define globle list
extern FMSH_listHead g_DMA_list;

u8 FmshFsbl_DmaInit(void);
s32 FDmaPs_dmacConfigure(FDmaPs_T *dmac);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of protection macro */
