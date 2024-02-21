#include <stdio.h>
#include "fmsh_common.h"
#include "fmsh_gic.h"
#include "exception_handler.h"

static const char* abort_status[][2]=
{
  // IFSR status        ,       DFSR status
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//0
  {"Unknown(reserved status)",                          "Alignment Fault"                               },//1
  {"Debug Event",                                       "Debug Event"                                   },//2
  {"Access flag - section",                             "Access flag - section"                         },//3
  {"Unknown(reserved status)",                          "Instruction cache maintenance"                 },//4
  {"Translation fault - section",                       "Translation fault - section"                   },//5
  {"Access flag - Page",                                "Access flag - Page"                            },//6
  {"Translation fault -Page",                           "Translation fault -Page"                       },//7
  {"Synchronous external abort",                        "Synchronous external abort, nontranslation"    },//8
  {"Domain fault - Section",                            "Domain fault - Section"                        },//9
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//10
  {"Domain fault - Page",                               "Domain fault - Page"                           },//11
  {"Synchronous external abort - L1 Translation",       "Synchronous external abort - L1 Translation"   },//12
  {"Permission fault - Section",                        "Permission fault - Section"                    },//13
  {"Synchronous external abort - L2 Translation",       "Synchronous external abort - L2 Translation"   },//14
  {"Permission fault - Page",                           "Permission fault - Page"                       },//15
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//16
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//17
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//18
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//19
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//20
  {"Unknown(reserved status)",                          "Unknown(reserved status)"                      },//21
  {"Unknown(reserved status)",                          "Asynchronous external abort"}
  
};
  
//typedef  unsigned int uint32_t;



/****************************************************************************/
/**
* The function is a common API used to initialize exception handlers across all
* processors supported. The exception handlers are being
* initialized statically and hence this function does not do anything.
* However, it is still present to avoid any compilation issues in case an
* application uses this API and also to take care of backward compatibility
* issues (in earlier versions of BSPs, this API was being used to initialize
* exception handlers).
*
* @param	None.
*
* @return	None.
*
* @note		None.
*****************************************************************************/
void FMSH_ExceptionInit(void)
{
	return;
}

/*****************************************************************************/
/**
*
* Makes the connection between the Id of the exception source and the
* associated Handler that is to run when the exception is recognized. The
* argument provided in this call as the Data is used as the argument
* for the Handler when it is called.
*
* @param	exception_id contains the ID of the exception source and should
*		be in the range of 0 to FMSH_EXCEPTION_ID_LAST.
		See FMSH_exception_l.h for further information.
* @param	Handler to the Handler for that exception.
* @param	Data is a reference to Data that will be passed to the
*		Handler when it gets called.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/

static void FMSH_ExceptionNullHandler(void *Data);
static void FMSH_ExceptionNullHandler(void *Data)
{
	(void *)Data;
DieLoop: goto DieLoop;
}

typedef struct {
	FMSH_ExceptionHandler Handler;
	void *Data;
} XExc_VectorTableEntry;

XExc_VectorTableEntry XExc_VectorTable[FMSH_EXCEPTION_ID_LAST + 1] =
{
	{FMSH_ExceptionNullHandler, NULL},
	{FMSH_ExceptionNullHandler, NULL},
	{FMSH_ExceptionNullHandler, NULL},
        {FMSH_ExceptionNullHandler, NULL},
	{FMSH_ExceptionNullHandler, NULL},
	//{FMSH_PrefetchAbortHandler, NULL},
	//{FMSH_DataAbortHandler, NULL},
	{FMSH_ExceptionNullHandler, NULL},
	{FMSH_ExceptionNullHandler, NULL},
};


void FMSH_ExceptionRegisterHandler(u32 Exception_id,
				    FMSH_ExceptionHandler Handler,
				    void *Data)
{
	XExc_VectorTable[Exception_id].Handler = Handler;
	XExc_VectorTable[Exception_id].Data = Data;
}
/************************************************************************/
void Abort_C_Handler( void)
{
        uint32_t v1,v2, dfsr;
        v1= 0;
        v2= 0;
        asm("mrc   p15, 0, %0, c5, c0, 0" : : "r"(v1));
        asm("mrc   p15, 0, %0, c6, c0, 0" : : "r"(v2));

        dfsr = ((v1 >> 4) & 0x0F);
        printf("\n\r######################################################################\n\r");
        printf("Data Abort occured in %x domain\n\r", (unsigned int)dfsr);
        dfsr = (((v1 & 0x400) >> 6) | (v1 & 0x0F));
        printf("Data abort fault reason is: %s\n\r", (char*)abort_status[dfsr][1]);
        printf("Data fault occured at Address = 0x%08x\n\n\r",(unsigned int)v2); 


        printf("-[Info]-Data fault status register value = 0x%x\n\r",(unsigned int)v1);

        while(1);
    
}


void Prefetch_C_Handler( void)
{
        uint32_t v1,v2, ifsr;
        v1= 0;
        v2= 0;

        asm("mrc   p15, 0, %0, c5, c0, 1" : : "r"(v1));
        asm("mrc   p15, 0, %0, c6, c0, 2" : : "r"(v2));

        ifsr = (((v1 & 0x400) >> 6) | (v1 & 0x0F));
        printf("\n\r######################################################################\n\r");
        printf("Instruction prefetch abort reason is: %s\n\r", (char*)abort_status[ifsr][0]);
        printf("Instruction prefetch Fault occured at Address = 0x%08x\n\n\r",(unsigned int)v2); 

        printf("-[INFO]- Prefetch Fault status register value by = 0x%x\n\r",(unsigned int)v1);

        while(1);
    
}

void  SWInterrupt(void)
{
        printf("SWI occured !!! \n\r");
        while(1);
}

void Undefined_C_Handler( void)
{
        printf("Undefined abort \n\r");
}

void * InstancePtr;
/*
* This is the C level wrapper for the FIQ interrupt called from the vectors.s
* file.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//extern FGicPs IntcInstance;
void IRQInterrupt(void)
{
    //FGicPs_InterruptHandler(&IntcInstance);
    XExc_VectorTable[FMSH_EXCEPTION_ID_IRQ_INT].Handler(XExc_VectorTable[
					FMSH_EXCEPTION_ID_IRQ_INT].Data);
}

/*****************************************************************************/
/**
*
* This is the C level wrapper for the IRQ interrupt called from the vectors.s
* file.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void FIQInterrupt(void)
{ 
  
    XExc_VectorTable[FMSH_EXCEPTION_ID_FIQ_INT].Handler(XExc_VectorTable[
					FMSH_EXCEPTION_ID_FIQ_INT].Data);
     
}





