#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define PRESS(key)	((keystate & (key)) && (keydelta & (key)))
#define REPEAT_START		500
#define REPEAT_INTERVAL		75

int handle_keys(unsigned long msec);

#endif	/* JOYSTICK_H_ */