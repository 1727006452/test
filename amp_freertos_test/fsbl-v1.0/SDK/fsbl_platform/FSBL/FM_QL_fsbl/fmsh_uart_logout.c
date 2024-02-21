#include "boot_main.h"

FUartPs_T g_UART;

u8 g_UartTXBuffer[1000];

u8 FmshFsbl_UartInit()
{ 
    u8 ret=FMSH_SUCCESS;
#ifdef STDOUT_BASEADDRESS
    /*Initialize UARTs and set baud rate*/
    FUartPs_Config *Config=NULL;
    
    /*Initialize UARTs and set baud rate*/
    Config= FUartPs_LookupConfig(STDOUT_BASEADDRESS==FPS_UART0_BASEADDR?FPAR_UARTPS_0_DEVICE_ID:FPAR_UARTPS_1_DEVICE_ID);
    if(Config==NULL)
      return FMSH_FAILURE;
    ret=FUartPs_init(&g_UART, Config);
    if(ret!=FMSH_SUCCESS)
      return ret;
    
    FUartPs_setBaudRate(&g_UART, STDOUT_BASEADDRESS==FPS_UART0_BASEADDR?FPAR_UARTPS_0_BAUDRATE : FPAR_UARTPS_1_BAUDRATE);
    FUartPs_setDataBits(&g_UART, Uart_eight_bits);
    FUartPs_setParity(&g_UART, Uart_no_parity);
    FUartPs_setStick(&g_UART, Uart_Stick_disable);
    FUartPs_setStopBits(&g_UART, Uart_one_stop_bit);
  
    FUartPs_read(&g_UART);
    FUartPs_enableFifos(&g_UART);
#endif
    return ret;
}

static void SendData(FUartPs_T *dev,char *UartBuffer,u32 NumBytes)
{
        u16 i;

        for(i = 0;i < NumBytes; i++) {
                FUartPs_write(dev,*UartBuffer);
                while((FUartPs_getLineStatus(dev) & Uart_line_thre) != Uart_line_thre);
                        UartBuffer++;
        }
}

u32 uart_printf(char *fmt,...)                               
{
        u32 Status = FMSH_SUCCESS;
#ifdef STDOUT_BASEADDRESS 
	va_list ap;
	char string[256];
	va_start(ap,fmt);
	vsnprintf(string,256,fmt,ap);
	SendData(&g_UART,string,strlen(string));  	
  	va_end(ap);
#endif
  	return Status;
}

char* FmshFsbl_GetErrorInfo(unsigned key) 
{
  char* err_msg = "";
  switch (key) {
    case ERROR_UNAVAILABLE_CPU:
      err_msg = "Invalid cpu ProcessorID!\r\n"; break;
    case PARTITION_SIGNATURE_VERIFY_FAILED:
      err_msg = "Partition signature verify failed!\r\n"; break; 
    case INVALID_BOOT_MODE:
      err_msg = "Invalid boot mode!\r\n"; break;
    case INVALID_ID:
      err_msg = "Invalid cpu ID!\r\n"; break;
    case SECURITY_VIOLATION:
      err_msg = "Security violation!\r\n"; break;
    case NO_VALID_BOOT_IMG_HEADER:
      err_msg = "No find valid BOOT_IMG header!\r\n"; break;
    case IMG_HEADER_CHECKSUM_ERROR:
      err_msg = "Img header checksum error!\r\n"; break;
      
    case PPK_HASH_MISMATCH:
      err_msg = "PPK hash mismatch!\r\n"; break;
    case SPK_ID_MISMATCH:
      err_msg = "SPK ID mismatch!\r\n"; break;
    case SPK_SIGNATURE_VERIFY_FAILED:
      err_msg = "SPK signature verify failed!\r\n"; break;
    case BOOT_HEADER_SIGNATURE_VERIFY_FAILED:
      err_msg = "Boot header signature verify failed!\r\n"; break;
    case ERROR_PH_CHECKSUM_FAILED:
      err_msg = "Partition Header checksum verify failed!\r\n"; break;
    case ERROR_NOT_PARTITION_OWNER:
      err_msg = "Partition is not belong to FSBL!\r\n"; break;
    case ERROR_XIP_AUTH_ENC_PRESENT:
      err_msg = "Encrypted Img runs in XIP mode!\r\n"; break;
    case ERROR_APU_XIP_EXCUTION_ADDRESS:
      err_msg = "XIP excution address is error!\r\n"; break;
    case MISMATCH_FSBL_LENGTH:
      err_msg = "Partition length mismatch!\r\n"; break;
    case PARTITION_AUTHENTICATE_FAILURE:
      err_msg = "Partition authenticate failed!\r\n"; break;
    case ERROR_INVALID_EXCUTION_ADDRESS:
      err_msg = "Excution address is error!\r\n"; break;
    case DECYPTION_FAILURE:
      err_msg = "Decyption failed!\r\n"; break;
    case PARTITION_CHECKSUM_ERROR:
      err_msg = "Partition checksum is error!\r\n"; break;  
    case SECURE_BOOT_FORCE_NOT_MATCH:
      err_msg = "Boot force mismatch!\r\n"; break;  
    default:
      err_msg = "Undefined error status"; break;
  }
  return err_msg;
}