///////////////////////
// Bridge
//  between MicroPython and the C game code.
//

#include "py/obj.h"
#define MP_REG(F) mp_store_global(MP_QSTR_##F, MP_OBJ_FROM_PTR(&F##_obj));

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
bool spawn_active() {
    return spawn_entry;
}

///////////////////////
// MicroPython Bridge
//
static mp_obj_t wrap_init(mp_obj_t frame_buf_obj) {
    frame_buf = (mp_obj_array_t*)frame_buf_obj;
    init();
    return mp_const_none;
} static MP_DEFINE_CONST_FUN_OBJ_1(init_obj, wrap_init);

static mp_obj_t wrap_tick() {
    tick();
    return mp_const_none;
} static MP_DEFINE_CONST_FUN_OBJ_0(tick_obj, wrap_tick);

static mp_obj_t wrap_spawn_tick() {
    spawn_tick();
    return mp_const_none;
} static MP_DEFINE_CONST_FUN_OBJ_0(spawn_tick_obj, wrap_spawn_tick);

static mp_obj_t wrap_ready_for_events() {
    return mp_obj_new_bool(ready_for_events());
} static MP_DEFINE_CONST_FUN_OBJ_0(ready_for_events_obj, wrap_ready_for_events);

static mp_obj_t wrap_send_event(mp_uint_t n_args, const mp_obj_t *args) {
    send_event(
        mp_obj_get_int(args[0]),
        mp_obj_get_int(args[1]),
        mp_obj_get_int(args[2]),
        mp_obj_get_int(args[3]),
        mp_obj_get_int(args[4]),
        ((mp_obj_array_t*)(args[5]))->items);
    return mp_const_none;
} static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(send_event_obj, 6, 6, wrap_send_event);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY;
    mp_store_global(MP_QSTR_init, MP_OBJ_FROM_PTR(&init_obj));
    mp_store_global(MP_QSTR_tick, MP_OBJ_FROM_PTR(&tick_obj));
    mp_store_global(MP_QSTR_spawn_tick, MP_OBJ_FROM_PTR(&spawn_tick_obj));
    mp_store_global(MP_QSTR_ready_for_events, MP_OBJ_FROM_PTR(&ready_for_events_obj));
    mp_store_global(MP_QSTR_send_event, MP_OBJ_FROM_PTR(&send_event_obj));
    MP_DYNRUNTIME_INIT_EXIT;
}

