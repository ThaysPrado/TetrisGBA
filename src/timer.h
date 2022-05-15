#ifndef _TIMER_H_
#define _TIMER_H_

#include "register.h"

#define enable_timer(x) \
	do { REG_TMCNT_H(x) |= TMCNT_EN; } while(0)

#define disable_timer(x) \
	do { REG_TMCNT_H(x) &= ~TMCNT_EN; } while(0)


extern volatile unsigned long timer_msec;
void reset_msec_timer(void);

#endif	/* _TIMER_H_ */