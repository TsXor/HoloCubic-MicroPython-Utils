# include "../../natlib/bisect.c"

// 一般情况下这个宏定义不需要改，除非你改了标点字符集。
#define ASCII_MAX			 0x0080
#define HALF_PUNCT_OFFSET	 128
#define HALF_PUNCT_MIN		 0x2000
#define HALF_PUNCT_MAX		 0x2070
#define FULL_OFFSET			 240 // 128+112
#define FULL_PUNCT_1_OFFSET	 0
#define FULL_PUNCT_1_MIN	 0x3000
#define FULL_PUNCT_1_MAX	 0x3040
#define FULL_PUNCT_2_OFFSET	 64
#define FULL_PUNCT_2_MIN	 0xff01
#define FULL_PUNCT_2_MAX	 0xff5f
#define CHINESE_OFFSET		 158 // 64+94
#define MISSINGCHAR_ORD		 0x303c
#define MISSINGCHAR_ADDR	 360 // mapaddr(MISSINGCHAR_ORD, 1, NULL, 0);

STATIC uint8_t chrtype(uint32_t cord) {
         if (cord < ASCII_MAX)
    { return 0; }
    else if ((HALF_PUNCT_MIN <= cord) & (cord < HALF_PUNCT_MAX))
    { return 1; }
    else if ((FULL_PUNCT_1_MIN <= cord) & (cord < FULL_PUNCT_1_MAX))
    { return 2; }
    else if ((FULL_PUNCT_2_MIN <= cord) & (cord < FULL_PUNCT_2_MAX))
    { return 3; }
    else
    { return 4; }
}

STATIC int mapzh(uint32_t cord, int len, uint16_t *zhmapper) {
	int coffset = bisect_left_uint16(zhmapper, len, (int)cord);
	if (cord != zhmapper[coffset]) {
		return -1;
	}
	return coffset;
}

STATIC mp_int_t mapaddr(mp_int_t cord, mp_int_t unit, uint16_t *zhmapper, mp_int_t len) {
	int size = 2 * unit * unit;
	mp_int_t faddr = 0;
	int zhid;
	switch (chrtype(cord)) {
		case 0: faddr = size * cord;
		        break;
		case 1: faddr = size * (HALF_PUNCT_OFFSET + cord - HALF_PUNCT_MIN);
		        break;
		case 2: faddr = size * FULL_OFFSET
		              + 2 * size * (FULL_PUNCT_1_OFFSET + cord - FULL_PUNCT_1_MIN);
		        break;
		case 3: faddr = size * FULL_OFFSET
		              + 2 * size * (FULL_PUNCT_2_OFFSET + cord - FULL_PUNCT_2_MIN);
		        break;
		case 4: faddr = (-1 == (zhid = mapzh(cord, len, zhmapper)))
		              ? size * MISSINGCHAR_ADDR
		              :(size * FULL_OFFSET
		              + 2 * size * (CHINESE_OFFSET + zhid));
		        break;
	}
	return faddr;
}

STATIC mp_int_t getuwidth(mp_int_t cord) {
	if (chrtype((uint32_t)cord) <= 1) {
		return 1;
	} else {
		return 2;
	}
}
