/******************************************************************************
*
* (c) Copyright 2009-13 FMSH, Inc. All rights reserved.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file cache.c
*
* This file contains the cache routes for the processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 1.00a liulei 12/06/19 Initial version
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#include <stdlib.h>
#include "cache.h"
#include "stdint.h"
#include "intrinsics.h"
#include "fmsh_common.h"

/*
 * CR1 bits (CP#15 CR1)
 */
#define CR_M    (1 << 0)    /* MMU enable               */
#define CR_A    (1 << 1)    /* Alignment abort enable       */
#define CR_C    (1 << 2)    /* Dcache enable            */
#define CR_W    (1 << 3)    /* Write buffer enable          */
#define CR_P    (1 << 4)    /* 32-bit exception handler     */
#define CR_D    (1 << 5)    /* 32-bit data address range        */
#define CR_L    (1 << 6)    /* Implementation defined       */
#define CR_B    (1 << 7)    /* Big endian               */
#define CR_S    (1 << 8)    /* System MMU protection        */
#define CR_R    (1 << 9)    /* ROM MMU protection           */
#define CR_F    (1 << 10)   /* Implementation defined       */
#define CR_Z    (1 << 11)   /* Implementation defined       */
#define CR_I    (1 << 12)   /* Icache enable            */
#define CR_V    (1 << 13)   /* Vectors relocated to 0xffff0000  */
#define CR_RR   (1 << 14)   /* Round Robin cache replacement    */
#define CR_L4   (1 << 15)   /* LDR pc can set T bit         */
#define CR_DT   (1 << 16)
#define CR_IT   (1 << 18)
#define CR_ST   (1 << 19)
#define CR_FI   (1 << 21)   /* Fast interrupt (lower latency mode)  */
#define CR_U    (1 << 22)   /* Unaligned access operation       */
#define CR_XP   (1 << 23)   /* Extended page tables         */
#define CR_VE   (1 << 24)   /* Vectored interrupts          */
#define CR_EE   (1 << 25)   /* Exception (Big) Endian       */
#define CR_TRE  (1 << 28)   /* TEX remap enable         */
#define CR_AFE  (1 << 29)   /* Access flag enable           */
#define CR_TE   (1 << 30)   /* Thumb exception enable       */

#define IRQ_FIQ_MASK 0xC0U  /* Mask IRQ and FIQ interrupts in cpsr */

#ifdef __GNUC__

#define isb()  asm volatile( "isb" )
#define dsb()  asm volatile( "dsb" ::: "memory" )
#define dmb()  asm volatile( "dmb" )
#define asm  asm

#elif defined (__ICCARM__)

#define isb()  __ISB()
#define dsb()  __DSB()
#define dmb()  __DMB()
#define asm  __asm

#endif

#define CONFIG_SYS_CACHELINE_SIZE  (64)  // unit: Byte

extern uint32_t MMUTable;

static inline unsigned int get_cr(void)
{
    unsigned int val;

    asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r" (val)
                              :
                              : "cc");
    return val;
}

static inline void set_cr(unsigned int val)
{
    asm volatile("mcr p15, 0, %0, c1, c0, 0" :
                              : "r" (val)
                              : "cc");
    isb();
}

static inline unsigned int get_cpsr(void)
{
    unsigned int val;

    asm volatile("mrs  %0, cpsr" : "=r" (val));

    return val;
}

static inline void set_cpsr(unsigned int val)
{
    asm volatile("msr  cpsr_cxsf, %0" : : "r" (val));
}

static inline uint32_t get_ccsidr(void)
{
    uint32_t ccsidr;

    /* Read current CP15 Cache Size ID Register */
    asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
    return ccsidr;
}

static inline uint32_t get_clidr(void)
{
    uint32_t clidr;

    asm volatile ("mrc p15, 1, %0, c0, c0, 1" : "=r"(clidr)); // read clidr
    return clidr;
}

static inline void set_csselr(uint32_t csselr)
{
    asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (csselr));
}

// clean & invalidate by set/way
static inline void v7_clean_invalidate_by_set_way(uint32_t way_set_level)
{
    asm volatile ("mcr p15, 0, %0, c7, c14, 2" : : "r" (way_set_level));
}

// invalidate by set/way
static inline void v7_invalidate_by_set_way(uint32_t way_set_level)
{
    asm volatile ("mcr p15, 0, %0, c7, c6, 2" : : "r" (way_set_level));
}

static void v7_dcache_clean_inval_range(uint32_t start, uint32_t stop, uint32_t line_len)
{
    uint32_t mva;

    /* Align start to cache line boundary */
    start &= ~(line_len - 1);
    for (mva = start; mva < stop; mva = mva + line_len) {
        /* DCCIMVAC - Clean & Invalidate data cache by MVA to PoC */
        asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (mva));
    }
}

/* the start address and stop address must be align with line_len */
static int v7_dcache_inval_range(uint32_t start, uint32_t stop, uint32_t line_len)
{
    uint32_t mva;

    /* check_cache_range, it must be align with cache line */
    if (start & (line_len - 1))
        return -1;

    if (stop & (line_len - 1))
        return -1;


    for (mva = start; mva < stop; mva = mva + line_len) {
        /* DCIMVAC - Invalidate data cache by MVA to PoC */
        asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (mva));
    }
    return 0;
}

/* Invalidate TLB */
static void v7_inval_tlb(void)
{
    /* Invalidate entire unified TLB */
    asm volatile ("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
    /* Invalidate entire data TLB */
    asm volatile ("mcr p15, 0, %0, c8, c6, 0" : : "r" (0));
    /* Invalidate entire instruction TLB */
    asm volatile ("mcr p15, 0, %0, c8, c5, 0" : : "r" (0));
    /* Full system DSB - make sure that the invalidation is complete */
    dsb();
    /* Full system ISB - make sure the instruction stream sees it */
    isb();
}

/* CCSIDR */
#define CCSIDR_LINE_SIZE_OFFSET     0
#define CCSIDR_LINE_SIZE_MASK       0x7
#define CCSIDR_ASSOCIATIVITY_OFFSET 3
#define CCSIDR_ASSOCIATIVITY_MASK   (0x3FF << 3)
#define CCSIDR_NUM_SETS_OFFSET      13
#define CCSIDR_NUM_SETS_MASK        (0x7FFF << 13)

#define ARMV7_DCACHE_CLEAN_INVAL_RANGE  (1)
#define ARMV7_DCACHE_INVAL_RANGE        (2)
static void v7_dcache_maint_range(uint32_t start, uint32_t stop, uint32_t range_op)
{
    uint32_t line_len, ccsidr;

    ccsidr = get_ccsidr();
    line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
            CCSIDR_LINE_SIZE_OFFSET) + 2;
    /* Converting from words to bytes */
    line_len += 2;
    /* converting from log2(linelen) to linelen */
    line_len = 1 << line_len;

    switch (range_op) {
    case ARMV7_DCACHE_CLEAN_INVAL_RANGE:
        v7_dcache_clean_inval_range(start, stop, line_len);
        break;
    case ARMV7_DCACHE_INVAL_RANGE:
        v7_dcache_inval_range(start, stop, line_len);
        break;
    }

    /* DSB to make sure the operation is complete */
    dsb();
}

static int check_cache_range(unsigned long start, unsigned long stop)
{
    int ok = 1;

    if (start & (CONFIG_SYS_CACHELINE_SIZE - 1))
        start = ((start >> 6) - 1) << 6;//ok = 0;

    if (stop & (CONFIG_SYS_CACHELINE_SIZE - 1))
        stop = ((stop >> 6) + 1) << 6;//ok = 0;

    if (!ok) {
        PRINTF("CACHE: Misaligned operation at range [%08lx, %08lx]\n",
                 start, stop);
        abort();
    }

    return ok;
}

#define ENTIRE_OP_CLEAN_INVAL  1
#define ENTIRE_OP_INVAL        2
/* operate entire dcache
 * optype : 1 -- clean & invalidate by set/way
            2 -- invalidate by set/way
 */
static void v7_dcache_entire_op(uint8_t op_type)
{
    uint32_t clidr, csidr, value;         // tmp value
    uint8_t Loc;            // the number of cache level
    uint32_t NumLineShift, NumWays, NumSets, LineBytes, WayShift;
    uint32_t Way, WayIndex, Set, SetIndex, CacheSize;
    int levelN = 0;

    clidr = get_clidr();

    Loc = (clidr >> 24) & 0x7;
    if (0 == Loc)   // if loc is 0, then no need to clean
      return;

    for (levelN = 0; levelN < Loc; levelN ++) // select cache level 1 and level 2, maybe only need level 1
    {
        set_csselr(levelN << 1); // write level N in cssr/CSSELR
        isb();

        csidr = get_ccsidr(); // read csidr

        /* Determine Sets number on every way */
        NumSets = (csidr >> 13U) & 0x7FFU;
        NumSets += 1U;  // the sets number = (1 << NumSet)

        /* Number of Ways */
        NumWays = (csidr & 0x3ffU) >> 3U;
        NumWays += 1U;

        /* Get the cacheline size, way size, index size from csidr */
        NumLineShift = (csidr & 0x07U) + 4U;   // the line byte number = (1 << NumLineShift)
        LineBytes = (1U << NumLineShift);

        /* Calculate Cache Size */
        CacheSize = LineBytes * NumSets * NumWays;
        (void) CacheSize;

        // calculate the WayShift
#if 0
        // WayShift = clz(NumWays - 1)
        WayShift = 0;
        value = NumWays - 1;
        while (!(value & 0x80000000U)) {
            WayShift ++;
            value <<= 1;
        }
#else
        WayShift = __CLZ(NumWays - 1);
#endif

        Way = 0U;
        Set = 0U;
        /* Invalidate all the cachelines */
        for (WayIndex =0U; WayIndex < NumWays; WayIndex++) {
            for (SetIndex =0U; SetIndex < NumSets; SetIndex++) {
              value = Way | Set | (levelN << 1);
              if (op_type == ENTIRE_OP_CLEAN_INVAL) {
                  v7_clean_invalidate_by_set_way(value); // clean & invalidate by set/way
              } else if (op_type == ENTIRE_OP_INVAL) {
                  v7_invalidate_by_set_way(value); // invalidate by set/way
              }
              Set += LineBytes;
            }
            Set = 0U;
            Way += (1U << WayShift); // 0x40000000U; // Way += (1 << clz(NumWays - 1))
        }
    }

    // swith back to cache level 0
    set_csselr(0 << 1); // write level N in cssr/CSSELR
    dsb();
    isb();

#if 0  // assembler
  asm(
    "dmb                        \n"          // ensure ordering with previous memory accesses
    "mrc p15, 1, r0, c0, c0, 1  \n"          // read clidr
    "mov r3, r0, lsr #23        \n"          // move LoC into position
    "ands    r3, r3, #7 << 1    \n"     // extract LoC*2 from clidr
    "beq finished               \n"     // if loc is 0, then no need to clean
 "start_flush_levels:          \n"
    "mov r10, #0                \n"     // start clean at cache level 0
"flush_levels:                 \n"
    "add r2, r10, r10, lsr #1  \n"      // work out 3x current cache level
    "mov r1, r0, lsr r2        \n"      // extract cache type bits from clidr
    "and r1, r1, #7            \n"      // mask of the bits for current cache only
    "cmp r1, #2                \n"      // see what cache we have at this level
    "blt skip                  \n"      // skip if no cache, or just i-cache
    "mcr p15, 2, r10, c0, c0, 0 \n"     // select current cache level in cssr
    "isb                        \n"     // isb to sych the new cssr&csidr
    "mrc p15, 1, r1, c0, c0, 0  \n"     // read the new csidr
    "and r2, r1, #7             \n"     // extract the length of the cache lines
    "add r2, r2, #4             \n"     // add 4 (line length offset)
    "movw    r4, #0x3ff         \n"
    "ands    r4, r4, r1, lsr #3 \n"     // find maximum number on the way size
    "clz r5, r4                 \n"     // find bit position of way size increment
    "movw    r7, #0x7fff        \n"
    "ands    r7, r7, r1, lsr #13 \n"    // extract max number of the index size
"loop1:                         \n"
    "mov r9, r7                 \n"      // create working copy of max index
"loop2:                         \n"
    "orr r11, r10, r4, lsl r5   \n"     // factor way and cache number into r11
    "orr r11, r11, r9, lsl r2   \n"     // factor index number into r11
    "mcr p15, 0, r11, c7, c14, 2 \n"     // clean & invalidate by set/way
    "subs    r9, r9, #1         \n"     // decrement the index
    "bge loop2                  \n"
    "subs    r4, r4, #1         \n"      // decrement the way
    "bge loop1                  \n"
"skip:                          \n"
    "add r10, r10, #2           \n"      // increment cache number
    "cmp r3, r10                \n"
    "bgt flush_levels           \n"
"finished:                      \n"
    "mov r10, #0                \n"       // swith back to cache level 0
    "mcr p15, 2, r10, c0, c0, 0 \n"     // select current cache level in cssr
    "dsb st                     \n"
    "isb                        \n"
    "bx  lr                     \n"
    );
#endif
}

static int mmu_enabled(void)
{
    return get_cr() & CR_M;
}

/* to activate the MMU we need to set up virtual memory: use 1M areas */
void mmu_setup(void)
{
/* ACTLR bits */
#define AUX_CTL_SMP             (1 << 6)

/* TTBR0 bits */
#define TTBR0_BASE_ADDR_MASK    0xFFFFC000
#define TTBR0_RGN_NC            (0 << 3)
#define TTBR0_RGN_WBWA          (1 << 3)
#define TTBR0_RGN_WT            (2 << 3)
#define TTBR0_RGN_WB            (3 << 3)
/* TTBR0[6] is IRGN[0] and TTBR[0] is IRGN[1] */
#define TTBR0_IRGN_NC           (0 << 0 | 0 << 6)
#define TTBR0_IRGN_WBWA         (0 << 0 | 1 << 6)
#define TTBR0_IRGN_WT           (1 << 0 | 0 << 6)
#define TTBR0_IRGN_WB           (1 << 0 | 1 << 6)

    uint32_t reg, actlr;

    /* Enable ACTLR SMP, it must be enable for MMU & CACHE */
    asm volatile ("mrc p15, 0, %0, c1, c0, 1" : "=r" (actlr)); /* Read ACTLR */
    actlr |= AUX_CTL_SMP;
    asm volatile ("mcr p15, 0, %0, c1, c0, 1"   /* Write ACTLR */
                        : : "r" (actlr) : "memory");

    invalidate_dcache_all();
    v7_inval_tlb();

    /* Set TTBCR to disable LPAE */
    asm volatile("mcr p15, 0, %0, c2, c0, 2"
        : : "r" (0) : "memory");

    /* Set TTBR0 */
    reg = (uint32_t)&MMUTable & TTBR0_BASE_ADDR_MASK;
    reg |= TTBR0_RGN_WB | TTBR0_IRGN_WB;

    asm volatile("mcr p15, 0, %0, c2, c0, 0"
             : : "r" (reg) : "memory");

    /* Set the access control to all-supervisor */
    asm volatile("mcr p15, 0, %0, c3, c0, 0"
             : : "r" (~0));

    /* and enable the mmu */
    reg = get_cr(); /* get control reg. */
    set_cr(reg | CR_M);
}


void invalidate_icache_all(void)
{
    uint32_t currmask;

    currmask = get_cpsr();
    set_cpsr(currmask | IRQ_FIQ_MASK);

    /*
     * Invalidate all instruction caches to PoU.
     * Also flushes branch target cache.
     */
    asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));

    /* Invalidate entire branch predictor array */
    asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));

    /* Full system DSB - make sure that the invalidation is complete */
    dsb();
    /* ISB - make sure the instruction stream sees it */
    isb();

    set_cpsr(currmask);
}

void icache_disable(void)
{
    uint32_t reg;

    reg = get_cr();
    set_cr(reg & ~CR_I);
}

void icache_enable(void)
{
    uint32_t reg;

    reg = get_cr(); /* get control reg. */
    set_cr(reg | CR_I);
}

/*
 * Invalidates range in all levels of D-cache/unified cache used:
 * Affects the range [start, stop - 1]
 */
void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
    uint32_t currmask;

    currmask = get_cpsr();
    set_cpsr(currmask | IRQ_FIQ_MASK);

    check_cache_range(start, stop);

    v7_dcache_maint_range(start, stop, ARMV7_DCACHE_INVAL_RANGE);

    set_cpsr(currmask);
}

/*
 * Flush range(clean & invalidate) from all levels of D-cache/unified
 * cache used:
 * Affects the range [start, stop - 1]
 */
void flush_dcache_range(unsigned long start, unsigned long stop)
{
    uint32_t currmask;

    currmask = get_cpsr();
    set_cpsr(currmask | IRQ_FIQ_MASK);

    check_cache_range(start, stop);

    v7_dcache_maint_range(start, stop, ARMV7_DCACHE_CLEAN_INVAL_RANGE);
    set_cpsr(currmask);
}

void flush_dcache_all(void)
{
    uint32_t currmask;

    currmask = get_cpsr();
    set_cpsr(currmask | IRQ_FIQ_MASK);
    v7_dcache_entire_op(ENTIRE_OP_CLEAN_INVAL);

    set_cpsr(currmask);
}

void invalidate_dcache_all(void)
{
    uint32_t currmask;

    currmask = get_cpsr();
    set_cpsr(currmask | IRQ_FIQ_MASK);

    v7_dcache_entire_op(ENTIRE_OP_INVAL);
    set_cpsr(currmask);
}

void dcache_disable(void)
{
    uint32_t reg;

    reg = get_cr(); /* get control reg. */

    /* if cache isn't enabled no need to disable */
    if ((reg & CR_C) != CR_C)
        return;

    /* if disabling data cache, disable mmu too */
    reg |= CR_M;
    flush_dcache_all();

    set_cr(reg & ~CR_C);
}


void dcache_enable(void)
{
    uint32_t reg;

    reg = get_cr(); /* get control reg. */

    /* The data cache is not active unless the mmu is enabled too */
    if (!mmu_enabled())
        mmu_setup();

    reg = get_cr();
    set_cr(reg | CR_C);
}
