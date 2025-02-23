#ifndef GAME_INIT_H
#define GAME_INIT_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include <PR/os_message.h>

#include "types.h"
#include "config.h"
#include "memory.h"
#include "input.h"

#define MARIO_ANIMS_POOL_SIZE 0x4000
#define DEMO_INPUTS_POOL_SIZE 0x800

struct GfxPool {
    Gfx buffer[GFX_POOL_SIZE];
    struct SPTask spTask;
};

enum ZBmodes {
    KEEP_ZBUFFER,
    CLEAR_ZBUFFER,
};

extern OSMesgQueue gGameVblankQueue;
extern OSMesgQueue gGfxVblankQueue;
extern OSMesg gGameMesgBuf[1];
extern OSMesg gGfxMesgBuf[1];
extern struct VblankHandler gGameVblankHandler;
extern uintptr_t gPhysicalFramebuffers[3];
extern uintptr_t gPhysicalZBuffer;
extern void* gMarioAnimsMemAlloc;
extern void* gDemoInputsMemAlloc;
extern struct SPTask* gGfxSPTask;
extern Gfx* gDisplayListHead;
extern u8* gGfxPoolEnd;
extern struct GfxPool* gGfxPool;
extern u8 gBorderHeight;
#ifdef VANILLA_STYLE_CUSTOM_DEBUG
extern u8 gCustomDebugMode;
#endif
extern u8* gAreaSkyboxStart[AREA_COUNT];
extern u8* gAreaSkyboxEnd[AREA_COUNT];
#ifdef EEP
extern s8 gEepromProbe;
#endif
#ifdef SRAM
extern s8 gSramProbe;
#endif

extern void (*gGoddardVblankCallback)(void);

// this area is the demo input + the header. when the demo is loaded in, there is a header the size
// of a single word next to the input list. this word is the current ID count.
extern struct DmaHandlerList gMarioAnimsBuf;
extern struct DmaHandlerList gDemoInputsBuf;

extern u8 gMarioAnims[];
extern u8 gDemoInputs[];

extern u16 sRenderedFramebuffer;
extern u16 sRenderingFramebuffer;
extern u32 gGlobalTimer;

void setup_game_memory(void);
void thread5_game_loop(UNUSED void* arg);
void clear_framebuffer(RGBA16FILL color);
void clear_viewport(Vp* viewport, RGBA16FILL color);
void make_viewport_clip_rect(Vp* viewport);
void init_rcp(s32 resetZB);
void end_master_display_list(void);
void render_init(void);
void select_gfx_pool(void);
void display_and_vsync(void);

#endif // GAME_INIT_H
