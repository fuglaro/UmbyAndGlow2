///////////////////////
// Game
//

#include "bridge.h"
#include "stdint.h"
#include "palette.h"
#include "patterns.h"
#include "tiles.h"

uint16_t colors[COLORS]; ///< 8 Bit Color Palette -> RGB565.
uint8_t tiles[TILES][TLE_LEN]; ///< 8 Bit Tile Intex.


// Screen size relative parameters.
#define SCR_WID 128 ///< Screen Width.
#define SCR_HGT 128 ///< Screen Height.
#define SCR_MSK_WID 127 ///< Screen Width modulo mask.
#define SCR_MSK_HGT 127 ///< Screen Height modulo mask.
#define TBF_WID 64 ///< Tile Buffer Width.
#define TBF_HGT 64 ///< Tile Buffer Height.
#define TBF_MSK_WID 63 ///< Tile Buffer Width modulo mask.
#define TBF_MSK_HGT 63 ///< Tile Buffer Height modulo mask.
#define TBF_OVER 24 //< The number of tiles off the edge of the screen.

#define START 1073741823 ///< Beginning coordinate (quarter max uint32 or half max int32).




///////////////////////
// Scroll Buffers
//
struct PatternShift {
    uint8_t(*ptrn)(int32_t,int32_t); ///< Pattern used to fill the buffer.
    int32_t x; //< The offset to shift the pattern horizontally.
    int32_t y; //< The offset to shift the pattern vertically.
}; //< Represents a pattern with an offset.
struct PatternSwitch {
    struct PatternShift ptrna; ///< Left/top pattern used to fill the buffer.
    struct PatternShift ptrnb; ///< Right/bottom pattern used to fill the buffer.
    uint32_t switch_pos; //< The lateral or depth position to switch patterns.
    bool depth_switch; //< Whether the switch is a depth stack switch (rather than lateral).
}; //< Represents two offset patterns with a vertical or lateral position change between then.


///////////////////////
// Tile Scroll Buffer
//
/**
* A 2D ring buffer for buffering a region of a tiled 2D plane.
* Tiles are 8x8 pixels in size.
* Uses column major ordering so bytes flow down and wrap right.
* Has 8 bit color (256 tile pallette) (4 pixels per i32 word).
* The buffer region is offset within the 2D plane with the x and y values.
* Attribute ptrn MUST be set before calling associated tilesb_* functions.
* Allows up to 8 superimposed patterns to feed the buffer.
* Includes an extra 192 pixels of data in all directions beyond the screen,
*   to allow for easy offscreen collision detection.
*/
struct TileScrollBuffer {
    uint8_t buf[TBF_WID*TBF_HGT]; ///< Buffer for the region of the 2D plane.
    uint32_t x, y; ///< Offsets the 2D plane.
    #define TBF_PTRNS 8
    #define TBF_MSK_PTRNS 7
    struct PatternSwitch ptrns[TBF_PTRNS]; // Stack of patterns that have position changes.
};
/**
* Scroll the 4 bit tile scrolling buffer laterally and/or vertically,
* filling the buffer with new content as needed.
* @param lateral Amount to scroll left and right.
* @param vertical Amount to scroll up and down.
*/
static void tilesb_scroll(struct TileScrollBuffer* buf, const int lateral, const int vertical) {
    uint32_t ox = buf->x;
    uint32_t oy = buf->y;
    buf->x += lateral;
    buf->y += vertical;
    uint32_t gx = buf->x>>3;
    uint32_t gy = buf->y>>3;
    int lat = gx - (ox>>3);
    int vert = gy - (oy>>3);
    uint8_t* bb = buf->buf;

    // 1=Fill space created by the the lateral shift.
    // 2=Fill the remaining space created by the vertical shift.
    const uint32_t rxa[2] = {
        gx + ((TBF_WID-lat)&-(lat>0)), // lat < 0 ? x : x+WID-lat
        gx - (lat&-(lat<0)) // lat > 0 ? x : x-lat
    };
    const uint32_t rxb[2] = {
        gx - lat + ((TBF_WID+lat)&-(lat>0)), // lat < 0 ? x-lat : x+WID
        gx + TBF_WID - (lat&-(lat>0)) // lat < 0 ? x+WID : x+WID-lat
    };
    const uint32_t rya[2] = {
        gy,
        gy + ((TBF_HGT-vert)&-(vert>0)), // vert < 0 ? y : y+HGT-vert
    };
    const uint32_t ryb[2] = {
        gy + TBF_HGT,
        gy - vert + ((TBF_HGT+vert)&-(vert>0)) // vert < 0 ? y-vert : y+HGT
    };

    const struct PatternSwitch* ptrns = buf->ptrns;
    const struct PatternShift* ptrn;
    for (int pass = 0; pass < 2; pass++) { // lateral fill, then remaining vertical fill.
        for (uint32_t x = rxa[pass]; x < rxb[pass]; x++) {
            const uint32_t xp = (x&TBF_MSK_WID)*TBF_HGT;
            for (uint32_t y = rya[pass]; y < ryb[pass]; y++) {
                uint8_t tile = 0;
                // Fill the tile by looping through patterns until it's filled.
                for (int p = 0; p < TBF_MSK_PTRNS; p++) {
                    const bool dep = ptrns[p].depth_switch;
                    const uint32_t pos = ptrns[p].switch_pos;
                    ptrn = ((dep && y < pos) || (!dep && x < pos)) ? &ptrns[p].ptrna : &ptrns[p].ptrnb;
                    tile = ptrn->ptrn(x + ptrn->x, y + ptrn->y);
                    if (tile) break;
                }
                bb[xp + (y&TBF_MSK_HGT)] = tile;
            }
        }
    }
}
/**
* Fill the contents of the 4 bit tile scrolling buffer,
* respecting the offset position of the buffer.
*/
static void tilesb_fill(struct TileScrollBuffer* buf) {
    // Rollback the tape laterally so we can scroll it back into position,
    // filling it as we go.
    buf->x -= TBF_WID;
    tilesb_scroll(buf, (TBF_WID<<3), 0);
}


///////////////////////
// Paint Scroll Buffer
//
/**
* A 2D ring buffer for buffering a region of a 2D plane.
* Uses column major ordering so bytes flow down and wrap right.
* Has 8 bit color (256 color pallette) (4 pixels per i32 word).
* The buffer region is offset within the 2D plane with the x and y values.
* Attribute ptrn MUST be set before calling associated paintsb_* functions.
* Is the same size of the screen.
*/
struct PaintScrollBuffer {
    uint8_t buf[SCR_WID*SCR_HGT]; ///< Buffer for the region of the 2D plane.
    uint32_t x, y; ///< Offsets the 2D plane.
    struct PatternSwitch ptrns; // The pattern with position changes.
};
/**
* Scroll the 4 bit color scrolling buffer laterally and/or vertically,
* filling the buffer with new content as needed.
* @param lateral Amount to scroll left and right.
* @param vertical Amount to scroll up and down.
*/
static void paintsb_scroll(struct PaintScrollBuffer* buf, const int lateral, const int vertical) {
    buf->x += lateral;
    buf->y += vertical;
    uint8_t* bb = buf->buf;

    // 1=Fill space created by the the lateral shift.
    // 2=Fill the remaining space created by the vertical shift.
    const uint32_t rxa[2] = {
        buf->x + ((SCR_WID-lateral)&-(lateral>0)), // lat < 0 ? x : x+WID-lat
        buf->x - (lateral&-(lateral<0)) // lat > 0 ? x : x-lat
    };
    const uint32_t rxb[2] = {
        buf->x - lateral + ((SCR_WID+lateral)&-(lateral>0)), // lat < 0 ? x-lat : x+WID
        buf->x + SCR_WID - (lateral&-(lateral>0)) // lat < 0 ? x+WID : x+WID-lat
    };
    const uint32_t rya[2] = {
        buf->y,
        buf->y + ((SCR_HGT-vertical)&-(vertical>0)), // vert < 0 ? y : y+HGT-vert
    };
    const uint32_t ryb[2] = {
        buf->y + SCR_HGT,
        buf->y - vertical + ((SCR_HGT+vertical)&-(vertical>0)) // vert < 0 ? y-vert : y+HGT
    };

    struct PatternShift* ptrn;
    struct PatternShift* ptrna = &buf->ptrns.ptrna;
    struct PatternShift* ptrnb = &buf->ptrns.ptrnb;
    const bool dep = buf->ptrns.depth_switch;
    const uint32_t pos = buf->ptrns.switch_pos;
    for (int pass = 0; pass < 2; pass++) { // lateral fill, then remaining vertical fill.
        for (uint32_t x = rxa[pass]; x < rxb[pass]; x++) {
            const uint32_t xp = (x&SCR_MSK_WID)*SCR_HGT;
            for (uint32_t y = rya[pass]; y < ryb[pass]; y++) {
                ptrn = ((dep && y < pos) || (!dep && x < pos)) ? ptrna : ptrnb;
                bb[xp + (y&SCR_MSK_HGT)] = ptrn->ptrn(x + ptrn->x, y + ptrn->y);
            }
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




///////////////////////
// Level Layers
//
struct PaintScrollBuffer back_scroll; ///< The non-interactive background layer.
struct PaintScrollBuffer front_scroll; ///< The front layer (interactivity handled separately).
struct TileScrollBuffer tile_scroll; //< The tiled front layer (just above the front paint layer).
/**
* Render all layers to the framebuffer for a range of columns.
* @param x1 Start of the range to render.
* @param x2 The position of the range to stop rendering.
*/
static void render(const int x1, const int x2) {
    const uint8_t* tb = tile_scroll.buf;
    const uint32_t tx = tile_scroll.x + 192;
    const uint32_t ty = tile_scroll.y + 192;
    const uint8_t* fb = front_scroll.buf;
    const uint32_t fx = front_scroll.x;
    const uint32_t fy = front_scroll.y;
    const uint8_t* bb = back_scroll.buf;
    const uint32_t bx = back_scroll.x;
    const uint32_t by = back_scroll.y;
    uint16_t* screen = get_frame_buf();
    for (uint32_t x = x1; x < x2; x++) {
        const uint32_t txx = tx + x;
        const uint8_t* tbx = tb + ((txx>>3)&TBF_MSK_WID)*TBF_HGT;
        const uint8_t* fbx = fb + ((fx+x)&SCR_MSK_WID)*SCR_HGT;
        const uint8_t* bbx = bb + ((bx+x)&SCR_MSK_WID)*SCR_HGT;
        uint16_t* screenx = screen + x;
        for (uint32_t y = 0; y < SCR_HGT; y++) {
            uint8_t pix = 0;
            const uint32_t tyy = ty + y;
            pix = tiles[tbx[(tyy>>3)&TBF_MSK_HGT]][(txx&TLE_MSK_WID) + (tyy&TLE_MSK_WID)*TLE_WID];
            if (!pix) pix = fbx[(fy+y)&SCR_MSK_HGT];
            if (!pix) pix = bbx[(by+y)&SCR_MSK_HGT];
            screenx[y*SCR_WID] = colors[pix];
        }
    }
}
/**
* Render all layers to the framebuffer for the left half of the screen.
*/
static void render1() {
    render(0, SCR_WID>>1);
}
/**
* Render all layers to the framebuffer for the right half of the screen.
*/
static void render2() {
    render(SCR_WID>>1, SCR_WID);
}




///////////////////////
// Game State
//
/// View position (top left of screen).
struct Point {
    uint32_t x;
    uint32_t y;
} cam;
/// The next player goal location or line (after any boss is defeated).
struct Checkpoint {
    #define CHKP_NONE 0
    #define CHKP_POSITIONAL 1
    #define CHKP_LATERAL 2
    #define CHKP_DEPTH 3
    uint8_t type;
    uint32_t x;
    uint32_t y;
} checkpoint;




///////////////////////
// Player State
//
/// The current player.
struct Player {
    uint32_t x;
    uint32_t y;
    void(*driver)();    
} player;
/**
* Automates the player, moving it through the goal locations,
* for testing and demos.
*/
static void driver_auto_clip() {
    player.x += checkpoint.x > player.x ? 1: checkpoint.x < player.x ? -1 : 0;
    player.y += checkpoint.y > player.y ? 1: checkpoint.y < player.y ? -1 : 0;
    // XXX TODO: search and destroy bosses.
}




///////////////////////
// Event Management
//
/**
* Check if the player has triggered the checkpoint, and if they have,
* remove the checkpoint goal so new script events can be recieved.
*/
void check_checkpoint() {
    # define CHKP_RANGE 8 // Box radius for triggering checkpoint.
    bool rx = player.x > checkpoint.x - CHKP_RANGE && player.x < checkpoint.x + CHKP_RANGE;
    bool ry = player.y > checkpoint.y - CHKP_RANGE && player.y < checkpoint.y + CHKP_RANGE;
    if ((checkpoint.type == CHKP_POSITIONAL && rx && ry)
        || (checkpoint.type == CHKP_LATERAL && rx)
        || (checkpoint.type == CHKP_DEPTH && ry)) checkpoint.type = CHKP_NONE;
}
/**
* Returns whether the game state is ready for more events from the script.
*/
bool ready_for_events() {
    return !checkpoint.type;
}
/**
* Recieves an event, and updates the game state accordingly.
* The parameters are dynamic based on the event type.
* See iterate_events inside script.py for details on the types and parameters.
*/
void send_event(const int type, const int i1, const int i2, const int i3, const int i4,
                const char* text) {
    debug("EVENT: %d %d %d %d %d '%s'\n", type, i1, i2, i3, i4, text);
    /* XXX
    10=DIALOG, 11=CHOICE
    12=SECRET
    13=BOSS, 14=SPAWN
    15=SPAWNER*/
    // 1=CHECKPOINT(POSITIONAL), 2=CHECKPOINT(LATERAL), 3=CHECKPOINT(DEPTH)
    if (type == CHKP_POSITIONAL || type == CHKP_LATERAL || type == CHKP_DEPTH) {
        checkpoint.type = type;
        checkpoint.x = player.x + i1;
        checkpoint.y = player.y + i2;
    }
    // 4,5=BACKGROUND, 6,7=FOREGROUND, 8-23=TILE(1-8)
    // even=VERTICAL_CHANGE, odd=LATERAL_CHANGE
    else if (type >= 4 && type <= 23) {
        if (i1 >= PATTERNS) return; // Overflow.
        // ARGS:: pattern, offsetX, offsetY, distance
        struct PaintScrollBuffer* buf = (type < 6) ? &back_scroll : &front_scroll;
        struct PatternSwitch* sw = &buf->ptrns;
        if (type >= 8) sw = &tile_scroll.ptrns[(type - 8)>>1];
        sw->depth_switch = !(type%2);
        int32_t x = i2 - checkpoint.x;
        int32_t y = i3 - checkpoint.y;
        uint32_t orig_pos = (sw->depth_switch ? checkpoint.y : checkpoint.x);
        sw->switch_pos = orig_pos + i4;
        if (type <= 5) { // Handle slower background scrolling.
            x = i2 - (checkpoint.x>>1) - (SCR_WID>>2);
            y = i3 - (checkpoint.y>>1) - ((SCR_HGT>>2));
            sw->switch_pos = (sw->switch_pos>>1) + (SCR_WID>>2);
        }
        if (type >= 8) { // Tile offset is in tile units, not pixel units.
            x = ((i2 - checkpoint.x)>>3);
            y = ((i3 - checkpoint.y)>>3);
            sw->switch_pos = (sw->switch_pos>>3) + TBF_OVER;
        }
        if (!i4) { // Set as both patterns.
            sw->ptrna.ptrn = patterns[i1];
            sw->ptrna.x = x;
            sw->ptrna.y = y;
            sw->ptrnb.ptrn = patterns[i1];
            sw->ptrnb.x = x;
            sw->ptrnb.y = y;
            if (type < 8) paintsb_fill(buf);
            else tilesb_fill(&tile_scroll);
        } else { // Reorder existing current pattern and add new one.
            bool atside2 = ((sw->depth_switch && player.y > sw->switch_pos)
                || (!sw->depth_switch && player.x > sw->switch_pos ));
            struct PatternShift* current = atside2 ? &sw->ptrnb : &sw->ptrna;
            struct PatternShift* other = atside2 ? &sw->ptrna : &sw->ptrnb;
            struct PatternShift* target = i4 > 0 ? &sw->ptrnb : &sw->ptrna;
            if (current == target) {
                other->ptrn = current->ptrn;
                other->x = current->x;
                other->y = current->y;
            }
            target->ptrn = patterns[i1];
            target->x = x;
            target->y = y;
        }
    }
}




/**
* Initialise all variables and starting values.
*/
void init() {
    debug("INIT\n");
    INIT_PALETTE; // Setup the color palette.
    INIT_TILES; // Setup the tiles index.
    init_patterns(); // Setup the patterns index table.

    // Initialise default values.
    cam.x = START - (SCR_WID>>1);
    cam.y = START - (SCR_HGT>>1);
    back_scroll.ptrns.ptrna.ptrn = patterns[0/*ptrn_0*/];
    back_scroll.ptrns.ptrnb.ptrn = patterns[0/*ptrn_0*/];
    back_scroll.x = cam.x>>1;
    back_scroll.y = cam.y>>1;
    front_scroll.ptrns.ptrna.ptrn = patterns[0/*ptrn_0*/];
    front_scroll.ptrns.ptrnb.ptrn = patterns[0/*ptrn_0*/];
    front_scroll.x = cam.x;
    front_scroll.y = cam.y;
    for (int i = 0; i < TBF_PTRNS; i++) {
        tile_scroll.ptrns[i].ptrna.ptrn = patterns[0/*ptrn_0*/];
        tile_scroll.ptrns[i].ptrnb.ptrn = patterns[0/*ptrn_0*/];
    }
    tile_scroll.x = cam.x;
    tile_scroll.y = cam.y;
    player.x = START;
    player.y = START;
    player.driver = &driver_auto_clip;
    checkpoint.x = START;
    checkpoint.y = START;

    debug("READY\n");
}

/**
* Update the game state, and draw for one frame.
*/
void tick() {

    // Update game state (4 sub-ticks per rendered tick).
    for (int i = 0; i < 4; i++) { 
        player.driver();
        check_checkpoint();
    }

    // Move camera.
    cam.x = player.x - (SCR_WID>>1); // XXX TODO: smoother camera motion.
    cam.y = player.y - (SCR_HGT>>1);
    paintsb_scroll(&back_scroll, (cam.x>>1) - back_scroll.x, (cam.y>>1) - back_scroll.y);
    paintsb_scroll(&front_scroll, cam.x - front_scroll.x, cam.y - front_scroll.y);
    tilesb_scroll(&tile_scroll, cam.x - tile_scroll.x, cam.y - tile_scroll.y);

    // Render to the screen buffer on both cores.
    spawn(&render2);
    render1();
    while (spawn_active()) {}

}

