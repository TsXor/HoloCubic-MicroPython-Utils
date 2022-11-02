# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
#import webrepl
#webrepl.start()

def reboot():
    import machine; machine.reset()

# mount SDcard to /sd
from sdstorage import mountsd
mountsd('/sd')
del mountsd

# prepare screen driver
import st7789
from tftscr import config as tftscr
tft = tftscr()
del tftscr

def cls():
    tft.fill(st7789.BLACK)

# prepare font
from myfont import myFont
font = myFont('/font.bin')

import gc
gc.collect()

print('init done')