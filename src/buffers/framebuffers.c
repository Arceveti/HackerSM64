#include <ultra64.h>

#include "config.h"
#include "framebuffers.h"

// 0x70800 bytes
ALIGNED64 RGBA16 gFramebuffers[3][SCREEN_SIZE];
