#include "py/dynruntime.h"

// https://segmentfault.com/q/1010000042696716
static inline unsigned char pown(unsigned char x, unsigned char p) {
    unsigned char result = 1;

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
const unsigned char map_squash[3][16] = {
	{  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255},
	{  0,  85, 170, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
	{  0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}
};

STATIC mp_obj_t _unsquash_dat(mp_obj_t a, mp_obj_t alen_ptr, mp_obj_t squash_level_ptr) {
	unsigned char squash_level = mp_obj_get_int(squash_level_ptr);
	mp_int_t alen = mp_obj_get_int(alen_ptr);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(a, &bufinfo, MP_BUFFER_RW);
	uint8_t *buf = bufinfo.buf;
	
	// These code are ported from py, so it is somehow messy.
	unsigned char step = pown(2, (3 - squash_level));
	unsigned char pointer = pown(2, step) - 1;
	unsigned char niter = pown(2, squash_level);
	for (unsigned char p = 0; p < niter-1; p++) {
		for (mp_int_t i = 0; (p*alen <= i) & (i < (p+1)*alen); i++) {
			unsigned char n = buf[i];
			unsigned char target = n & pointer;
			buf[i] = target;
			unsigned char left = n >> step;
			buf[i+alen] = left;
		}
	}
	
	for (mp_int_t j = 0; j < niter*alen; j++) {
		buf[j] = map_squash[squash_level-1][buf[j]];
	}
	
	return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(_unsquash_dat_obj, _unsquash_dat);


mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR__unsquash_dat, MP_OBJ_FROM_PTR(&_unsquash_dat_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
