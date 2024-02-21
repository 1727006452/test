
#include "fmsh_spips_lib.h" 

/*****************************************************************************
* This function sets spi device work in master/slave
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
void FSpiPs_Mst(FSpiPs_T *spi)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_MSTR_OFFSET, 0x1);
    spi->isMaster = TRUE;
}

void FSpiPs_Slv(FSpiPs_T *spi)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_MSTR_OFFSET, 0x0);
    spi->isMaster = FALSE;
}

/*****************************************************************************
* This function enables/disables spi device
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
void FSpiPs_Enable(FSpiPs_T* spi)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_SSIENR_OFFSET, 0x1);
    spi->isEnable = TRUE;
}

void FSpiPs_Disable(FSpiPs_T* spi)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_SSIENR_OFFSET, 0x0);
    spi->isEnable = FALSE;
}

/*****************************************************************************
* This function sets spi device CPOL/CPHA with 4 clock modes. It can only be set 
* when device is disabled.
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SetSckMode(FSpiPs_T* spi, u32 sckMode)
{
    u32 configReg;
    
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }
    if(sckMode > 3)
    {
        return FMSH_FAILURE;
    }
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET);
    configReg &= ~(SPIPS_CTRL0_SCPH_MASK | SPIPS_CTRL0_SCPOL_MASK);
    configReg |= (sckMode << SPIPS_CTRL0_SCPH_SHIFT);
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET, configReg); 
    
    return FMSH_SUCCESS;
}

/*****************************************************************************
* This function sets spi device transfer modes. It can only be set 
* when device is disabled. tmod 0 is transfer, tmod 1 is transmit only
* tmod 2 is receive only, tmod 3 is eeprom mode.
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SetTMod(FSpiPs_T* spi, u32 tmod)
{
    u32 configReg;
    
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }    
    if(tmod > 3)
    {
        return FMSH_FAILURE;
    }
    
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET);
    configReg &= ~SPIPS_CTRL0_TMOD_MASK;
    configReg |= (tmod << SPIPS_CTRL0_TMOD_SHIFT);
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET, configReg); 
    
    return FMSH_SUCCESS;
}

int FSpiPs_SetSlvOut(FSpiPs_T* spi, BOOL enable)
{
    u32 configReg;
    
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }    
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET);
    if(enable) 
    {
        configReg &= ~SPIPS_CTRL0_SLVOE_MASK;
    }
    else 
    {
        configReg |= SPIPS_CTRL0_SLVOE_MASK;
    }
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET, configReg);  
    
    return FMSH_SUCCESS;
}

int FSpiPs_SetLoopBack(FSpiPs_T* spi, BOOL enable)
{
    u32 configReg;
    
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }    
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET);
    if(enable)  
    {
        configReg |= SPIPS_CTRL0_SRL_MASK;
    }
    else  
    {
        configReg &= ~SPIPS_CTRL0_SRL_MASK;
    }
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET, configReg); 
    
    return FMSH_SUCCESS;
}

int FSpiPs_SetDFS32(FSpiPs_T* spi, u32 dfs32)
{
    u32 configReg;
    
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }
    if(dfs32<4 || dfs32 > 0x20)
    {
        return FMSH_FAILURE;
    }
    
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET);
    configReg &= ~SPIPS_CTRL0_DFS32_MASK;
    configReg |= ((dfs32-1) << SPIPS_CTRL0_DFS32_SHIFT);
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_CTRLR0_OFFSET, configReg); 
    
    spi->frameSize = dfs32;
    
    return FMSH_SUCCESS;
}

/*****************************************************************************
* This function sets receivedata frame number. 
* it can only be set when device is disabled and device works in master
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SetDFNum(FSpiPs_T* spi, u32 dfNum)
{
    if(spi->isEnable == TRUE || spi->isMaster == FALSE)
    {
        return FMSH_FAILURE;
    }    
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_CTRLR1_OFFSET, dfNum-1);
    
    return FMSH_SUCCESS;
}

/*****************************************************************************
* This function sets slave select. it can only be set when device works in master
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SetSlave(FSpiPs_T* spi, u32 slaveNo)
{    
    if(spi->isMaster == FALSE)
    {
        return FMSH_FAILURE;
    }
    if(slaveNo == 0)
    {
        FMSH_WriteReg(spi->config.baseAddress, SPIPS_SER_OFFSET, 0x0); 
    }
    else
    {
        FMSH_WriteReg(spi->config.baseAddress, SPIPS_SER_OFFSET, 0x1 << (slaveNo - 1)); 
    }
    
    return FMSH_SUCCESS;
}

/*****************************************************************************
* This function sets baud rate. it can only be set when device is disabled and device 
* works in master
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SetSckDv(FSpiPs_T* spi, u32 sckdv)
{
    if(spi->isEnable == TRUE || spi->isMaster == FALSE)
    {
        return FMSH_FAILURE;
    }    
    
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_BAUDR_OFFSET, sckdv);  
    
    return FMSH_SUCCESS;
}

/*****************************************************************************
* This function sets fifo level. it can only be set when device is disabled
*
* @param
*
* @return
*		- FMSH_SUCCESS if spi device exists.
*		- FMSH_FAILURE if spi device not exists.
*
* @note		
*
******************************************************************************/
int FSpiPs_SetTxEmptyLvl(FSpiPs_T* spi, u8 tlvl)
{
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }    
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_TXFTLR_OFFSET, tlvl);
    
    return FMSH_SUCCESS;
}

u32 FSpiPs_GetTxLevel(FSpiPs_T* spi)
{
    return FMSH_ReadReg(spi->config.baseAddress, SPIPS_TXFLR_OFFSET);
}

int FSpiPs_SetRxFullLvl(FSpiPs_T* spi, u8 tlvl)
{
    if(spi->isEnable == TRUE)
    {
        return FMSH_FAILURE;
    }    
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_RXFTLR_OFFSET, tlvl);
    
    return FMSH_SUCCESS;
}

u32 FSpiPs_GetRxLevel(FSpiPs_T* spi)
{
    return FMSH_ReadReg(spi->config.baseAddress, SPIPS_RXFLR_OFFSET);
}

/*****************************************************************************
* This function enables/disables interrupt mask
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
void FSpiPs_EnableIntr( FSpiPs_T* spi, u32 mask)
{    
    u32 configReg;
    
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_IMR_OFFSET);
    configReg |= mask;
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_IMR_OFFSET, configReg);
}

void FSpiPs_DisableIntr( FSpiPs_T* spi, u32 mask)
{
    u32 configReg;
    
    configReg = FMSH_ReadReg(spi->config.baseAddress, SPIPS_IMR_OFFSET);
    configReg &= ~mask;
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_IMR_OFFSET, configReg);
}

void FSpiPs_ClearIntrStatus(FSpiPs_T* spi)
{    
    FMSH_ReadReg(spi->config.baseAddress, SPIPS_ICR_OFFSET);
}

/*****************************************************************************
* This function sets DMA trigger level
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
void FSpiPs_SetDMATLvl(FSpiPs_T* spi, u32 tlvl)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_DMATDLR_OFFSET, tlvl);
}

void FSpiPs_SetDMARLvl(FSpiPs_T* spi, u32 tlvl)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_DMARDLR_OFFSET, tlvl);
}

void FSpiPs_EnableDMATx(FSpiPs_T* spi)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_DMACR_OFFSET, 0x2);
}

void FSpiPs_EnableDMARx(FSpiPs_T* spi)
{
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_DMACR_OFFSET, 0x1);
}

void FSpiPs_DisableDMA(FSpiPs_T* spi)
{    
    FMSH_WriteReg(spi->config.baseAddress, SPIPS_DMACR_OFFSET, 0);
}

/*****************************************************************************
* This function gets status register value.
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
u32 FSpiPs_GetStatus(FSpiPs_T* spi)
{  
    return FMSH_ReadReg(spi->config.baseAddress, SPIPS_SR_OFFSET);
}

/*****************************************************************************
* This function receives data from register(FIFO) if it is not empty. exit if 
* timeover
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
u32 FSpiPs_Recv(FSpiPs_T* spi )
{
    u32 count = 0;
    u8 status;
    
    status = FMSH_ReadReg(spi->config.baseAddress, SPIPS_SR_OFFSET); 
    while((status & SPIPS_SR_RFNE) == 0)  /* loop if RX fifo empty */
    {
        delay_us(1);
        count++;
        if(count > 10000)
        {
            break;
        }        
        status = FMSH_ReadReg(spi->config.baseAddress, SPIPS_SR_OFFSET); 
    }      

    return FMSH_ReadReg(spi->config.baseAddress, SPIPS_DR_OFFSET); 
}

/*****************************************************************************
* This function sends data to register(FIFO) if it is not full. exit if 
* timeover
*
* @param
*
* @return
*
* @note		
*
******************************************************************************/
void FSpiPs_Send(FSpiPs_T* spi, u32 Data )
{
    u32 count = 0;
    u8 status;
    
    status = FMSH_ReadReg(spi->config.baseAddress, SPIPS_SR_OFFSET); 
    while((status & SPIPS_SR_TFNF) == 0)   /* loop if TX fifo full */
    {
        delay_us(1);
        count++;
        if(count > 10000)
        {
            break;
        }
        status = FMSH_ReadReg(spi->config.baseAddress, SPIPS_SR_OFFSET); 
    }

    FMSH_WriteReg(spi->config.baseAddress, SPIPS_DR_OFFSET, Data);  
}
