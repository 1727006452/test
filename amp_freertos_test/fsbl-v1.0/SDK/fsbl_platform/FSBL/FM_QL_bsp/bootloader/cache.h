/*****************************************************************************/
/**
*
* @file cache.h
*
* This header file contains specific exception related APIs.
* For exception related functions that can be used across all FMSH supported
* processors, please use FMSH_exception.h.
*
* </pre>
*
******************************************************************************/

#ifndef FMSH_CACHE_H /* prevent circular inclusions */
#define FMSH_CACHE_H /* by using protection macros */

#define PSOC_CACHE_ENABLE       0


void invalidate_icache_all(void);
void icache_disable(void);
void icache_enable(void);

/*
 * Invalidates range in all levels of D-cache/unified cache used:
 * Affects the range [start, stop - 1]
 */
void invalidate_dcache_range(unsigned long start, unsigned long stop);

/*
 * Flush range(clean & invalidate) from all levels of D-cache/unified
 * cache used:
 * Affects the range [start, stop - 1]
 */
void flush_dcache_range(unsigned long start, unsigned long stop);

void flush_dcache_all(void);
void invalidate_dcache_all(void);

void dcache_disable(void);
void dcache_enable(void);

#endif
