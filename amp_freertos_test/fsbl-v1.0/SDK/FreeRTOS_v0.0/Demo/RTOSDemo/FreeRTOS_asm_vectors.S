/******************************************************************************
*
* (c) Copyright 2009-13 FMSH, Inc. All rights reserved.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file asm_vectors.s
*
* This file contains the initial vector table for the Cortex A7 processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 1.00a liulei 11/26/19 Initial version
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

// .org 0
// .text
// .arm

		EXTERN  resetVector
		PUBLIC  _freertos_vector_table

		EXTERN  FIQInterrupt
		EXTERN  Abort_C_Handler
		EXTERN  Prefetch_C_Handler
		PUBLIC  vPortInstallFreeRTOSVectorTable
		EXTERN  FreeRTOS_IRQ_Handler
		EXTERN  FreeRTOS_SWI_Handler

		SECTION .freertos_vectors:CODE:NOROOT(2)
_freertos_vector_table:
	B	  resetVector
	B	  FreeRTOS_Undefined
	ldr   pc, _swi
	B	  FreeRTOS_PrefetchAbortHandler
	B	  FreeRTOS_DataAbortHandler
	NOP	  /* Placeholder for address exception vector*/
	LDR   PC, _irq
	B	  FreeRTOS_FIQHandler

_irq:   DCD FreeRTOS_IRQ_Handler
_swi:   DCD FreeRTOS_SWI_Handler


FreeRTOS_FIQHandler:			/* FIQ vector handler */
	stmdb	sp!,{r0-r3,r12,lr}	/* state save from compiled code */
FIQLoop:
	blx	FIQInterrupt			/* FIQ vector */
	ldmia	sp!,{r0-r3,r12,lr}	/* state restore from compiled code */
	subs	pc, lr, #4			/* adjust return */

FreeRTOS_Undefined:				/* Undefined handler */
	b		.

FreeRTOS_DataAbortHandler:		/* Data Abort handler */
	stmdb	sp!,{r0-r3,r12,lr}	/* state save from compiled code */
	blx	Abort_C_Handler		/*DataAbortInterrupt :call C function here */
	ldmia	sp!,{r0-r3,r12,lr}	/* state restore from compiled code */
	subs	pc, lr, #4			/* adjust return */

FreeRTOS_PrefetchAbortHandler:	/* Prefetch Abort handler */
	stmdb	sp!,{r0-r3,r12,lr}	/* state save from compiled code */
	blx	Prefetch_C_Handler	/* PrefetchAbortInterrupt: call C function here */
	ldmia	sp!,{r0-r3,r12,lr}	/* state restore from compiled code */
	subs	pc, lr, #4			/* adjust return */

vPortInstallFreeRTOSVectorTable:

	/* Set VBAR to the vector table that contains the FreeRTOS handlers. */
	ldr	r0, =_freertos_vector_table
	mcr	p15, 0, r0, c12, c0, 0
	dsb
	isb
	bx lr


	END
