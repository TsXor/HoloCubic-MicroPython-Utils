from lvgl import font_t
import struct, gc

helpers = []

class myFontHelper:
    def __init__(self, font_path, log_missing_char=False):
        self.fp = open(font_path, 'rb')
        
        # get header info
        self.size, self.squash_level, mapper_len = struct.unpack('<BBH', self.readfp(0, 4))
        self.unit = self.size // 2
        self.dataddr = 4 + mapper_len
        
        # get font mapper
        self.mapper_buf = self.readfp(4, mapper_len)

    def readfp(self, address, size):
        self.fp.seek(address, 0)
        return self.fp.read(size)

    def get_glyph_dsc(self, unicode_letter):
        uwidth = getuwidth(unicode_letter)
        w = self.unit * uwidth
        return struct.pack('<BB', w, self.size)

    def get_glyph_bitmap(self, unicode_letter):
        ratio = 2 ** self.squash_level
        uwidth = getuwidth(unicode_letter)
        addr = self.dataddr + mapaddr(unicode_letter, self.unit, self.mapper_buf) // ratio
        bmpsize = uwidth * self.size * self.unit // ratio
        niter = 2 ** self.squash_level
        dat = bytearray(self.readfp(addr, bmpsize) + b'\x00' * (bmpsize * (niter-1)))
        unsquash_dat(dat, self.squash_level)
        return dat

    def __del__(self):
        self.fp.close()

def myFont(font_path):
    global helpers
    dsc = myFontHelper(font_path)
    dsc_cb = dsc.get_glyph_dsc
    bmp_cb = dsc.get_glyph_bitmap
    dsctup = (dsc_cb, bmp_cb)
    helpers.append((dsc, dsctup))

    myf = assembler(font_t, dsctup)
    myf.line_height = dsc.size;                     # /*The real line height where any text fits*/
    myf.base_line = 0;                              # /*Base line measured from the top of line_height*/

    return myf