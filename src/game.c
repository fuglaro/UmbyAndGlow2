///////////////////////
// Game
//

#include "bridge.h"
#include "stdint.h"
#include "palette.h"

uint16_t cols[256]; ///< 8 Bit Color Palette -> RGB565.

// Screen size relative parameters.
#define SCR_WID 128 ///< Screen Width.
#define SCR_HGT 128 ///< Screen Height.
#define SCR_MSK_WID 127 ///< Screen Width modulo mask.
#define SCR_MSK_HGT 127 ///< Screen Height modulo mask.

#define MID_UINT 2147483647 ///< Half the max u32 int.


// XXX

/// Pattern: pure 0 everywhere.
static uint8_t ptrn_0(uint32_t x, uint32_t y) {
    return 0;
}
/*
/// Pattern: 8x8 checkerboard.
static uint8_t ptrn_checker8(uint x, uint y) {
    return (((x>>3)+(y>>3))&1)*255;
}
*/
/// Pattern: 16x8 checkerboard.
static uint8_t ptrn_checker168(uint32_t x, uint32_t y) {
    return (((x>>4)+(y>>3))&1)*255;
}
/*
/// Pattern: diagonal gradient 0-50 wrapping.
static uint8_t ptrn_diag_gradient50(uint x, uint y) {
    return (x + y)%50;
}
*/




///////////////////////
// Paint Scroll Buffer
/**
* A 2D ring buffer for buffering a region of a 2D plane.
* Uses column major ordering so bytes flow down and wrap right.
* Has 8 bit color (256 color pallette) (4 pixels per i32 word).
* The buffer region is offset within the 2D plane with the x and y values.
* Attribute ptrn MUST be set before calling associated paintsb_* functions.
*/
struct PaintScrollBuffer {
    uint8_t buf[SCR_WID*SCR_HGT]; ///< Buffer for the region of the 2D plane.
    uint32_t x, y; ///< Offsets the 2D plane.
    uint8_t(*ptrn)(uint32_t,uint32_t); ///< The pattern function to use to fill the buffer.
};
/**
* Scroll the 4 bit color scrolling buffer laterally and/or vertically,
* filling the buffer with new content as needed.
* @param lateral Amount to scroll left and right.
* @param vertical Amount to scroll up and down.
*/
static void paintsb_scroll(struct PaintScrollBuffer* buf, int lateral, int vertical) {
    buf->x += lateral;
    buf->y += vertical;
    uint8_t* bb = buf->buf;
    uint8_t(*ptrn)(uint32_t,uint32_t) = buf->ptrn;
    // Fill space created by the the lateral shift.
    const uint32_t bx = buf->x + (lateral&-(lateral<0)); // min(x,x+lateral)
    const uint32_t bxe = buf->x + (lateral&-(lateral>0)); // max(x,x+lateral)
    const uint32_t by = buf->y;
    const uint32_t bye = by+SCR_HGT;
    for (uint32_t x = bx; x < bxe; x++) {
        uint32_t xp = (x&SCR_MSK_WID)*SCR_HGT;
        for (uint32_t y = by; y < bye; y++) {
            bb[xp + (y&SCR_MSK_HGT)] = ptrn(x, y);
        }
    }
    // Fill the remaining space created by the vertical shift. 
    const uint32_t bx2 = buf->x + (lateral&-(lateral>0)); // max(x,x+lateral)
    const uint32_t bxe2 = buf->x + (lateral&-(lateral<0)) + SCR_WID; // min(x,x+lateral) + WID
    const uint32_t by2 = buf->y + (vertical&-(vertical<0)); // min(y,y+vertical)
    const uint32_t bye2 = buf->y + (vertical&-(vertical>0)); // max(y,y+vertical)
    for (uint32_t x = bx2; x < bxe2; x++) {
        uint32_t xp = (x&SCR_MSK_WID)*SCR_HGT;
        for (uint32_t y = by2; y < bye2; y++) {
            bb[xp + (y&SCR_MSK_HGT)] = ptrn(x, y);
        }
    }
}
/**
* Fill the contents of the 4 bit color scrolling buffer,
* respecting the offset position of the buffer.
*/
static void paintsb_fill(struct PaintScrollBuffer* buf) {
    // Rollback the tape laterally so we can scroll it back into position,
    // filling it as we go.
    buf->x -= SCR_WID;
    paintsb_scroll(buf, SCR_WID, 0);
}



// The layers of the level.
struct PaintScrollBuffer back_scroll; ///< The non-interactive background layer.




/**
* Render all layers to the framebuffer.
*/
static void render() {
    const uint8_t* bb = back_scroll.buf;
    const uint32_t bx = back_scroll.x;
    const uint32_t by = back_scroll.y;
    uint16_t* fb = get_frame_buf();
    for (uint32_t x = 0; x < SCR_WID; x++) {
        uint32_t xp = ((bx+x)&SCR_MSK_WID)*SCR_HGT;
        for (uint32_t y = 0; y < SCR_HGT; y++) {
            fb[x + y*SCR_WID] = cols[bb[xp + ((by+y)&SCR_MSK_HGT)]];
        }
    }
}



/**
* Initialise all variables and starting values.
*/
void init() {
    debug("INIT\n");
    INIT_PALETTE // Setup the color palette.
    // Prepare the level.
    back_scroll.ptrn = &ptrn_0;
    back_scroll.x = back_scroll.y = MID_UINT;


    // XXX
    back_scroll.ptrn = &ptrn_checker168;


    // Load the level.
    paintsb_fill(&back_scroll);
    debug("READY\n");
}

/**
* Update the game state, and draw for one frame.
*/
void tick() {
    paintsb_scroll(&back_scroll, -1, -1); // XXX
    render();
}

