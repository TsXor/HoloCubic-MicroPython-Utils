from PIL import Image, ImageDraw, ImageFont
from functools import partial
import multiprocessing
from tqdm import tqdm, trange

MAX_PROC = 15

def drawchr(size, font, half, text):
    rsize = font.getsize(text+' 我')
    img = Image.new('L', rsize, 0)
    drawman = ImageDraw.Draw(img)
    drawman.text((0, 0), text, font=font, fill=255)
    rx, ry = rsize
    if half:
        img = img.crop((0, ry-size, size//2, ry))
    else:
        img = img.crop((0, ry-size, size, ry))
    return img

def worker(func, queue, task):
    for idx, elem in task:
        r = func(elem)
        queue.put((idx, r))

def distlist(alist, block):
    nblocks = (nelems := len(alist)) // block + 1
    return [[(idx, alist[idx]) for blk in range(nblocks) if (idx := blk*block+offset) < nelems] for offset in range(block)]

def mypool(alist, func, block=MAX_PROC, unit='it'):
    nelems = len(alist)
    newlist = [None for i in range(nelems)]
    iqueue = multiprocessing.Queue()
    tasks = distlist(alist, block)
    this_worker = partial(worker, func, iqueue)
    with tqdm(total=block, desc='Distributing tasks...', unit='proc', leave=False) as pbar:
        for task in tasks:
            proc = multiprocessing.Process(target=this_worker, args=(task,))
            proc.start(); pbar.update(1)
    for i in trange(nelems, unit=unit):
        idx, e = iqueue.get()
        newlist[idx] = e
    return newlist

def chrimg(fontpath, fontindex, fontsize, chars, half=False):
    font = ImageFont.truetype(str(fontpath), fontsize, fontindex)
    print('Generating images...')
    drawf = partial(drawchr, fontsize, font, half)
    chrlist = sorted(chars, key=lambda c: ord(c))
    chrmap = [ord(c) for c in chrlist]
    imglist = mypool(chrlist, drawf, unit='char')
    return chrmap, imglist
