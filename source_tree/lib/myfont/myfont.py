from array import array
from bisect import bisect_left
from ._unsquash_dat import _unsquash_dat
from rgb565 import gray2rgb565
from textlen import textlen
from arrspin import spin

ASCII_MAX = const(0x80)
HALF_PUNCT_OFFSET = const(128)
HALF_PUNCT_MIN = const(0x2000)
HALF_PUNCT_MAX = const(0x2070)
FULL_OFFSET = const(128+112)
FULL_PUNCT_1_OFFSET = const((128+112)-FULL_OFFSET)
FULL_PUNCT_1_MIN = const(0x3000)
FULL_PUNCT_1_MAX = const(0x3040)
FULL_PUNCT_2_OFFSET = const((128+112+64)-FULL_OFFSET)
FULL_PUNCT_2_MIN = const(0xff01)
FULL_PUNCT_2_MAX = const(0xff5f)
CHINESE_OFFSET = const((128+112+64+94)-FULL_OFFSET)
MISSINGCHAR_ORD = const(0x303c)

@micropython.native
def mapaddr(addr_base:int, cord: int, full_size: int, half_size: int, zhmapper):
    if cord < ASCII_MAX:
        return addr_base + half_size * cord, 1
    elif HALF_PUNCT_MIN <= cord < HALF_PUNCT_MAX:
        return addr_base + half_size * (HALF_PUNCT_OFFSET + cord - HALF_PUNCT_MIN), 1
    elif FULL_PUNCT_1_MIN <= cord < FULL_PUNCT_1_MAX:
        return addr_base + half_size * FULL_OFFSET + full_size * (FULL_PUNCT_1_OFFSET + cord - FULL_PUNCT_1_MIN), 0
    elif FULL_PUNCT_2_MIN <= cord < FULL_PUNCT_2_MAX:
        return addr_base + half_size * FULL_OFFSET + full_size * (FULL_PUNCT_2_OFFSET + cord - FULL_PUNCT_2_MIN), 0
    else:
        coffset = bisect_left(zhmapper, cord)
        if cord != zhmapper[coffset]:
            return addr_base + half_size * FULL_OFFSET + full_size * (FULL_PUNCT_1_OFFSET + MISSINGCHAR_ORD - FULL_PUNCT_1_MIN), 0
        return addr_base + half_size * FULL_OFFSET + full_size * (CHINESE_OFFSET + coffset), 0


class myFont:
    def __init__(self, font_path, log_missing_char=False):
        self.fp = open(font_path, 'rb')
        
        # get header info
        opts = ('little', False)
        self.size = int.from_bytes(self.readfp(0, 1), *opts)
        self.unit = self.size // 2
        self.squash_level = int.from_bytes(self.readfp(1, 1), *opts)
        mapper_len = int.from_bytes(self.readfp(2, 2), *opts)
        self.dataddr = 4 + mapper_len
        self.chrsize = (self.size ** 2) // (2 ** self.squash_level)
        self.halfsize = self.chrsize // 2
        
        # get font mapper
        mapper_bytes = self.readfp(4, mapper_len)
        self.mapper = array('H', mapper_bytes)
        del mapper_bytes
    
    def readfp(self, address, size):
        self.fp.seek(address, 0)
        return self.fp.read(size)

    def readchr(self, char):
        addr, ishalf = mapaddr(self.dataddr, ord(char), self.chrsize, self.halfsize, self.mapper)
        if ishalf:
            dat = self.readfp(addr, self.halfsize)
        else:
            dat = self.readfp(addr, self.chrsize)
        arr = bytearray(dat); alen = len(arr)
        niter = 2 ** self.squash_level
        arr += b'\x00' * (alen * (niter-1))
        _unsquash_dat(arr, alen, self.squash_level)
        return arr, ishalf

    def drawchr(self, char, tft, pos):
        arr, ishalf = self.readchr(char)
        if ishalf:
            tft.blit_buffer(gray2rgb565(arr), *pos, self.unit, self.size)
        else:
            tft.blit_buffer(gray2rgb565(arr), *pos, self.size, self.size)

    @micropython.native
    def drawline(self, strn, tft, pos, crop=True):
        utlen = textlen(strn)[-1]
        buf = bytearray(b'')
        for char in strn:
            arr, ishalf = self.readchr(char)
            buf += spin(arr, self.unit if ishalf else self.size)
        if crop:
            # deal with overflow
            # 注意：此方法可能会从中间截断全角字符
            uleft = (tft.width() - pos[0]) // self.unit
            if uleft < utlen:
                buf = buf[:uleft*self.unit*self.size]
                utlen = uleft
        buf = spin(buf, self.size)
        tft.blit_buffer(gray2rgb565(buf), *pos, utlen*self.unit, self.size)

    def drawbox(self, strn, tft, pos, size, spacing=None):
        spacing = self.size if spacing is None else spacing
        w, h = size
        
    def __del__(self):
        self.fp.close()