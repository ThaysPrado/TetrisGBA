#ifndef _INTR_H_
#define _INTR_H_

#include "register.h"

/* interrupts */
enum {
	INTR_VBLANK,
	INTR_HBLANK,
	INTR_VCOUNT,
	INTR_TIMER0,
	INTR_TIMER1,
	INTR_TIMER2,
	INTR_TIMER3,
	INTR_COMM,
	INTR_DMA0,
	INTR_DMA1,
	INTR_DMA2,
	INTR_DMA3,
	INTR_KEY,
	INTR_GPAK
};

void intr_init(void);

/* set/clear interrupts */
#define set_intr()	\
	do { REG_IME |= 0x0001; } while(0)
#define clr_intr()	\
	do { REG_IME &= 0xfffe; } while(0)

/* set an interrupt handler */
void interrupt(int intr, void (*handler)(void));

/* mask/unmask an interrupt */
#define mask(intr)		do {REG_IE &= ~(1 << (intr));} while(0)
#define unmask(intr)	do {REG_IE |= 1 << (intr);} while(0)

#endif	/* _INTR_H_ */