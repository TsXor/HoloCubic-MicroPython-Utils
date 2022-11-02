def textlen(strn):
    out = bytearray(b'\x00'*len(strn))
    _textlen(strn.encode(), out)
    return out