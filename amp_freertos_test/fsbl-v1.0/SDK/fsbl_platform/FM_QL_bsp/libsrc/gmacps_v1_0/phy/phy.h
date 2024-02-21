#ifndef  __GMAC_PHY___
#define  __GMAC_PHY___

#include "fmsh_gmac_hw.h"
#include "fmsh_gmac_lib.h"
#include "fmsh_ps_parameters.h"
#include <stdio.h>

//#include "fmsh_print.h"

//#define printf    fmsh_print

extern u32 g_link_speed_neg_en;
extern u32 g_GmacLinkStatus;

void FGmacPs_GmacLink_Updata(FGmacPs_Instance_T * pGmac);


#endif