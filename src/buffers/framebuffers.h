#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#include <PR/ultratypes.h>

#include "config.h"
#include "types.h"

// level_script.c assumes that the frame buffers are adjacent, while game.c's
// -g codegen implies that they are separate variables. This is impossible to
// reconcile without undefined behavior. Avoid that when possible.
extern RGBA16 gFramebuffers[3][SCREEN_SIZE];
#define gFramebuffer0 gFramebuffers[0]
#define gFramebuffer1 gFramebuffers[1]
#define gFramebuffer2 gFramebuffers[2]

#define FRAMEBUFFER_SIZE (size_t)(SCREEN_SIZE * sizeof(RGBA16))

#endif // FRAMEBUFFERS_H
