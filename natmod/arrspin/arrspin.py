def spin(arr, width):
    out = bytearray(b'\x00'*len(arr))
    _spin(arr, width, out)
    return out