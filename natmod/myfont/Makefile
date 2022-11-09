# Location of top-level MicroPython directory
MPY_DIR = ../../../lv_micropython
LVGL_PATH = ../../../lv_micropython/lib/lv_bindings/lvgl

# Name of module
MOD = myfont

# Source files (.c or .py)
SRC = myfont_wrap.c myfont.py

# Architecture to build for (x86, x64, armv7m, xtensa, xtensawin)
ARCH = xtensawin

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk
include $(MPY_DIR)/lib/lv_bindings/lvgl/lvgl.mk
