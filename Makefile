
# Location of top-level MicroPython directory
MPY_DIR = micropython

# Name of module
MOD = game

# Source files (.c or .py)
SRC = src/game.c src/bridge.c

# Architecture to build for (x86, x64, armv6m, armv7m, xtensa, xtensawin)
ARCH = armv7emsp

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk

# Ensure header dependencies are respected in rebuilds.
build/src/game.o: | src/palette.h src/bridge.h
build/src/bridge.o: | src/game.h
