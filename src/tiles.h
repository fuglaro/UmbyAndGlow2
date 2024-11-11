// Define the 8 Bit Tiles and provide helper macros.
#include "stdint.h"

#define TILES 256
#define TLE_WID 8
#define TLE_MSK_WID 7
#define TLE_LEN 64

extern uint8_t tiles[TILES][TLE_LEN];

#define T(I) for (int i=0;i<TLE_LEN;i++) tiles[I][i] = tile##I[i];
#define INIT_TILES \
uint8_t tile1[TLE_LEN] = {\
9,9,9,  9,  9,9,9,9,\
9,1,1,  1,  1,1,1,9,\
9,1,1,  1,  1,1,1,9,\
9,1,1,255,255,1,1,9,\
9,1,1,255,255,1,1,9,\
9,1,1,  1,  1,1,1,9,\
9,1,1,  1,  1,1,1,9,\
9,9,9,  9,  9,9,9,9\
};T(1)


