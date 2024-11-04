# Setup, load, and run the game.
# Built to be launched from the ThumbyColor menu.

import gc
gc.threshold(2000)
gc.enable()
root = __file__.rpartition("/")[0]
import os
os.chdir(root)
import _thread
import engine_main
from engine import tick as engine_tick, fps_limit, freq
from engine_draw import front_fb_data
from time import ticks_ms
from script import events_batcher
gc.collect()
fps_limit(40)
freq(128000000)

# Initialise the C code.
import game
game.init(front_fb_data())

# Initialise the story.
story = events_batcher(game)

# Ready the second core.
_thread.stack_size(4096)
@micropython.native
def spawn_loop(tick):
    while True:
        tick()
_thread.start_new_thread(spawn_loop, (game.spawn_tick,))

# Launch the game loop.
t = 0
eng_wait = 0
while True:

    # Update the game one tick.
    next(story)
    game.tick()

    # Wait for the next display update and profile.
    eng_start = ticks_ms()
    while not engine_tick():
        continue
    eng_wait += ticks_ms() - eng_start
    t += 1
    if t == 40:
        # XXX TODO: disable on 2 player comms.
        print("WAIT: %d%%" % (eng_wait // 10))
        gc.collect()
        print("FREE: %dK" % (gc.mem_free() // 1000))
        t = 0
        eng_wait = 0

