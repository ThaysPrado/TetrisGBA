#include <string.h>
#include "register.h"
#include "timer.h"

#define LIMITE_TIME		2000

void splash_screen(void)
{
	unsigned long start;

	/* mode 3 */
	REG_DISPCNT = 3 | DISPCNT_BG2;

	start = timer_msec;

	while(timer_msec - start < LIMITE_TIME);

	*(unsigned int*)0x04000000 = 0x0403;

    ((unsigned short*)0x06000000)[120+80*240] = 0x001F;
    ((unsigned short*)0x06000000)[136+80*240] = 0x03E0;
    ((unsigned short*)0x06000000)[120+96*240] = 0x7C00;

	while(REG_KEYINPUT & KEY_START);
	while((REG_KEYINPUT & KEY_START) == 0);
}