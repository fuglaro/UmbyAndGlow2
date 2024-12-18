///////////////////////
// Patterns
//  for procedurally generating levels.
//  All patterns take an x, y position,
//  and return an index into the 8 bit color palette.

#include "patterns.h"
#include "palette.h"


/// Pattern: pure 0 everywhere.
static uint8_t ptrn_0(int32_t x, int32_t y) {
    return 0;
}

/// Pattern: 8x8 checkerboard.
static uint8_t ptrn_checker8(int32_t x, int32_t y) {
    return (((x>>3)+(y>>3))&1);
}

/// Pattern: 16x8 checkerboard.
static uint8_t ptrn_checker168(int32_t x, int32_t y) {
    return (((x>>4)+(y>>3))&1);
}


/// Pattern: 8x8 checkerboard.
static uint8_t ptrn_checker8_red(int32_t x, int32_t y) {
    return (((x>>3)+(y>>3))&1)*255;
}

/// Pattern: 16x8 checkerboard.
static uint8_t ptrn_checker168_red(int32_t x, int32_t y) {
    return (((x>>4)+(y>>3))&1)*255;
}

/// Pattern: diagonal gradient 0-50 wrapping.
static uint8_t ptrn_diag_gradient50(int32_t x, int32_t y) {
    int32_t xy = (x + y);
    return COL_GRAYS + (xy<0?-xy:xy)%50*COL_GRAYS_N/50;
}


uint8_t (*patterns[PATTERNS]) (int32_t, int32_t);

void init_patterns() { INIT_PATTERNS }

