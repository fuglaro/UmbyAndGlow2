# Setup, load, and run the game.
# Built to be launched from the ThumbyColor menu.

import os
os.chdir(__file__.rpartition("/")[0])
import engine_main
from engine import tick as engine_tick, fps_limit
from engine_draw import front_fb_data
fps_limit(40)

import game
game.init(front_fb_data())

while True:
    game.tick()
    while not engine_tick():
        continue
