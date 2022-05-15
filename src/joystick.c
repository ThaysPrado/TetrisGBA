#include "joystick.h"
#include "register.h"
#include "game.h"

int handle_keys(unsigned long msec)
{
	static uint16_t prevstate;
	static unsigned long first_press[4], last_inp[4];
	static const char input[] = {'d', 'a', 'w', 's'};

	int i, upd = 0;
	uint16_t keystate, keydelta;

	keystate = ~REG_KEYINPUT;
	keydelta = keystate ^ prevstate;
	prevstate = keystate;

	if(PRESS(KEY_A)) {
		game_input('x');
		upd = 1;
	}
	if(PRESS(KEY_B)) {
		game_input('z');
		upd = 1;
	}
	if(PRESS(KEY_START)) {
		game_input('k');
		upd = 1;
	}
	if(PRESS(KEY_SELECT)) {
		game_input('l');
		upd = 1;
	}

	for(i=0; i<4; i++) {
		uint16_t bit = KEY_RIGHT << i;

		if(keystate & bit) {
			if(keydelta & bit) {
				game_input(input[i]);
				upd = 1;
				first_press[i] = msec;
			} else {
				if(msec - first_press[i] >= REPEAT_START && msec - last_inp[i] >= REPEAT_INTERVAL) {
					game_input(input[i]);
					upd = 1;
					last_inp[i] = msec;
				}
			}
		}
	}

	return upd;
}