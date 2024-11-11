#include "stdint.h"


#define PATTERN(N, F) patterns[N] = &F;
#define INIT_PATTERNS \
PATTERN(      0, ptrn_0 )\
PATTERN(      1, ptrn_checker8_red)\
PATTERN(      2, ptrn_checker168_red)\
PATTERN(      3, ptrn_diag_gradient50)\
PATTERN(      4, ptrn_checker8)\
PATTERN(      5, ptrn_checker168)
#define \
PATTERNS      6 // !!! Ensure PATTERNS (array length) is updated !!!


extern uint8_t (*patterns[PATTERNS]) (int32_t, int32_t);
void init_patterns();
