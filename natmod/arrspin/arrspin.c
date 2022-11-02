#include "py/dynruntime.h"

STATIC mp_obj_t _spin(mp_obj_t a, mp_obj_t width_ptr, mp_obj_t o) {
	mp_int_t a_len = mp_obj_get_int(mp_obj_len(a));
	mp_int_t width = mp_obj_get_int(width_ptr);
	mp_int_t height = a_len / width;
	
	mp_buffer_info_t bufinfo_i;
	mp_get_buffer_raise(a, &bufinfo_i, MP_BUFFER_READ);
	uint8_t *buf_i = bufinfo_i.buf;
	mp_buffer_info_t bufinfo_o;
	mp_get_buffer_raise(o, &bufinfo_o, MP_BUFFER_WRITE);
	uint8_t *buf_o = bufinfo_o.buf;
	
	// original index i = w * y + x
	// therefore x = i % w and y = i / w
	// new index j = h * x + y = h * (i % w) + (i / w)
	for (mp_int_t i = 0; i < a_len; i++) {
		buf_o[height * (i % width) + (i / width)] = buf_i[i];
	}
	
	return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(_spin_obj, _spin);


mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR__spin, MP_OBJ_FROM_PTR(&_spin_obj));

    MP_DYNRUNTIME_INIT_EXIT
}