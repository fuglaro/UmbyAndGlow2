# Setup, load, and run the game.
# Built to be launched from the ThumbyColor menu.

import os
os.chdir(__file__.rpartition("/")[0])
import gc
gc.threshold(2000)
gc.enable()
import engine_main
from engine import tick as engine_tick, fps_limit, freq
from engine_draw import front_fb_data
from time import ticks_ms
gc.collect()
fps_limit(40)

freq(128000000)

import game
game.init(front_fb_data())

t = 0
eng_wait = 0
while True:

    game.tick()

    eng_start = ticks_ms()
    while not engine_tick():
        continue
    eng_wait += ticks_ms() - eng_start

    t += 1
    if t == 40:
        print("WAIT: %d%%" % (eng_wait // 10))
        gc.collect()
        print("FREE: %dK" % (gc.mem_free() // 1000))
        t = 0
        eng_wait = 0

