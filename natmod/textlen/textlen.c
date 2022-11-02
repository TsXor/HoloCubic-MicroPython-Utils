#include "py/dynruntime.h"

// 0000 - 007F 0xxxxxxx
// 0080 - 07FF 110xxxxx 10xxxxxx
// 0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx

static inline unsigned char heading_ones(unsigned char x) {
    unsigned char result = 0;

    while (x & (0b10000000 >> result)) {
        result++;
    }

    return result;
}

STATIC mp_obj_t _textlen(mp_obj_t a, mp_obj_t o) {
	mp_int_t a_len = mp_obj_get_int(mp_obj_len(a));
	
	mp_buffer_info_t bufinfo_i;
	mp_get_buffer_raise(a, &bufinfo_i, MP_BUFFER_READ);
	uint8_t *buf_i = bufinfo_i.buf;
	mp_buffer_info_t bufinfo_o;
	mp_get_buffer_raise(o, &bufinfo_o, MP_BUFFER_WRITE);
	uint8_t *buf_o = bufinfo_o.buf;
	
	mp_int_t pos = 0; mp_int_t ulen = 0; unsigned char clen;
	for (mp_int_t i = 0; i < a_len;) {
		clen = heading_ones(buf_i[i]);
		if (clen) {
			// HALF_PUNCT_CHARS = getrange(0x2000, 0x2070)
			// so: HALF_PUNCT_CHARS utf-8 first byte is 0xE2
			if (buf_i[i] == 0xE2) {
				ulen++;
			} else {
				ulen += 2;
			}
		} else {
			clen = 1; ulen++;
		}
		
		buf_o[pos] = ulen;
		pos++; i += clen;
	}
	
	return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(_textlen_obj, _textlen);


mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR__textlen, MP_OBJ_FROM_PTR(&_textlen_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
