#include "phy.h"

u32 g_link_speed_neg_en=1;  //auto neg enable flag

u32 g_GmacLinkStatus=0;  //0-down 1-up


static u32 g_GmacLinkStatus_Old=0;  //0-down 1-up

static const char *Speed_Desr[3]=
{
	"10M",
	"100M",
	"1000M"
};

static const char *Duplex_Desr[2] = 
{
	"half duplex",
	"full duplex"
};

/* PC��ǿ���л����ʺ�ģʽ���������м�̬����Ҫ��ʱ����һ�£�
 * 1.main��ѭ����μ�⣬��demo����ʽ
 * 2.�жϴ��������ñ�־�������Ҫ��עlink�仯�ж��Ƿ���Զ�ν��룬
 * ��������һ��1�붨ʱ����1�������ʱ����
 */
void FGmacPs_GmacLink_Updata(FGmacPs_Instance_T * pGmac)
{
	u32 LinkSpeed = 0;
	u32 LinkSpeed_Old;
	u32 ClkRate;
	u32 Reg_Ctrl;
	u32 DuplMode = 0;
	u32 DuplMode_Old;
	u32 BaseAddress=(u32)pGmac->base_address;
	FGmacPs_LinkStatus_T *pGmacLinkSts=NULL;

	pGmacLinkSts=pGmac->gmac_link_status;
	LinkSpeed_Old = pGmacLinkSts->link_speed;
	DuplMode_Old = pGmacLinkSts->link_mod;
	
	BaseAddress = (u32)(pGmac->base_address);
	
	FGmac_Ps_GetLinkStatus(pGmac);
	pGmacLinkSts=pGmac->gmac_link_status;

	if((LinkSpeed_Old == pGmacLinkSts->link_speed) && (DuplMode_Old == pGmacLinkSts->link_mod) && 
		(g_GmacLinkStatus_Old ==  pGmacLinkSts->link_status))
	{
		return;
	}

	g_GmacLinkStatus = pGmacLinkSts->link_status;
	g_GmacLinkStatus_Old = pGmacLinkSts->link_status;
	
	//printf("get gamc LinkSpeed 0x%x,DuplMode 0x%x\r\n",pGmacLinkSts->link_speed,pGmacLinkSts->link_mod);

	LinkSpeed = pGmacLinkSts->link_speed;
	/* it anded with GMAC_MII_STS_LNKMOD, it will be 0 or 1 */
	DuplMode = pGmacLinkSts->link_mod;
	
	g_GmacLinkStatus = pGmacLinkSts->link_status;
	
	Reg_Ctrl = FMSH_ReadReg(BaseAddress,0);
	if(DuplMode == 1) //full
	{
		Reg_Ctrl |= GMAC_MCR_DM;
	}
	else		//half
	{
		Reg_Ctrl &= ~GMAC_MCR_DM;
	}
	switch (LinkSpeed) {
	case speed_1000:
		ClkRate = 125 * 1000 * 1000;
		Reg_Ctrl &= ~GMAC_MCR_PS;
		FMSH_WriteReg(BaseAddress,0,Reg_Ctrl);
		break;
	case speed_100:
		ClkRate = 25 * 1000 * 1000;
		Reg_Ctrl |= GMAC_MCR_FES | GMAC_MCR_PS;
		FMSH_WriteReg(BaseAddress,0,Reg_Ctrl);
		break;
	case speed_10:
		ClkRate = 25 * 100 * 1000;
		Reg_Ctrl &= ~GMAC_MCR_FES;
		Reg_Ctrl |= GMAC_MCR_PS;
		FMSH_WriteReg(BaseAddress,0,Reg_Ctrl);
		break;	
	default:
		ClkRate = 25 * 1000 * 1000;
		break;
	}
	//printf("get ClkRate %d,Reg_Ctrl 0x%x\r\n",ClkRate,Reg_Ctrl);
    printf("LinkSpeed=%d, DuplMode=%d\r\n", LinkSpeed, DuplMode);
	printf("speed:%s %s\r\n", Speed_Desr[LinkSpeed], Duplex_Desr[DuplMode]);
	FGmac_Ps_Set_Gem_Rate(pGmac,ClkRate);
	return;
}

