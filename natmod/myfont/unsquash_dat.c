# include "py/dynruntime.h"

// https://segmentfault.com/q/1010000042696716
static inline uint8_t pown(uint8_t x, uint8_t p) {
    uint8_t result = 1;

    while (p) {
        if (p & 0x1) {
            result *= x;
        }
        x *= x;
        p >>= 1;
    }

    return result;
}

// 打 表
const uint8_t map_squash[3][16] = {
	{  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255},
	{  0,  85, 170, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
	{  0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}
};

// in-place
STATIC void unsquash_dat(uint8_t *buf, int alen, uint8_t squash_level) {
	
	// These code are ported from py, so it is somehow messy.
	uint8_t step = pown(2, (3 - squash_level));
	uint8_t pointer = pown(2, step) - 1;
	uint8_t niter = pown(2, squash_level);
	
	alen /= niter;
	
	// new space should be allocated at python side
	//buf = (uint8_t *)m_realloc(buf, niter * alen);
	
	for (uint8_t p = 0; p < niter-1; p++) {
		for (int i = 0; (p*alen <= i) & (i < (p+1)*alen); i++) {
			uint8_t n = buf[i];
			uint8_t target = n & pointer;
			buf[i] = target;
			uint8_t left = n >> step;
			buf[i+alen] = left;
		}
	}
	
	for (int j = 0; j < niter*alen; j++) {
		buf[j] = map_squash[squash_level-1][buf[j]];
	}
	
	return;
}
