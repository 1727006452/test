 
#ifndef _FMSH_SPIPS_SW_H_	/* prevent circular inclusions */
#define _FMSH_SPIPS_SW_H_	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
    
/**********************************Include File*********************************/

/**********************************Constant Definition**************************/

/**********************************Type Definition******************************/

/**********************************Macro (inline function) Definition***********/

/**********************************Variable Definition**************************/

/**********************************Function Prototype***************************/
int FSpiPs_Initialize(FSpiPs_T* spi, u16 deviceId);
int FSpiPs_Initialize_Master(FSpiPs_T* spi);
int FSpiPs_Initialize_Slave(FSpiPs_T* spi);
int FSpiPs_Transfer(FSpiPs_T* spi, u8* sendBuffer, u8* recvBuffer, u32 byteCount);
int FSpiPs_PolledTransfer(FSpiPs_T* spi, u8* sendBuffer, u8* recvBuffer, u32 byteCount);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* prevent circular inclusions */