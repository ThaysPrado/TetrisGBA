#ifndef TETROMINO_H_
#define TETROMINO_H_

#define BLK(x, y)       ((x) | (y) << 4)
#define BLKX(c)         ((unsigned char)(c) & 0xf)
#define BLKY(c)         ((unsigned char)(c) >> 4)

#define NUM_PIECES	7

static unsigned char pieces[NUM_PIECES][4][4] = {
    // O block - all rotation options
    {
        { BLK(1, 1), BLK(2, 1), BLK(1, 2), BLK(2, 2) },
		{ BLK(1, 1), BLK(2, 1), BLK(1, 2), BLK(2, 2) },
		{ BLK(1, 1), BLK(2, 1), BLK(1, 2), BLK(2, 2) },
		{ BLK(1, 1), BLK(2, 1), BLK(1, 2), BLK(2, 2) }
    },
    // I block - all rotation options
	{
		{ BLK(0, 2), BLK(1, 2), BLK(2, 2), BLK(3, 2) },
		{ BLK(1, 0), BLK(1, 1), BLK(1, 2), BLK(1, 3) },
		{ BLK(0, 2), BLK(1, 2), BLK(2, 2), BLK(3, 2) },
		{ BLK(1, 0), BLK(1, 1), BLK(1, 2), BLK(1, 3) }
	},
    // T block - all rotation options
	{
		{ BLK(0, 1), BLK(1, 1), BLK(2, 1), BLK(1, 2) },
		{ BLK(1, 0), BLK(1, 1), BLK(1, 2), BLK(0, 1) },
		{ BLK(0, 1), BLK(1, 1), BLK(2, 1), BLK(1, 0) },
		{ BLK(1, 0), BLK(1, 1), BLK(1, 2), BLK(2, 1) }
	},
    // L block - all rotation options
	{
		{ BLK(0, 1), BLK(0, 2), BLK(1, 1), BLK(2, 1) },
		{ BLK(0, 0), BLK(1, 0), BLK(1, 1), BLK(1, 2) },
		{ BLK(0, 1), BLK(1, 1), BLK(2, 1), BLK(2, 0) },
		{ BLK(1, 0), BLK(1, 1), BLK(1, 2), BLK(2, 2) }
	},
	// J block - all rotation options
	{
		{ BLK(0, 1), BLK(1, 1), BLK(2, 1), BLK(2, 2) },
		{ BLK(1, 0), BLK(1, 1), BLK(1, 2), BLK(0, 2) },
		{ BLK(0, 0), BLK(0, 1), BLK(1, 1), BLK(2, 1) },
		{ BLK(1, 0), BLK(2, 0), BLK(1, 1), BLK(1, 2) }
	},
    // Z block - all rotation options
	{
		{ BLK(0, 1), BLK(1, 1), BLK(1, 2), BLK(2, 2) },
		{ BLK(0, 1), BLK(1, 1), BLK(1, 0), BLK(0, 2) },
		{ BLK(0, 1), BLK(1, 1), BLK(1, 2), BLK(2, 2) },
		{ BLK(0, 1), BLK(1, 1), BLK(1, 0), BLK(0, 2) }
	},
	// S block - all rotation options
	{
		{ BLK(1, 1), BLK(2, 1), BLK(0, 2), BLK(1, 2) },
		{ BLK(0, 0), BLK(0, 1), BLK(1, 1), BLK(1, 2) },
		{ BLK(1, 1), BLK(2, 1), BLK(0, 2), BLK(1, 2) },
		{ BLK(0, 0), BLK(0, 1), BLK(1, 1), BLK(1, 2) }
	}
};

static int tetromino_spawn[NUM_PIECES][2] = {
	{-1, -2}, {-1, -2}, {-2, -2}, {-1, -2}, {-1, -2}, {-1, -2}, {-1, -2}
};

#endif /* TETROMINO_H_ */