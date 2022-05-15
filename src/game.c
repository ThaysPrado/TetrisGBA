#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <assert.h>

#include "game.h"
#include "tetromino.h"
#include "tileset.h"
#include "timer.h"
#include "register.h"

enum { ERASE_PIECE, DRAW_PIECE };

// dimensions of the playfield
#define ROWS       18
#define COLS       10
// offsets of the playfield - left side
#define X_OFFS     2
#define Y_OFFS     0

#define BLINK_UPD_RATE	100
#define GAME_OVER_FILL_RATE	50
#define WAIT_INF	0x7fffffff

int scr[SCR_COLS * SCR_ROWS];
long tick_interval;
uint16_t *scrmem, *chrmem;

static void draw_bg(void);
static void draw_tetramino(int piece, const int *pos, int rot, int mode);
static int spawn(void);
static int collision(int piece, const int *pos);
static void stick(int piece, const int *pos);
static void draw_line(int row, int blink);
static void erase_completed(void);
static void draw_pf(void);

static int pos[2], next_pos[2];
static int cur_piece, next_piece;
static int cur_rot, prev_rot;
static int complines[4];
static int num_complines;
static int game_over;
static int pause;
static int lines;
static int just_spawned;

// Background Composition
static const char *bgdata[SCR_ROWS] = {
	" L..........R{-----}          ",
	" L..........R(.....)          ",
	" L..........R[_____]          ",
	" L..........R.......          ",
	" L..........R                 ",
	" L..........R{-----}          ",
	" L..........R(.....)          ",
	" L..........R(.....)          ",
	" L..........R>=====<          ",
	" L..........R(.....)          ",
	" L..........R(.....)          ",
	" L..........R[_____]          ",
	" L..........R {----}          ",
	" L..........R (....)          ",
	" L..........R (....)          ",
	" L..........R (....)          ",
	" L..........R (....)          ",
	" L..........R [____]          ",
	" `BBBBBBBBBB/                 ",
	"                              "
};

// TODO: - include levels
static const long level_speed = 600;

int init_game(void)
{
    int i, j;
    int *row = scr;

    uint32_t seed = REG_TM0CNT_L | (timer_msec << 16);
	srand(seed);

    // SET INITIAL FLAGS
    pause = 0;
    game_over = 0;
    num_complines = 0;
    lines = 0;
	tick_interval = level_speed;
	cur_piece = -1;
	next_piece = rand() % NUM_PIECES;

    memset(scrmem, 0, VIRT_COLS * VIRT_ROWS * 2);

    // fill the screen buffer
	for(i=0; i<SCR_ROWS; i++) {
		for(j=0; j<SCR_COLS; j++) {
			int tile;
			switch(bgdata[i][j]) {
			case 'L':
				tile = TILE_LPFSEP;
				break;
			case 'R':
				tile = TILE_RPFSEP;
				break;
			case 'B':
				tile = TILE_BPFSEP;
				break;
			case '`':
				tile = TILE_BLPFSEP;
				break;
			case '/':
				tile = TILE_BRPFSEP;
				break;
			case '.':
				tile = TILE_PF;
				break;
			case '{':
				tile = TILE_FRM_TL;
				break;
			case '}':
				tile = TILE_FRM_TR;
				break;
			case '[':
				tile = TILE_FRM_BL;
				break;
			case ']':
				tile = TILE_FRM_BR;
				break;
			case '>':
				tile = TILE_FRM_LTEE;
				break;
			case '<':
				tile = TILE_FRM_RTEE;
				break;
			case '-':
				tile = TILE_FRM_THLINE;
				break;
			case '_':
				tile = TILE_FRM_BHLINE;
				break;
			case '=':
				tile = TILE_FRM_MHLINE;
				break;
			case '(':
				tile = TILE_FRM_LVLINE;
				break;
			case ')':
				tile = TILE_FRM_RVLINE;
				break;
			case ' ':
			default:
				tile = TILE_BLACK;
			}
			row[j] = tile;
		}
		row += SCR_COLS;
	}

	draw_bg();

	return 0;
}

static void draw_bg(void)
{
	int i, j;
	int *sptr = scr;
	uint16_t *dptr = scrmem;

	for(i = 0; i < SCR_ROWS; i++) {
		for(j = 0; j < SCR_COLS; j++) {
			*dptr++ = *sptr++;
		}
		dptr += VIRT_COLS - SCR_COLS;
	}
}

void game_input(int input)
{
	switch(input) {
	case 'a':
		next_pos[1] = pos[1] - 1;
		if(collision(cur_piece, next_pos)) {
			next_pos[1] = pos[1];
		}
		break;

	case 'd':
		next_pos[1] = pos[1] + 1;
		if(collision(cur_piece, next_pos)) {
			next_pos[1] = pos[1];
		}
		break;

	case 's':
		// ignore drops until the first update after a spawn
		if(!just_spawned) {
			next_pos[0] = pos[0] + 1;
			if(collision(cur_piece, next_pos)) {
				next_pos[0] = pos[0];
			}
		}
		break;
	
	case 'x':
		// button a
		prev_rot = cur_rot;
		cur_rot = (cur_rot + 1) & 3;
		if(collision(cur_piece, next_pos)) {
			cur_rot = prev_rot;
		}
		break;

	case 'z':
		// button b
		next_pos[0] = pos[0] + 1;
		while(!collision(cur_piece, next_pos)) {
			next_pos[0]++;
		}
		next_pos[0]--;
		break;

	case 'k':
		if(game_over) {
			init_game();
		} else {
			pause ^= 1;
		}
		break;

	case 'l':
		init_game();
		break;

	default:
		break;
	}
}

long update(long msec)
{
	static long prev_tick;
	long dt;

	if (pause) {
		prev_tick = msec;
		return WAIT_INF;
	}

	dt = msec - prev_tick;

	if(game_over) {
		int i, row = ROWS - game_over;
		int *ptr;

		if(row >= 0) {
			ptr = scr + (row + Y_OFFS) * SCR_COLS + X_OFFS;
			for(i = 0; i < COLS; i++) {
				*ptr++ = TILE_GAMEOVER;
			}
			draw_line(row, 1);

			game_over++;
			return GAME_OVER_FILL_RATE;
		}

		return WAIT_INF;
	}

	if(num_complines) {
		int i, blink = dt >> 8;

		if(blink > 6) {
			erase_completed();
			num_complines = 0;
			return 0;
		}

		for(i = 0; i < num_complines; i++) {
			draw_line(complines[i], blink & 1);
		}
		return BLINK_UPD_RATE;
	}


	// fall
	while(dt >= tick_interval) {
		if(cur_piece >= 0) {
			just_spawned = 0;
			next_pos[0] = pos[0] + 1;
			if(collision(cur_piece, next_pos)) {
				next_pos[0] = pos[0];
				stick(cur_piece, next_pos);
				cur_piece = -1;
				return 0;
			}
		} else {
			// respaw
			if(spawn() == -1) {
				game_over = 1;
				return 0;
			}
		}

		dt -= tick_interval;
		prev_tick = msec;
	}

	if(cur_piece >= 0 && (memcmp(pos, next_pos, sizeof pos) != 0 || cur_rot != prev_rot)) {
		draw_tetramino(cur_piece, pos, prev_rot, ERASE_PIECE);
		draw_tetramino(cur_piece, next_pos, cur_rot, DRAW_PIECE);
		memcpy(pos, next_pos, sizeof pos);
		prev_rot = cur_rot;
	}

	return tick_interval - dt;
}

// TODO: - improve random pieces algorithm
static int spawn(void)
{
	static const int preview_pos[] = {13, 13};
	int r, tries = 2;

	do {
		r = rand() % NUM_PIECES;
	} while(tries-- > 0 && (r | cur_piece | next_piece) == cur_piece);

	draw_tetramino(next_piece, preview_pos, 0, ERASE_PIECE);
	draw_tetramino(r, preview_pos, 0, DRAW_PIECE);

	cur_piece = next_piece;
	next_piece = r;

	prev_rot = cur_rot = 0;
	pos[0] = tetromino_spawn[cur_piece][0];
	next_pos[0] = pos[0] + 1;
	pos[1] = next_pos[1] = COLS / 2 + tetromino_spawn[cur_piece][1];

	if(collision(cur_piece, next_pos)) {
		return -1;
	}

	just_spawned = 1;
	return 0;
}

static void draw_tetramino(int piece, const int *pos, int rot, int mode)
{
	int i, tile, pal;
	uint16_t tval;
	unsigned char *p = pieces[piece][rot];

	if(mode == ERASE_PIECE) {
		tile = TILE_PF;
		pal = 0;
	} else {
		tile = TILE_BLOCK;
		pal = FIRST_PIECE_PAL + piece;
	}
	tval = tile | BGTILE_PAL(pal);

	for(i=0; i<4; i++) {
		int x = X_OFFS + pos[1] + BLKX(*p);
		int y = Y_OFFS + pos[0] + BLKY(*p);
		p++;

		if(y < 0) continue;

		scrmem[y * VIRT_COLS + x] = tval;
	}
}

static void draw_line(int row, int blink)
{
	int i;
	uint16_t *dptr = scrmem + row * VIRT_COLS + X_OFFS;

	if(blink) {
		int *sptr = scr + (row + Y_OFFS) * SCR_COLS + X_OFFS;
		for(i = 0; i< COLS; i++) {
			*dptr++ = *sptr++;
		}
	} else {
		for(i = 0; i < COLS; i++) {
			*dptr++ = TILE_PF;
		}
	}
}

static int collision(int piece, const int *pos)
{
	int i;
	unsigned char *p = pieces[piece][cur_rot];

	for(i=0; i<4; i++) {
		int x = X_OFFS + pos[1] + BLKX(*p);
		int y = Y_OFFS + pos[0] + BLKY(*p);
		p++;

		if(y < 0) continue;

		if(scr[y * SCR_COLS + x] != TILE_PF) return 1;
	}

	return 0;
}

static void stick(int piece, const int *pos)
{
	int i, j, nblank;
	int *pfline;
	unsigned char *p = pieces[piece][cur_rot];

	num_complines = 0;

	for(i=0; i<4; i++) {
		int x = pos[1] + BLKX(*p);
		int y = pos[0] + BLKY(*p);
		p++;

		pfline = scr + (y + Y_OFFS) * SCR_COLS + X_OFFS;
		pfline[x] = TILE_BLOCK;

		nblank = 0;
		for(j = 0; j < COLS; j++) {
			if(pfline[j] == TILE_PF) {
				nblank++;
			}
		}

		if(nblank == 0) {
			complines[num_complines++] = y;
		}
	}
}

static void erase_completed(void)
{
	int i, j, srow, drow;
	int *pfstart = scr + Y_OFFS * SCR_COLS + X_OFFS;
	int *dptr;

	// sort completed lines
	for(i=0; i<num_complines-1; i++) {
		for(j=i+1; j<num_complines; j++) {
			if(complines[j] > complines[i]) {
				int tmp = complines[j];
				complines[j] = complines[i];
				complines[i] = tmp;
			}
		}
	}

	srow = drow = ROWS - 1;
	dptr = pfstart + drow * SCR_COLS;

	for(i = 0; i < ROWS; i++) {
		for(j=0; j<num_complines; j++) {
			if(complines[j] == srow) {
				srow--;
			}
		}

		if(srow < 0) {
			for(j = 0; j< COLS; j++) {
				dptr[j] = TILE_PF;
			}

		} else if(srow != drow) {
			int *sptr = pfstart + srow * SCR_COLS;
			memcpy(dptr, sptr, COLS * sizeof *dptr);
		}

		srow--;
		drow--;
		dptr -= SCR_COLS;
	}

	draw_pf();
}

static void draw_pf(void)
{
	int i, j;
	int *sptr = scr + Y_OFFS * SCR_COLS + X_OFFS;
	uint16_t *dptr = scrmem + X_OFFS;

	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++) {
			*dptr++ = *sptr++;
		}
		sptr += SCR_COLS - COLS;
		dptr += VIRT_COLS - COLS;
	}
}