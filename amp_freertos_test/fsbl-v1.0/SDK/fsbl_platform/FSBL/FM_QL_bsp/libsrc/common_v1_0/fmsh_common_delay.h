#ifndef FMSH_COMMON_DELAY_H
#define FMSH_COMMON_DELAY_H

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif

#define GTC_CLK_FREQ    (PS_CLK_FREQ/2000000) 

void global_timer_enable();    
u64 get_current_time()   ;
void delay_ms(double time_ms);
void delay_us(u32 time_us);
void delay_1ms();
void delay_1us();

#endif
