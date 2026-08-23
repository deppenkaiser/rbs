#pragma once

#include "rbs.h"

// Token-Definitionen
typedef enum {
    TOKEN_COUNTER = 0,
    TOKEN_INITIALIZE,
    TOKEN_CALCULATE_COUNTER,
    TOKEN_CHECK_STOP,
    TOKEN_STOP,
    TOKEN_COUNT
} SMToken;