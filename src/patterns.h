#include "stdint.h"


#define PATTERN(N, F) patterns[N] = &F;
#define INIT_PATTERNS \
PATTERN(      0, ptrn_0 )\
PATTERN(      1, ptrn_checker8)\
PATTERN(      2, ptrn_checker168)\
PATTERN(      3, ptrn_diag_gradient50)
#define \
PATTERNS      4 // !!! Ensure PATTERNS (array length) is updated !!!


extern uint8_t (*patterns[PATTERNS]) (uint32_t, uint32_t);
void init_patterns();
