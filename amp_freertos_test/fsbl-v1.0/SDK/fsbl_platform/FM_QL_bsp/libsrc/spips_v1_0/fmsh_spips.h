
#ifndef _FMSH_SPIPS_H_	    /* prevent circular inclusions */ 
#define _FMSH_SPIPS_H_	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif
    
/***************************** Include Files *********************************/ 

/************************** Constant Definitions *****************************/ 
#ifndef FPAR_SPIPS_0_DEVICE_ID    
    #define FPS_SPI0_DEVICE_ID          (0)   
#else
    #define FPS_SPI0_DEVICE_ID          FPAR_SPIPS_0_DEVICE_ID
#endif

#ifndef  FPS_SPI0_BASEADDR  
    #define FPS_SPI0_BASEADDR           (0xe0001000)
#endif
    
#ifndef FPAR_SPIPS_1_DEVICE_ID    
    #define FPS_SPI1_DEVICE_ID          (1)        
#else
    #define FPS_SPI1_DEVICE_ID          FPAR_SPIPS_1_DEVICE_ID  
#endif

#ifndef  FPS_SPI1_BASEADDR  
    #define FPS_SPI1_BASEADDR           (0xe0021000)
#endif
    
#define FPS_SPI_NUM_INSTANCES       (2)

#define SPIPS_TRANSFER_DONE         (0x80)
    
/**************************** Type Definitions *******************************/   
    
typedef void (*FSpiPs_StatusHandler) (void *CallBackRef, u32 StatusEvent,
                                      u32 ByteCount);

typedef struct {
    u16 deviceId;		            /**< Unique ID of device */
    u32 baseAddress;                /**< APB Base address of the device */
} FSpiPs_Config_T;

typedef struct FSpiPs_Tag{
    FSpiPs_Config_T config;         /**< Configuration structure */
    
    u32 flag;
    BOOL isEnable;   
    BOOL isBusy;
    BOOL isMaster;                  /**< Master/Slave */  
    u8 frameSize;
    
    u32 totalBytes;
    u32 remainingBytes;
    u32 requestedBytes;	 
    u8* sendBufferPtr;	            /**< Buffer to send (state) */
    u8* recvBufferPtr;	            /**< Buffer to receive (state) */
                   
    int (*Transfer)(struct FSpiPs_Tag spi, void* sendBuffer, void* recvBuffer, u32 byteCount);
    
    FSpiPs_StatusHandler statusHandler;
    void* statusRef;
} FSpiPs_T;

/**********************************Variable Definition**************************/

/**********************************Function Prototype***************************/

FSpiPs_Config_T* FSpiPs_LookupConfig(u16 deviceId);
int FSpiPs_CfgInitialize(FSpiPs_T* spi, FSpiPs_Config_T* ConfigPtr);
void FSpiPs_Reset(FSpiPs_T* spi);
int FSpiPs_SelfTest(FSpiPs_T* spi);
void FSpiPs_SetStatusHandler(FSpiPs_T* spi, void* callBackRef, FSpiPs_StatusHandler funcPtr);
void FSpiPs_InterruptHandler(void* instancePtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* prevent circular inclusions */