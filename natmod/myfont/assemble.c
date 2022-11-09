# include "lvgl.h"

typedef struct mp_lv_struct_t
{
    mp_obj_base_t base;
    void *data;
} mp_lv_struct_t;

struct sizetup {
	uint8_t w;
	uint8_t h;
};

struct my_dsc {
	mp_obj_t dsc_cb;
	mp_obj_t bmp_cb;
};

bool get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next) {
	mp_obj_t func = mp_obj_subscr((mp_obj_t)font->dsc, mp_obj_new_int(0), MP_OBJ_SENTINEL);
	mp_obj_t args[1]; args[0] = mp_obj_new_int(unicode_letter);
	mp_obj_t size_obj = mp_call_function_n_kw(func, 1, 0, args);
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(size_obj, &bufinfo, MP_BUFFER_READ);
	struct sizetup *size = bufinfo.buf;

	dsc_out->adv_w = size->w;   /*Horizontal space required by the glyph in [px]*/
	dsc_out->box_h = size->h;   /*Height of the bitmap in [px]*/
	dsc_out->box_w = size->w;   /*Width of the bitmap in [px]*/
	dsc_out->ofs_x = 0;         /*X offset of the bitmap in [pf]*/
	dsc_out->ofs_y = 0;         /*Y offset of the bitmap measured from the as line*/
	dsc_out->bpp   = 8;         /*Bits per pixel: 1/2/4/8*/

	return true;
}

const uint8_t * get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter) {
	mp_obj_t func = mp_obj_subscr((mp_obj_t)font->dsc, mp_obj_new_int(1), MP_OBJ_SENTINEL);
	mp_obj_t args[1]; args[0] = mp_obj_new_int(unicode_letter);
	mp_obj_t bmp_obj = mp_call_function_n_kw(func, 1, 0, args);
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(bmp_obj, &bufinfo, MP_BUFFER_READ);
	uint8_t *bitmap = bufinfo.buf;

	return bitmap;
}

STATIC mp_obj_t assembler(mp_obj_t font_type, mp_obj_t dsc) {
	
	//lv_font_t *lv_font_obj = (lv_font_t *)((mp_lv_struct_t *)font_obj)->data;
	lv_font_t *lv_font_obj = (lv_font_t *)m_malloc(sizeof(lv_font_t));
	lv_font_obj->dsc = dsc;
	lv_font_obj->get_glyph_dsc = get_glyph_dsc_cb;
	lv_font_obj->get_glyph_bitmap = get_glyph_bitmap_cb;
	
	mp_obj_base_t *tbase = (mp_obj_base_t *)m_malloc(sizeof(mp_obj_base_t));
	tbase->type = (mp_obj_type_t *)font_type;
	mp_lv_struct_t *pyf = (mp_lv_struct_t *)m_malloc(sizeof(mp_lv_struct_t));
	pyf->base = *tbase;
	pyf->data = lv_font_obj;
	
	return (mp_obj_t)pyf;
}