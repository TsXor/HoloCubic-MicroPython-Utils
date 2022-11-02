import os, machine

sd = machine.SDCard(
    slot=2,
    sck=14,
    cs=15,
    miso=26,
    mosi=13
)

def mountsd(path):
    os.mount(sd, path)