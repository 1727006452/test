/******************************************************************************
*
* Copyright (C) 2009 - 2019 FMSH, Inc.  All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file mmu_table.s
*
*
* The overview of translation table memory attributes is described below.
*
*|                       | Memory Range            | Definition in Translation Table   |
*|-----------------------|-------------------------|-----------------------------------|
*| ROM & AXI_SRAM        | 0x00000000 - 0x00100000 | Normal write-back Cacheable       |
*| DDR                   | 0x00100000 - 0x3FFFFFFF | Normal write-back Cacheable       |
*| PL                    | 0x40000000 - 0xBFFFFFFF | Strongly Ordered                  |
*| Reserved              | 0xC0000000 - 0xDFFFFFFF | Unassigned                        |
*| Memory mapped devices | 0xE0000000 - 0xE1EFFFFF | Device Memory                     |
*| AHB_SRAM              | 0xE1F00000 - 0xE1FFFFFF | Normal write-back Cacheable       |
*| Linear QSPI - XIP     | 0xE2000000 - 0xE5FFFFFF | Normal write-through cacheable    |
*| Memory mapped devices | 0xE6000000 - 0xFFFFFFFF | Device Memory                     |
*
* @note
*
* For region 0x00000000 - 0x00100000, a system where AXI_SRAM is less than 1MB,
* region after DDR and before PL is marked as undefined/reserved in translation
* table. Some region are reserved  but due to granual size of 1MB, it is not
* possible to define separate regions for them.

**************************************************************************************/

#define DDR_START   (0x00100000)
#define DDR_END     (0x3fffffff)
#define DDR_SIZE    ((DDR_END - DDR_START) + 1)
#define DDR_REG     (DDR_SIZE / 0x100000)

    SECTION .mmu_tbl:CODE:NOROOT(2)
    PUBLIC MMUTable
    ARM
        
MMUTable:
    /* Each table entry occupies one 32-bit word and there are
     * 4096 entries, so the entire table takes up 16KB.
     * Each entry covers a 1MB section.
     */
SECT    set     0
                /*0x00000000 - 0x00100000 (cacheable )*/
        DC32    SECT + 0x15de6      /* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
SECT    set     SECT+0x100000

        rept    DDR_REG         /*  (DDR Cacheable) */
        DC32    SECT + 0x15de6      /* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
SECT    set     SECT+0x100000
        endr

        rept    0x0400          /* 0x40000000 - 0x7fffffff (FPGA slave0) */
        DC32    SECT + 0xc02        /* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
SECT    set     SECT+0x100000
        endr
        
        rept    0x0400          /* 0x80000000 - 0xbfffffff (FPGA slave1) */
        DC32    SECT + 0xc02        /* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
SECT    set     SECT+0x100000
        endr
 
        rept    0x0200          /* 0xc0000000 - 0xdfffffff (unassigned/reserved).
                                    * Generates a translation fault if accessed */
        DC32    SECT + 0       /* S=b0 TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
SECT    set     SECT+0x100000
        endr

        rept    0x001F              /* 0xE0000000 - 0xE1EFFFFF Device Memory */
        DC32    SECT + 0xc06        /* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
SECT    set     SECT+0x100000
        endr
        
                /* 0xE1F00000 - 0xE1FFFFFF  (AHB_SRAM )*/
        DC32    SECT + 0x15de6      /* S=b1 TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 */
SECT    set     SECT+0x100000

        rept    0x0040              /* 0xE2000000 - 0xE5FFFFFF (Linear QSPI - XIP) */
        DC32    SECT + 0xc0a        /* S=b0 TEX=b000 AP=b11, Domain=b0, C=b1, B=b0 */
SECT    set     SECT+0x100000
        endr

        rept    0x01A0              /* 0xE6000000 - 0xFFFFFFFF Device Memory */
        DC32    SECT + 0xc06        /* S=b0 TEX=b000 AP=b11, Domain=b0, C=b0, B=b1 */
SECT    set     SECT+0x100000
        endr

    END
