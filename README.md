
# Build

* Ensure you have the following requirements:
  * CPython3
  * GNU make
  * A C compiler for the target architecture (e.g: arm-none-eabi-newlib)
* Setup dependencies:
```
git submodule update --init --recursive
pip install mpremote pyelftools
(cd micropython && make -C mpy-cross && cd ports/rp2 && make submodules && make clean && make)
```
* Connect your device via USB.
* Then build and run:
```
make V=1
./uploadrun
```

## Debug

* Debugging can be enabled with:
```
make clean
make V=1 CF=-DDEBUG && ./uploadrun
```

# Notes

* See https://docs.micropython.org/en/latest/develop/natmod.html#supported-features-and-limitations for notes on limitations.
* Micropython is loaded via a git submodule at version 1.23.0. You may want to update this.
* The architecture is set to build for an rp2350, and was tested on a Thumby Color Dev Kit. This can be changed in build.zig and Makefile.

# TODO and Design Notes

* Collision Scroll buffer (can be a different pattern).
* Foreground Paint Scroll Buffer carving from Collision Scroll buffer.
* Shading based on the Collision Scroll buffer, e.g: grass.
* Test level.
* Clipper.
* U&G's cave pattern.
* Umby.
* Glow.

## Other Things
* Checkpoint sprite.
* Green mission arrows (corners lit to indicate direction).
* Monsters (and spike).
* Grass shading on cave pattern.
* Dialog / choice screen (pauses gameplay, shows character faces).
* Make different face picture per emotion.
* 2 player comms (IGNORE UNTIL POST LAUNCH - must be funded since this is work not play).

## Collision Scroll Buffer
* 1 bit per pixel, 8 pixels per byte.
* air / ground.
* does not include tile buffer, which manages it's own collision.
* Caches destruction of non-tile foreground landscape.
* column major order.
* a full 192 pixels in every direction for offscreen collision detection.

## Hazards
* Certain color ranges cause player harm like lava.
* Fluid dynamics run on second core during parts of loop cycle where it can't to anything else - a form of busy wait.
* Needs their own buffer due to wider if dynamics run offscreen.
* Maybe no fluid dynamics...

## Player Characters
* Umby
* Glow
* Clipper (Test Worm that floats and hits objects and can explode.)
* A.N.D.R.E.W (Autonomously Navigating Drone-Robot Earth-Worm)

## Monsters
* Some monsters can be recollored by offsetting into the palette, including into palette cycling ranges.
* Slater (pillbug - recollorable)

## Weapons
* Grenade
* Robot

## Items
* Checkpoint
* Savepoint
