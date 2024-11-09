#include "stdint.h"
#include "py/dynruntime.h"

#ifdef DEBUG
#define debug(...) mp_printf(&mp_plat_print, __VA_ARGS__);
#else
#define debug(...)
#endif

uint16_t* get_frame_buf();

void spawn(void (*entry)(void));
bool spawn_active();
