
#ifndef FMSH_COMMON_IO_H
#define FMSH_COMMON_IO_H

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif

#include "stdio.h" 
    
#define FMSH_ReadReg(baseAddr, offSet)		*((volatile unsigned int *)(baseAddr + offSet))
#define FMSH_WriteReg(baseAddr, offSet, data)	*((volatile unsigned int *)(baseAddr + offSet)) = data   

/*the following macro performs an 8-bit read*/
#define FMSH_IN8_8(p)     ((uint8_t) *((volatile uint8_t *)(&p)))
/*the following macro performs an 8-bit write*/
#define FMSH_OUT8_8(v,p)  *((volatile uint8_t *) (&p)) = (uint8_t) (v)
/*the following macro performs a 16-bit read*/
#define FMSH_IN16_16(p)      ((uint16_t) *((volatile uint16_t *)(p)))  
/*the following macro performs a 16-bit write*/
#define FMSH_OUT16_16(v,p)  *((volatile uint16_t *) (p)) = (uint16_t)(v)   
/*the following macro performs a 32-bit write*/
#define FMSH_OUT32_32(v,p)  *((volatile uint32_t *)(&p)) = (v)     
/*the following macro performs a 32-bit reads*/
#define FMSH_IN32_32(p)   (uint32_t)*((uint32_t *)(&p))

#ifdef __cplusplus
}
#endif

#endif  // FMSH_COMMON_IO_H

