/******************************************************************************
******************************************************************************/
/*****************************************************************************/
/**
*
* @file fmsh_testmem.h
*
* This file contains utility functions to test memory.
*
* <b>Memory test description</b>
*
* A subset of the memory tests can be selected or all of the tests can be run
* in order. If there is an error detected by a subtest, the test stops and the
* failure code is returned. Further tests are not run even if all of the tests
* are selected.
*
* Subtest descriptions:
* <pre>
* FMSH_TESTMEM_ALLMEMTESTS:
*       Runs all of the following tests
*
* FMSH_TESTMEM_INCREMENT:
*       Incrementing Value Test.
*       This test starts at 'FMSH_TESTMEM_INIT_VALUE' and uses the
*	incrementing value as the test value for memory.
*
* FMSH_TESTMEM_WALKONES:
*       Walking Ones Test.
*       This test uses a walking '1' as the test value for memory.
*       location 1 = 0x00000001
*       location 2 = 0x00000002
*       ...
*
* FMSH_TESTMEM_WALKZEROS:
*       Walking Zero's Test.
*       This test uses the inverse value of the walking ones test
*       as the test value for memory.
*       location 1 = 0xFFFFFFFE
*       location 2 = 0xFFFFFFFD
*       ...
*
* FMSH_TESTMEM_INVERSEADDR:
*       Inverse Address Test.
*       This test uses the inverse of the address of the location under test
*       as the test value for memory.
*
* FMSH_TESTMEM_FIXEDPATTERN:
*       Fixed Pattern Test.
*       This test uses the provided patters as the test value for memory.
*       If zero is provided as the pattern the test uses '0xDEADBEEF".
* </pre>
*
* <i>WARNING</i>
*
* The tests are <b>DESTRUCTIVE</b>. Run before any initialized memory spaces
* have been set up.
*
* The address provided to the memory tests is not checked for
* validity except for the NULL case. It is possible to provide a code-space
* pointer for this test to start with and ultimately destroy executable code
* causing random failures.
*
* @note
*
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** width,
* the patterns used in FMSH_TESTMEM_WALKONES and FMSH_TESTMEM_WALKZEROS will
* repeat on a boundry of a power of two making it more difficult to detect
* addressing errors. The FMSH_TESTMEM_INCREMENT and FMSH_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver    Who    Date    Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a lxl  07/01/19 First release
* </pre>
*
******************************************************************************/

#ifndef _FMSH_TESTMEM_H	/* prevent circular inclusions */
#define _FMSH_TESTMEM_H	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
 #include "fmsh_common.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/

/* xutil_memtest defines */

#define FMSH_TESTMEM_INIT_VALUE	1U

/** @name Memory subtests
 * @{
 */
/**
 * See the detailed description of the subtests in the file description.
 */
#define FMSH_TESTMEM_ALLMEMTESTS     0x00U
#define FMSH_TESTMEM_INCREMENT       0x01U
#define FMSH_TESTMEM_WALKONES        0x02U
#define FMSH_TESTMEM_WALKZEROS       0x03U
#define FMSH_TESTMEM_INVERSEADDR     0x04U
#define FMSH_TESTMEM_FIXEDPATTERN    0x05U
#define FMSH_TESTMEM_MAXTEST         FMSH_TESTMEM_FIXEDPATTERN
/* @} */

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/* xutil_testmem prototypes */

extern s32 Fmsh_TestMem32(u32 *Addr, u32 Words, u32 Pattern, u8 Subtest);
extern s32 Fmsh_TestMem16(u16 *Addr, u32 Words, u16 Pattern, u8 Subtest);
extern s32 Fmsh_TestMem8(u8 *Addr, u32 Words, u8 Pattern, u8 Subtest);

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
