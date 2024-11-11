
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

## Front Interaction Scroll Buffer
• 1 bit per pixel, 8 pixels per byte.
• air / ground.
• column major order.
• a full extra screen sized buffer in every direction for offscreen collision detection such that the full buffer size is 9 times that of the screen size.
• the bottom edge byte of each scrolling column is ignored and is used to progressively fill.
• 128*3*128*3 -> 18,434 bytes

## Front 8x8 Tile Scroll Buffer
• 4 bits per pixel, 2 pixels per byte.
• 15 tiles with alpha.
• column major order.
• a full extra screen sized buffer in every direction for offscreen collision detection such that the full buffer size is 9 times that of the screen size.
• 16*3*16*3*4 bits -> 9,216 bytes

## Hazards
* Certain color ranges cause player harm like lava.
* Fluid dynamics run on second core during parts of loop cycle where it can't to anything else - a form of busy wait.
* Needs their own buffer due to wider if dynamics run offscreen.
* Maybe no fluid dynamics...

## Player Characters
* Umby
* Glow
* Clip
* A.N.D.R.E.W (Autonomously Navigating Drone-Robot Earth-Worm)

## Weapons
* Grenade
* Robot

## Items
* Checkpoint
* Savepoint
