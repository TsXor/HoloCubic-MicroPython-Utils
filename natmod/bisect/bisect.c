#include "py/dynruntime.h"


STATIC mp_int_t bisect_left_impl(mp_obj_t a, mp_int_t x) {
    mp_int_t lo = 0; mp_int_t hi = mp_obj_get_int(mp_obj_len(a));
	mp_int_t mid; mp_obj_t curptr; 
    while (lo < hi) {
        mid = (lo + hi) >> 1; // 右移一位等价于地板除2
		curptr = mp_obj_subscr(a, mp_obj_new_int(mid), MP_OBJ_SENTINEL);
        if (mp_obj_get_int(curptr) < x) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

STATIC mp_obj_t bisect_left(mp_obj_t a_obj, mp_obj_t x_obj) {
    mp_obj_t a = a_obj;
    mp_int_t x = mp_obj_get_int(x_obj);

    mp_int_t result = bisect_left_impl(a, x);

    return mp_obj_new_int(result);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(bisect_left_obj, bisect_left);


mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR_bisect_left, MP_OBJ_FROM_PTR(&bisect_left_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
