from chrimg import chrimg
from PIL import Image
import numpy as np
import pathlib
from chars import *

self_dir = pathlib.Path(__file__).parent
REPLACE_ASCII = 'abcdefghijklmnopqrstuvwxyz1234567890'

def ishow(arr):
    Image.fromarray(arr).show()

def squash_byte(arr, squash_level=1):
    '''通过丢弃精度，将多个byte中的数据塞进一个byte'''
    assert squash_level in (1, 2, 3)
    assert size % (2 ** squash_level) == 0
    offset = 2 ** (3 - squash_level)
    arr = arr.astype(np.int32) * (2 ** offset - 1) / 255
    arr = np.rint(arr).astype(np.uint8)
    stride = 2 ** squash_level
    narr = pack_byte(arr, stride)
    return narr

def pack_byte(arr, stride):
    assert arr.shape[-2] % stride == 0
    blk = arr.shape[-2] // stride
    offset = 8 // stride
    n = [arr[..., i*blk:(i+1)*blk, :] for i in range(stride)]
    narr = np.zeros_like(n[0], dtype=np.uint8)
    for i, a in enumerate(n):
        narr += a << np.uint8(i * offset)
    return narr

def main(size, squash_level=1):
    fontpath = self_dir / 'fonts' / 'sarasa-regular.ttc'
    fontindex = 25 # Sarasa Term SC
    _, chrimgs_ASCII = chrimg(fontpath, fontindex, size, ASCII_CHARS, True)
    for c in REPLACE_ASCII:
        img = Image.open(self_dir / 'ascii_replace' / str(size) / ('%s.png'%c)).convert('L')
        chrimgs_ASCII[ord(c)] = img
    _, chrimgs_HALF_PUNCT = chrimg(fontpath, fontindex, size, HALF_PUNCT_CHARS, True)
    _, chrimgs_FULL_PUNCT = chrimg(fontpath, fontindex, size, FULL_PUNCT_CHARS, False)
    chrmap, chrimgs_CHINESE = chrimg(fontpath, fontindex, size, CHINESE_CHARS, False)
    chrarrs_HALF = [np.asarray(i) for i in chrimgs_ASCII] + [np.asarray(i) for i in chrimgs_HALF_PUNCT]
    chrarrs_FULL = [np.asarray(i) for i in chrimgs_FULL_PUNCT] + [np.asarray(i) for i in chrimgs_CHINESE]
    chrbin_HALF = np.array(chrarrs_HALF)
    chrbin_FULL = np.array(chrarrs_FULL)
    chrbin_HALF = chrbin_HALF.astype(np.uint8)
    chrbin_FULL = chrbin_FULL.astype(np.uint8)
    if squash_level:
        chrbin_HALF = squash_byte(chrbin_HALF, squash_level)
        chrbin_FULL = squash_byte(chrbin_FULL, squash_level)
    chrmap = np.array(chrmap, dtype=np.uint16)
    with open(self_dir / 'font.bin', 'wb') as fp:
        kwopt = {'byteorder':'little', 'signed':False}
        chrmap_dat = chrmap.tobytes(order='C'); chrmap_len = len(chrmap_dat)
        chrbin_HALF_dat = chrbin_HALF.tobytes(order='C')
        chrbin_FULL_dat = chrbin_FULL.tobytes(order='C')
        fp.write(size.to_bytes(1, **kwopt))
        fp.write(squash_level.to_bytes(1, **kwopt))
        fp.write(chrmap_len.to_bytes(2, **kwopt))
        fp.write(chrmap_dat)
        fp.write(chrbin_HALF_dat)
        fp.write(chrbin_FULL_dat)

if __name__ == '__main__':
    import sys
    size = int(sys.argv[1])
    squash_level = int(sys.argv[2])
    main(size, squash_level)