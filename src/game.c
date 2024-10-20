///////////////////////
// MicroPython Setup
//
#include "py/dynruntime.h"
#define debug(...) mp_printf(&mp_plat_print, __VA_ARGS__);
mp_obj_array_t* frame_buf;
uint16_t* get_frame_buf() {
    return frame_buf->items;
}








///////////////////////
// Game
//

uint16_t cols[256]; ///< 8 Bit Color Palette -> RGB565.
/// Generate RGB565 from RGB888.
#define C(R,G,B) ( ((R*31/255)<<11) + ((G*63/255)<<5) + (B*31/255) )
// The 8 Bit Color Palette.
#define COL0   C(   0,   0,   0 )
#define COL1   C(   0,   0,   0 )
#define COL2   C(   0,   0,   0 )
#define COL3   C(   0,   0,   0 )
#define COL4   C(   0,   0,   0 )
#define COL5   C(   0,   0,   0 )
#define COL6   C(   0,   0,   0 )
#define COL7   C(   0,   0,   0 )
#define COL8   C(   0,   0,   0 )
#define COL9   C(   0,   0,   0 )
#define COL10  C(   0,   0,   0 )
#define COL11  C(   0,   0,   0 )
#define COL12  C(   0,   0,   0 )
#define COL13  C(   0,   0,   0 )
#define COL14  C(   0,   0,   0 )
#define COL15  C(   0,   0,   0 )
#define COL16  C(   0,   0,   0 )
#define COL17  C(   0,   0,   0 )
#define COL18  C(   0,   0,   0 )
#define COL19  C(   0,   0,   0 )
#define COL20  C(   0,   0,   0 )
#define COL21  C(   0,   0,   0 )
#define COL22  C(   0,   0,   0 )
#define COL23  C(   0,   0,   0 )
#define COL24  C(   0,   0,   0 )
#define COL25  C(   0,   0,   0 )
#define COL26  C(   0,   0,   0 )
#define COL27  C(   0,   0,   0 )
#define COL28  C(   0,   0,   0 )
#define COL29  C(   0,   0,   0 )
#define COL30  C(   0,   0,   0 )
#define COL31  C(   0,   0,   0 )
#define COL32  C(   0,   0,   0 )
#define COL33  C(   0,   0,   0 )
#define COL34  C(   0,   0,   0 )
#define COL35  C(   0,   0,   0 )
#define COL36  C(   0,   0,   0 )
#define COL37  C(   0,   0,   0 )
#define COL38  C(   0,   0,   0 )
#define COL39  C(   0,   0,   0 )
#define COL40  C(   0,   0,   0 )
#define COL41  C(   0,   0,   0 )
#define COL42  C(   0,   0,   0 )
#define COL43  C(   0,   0,   0 )
#define COL44  C(   0,   0,   0 )
#define COL45  C(   0,   0,   0 )
#define COL46  C(   0,   0,   0 )
#define COL47  C(   0,   0,   0 )
#define COL48  C(   0,   0,   0 )
#define COL49  C(   0,   0,   0 )
#define COL50  C(   0,   0,   0 )
#define COL51  C(   0,   0,   0 )
#define COL52  C(   0,   0,   0 )
#define COL53  C(   0,   0,   0 )
#define COL54  C(   0,   0,   0 )
#define COL55  C(   0,   0,   0 )
#define COL56  C(   0,   0,   0 )
#define COL57  C(   0,   0,   0 )
#define COL58  C(   0,   0,   0 )
#define COL59  C(   0,   0,   0 )
#define COL60  C(   0,   0,   0 )
#define COL61  C(   0,   0,   0 )
#define COL62  C(   0,   0,   0 )
#define COL63  C(   0,   0,   0 )
#define COL64  C(   0,   0,   0 )
#define COL65  C(   0,   0,   0 )
#define COL66  C(   0,   0,   0 )
#define COL67  C(   0,   0,   0 )
#define COL68  C(   0,   0,   0 )
#define COL69  C(   0,   0,   0 )
#define COL70  C(   0,   0,   0 )
#define COL71  C(   0,   0,   0 )
#define COL72  C(   0,   0,   0 )
#define COL73  C(   0,   0,   0 )
#define COL74  C(   0,   0,   0 )
#define COL75  C(   0,   0,   0 )
#define COL76  C(   0,   0,   0 )
#define COL77  C(   0,   0,   0 )
#define COL78  C(   0,   0,   0 )
#define COL79  C(   0,   0,   0 )
#define COL80  C(   0,   0,   0 )
#define COL81  C(   0,   0,   0 )
#define COL82  C(   0,   0,   0 )
#define COL83  C(   0,   0,   0 )
#define COL84  C(   0,   0,   0 )
#define COL85  C(   0,   0,   0 )
#define COL86  C(   0,   0,   0 )
#define COL87  C(   0,   0,   0 )
#define COL88  C(   0,   0,   0 )
#define COL89  C(   0,   0,   0 )
#define COL90  C(   0,   0,   0 )
#define COL91  C(   0,   0,   0 )
#define COL92  C(   0,   0,   0 )
#define COL93  C(   0,   0,   0 )
#define COL94  C(   0,   0,   0 )
#define COL95  C(   0,   0,   0 )
#define COL96  C(   0,   0,   0 )
#define COL97  C(   0,   0,   0 )
#define COL98  C(   0,   0,   0 )
#define COL99  C(   0,   0,   0 )
#define COL100 C(   0,   0,   0 )
#define COL101 C(   0,   0,   0 )
#define COL102 C(   0,   0,   0 )
#define COL103 C(   0,   0,   0 )
#define COL104 C(   0,   0,   0 )
#define COL105 C(   0,   0,   0 )
#define COL106 C(   0,   0,   0 )
#define COL107 C(   0,   0,   0 )
#define COL108 C(   0,   0,   0 )
#define COL109 C(   0,   0,   0 )
#define COL110 C(   0,   0,   0 )
#define COL111 C(   0,   0,   0 )
#define COL112 C(   0,   0,   0 )
#define COL113 C(   0,   0,   0 )
#define COL114 C(   0,   0,   0 )
#define COL115 C(   0,   0,   0 )
#define COL116 C(   0,   0,   0 )
#define COL117 C(   0,   0,   0 )
#define COL118 C(   0,   0,   0 )
#define COL119 C(   0,   0,   0 )
#define COL120 C(   0,   0,   0 )
#define COL121 C(   0,   0,   0 )
#define COL122 C(   0,   0,   0 )
#define COL123 C(   0,   0,   0 )
#define COL124 C(   0,   0,   0 )
#define COL125 C(   0,   0,   0 )
#define COL126 C(   0,   0,   0 )
#define COL127 C(   0,   0,   0 )
#define COL128 C(   0,   0,   0 )
#define COL129 C(   0,   0,   0 )
#define COL130 C(   0,   0,   0 )
#define COL131 C(   0,   0,   0 )
#define COL132 C(   0,   0,   0 )
#define COL133 C(   0,   0,   0 )
#define COL134 C(   0,   0,   0 )
#define COL135 C(   0,   0,   0 )
#define COL136 C(   0,   0,   0 )
#define COL137 C(   0,   0,   0 )
#define COL138 C(   0,   0,   0 )
#define COL139 C(   0,   0,   0 )
#define COL140 C(   0,   0,   0 )
#define COL141 C(   0,   0,   0 )
#define COL142 C(   0,   0,   0 )
#define COL143 C(   0,   0,   0 )
#define COL144 C(   0,   0,   0 )
#define COL145 C(   0,   0,   0 )
#define COL146 C(   0,   0,   0 )
#define COL147 C(   0,   0,   0 )
#define COL148 C(   0,   0,   0 )
#define COL149 C(   0,   0,   0 )
#define COL150 C(   0,   0,   0 )
#define COL151 C(   0,   0,   0 )
#define COL152 C(   0,   0,   0 )
#define COL153 C(   0,   0,   0 )
#define COL154 C(   0,   0,   0 )
#define COL155 C(   0,   0,   0 )
#define COL156 C(   0,   0,   0 )
#define COL157 C(   0,   0,   0 )
#define COL158 C(   0,   0,   0 )
#define COL159 C(   0,   0,   0 )
#define COL160 C(   0,   0,   0 )
#define COL161 C(   0,   0,   0 )
#define COL162 C(   0,   0,   0 )
#define COL163 C(   0,   0,   0 )
#define COL164 C(   0,   0,   0 )
#define COL165 C(   0,   0,   0 )
#define COL166 C(   0,   0,   0 )
#define COL167 C(   0,   0,   0 )
#define COL168 C(   0,   0,   0 )
#define COL169 C(   0,   0,   0 )
#define COL170 C(   0,   0,   0 )
#define COL171 C(   0,   0,   0 )
#define COL172 C(   0,   0,   0 )
#define COL173 C(   0,   0,   0 )
#define COL174 C(   0,   0,   0 )
#define COL175 C(   0,   0,   0 )
#define COL176 C(   0,   0,   0 )
#define COL177 C(   0,   0,   0 )
#define COL178 C(   0,   0,   0 )
#define COL179 C(   0,   0,   0 )
#define COL180 C(   0,   0,   0 )
#define COL181 C(   0,   0,   0 )
#define COL182 C(   0,   0,   0 )
#define COL183 C(   0,   0,   0 )
#define COL184 C(   0,   0,   0 )
#define COL185 C(   0,   0,   0 )
#define COL186 C(   0,   0,   0 )
#define COL187 C(   0,   0,   0 )
#define COL188 C(   0,   0,   0 )
#define COL189 C(   0,   0,   0 )
#define COL190 C(   0,   0,   0 )
#define COL191 C(   0,   0,   0 )
#define COL192 C(   0,   0,   0 )
#define COL193 C(   0,   0,   0 )
#define COL194 C(   0,   0,   0 )
#define COL195 C(   0,   0,   0 )
#define COL196 C(   0,   0,   0 )
#define COL197 C(   0,   0,   0 )
#define COL198 C(   0,   0,   0 )
#define COL199 C(   0,   0,   0 )
#define COL200 C(   0,   0,   0 )
#define COL201 C(   0,   0,   0 )
#define COL202 C(   0,   0,   0 )
#define COL203 C(   0,   0,   0 )
#define COL204 C(   0,   0,   0 )
#define COL205 C(   0,   0,   0 )
#define COL206 C(   0,   0,   0 )
#define COL207 C(   0,   0,   0 )
#define COL208 C(   0,   0,   0 )
#define COL209 C(   0,   0,   0 )
#define COL210 C(   0,   0,   0 )
#define COL211 C(   0,   0,   0 )
#define COL212 C(   0,   0,   0 )
#define COL213 C(   0,   0,   0 )
#define COL214 C(   0,   0,   0 )
#define COL215 C(   0,   0,   0 )
#define COL216 C(   0,   0,   0 )
#define COL217 C(   0,   0,   0 )
#define COL218 C(   0,   0,   0 )
#define COL219 C(   0,   0,   0 )
#define COL220 C(   0,   0,   0 )
#define COL221 C(   0,   0,   0 )
#define COL222 C(   0,   0,   0 )
#define COL223 C(   0,   0,   0 )
#define COL224 C(   0,   0,   0 )
#define COL225 C(   0,   0,   0 )
#define COL226 C(   0,   0,   0 )
#define COL227 C(   0,   0,   0 )
#define COL228 C(   0,   0,   0 )
#define COL229 C(   0,   0,   0 )
#define COL230 C(   0,   0,   0 )
#define COL231 C(   0,   0,   0 )
#define COL232 C(   0,   0,   0 )
#define COL233 C(   0,   0,   0 )
#define COL234 C(   0,   0,   0 )
#define COL235 C(   0,   0,   0 )
#define COL236 C(   0,   0,   0 )
#define COL237 C(   0,   0,   0 )
#define COL238 C(   0,   0,   0 )
#define COL239 C(   0,   0,   0 )
#define COL240 C(   0,   0,   0 )
#define COL241 C(   0,   0,   0 )
#define COL242 C(   0,   0,   0 )
#define COL243 C(   0,   0,   0 )
#define COL244 C(   0,   0,   0 )
#define COL245 C(   0,   0,   0 )
#define COL246 C(   0,   0,   0 )
#define COL247 C(   0,   0,   0 )
#define COL248 C(   0,   0,   0 )
#define COL249 C(   0,   0,   0 )
#define COL250 C(   0,   0,   0 )
#define COL251 C(   0,   0,   0 )
#define COL252 C(   0,   0,   0 )
#define COL253 C(   0,   0,   0 )
#define COL254 C(   0,   0,   0 )
#define COL255 C( 255,   0,   0 )

// Screen size relative parameters.
#define SCR_WID 128 ///< Screen Width.
#define SCR_HGT 128 ///< Screen Height.
#define SCR_MSK_WID 127 ///< Screen Width modulo mask.
#define SCR_MSK_HGT 127 ///< Screen Height modulo mask.

#define MID_UINT 2147483647 ///< Half the max u32 int.




/// Pattern: pure 0 everywhere.
uint8_t ptrn_0(uint x, uint y) {
    return 0;
}
/// Pattern: 8x8 checkerboard.
uint8_t ptrn_checker8(uint x, uint y) {
    return (((x>>3)+(y>>3))&1)*255;
}
/// Pattern: 16x8 checkerboard.
uint8_t ptrn_checker168(uint x, uint y) {
    return (((x>>4)+(y>>3))&1)*255;
}
/// Pattern: diagonal gradient 0-50 wrapping.
uint8_t ptrn_diag_gradient50(uint x, uint y) {
    return (x + y)%50;
}




/**
* Paint Scroll Buffer
* A 2D ring buffer for buffering a region of a 2D plane.
* Uses column major ordering so bytes flow down and wrap right.
* Has 8 bit color (256 color pallette) (4 pixels per i32 word).
* The buffer region is offset within the 2D plane with the x and y values.
* Attribute ptrn MUST be set before calling associated paintsb_* functions.
*/
struct PaintScrollBuffer {
    uint8_t buf[SCR_WID*SCR_HGT]; ///< Buffer for the region of the 2D plane.
    uint x, y; ///< Offsets the 2D plane.
    uint8_t(*ptrn)(uint,uint); ///< The pattern function to use to fill the buffer.
};
/**
* Scroll the 4 bit color scrolling buffer laterally and/or vertically,
* filling the buffer with new content as needed.
* @param lateral Amount to scroll left and right.
* @param vertical Amount to scroll up and down.
*/
void paintsb_scroll(struct PaintScrollBuffer* buf, int lateral, int vertical) {
    buf->x += lateral;
    buf->y += vertical;
    uint8_t* bb = buf->buf;
    uint8_t(*ptrn)(uint,uint) = buf->ptrn;
    // Fill space created by the the lateral shift.
    const uint bx = buf->x + (lateral&-(lateral<0)); // min(x,x+lateral)
    const uint bxe = buf->x + (lateral&-(lateral>0)); // max(x,x+lateral)
    const uint by = buf->y;
    const uint bye = by+SCR_HGT;
    for (uint x = bx; x < bxe; x++) {
        uint xp = (x&SCR_MSK_WID)*SCR_HGT;
        for (uint y = by; y < bye; y++) {
            bb[xp + (y&SCR_MSK_HGT)] = ptrn(x, y);
        }
    }
    // Fill the remaining space created by the vertical shift. 
    const uint bx2 = buf->x + (lateral&-(lateral>0)); // max(x,x+lateral)
    const uint bxe2 = buf->x + (lateral&-(lateral<0)) + SCR_WID; // min(x,x+lateral) + WID
    const uint by2 = buf->y + (vertical&-(vertical<0)); // min(y,y+vertical)
    const uint bye2 = buf->y + (vertical&-(vertical>0)); // max(y,y+vertical)
    for (uint x = bx2; x < bxe2; x++) {
        uint xp = (x&SCR_MSK_WID)*SCR_HGT;
        for (uint y = by2; y < bye2; y++) {
            bb[xp + (y&SCR_MSK_HGT)] = ptrn(x, y);
        }
    }
}
/**
* Fill the contents of the 4 bit color scrolling buffer,
* respecting the offset position of the buffer.
*/
void paintsb_fill(struct PaintScrollBuffer* buf) {
    // Rollback the tape laterally so we can scroll it back into position,
    // filling it as we go.
    buf->x -= SCR_WID;
    paintsb_scroll(buf, SCR_WID, 0);
}




struct PaintScrollBuffer back_scroll; ///< The non-interactive background layer.




/// Render all layers to the framebuffer.
void render() {
    const uint8_t* bb = back_scroll.buf;
    const uint bx = back_scroll.x;
    const uint by = back_scroll.y;
    uint16_t* fb = get_frame_buf();
    for (uint x = 0; x < SCR_WID; x++) {
        uint xp = ((bx+x)&SCR_MSK_WID)*SCR_HGT;
        for (uint y = 0; y < SCR_HGT; y++) {
            fb[x + y*SCR_WID] = cols[bb[xp + ((by+y)&SCR_MSK_HGT)]];
        }
    }
}



/// Initialise all variables and starting values.
static void init() {
    debug("INIT\n");
    // Setup the color pallette.
    cols[255] = C(255,90,90);
    #define P(I) cols[I] = COL##I;
    P(0)P(1)P(2)P(3)P(4)P(5)P(6)P(7)P(8)P(9)P(10)P(11)P(12)P(13)P(14)P(15)P(16)P(17)P(18)P(19)
    P(20)P(21)P(22)P(23)P(24)P(25)P(26)P(27)P(28)P(29)P(30)P(31)P(32)P(33)P(34)P(35)P(36)P(37)
    P(38)P(39)P(40)P(41)P(42)P(43)P(44)P(45)P(46)P(47)P(48)P(49)P(50)P(51)P(52)P(53)P(54)P(55)
    P(56)P(57)P(58)P(59)P(60)P(61)P(62)P(63)P(64)P(65)P(66)P(67)P(68)P(69)P(70)P(71)P(72)P(73)
    P(74)P(75)P(76)P(77)P(78)P(79)P(80)P(81)P(82)P(83)P(84)P(85)P(86)P(87)P(88)P(89)P(90)P(91)
    P(92)P(93)P(94)P(95)P(96)P(97)P(98)P(99)P(100)P(101)P(102)P(103)P(104)P(105)P(106)P(107)
    P(108)P(109)P(110)P(111)P(112)P(113)P(114)P(115)P(116)P(117)P(118)P(119)P(120)P(121)P(122)
    P(123)P(124)P(125)P(126)P(127)P(128)P(129)P(130)P(131)P(132)P(133)P(134)P(135)P(136)P(137)
    P(138)P(139)P(140)P(141)P(142)P(143)P(144)P(145)P(146)P(147)P(148)P(149)P(150)P(151)P(152)
    P(153)P(154)P(155)P(156)P(157)P(158)P(159)P(160)P(161)P(162)P(163)P(164)P(165)P(166)P(167)
    P(168)P(169)P(170)P(171)P(172)P(173)P(174)P(175)P(176)P(177)P(178)P(179)P(180)P(181)P(182)
    P(183)P(184)P(185)P(186)P(187)P(188)P(189)P(190)P(191)P(192)P(193)P(194)P(195)P(196)P(197)
    P(198)P(199)P(200)P(201)P(202)P(203)P(204)P(205)P(206)P(207)P(208)P(209)P(210)P(211)P(212)
    P(213)P(214)P(215)P(216)P(217)P(218)P(219)P(220)P(221)P(222)P(223)P(224)P(225)P(226)P(227)
    P(228)P(229)P(230)P(231)P(232)P(233)P(234)P(235)P(236)P(237)P(238)P(239)P(240)P(241)P(242)
    P(243)P(244)P(245)P(246)P(247)P(248)P(249)P(250)P(251)P(252)P(253)P(254)P(255)
    // Prepare the level.
    back_scroll.ptrn = &ptrn_0;
    back_scroll.x = back_scroll.y = MID_UINT;


    // XXX
    back_scroll.ptrn = &ptrn_checker168;


    // Load the level.
    paintsb_fill(&back_scroll);
    debug("READY\n");
}

/// Update the game state, and draw for one frame.
static void tick() {
    paintsb_scroll(&back_scroll, -1, -1); // XXX
    render();
}








///////////////////////
// MicroPython Bridge
//
static mp_obj_t wrap_init(mp_obj_t frame_buf_obj) {
    frame_buf = (mp_obj_array_t*)frame_buf_obj;
    init();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(init_obj, wrap_init);
static mp_obj_t wrap_tick() {
    tick();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(tick_obj, wrap_tick);
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    mp_store_global(MP_QSTR_init, MP_OBJ_FROM_PTR(&init_obj));
    mp_store_global(MP_QSTR_tick, MP_OBJ_FROM_PTR(&tick_obj));
    MP_DYNRUNTIME_INIT_EXIT
}

