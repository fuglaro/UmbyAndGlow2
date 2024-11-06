#include "stdint.h"
#include "py/dynruntime.h"

#define debug(...) mp_printf(&mp_plat_print, __VA_ARGS__);

uint16_t* get_frame_buf();

void spawn(void (*entry)(void));
bool spawn_active();
