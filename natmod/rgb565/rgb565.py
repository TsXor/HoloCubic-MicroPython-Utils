from array import array

def gray2rgb565(a):
    #assert type(a) == bytearray
    o = bytearray(b'\x00'*(2*len(a)))
    _gray2rgb565(a, o)
    
    return o