///////////////////////
// Game
//

#include "bridge.h"
#include "stdint.h"
#include "palette.h"
#include "patterns.h"

uint16_t colors[256]; ///< 8 Bit Color Palette -> RGB565.

// Screen size relative parameters.
#define SCR_WID 128 ///< Screen Width.
#define SCR_HGT 128 ///< Screen Height.
#define SCR_MSK_WID 127 ///< Screen Width modulo mask.
#define SCR_MSK_HGT 127 ///< Screen Height modulo mask.

#define START 1073741823 ///< Beginning coordinate (quarter max uint32 or half max int32).




///////////////////////
// Paint Scroll Buffer
//
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
    struct PatternShift {
        uint8_t(*ptrn)(int32_t,int32_t); ///< Pattern used to fill the buffer.
        int32_t x; //< The offset to shift the pattern horizontally.
        int32_t y; //< The offset to shift the pattern vertically.
    } ptrna; ///< Left/top pattern used to fille th buffer.
    struct PatternShift ptrnb; ///< Right/bottom pattern used to fill the buffer.
    uint32_t switch_pos; //< The lateral or depth position to switch patterns.
    bool depth_switch; //< Whether the switch is a depth stack switch (rather than lateral).
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

    // lat < 0 ? x : x+WID-lat
    const uint32_t bx = buf->x + ((SCR_WID-lateral)&-(lateral>0));
    // lat < 0 ? x-lat : x+WID
    const uint32_t bxe = buf->x - lateral + ((SCR_WID+lateral)&-(lateral>0));
    const uint32_t by = buf->y;
    const uint32_t bye = by+SCR_HGT;
    // lat > 0 ? x : x-lat
    const uint32_t bx2 = buf->x - (lateral&-(lateral<0));
    // lat < 0 ? x+WID : x+WID-lat
    const uint32_t bxe2 = buf->x + SCR_WID - (lateral&-(lateral>0));
    // vert < 0 ? y : y+HGT-vert
    const uint32_t by2 = buf->y + ((SCR_HGT-vertical)&-(vertical>0));
    // vert < 0 ? y-vert : y+HGT
    const uint32_t bye2 = buf->y - vertical + ((SCR_HGT+vertical)&-(vertical>0));

    struct PatternShift* ptrn;
    struct PatternShift* ptrna = &buf->ptrna;
    struct PatternShift* ptrnb = &buf->ptrnb;
    bool dep = buf->depth_switch;
    uint32_t pos = buf->switch_pos;
    // Fill space created by the the lateral shift.
    for (uint32_t x = bx; x < bxe; x++) {
        uint32_t xp = (x&SCR_MSK_WID)*SCR_HGT;
        for (uint32_t y = by; y < bye; y++) {
            ptrn = ((dep && y < pos) || (!dep && x < pos)) ? ptrna : ptrnb;
            bb[xp + (y&SCR_MSK_HGT)] = ptrn->ptrn(x + ptrn->x, y + ptrn->y);
        }
    }
    // Fill the remaining space created by the vertical shift.
    for (uint32_t x = bx2; x < bxe2; x++) {
        uint32_t xp = (x&SCR_MSK_WID)*SCR_HGT;
        for (uint32_t y = by2; y < bye2; y++) {
            ptrn = ((dep && y < pos) || (!dep && x < pos)) ? ptrna : ptrnb;
            bb[xp + (y&SCR_MSK_HGT)] = ptrn->ptrn(x + ptrn->x, y + ptrn->y);
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
/**
* Render all layers to the framebuffer for a range of columns.
* @param x1 Start of the range to render.
* @param x2 The position of the range to stop rendering.
*/
static void render(const int x1, const int x2) {
    const uint8_t* fb = front_scroll.buf;
    const uint32_t fx = front_scroll.x;
    const uint32_t fy = front_scroll.y;
    const uint8_t* bb = back_scroll.buf;
    const uint32_t bx = back_scroll.x;
    const uint32_t by = back_scroll.y;
    uint16_t* screen = get_frame_buf();
    for (uint32_t x = x1; x < x2; x++) {
        const uint8_t* fbx = fb + ((fx+x)&SCR_MSK_WID)*SCR_HGT;
        const uint8_t* bbx = bb + ((bx+x)&SCR_MSK_WID)*SCR_HGT;
        uint16_t* screenx = screen + x;
        for (uint32_t y = 0; y < SCR_HGT; y++) {
            uint8_t pix = fbx[(fy+y)&SCR_MSK_HGT];
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
    8=TILE(VERTICAL_CHANGE), 9=TILE(LATERAL_CHANGE)
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
    // 4,5=BACKGROUND, 6,7=FOREGROUND
    // 4,6=VERTICAL_CHANGE, 5,7=LATERAL_CHANGE
    else if (type >= 4 && type <= 7) {
        if (i1 >= PATTERNS) return; // Overflow.
        // ARGS:: pattern, offsetX, offsetY, distance
        struct PaintScrollBuffer* buf = (type < 6) ? &back_scroll : &front_scroll;
        buf->depth_switch = !(type%2);
        int32_t x = i2 - checkpoint.x;
        int32_t y = i3 - checkpoint.y;
        uint32_t orig_pos = (buf->depth_switch ? checkpoint.y : checkpoint.x);
        buf->switch_pos = orig_pos + i4;
        if (type <= 5) { // Handle slower background scrolling.
            x = i2 - (checkpoint.x>>1) - (SCR_WID>>2);
            y = i3 - (checkpoint.y>>1) - ((SCR_HGT>>2));
            buf->switch_pos = (buf->switch_pos>>1) + (SCR_WID>>2);
        }
        if (!i4) { // Set as both patterns.
            buf->ptrna.ptrn = patterns[i1];
            buf->ptrna.x = x;
            buf->ptrna.y = y;
            buf->ptrnb.ptrn = patterns[i1];
            buf->ptrnb.x = x;
            buf->ptrnb.y = y;
            paintsb_fill(buf);
        } else { // Reorder existing current pattern and add new one.
            bool atside2 = ((buf->depth_switch && player.y > buf->switch_pos)
                || (!buf->depth_switch && player.x > buf->switch_pos ));
            struct PatternShift* current = atside2 ? &buf->ptrnb : &buf->ptrna;
            struct PatternShift* other = atside2 ? &buf->ptrna : &buf->ptrnb;
            struct PatternShift* target = i4 > 0 ? &buf->ptrnb : &buf->ptrna;
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
    INIT_PALETTE // Setup the color palette.
    init_patterns(); // Setup the patterns index table.

    // Initialise default values.
    cam.x = START - (SCR_WID>>1);
    cam.y = START - (SCR_HGT>>1);
    back_scroll.ptrna.ptrn = patterns[0/*ptrn_0*/];
    back_scroll.ptrnb.ptrn = patterns[0/*ptrn_0*/];
    back_scroll.x = cam.x>>1;
    back_scroll.y = cam.y>>1;
    front_scroll.ptrna.ptrn = patterns[0/*ptrn_0*/];
    front_scroll.ptrnb.ptrn = patterns[0/*ptrn_0*/];
    front_scroll.x = cam.x;
    front_scroll.y = cam.y;
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

    // Render to the screen buffer on both cores.
    spawn(&render2);
    render1();
    while (spawn_active()) {}

}

