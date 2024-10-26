
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
uploadrun
```

# Notes

* See https://docs.micropython.org/en/latest/develop/natmod.html#supported-features-and-limitations for notes on limitations.
* Micropython is loaded via a git submodule at version 1.23.0. You may want to update this.
* The architecture is set to build for an rp2350, and was tested on a Thumby Color Dev Kit. This can be changed in build.zig and Makefile.

