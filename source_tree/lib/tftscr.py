from machine import SPI, Pin
import st7789

TFA = 0
BFA = 0

_spi = SPI(
    1,
    baudrate=31250000,
    polarity=1,
    sck=Pin(18),
    mosi=Pin(23)
)

def config(buffer_size=0, options=0):
    tft = st7789.ST7789(
        _spi,
        240, 240,
        reset=Pin(4, Pin.OUT),
        #cs=Pin(13, Pin.OUT), # CS is connected to GND
        dc=Pin(2, Pin.OUT),
        backlight=Pin(5, Pin.OUT),
        color_order=st7789.RGB,
        inversion=True,
        rotation=0,
        options=options,
        buffer_size=buffer_size
    )
    tft.init(); tft.init()
    tft.vscrdef(TFA, tft.width(), BFA)
    tft.fill(st7789.BLACK)
    return tft
