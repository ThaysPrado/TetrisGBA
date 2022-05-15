#include <string.h>
#include "register.h"
#include "game.h"
#include "tileset.h"
#include "timer.h"
#include "intr.h"
#include "joystick.h"

#define SCR_BASE_BLOCK	0
#define CHR_BASE_BLOCK	2

void splash_screen(void);

int main(void)
{
	unsigned long msec;
	uint16_t next_upd = 0;

	intr_init();
	reset_msec_timer();
	set_intr();

	splash_screen();
	
	/* set mode 0 and enable BG0 */
	REG_DISPCNT = DISPCNT_BG0;
	/* BG0: scr/chr base blocks */
	REG_BG0CNT = BGCNT_CHR_BASE(CHR_BASE_BLOCK) | BGCNT_SCR_BASE(SCR_BASE_BLOCK);

	scrmem = (uint16_t*)VRAM_SCR_BLOCK_ADDR(SCR_BASE_BLOCK);
	chrmem = (uint16_t*)VRAM_CHR_BLOCK_ADDR(CHR_BASE_BLOCK);

	setup_tileset(chrmem);
	init_game();

	for(;;) {
		while(REG_VCOUNT < 160);	/* wait vsync */

		msec = timer_msec;

		if(handle_keys(msec) || msec >= next_upd) {
			next_upd = update(timer_msec) + msec;
		}
	}
	return 0;
}