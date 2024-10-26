///////////////////////
// Bridge
//  between MicroPython and the C game code.
//

#include "bridge.h"
#include "game.h"

///////////////////////
// MicroPython Setup
//
mp_obj_array_t* frame_buf;
uint16_t* get_frame_buf() {
    return frame_buf->items;
}

///////////////////////
// Multicore Helpers
//
void (*spawn_entry)(void);
static void spawn_tick() {
    if (spawn_entry != 0) {
        spawn_entry();
        spawn_entry = 0;
    }
}
void spawn(void (*entry)(void)) {
    spawn_entry = entry;
}
void spawn_wait() {
    while (spawn_entry != (void (*)(void)) 0) {}
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
static mp_obj_t wrap_spawn_tick() {
    spawn_tick();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(spawn_tick_obj, wrap_spawn_tick);
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY
    mp_store_global(MP_QSTR_init, MP_OBJ_FROM_PTR(&init_obj));
    mp_store_global(MP_QSTR_tick, MP_OBJ_FROM_PTR(&tick_obj));
    mp_store_global(MP_QSTR_spawn_tick, MP_OBJ_FROM_PTR(&spawn_tick_obj));
    MP_DYNRUNTIME_INIT_EXIT
}

