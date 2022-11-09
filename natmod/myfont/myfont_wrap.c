# include "py/dynruntime.h"
# include "lvgl.h"

// 技术力太低，也不知道为啥include头文件就没用
# include "unsquash_dat.c"
# include "mymapper.c"
# include "assemble.c"

STATIC mp_obj_t getuwidth_wrap(mp_obj_t unicode_letter) {
	return mp_obj_new_int(getuwidth(
		mp_obj_get_int(unicode_letter)
	));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(getuwidth_obj, getuwidth_wrap);

STATIC mp_obj_t mapaddr_wrap(mp_obj_t cord, mp_obj_t unit, mp_obj_t zhmapper) {
	mp_buffer_info_t zhmapper_info;
	mp_get_buffer_raise(zhmapper, &zhmapper_info, MP_BUFFER_READ);

	return mp_obj_new_int(mapaddr(
		mp_obj_get_int(cord),
		mp_obj_get_int(unit),
		zhmapper_info.buf,
		zhmapper_info.len / 2
	));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mapaddr_obj, mapaddr_wrap);

STATIC mp_obj_t unsquash_dat_wrap(mp_obj_t buf, mp_obj_t squash_level) {
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_RW);
	
	unsquash_dat(
		bufinfo.buf,
		bufinfo.len,
		mp_obj_get_int(squash_level)
	);
	
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(unsquash_dat_obj, unsquash_dat_wrap);

STATIC MP_DEFINE_CONST_FUN_OBJ_2(assembler_obj, assembler);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

	mp_store_global(MP_QSTR_getuwidth, MP_OBJ_FROM_PTR(&getuwidth_obj));
	mp_store_global(MP_QSTR_mapaddr, MP_OBJ_FROM_PTR(&mapaddr_obj));
	mp_store_global(MP_QSTR_unsquash_dat, MP_OBJ_FROM_PTR(&unsquash_dat_obj));
	mp_store_global(MP_QSTR_assembler, MP_OBJ_FROM_PTR(&assembler_obj));

    MP_DYNRUNTIME_INIT_EXIT
}