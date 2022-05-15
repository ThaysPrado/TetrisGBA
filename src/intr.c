#include "intr.h"

#define MAX_INTR	14
static void (*intr_table[MAX_INTR])(void);

__attribute__ ((target("arm")))
static void intr_handler(void)
{
	int i;
	uint16_t iflags;

	clr_intr();
	iflags = REG_IF & 0x3fff;


	for(i=0; i<MAX_INTR; i++) {
		if((iflags & (1 << i)) && intr_table[i]) {
			intr_table[i]();
		}
	}

	REG_IF = iflags;
	set_intr();
}

void intr_init(void)
{
	INTR_VECTOR = (uint32_t)intr_handler;
}

void interrupt(int intr, void (*handler)(void))
{
	intr_table[intr] = handler;
}