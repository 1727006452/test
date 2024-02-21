
/*****************************************************************************/
/**
*
* @file fmsh_exit.s
*
* This is the main file which contains exit code for the FSBL.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  lq   11/23/19 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/
        PUBLIC   FMSH_EXIT
        SECTION .FMSH_EXIT:CODE:NOROOT(2)

FMSH_EXIT:

  mov	 lr, r0	/* move the destination address into link register */
  push {lr}
  
  mcr	 p15,0,r0,c7,c5,0	      ; Invalidate Instruction cache 
  mcr	 p15,0,r0,c7,c5,6	      ; Invalidate branch predictor array 
  dsb
  isb				      ; make sure it completes

  MRC     p15, 0, r0, c1, c0, 0       ; Read System Control Register
  BIC     r0, r0, #(0x1 << 12)        ; Clear I bit 12 to disable I Cache
  BIC     r0, r0, #(0x1 <<  2)        ; Clear C bit  2 to disable D Cache
  BIC     r0, r0, #0x1                ; Clear M bit  0 to disable MMU
  MCR     p15, 0, r0, c1, c0, 0       ; Write System Control Register
  isb				      ; make sure it completes 

  sev
  
  cmp r1,#0
  beq FmshFsbl_Loop

  pop	{pc}
  
FmshFsbl_Loop:
	wfe				/* wait for event */
	b FmshFsbl_Loop  
  END
