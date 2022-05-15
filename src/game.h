#ifndef GAME_H_
#define GAME_H_

#define SCR_WIDTH	240
#define SCR_HEIGHT	160
#define VIRT_WIDTH	256
#define VIRT_HEIGHT	256

#define SCR_ROWS	(SCR_HEIGHT / 8)
#define SCR_COLS	(SCR_WIDTH / 8)
#define VIRT_ROWS	(VIRT_HEIGHT / 8)
#define VIRT_COLS	(VIRT_WIDTH / 8)

extern uint16_t *scrmem, *chrmem;
extern long tick_interval;

int init_game(void);
void cleanup_game(void);

long update(long msec);
void game_input(int c);

#endif	/* GAME_H_ */