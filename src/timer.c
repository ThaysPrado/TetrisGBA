#include "intr.h"
#include "timer.h"

/* clock is 16.78MHz
 * - no prescale: 59.595ns
 * - prescale 64: 3.814us
 * - prescale 256: 15.256us
 * - prescale 1024: 61.025us
 */

volatile unsigned long timer_msec;

static void timer_intr(void);

void reset_msec_timer(void)
{
	REG_TM0CNT_H &= ~TMCNT_EN;
	interrupt(INTR_TIMER0, timer_intr);
	timer_msec = 0;
	REG_TM0CNT_L = 65535 - 16779;
	REG_TM0CNT_H |= TMCNT_IE | TMCNT_EN;
	unmask(INTR_TIMER0);
}


static void timer_intr(void)
{
	timer_msec++;
}