/*
     IAR startup file for FMQL7000 Series PS.
 */
 /*****************************************************************************/
/**
*
* @file init.s
*
* This is the startup file which contains code for the FSBL.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.0   kwb   20/11/13 Change macro definition "__ARM_NEON__" to "__ARMVFP__".
* 1.1   kwb   20/03/02 Change CSTACK from " 0x1000 bytes of SYS stack" to " End of SYS stack".
* </pre>
*
* @note
*
******************************************************************************/

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(2)
        SECTION FIQ_STACK:DATA:NOROOT(2)
        SECTION UND_STACK:DATA:NOROOT(2)
        SECTION ABT_STACK:DATA:NOROOT(2)
        SECTION CSTACK:DATA:NOROOT(3)

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#define __ASSEMBLY__

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#define AIC         0xFC06E000
#define AIC_IVR     0x10
#define AIC_EOICR   0x38
#define L2CC_CR     0x00A00100

#define REG_SFR_AICREDIR        0xF8028054
#define REG_SFR_UID             0xF8028050   
#define AICREDIR_KEY            0x5F67B102


MODE_MSK DEFINE 0x1F            ; Bit mask for mode bits in CPSR
#define ARM_MODE_ABT     0x17
#define ARM_MODE_FIQ     0x11
#define ARM_MODE_IRQ     0x12
#define ARM_MODE_SVC     0x13
#define ARM_MODE_SYS     0x1F
#define ARM_MODE_UND     0x1B
#define I_BIT            0x80
#define F_BIT            0x40


//------------------------------------------------------------------------------
//         Startup routine
//------------------------------------------------------------------------------

/*
   Exception vectors
 */
        SECTION .vectors:CODE:NOROOT(2)

        PUBLIC  resetVector
        PUBLIC  IRQ_Handler
        PUBLIC  FIQ_Handler
        EXTERN  Undefined_C_Handler
        PUBLIC  SVCHandler
        EXTERN  Prefetch_C_Handler
        EXTERN  Abort_C_Handler
        EXTERN  SWInterrupt
        EXTERN  FIQInterrupt
        EXTERN  IRQInterrupt
        ARM

__iar_init$$done:               ; The interrupt vector is not needed
                                ; until after copy initialization is done

resetVector:
        ; All default exception handlers (except reset) are
        ; defined as weak symbol definitions.
        ; If a handler is defined by the application it will take precedence.
        LDR     pc, =resetHandler        ; Reset
        LDR     pc, Undefined_Addr       ; Undefined instructions
        LDR     pc, SWI_Addr             ; Software interrupt (SWI/SYS)
        LDR     pc, Prefetch_Addr        ; Prefetch abort
        LDR     pc, Abort_Addr          ; Data abort
        B       .                        ; RESERVED
        LDR     PC,IRQ_Addr              ; 0x18 IRQ
        LDR     PC,FIQ_Addr              ; 0x1c FIQ

Undefined_Addr: DCD   Undefined_C_Handler
SWI_Addr:       DCD   SVCHandler
Abort_Addr:     DCD   Abort_C_Handler
Prefetch_Addr:  DCD   Prefetch_C_Handler
IRQ_Addr:       DCD   IRQ_Handler
FIQ_Addr:       DCD   FIQ_Handler

/*
   Handles incoming interrupt requests by branching to the corresponding
   handler, as defined in the AIC. Supports interrupt nesting.
  
 */
//
IRQ_Handler: 					/* IRQ vector handler */

	stmdb	sp!,{r0-r3,r12,lr}		/* state save from compiled code*/
#ifdef __ARMVFP__
	vpush {d0-d7}
	vpush {d16-d31}
	vmrs r1, FPSCR
	push {r1}
	vmrs r1, FPEXC
	push {r1}
#endif

#ifdef PROFILING
	ldr	r2, =prof_pc
	subs	r3, lr, #0
	str	r3, [r2]
#endif

	bl	IRQInterrupt			/* IRQ vector */

#ifdef __ARMVFP__
	pop 	{r1}
	vmsr    FPEXC, r1
	pop 	{r1}
	vmsr    FPSCR, r1
	vpop    {d16-d31}
	vpop    {d0-d7}
#endif
	ldmia	sp!,{r0-r3,r12,lr}		/* state restore from compiled code */


	subs	pc, lr, #4			/* adjust return */


FIQ_Handler:					/* FIQ vector handler */
	stmdb	sp!,{r0-r3,r12,lr}		/* state save from compiled code */
#ifdef __ARMVFP__
	vpush {d0-d7}
	vpush {d16-d31}
	vmrs r1, FPSCR
	push {r1}
	vmrs r1, FPEXC
	push {r1}
#endif

FIQ:
	bl	FIQInterrupt			/* FIQ vector */

#ifdef __ARMVFP__
	pop 	{r1}
	vmsr    FPEXC, r1
	pop 	{r1}
	vmsr    FPSCR, r1
	vpop    {d16-d31}
	vpop    {d0-d7}
#endif
	ldmia	sp!,{r0-r3,r12,lr}		/* state restore from compiled code */
	subs	pc, lr, #4			/* adjust return */
       

SVCHandler:					/* SWI handler */
	stmdb	sp!,{r0-r3,r12,lr}		/* state save from compiled code */

	tst	r0, #0x20			/* check the T bit */
	ldrneh	r0, [lr,#-2]			/* Thumb mode */
	bicne	r0, r0, #0xff00			/* Thumb mode */
	ldreq	r0, [lr,#-4]			/* ARM mode */
	biceq	r0, r0, #0xff000000		/* ARM mode */

	bl	SWInterrupt			/* SWInterrupt: call C function here */

	ldmia	sp!,{r0-r3,r12,lr}		/* state restore from compiled code */

	movs	pc, lr		/*return to the next instruction after the SWI instruction */



/*
   After a reset, execution starts here, the mode is ARM, supervisor
   with interrupts disabled.
   Initializes the chip and branches to the main() function.
 */
        SECTION .cstartup:CODE:NOROOT(2)

        PUBLIC  resetHandler
     
        EXTERN  ?main
        REQUIRE resetVector
       
        ARM

resetHandler: 
        mov r0,  #0
        mov r1,  r0
        mov r2,  r0
        mov r3,  r0
        mov r4,  r0
        mov r5,  r0
        mov r6,  r0
        mov r7,  r0
        mov r8,  r0
        mov r9,  r0
        mov r10,  r0
        mov r11,  r0
        mov r12,  r0

////////Disable all USER_LVL_SHFTRs//////////
        ldr r0,=0xE0040018
        ldr r1,[r0]
       
        and	r1, r1, #0x0200
        cmp	r1, #0
        bne	Skip0 
        
        ldr r0,=0xE0026008
        ldr r1,=0xDF0D767B
        str r1,[r0]
        
        ldr r0,=0xE0026838
        mov r1,#0
        str r1,[r0]
        
        ldr r0,=0xE0026004
        ldr r1,=0xDF0D767B
        str r1,[r0]
 /////////////////////////////////////////////  

Skip0:  		
		ldr r0,=resetVector 
		mcr p15,0,r0,c12,c0,0 ; VBAR
		
		mcr p15,0,r0,c12,c0,1 ; MVBAR
		
		;mcr p15,4,r0,c12,c0,0 ; HVBAR
		
		mov r0, #0
		
        mrc	p15,0,r1,c0,c0,5
		and	r1, r1, #0xf
		cmp	r1, #0
	
	beq	OKToRun
EndlessLoop0:
	wfe
	b	EndlessLoop0
OKToRun:     
        LDR     r4, =SFE(CSTACK)     ; End of SVC stack
        BIC     r4,r4,#0x7           ; Make sure SP is 8 aligned       
        MOV     sp, r4
                        

        ;; Set up the normal interrupt stack pointer.

        MSR     CPSR_c, #(ARM_MODE_IRQ | F_BIT | I_BIT)
        LDR     sp, =SFE(IRQ_STACK)     ; End of IRQ_STACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned
        
        
        ;; Set up the fast interrupt stack pointer.

        MSR     CPSR_c, #(ARM_MODE_FIQ | F_BIT | I_BIT)
        LDR     sp, =SFE(FIQ_STACK)     ; End of FIQ_STACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned
        
        MSR     CPSR_c, #(ARM_MODE_ABT | F_BIT | I_BIT)
        LDR     sp, =SFE(ABT_STACK)     ; End of ABT_STACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned

        MSR     CPSR_c, #(ARM_MODE_UND | F_BIT | I_BIT)
        LDR     sp, =SFE(UND_STACK)     ; End of UND_STACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned
        
        MSR     CPSR_c, #(ARM_MODE_SYS | F_BIT | I_BIT)
        LDR     sp, =SFE(CSTACK)        ; End of SYS stack
        //LDR     sp, =SFE(CSTACK-0x3000) ; 0x1000 bytes of SYS stack
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned
        
        MSR     CPSR_c, #(ARM_MODE_SVC )
    //    MSR     CPSR_c, #(ARM_MODE_SVC | F_BIT | I_BIT)
        
        CPSIE   A
        
        /* Enable VFP */
        /* - Enable access to CP10 and CP11 in CP15.CACR */
        MRC     p15, 0, r0, c1, c0, 2
        ORR     r0, r0, #0xf00000
        MCR     p15, 0, r0, c1, c0, 2
        /* - Enable access to CP10 and CP11 in CP15.NSACR */
        /* - Set FPEXC.EN (B30) */
#ifdef __ARMVFP__        
        MOV     r3, #0x40000000 
        VMSR    FPEXC, r3
#endif        
                
	/* Write to ACTLR */
	mrc	p15, 0, r0, c1, c0, 1		/* Read ACTLR*/
	orr	r0, r0, #(0x01 << 6)		/* set SMP bit */
	//orr	r0, r0, #(0x01 )		/* Cache/TLB maintenance broadcast */
	mcr	p15, 0, r0, c1, c0, 1		/* Write ACTLR*/        
                
                
        /* Branch to () */
        LDR     r0, =?main
        BLX     r0

        /* Loop indefinitely when program is finished */

       loop4:
        B       loop4


;------------------------------------------------------------------------------
;- Function             : FIQ_Handler
;- Treatments           : FIQ Controller Interrupt Handler.
;- Called Functions     : AIC_IVR[interrupt]
;------------------------------------------------------------------------------

        
 END
