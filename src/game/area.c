#include <PR/ultratypes.h>
#include <string.h>

#include "config.h"
#include "area.h"
#include "sm64.h"
#include "gfx_dimensions.h"
#include "behavior_data.h"
#include "game_init.h"
#include "object_list_processor.h"
#include "engine/surface_load.h"
#include "ingame_menu.h"
#include "screen_transition.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "print.h"
#include "hud.h"
#include "audio/external.h"
#include "area.h"
#include "rendering_graph_node.h"
#include "level_update.h"
#include "engine/geo_layout.h"
#include "save_file.h"
#include "level_table.h"
#include "dialog_ids.h"
#include "seq_ids.h"
#include "puppyprint.h"
#include "debug_box.h"
#include "engine/colors.h"
#include "joybus.h"
#include "profiling.h"
#include "fasttext.h"
#include "segment2.h"
#ifdef S2DEX_TEXT_ENGINE
#include "s2d_engine/init.h"
#endif

struct SpawnInfo gPlayerSpawnInfos[1];
struct GraphNode* gGraphNodePointers[MODEL_ID_COUNT];
struct Area gAreaData[AREA_COUNT];

struct WarpTransition gWarpTransition;

s16 gCurrCourseNum;
s16 gCurrActNum = 1;
s16 gCurrAreaIndex;
s16 gSavedCourseNum;
s16 gMenuOptSelectIndex;
s16 gSaveOptSelectIndex;

struct SpawnInfo* gMarioSpawnInfo = &gPlayerSpawnInfos[0];
struct GraphNode** gLoadedGraphNodes = gGraphNodePointers;
struct Area* gAreas = gAreaData;
struct Area* gCurrentArea = NULL;
struct CreditsEntry* gCurrCreditsEntry = NULL;

Vp* gViewportOverride = NULL;
Vp* gViewportClip = NULL;
s16 gWarpTransDelay = 0;
RGBA16FILL gFBSetColor = 0;
RGBA16FILL gWarpTransFBSetColor = 0;
Color gWarpTransRed= 0;
Color gWarpTransGreen = 0;
Color gWarpTransBlue = 0;
s16 gCurrSaveFileNum = 1;
s16 gCurrLevelNum = LEVEL_MIN;

/*
 * The following two tables are used in get_mario_spawn_type() to determine spawn type
 * from warp behavior.
 * When looping through sWarpSpawnTypes, if the behavior function in the table matches
 * the spawn behavior executed, the index of that behavior is used with sSpawnTypeFromWarpBhv
*/
struct WarpSpawnType sWarpSpawnTypes[] = {
    { .behavior = bhvDoorWarp,                .spawnType = MARIO_SPAWN_DOOR_WARP              },
    { .behavior = bhvStar,                    .spawnType = MARIO_SPAWN_IDLE                   },
    { .behavior = bhvExitPodiumWarp,          .spawnType = MARIO_SPAWN_PIPE                   },
    { .behavior = bhvWarp,                    .spawnType = MARIO_SPAWN_PIPE                   },
    { .behavior = bhvWarpPipe,                .spawnType = MARIO_SPAWN_PIPE                   },
    { .behavior = bhvFadingWarp,              .spawnType = MARIO_SPAWN_TELEPORT               },
    { .behavior = bhvInstantActiveWarp,       .spawnType = MARIO_SPAWN_INSTANT_ACTIVE         },
    { .behavior = bhvAirborneWarp,            .spawnType = MARIO_SPAWN_AIRBORNE               },
    { .behavior = bhvHardAirKnockBackWarp,    .spawnType = MARIO_SPAWN_HARD_AIR_KNOCKBACK     },
    { .behavior = bhvSpinAirborneCircleWarp,  .spawnType = MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE   },
    { .behavior = bhvDeathWarp,               .spawnType = MARIO_SPAWN_DEATH                  },
    { .behavior = bhvSpinAirborneWarp,        .spawnType = MARIO_SPAWN_SPIN_AIRBORNE          },
    { .behavior = bhvFlyingWarp,              .spawnType = MARIO_SPAWN_FLYING                 },
    { .behavior = bhvSwimmingWarp,            .spawnType = MARIO_SPAWN_SWIMMING               },
    { .behavior = bhvPaintingStarCollectWarp, .spawnType = MARIO_SPAWN_PAINTING_STAR_COLLECT  },
    { .behavior = bhvPaintingDeathWarp,       .spawnType = MARIO_SPAWN_PAINTING_DEATH         },
    { .behavior = bhvAirborneStarCollectWarp, .spawnType = MARIO_SPAWN_AIRBORNE_STAR_COLLECT  },
    { .behavior = bhvAirborneDeathWarp,       .spawnType = MARIO_SPAWN_AIRBORNE_DEATH         },
    { .behavior = bhvLaunchStarCollectWarp,   .spawnType = MARIO_SPAWN_LAUNCH_STAR_COLLECT    },
    { .behavior = bhvLaunchDeathWarp,         .spawnType = MARIO_SPAWN_LAUNCH_DEATH           },
};

Vp gViewport = {
    .vp = {
        .vscale = { (SCREEN_WIDTH * 2), (SCREEN_HEIGHT * 2), (G_MAXZ / 2), 0 },
        .vtrans = { (SCREEN_WIDTH * 2), (SCREEN_HEIGHT * 2), (G_MAXZ / 2), 0 },
    }
};

void override_viewport_and_clip(Vp* vpOverride, Vp* vpClip, Color red, Color green, Color blue) {
    RGBA16 color = RGBA_TO_RGBA16(red, green, blue, 0xFF);

    gFBSetColor = ((color << 16) | color);
    gViewportOverride = vpOverride;
    gViewportClip = vpClip;
}

void set_warp_transition_rgb(Color red, Color green, Color blue) {
    RGBA16 color = RGBA_TO_RGBA16(red, green, blue, 0xFF);

    gWarpTransFBSetColor = ((color << 16) | color);
    gWarpTransRed   = red;
    gWarpTransGreen = green;
    gWarpTransBlue  = blue;
}

void print_intro_text(void) {
    if ((gGlobalTimer & 31) < 20) {
#ifdef VERSION_EU
        print_text(20, 20, "START");
#else
        print_text_centered(60, 38, "PRESS");
        print_text_centered(60, 20, "START");
#endif
    }
}

u32 get_mario_spawn_type(struct Object* obj) {
    s32 i;
    const BehaviorScript* behavior = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, obj->behavior);

    for (i = 0; i < ARRAY_COUNT(sWarpSpawnTypes); i++) {
        if (sWarpSpawnTypes[i].behavior == behavior) {
            return sWarpSpawnTypes[i].spawnType;
        }
    }

    return MARIO_SPAWN_NONE;
}

struct ObjectWarpNode* area_get_warp_node(u8 id) {
    struct ObjectWarpNode* node = NULL;

    for (node = gCurrentArea->warpNodes; node != NULL; node = node->next) {
        if (node->node.id == id) {
            break;
        }
    }

    return node;
}

struct ObjectWarpNode* area_get_warp_node_from_params(struct Object* obj) {
    return area_get_warp_node(GET_BPARAM2(obj->oBehParams));
}

void load_obj_warp_nodes(void) {
    struct ObjectWarpNode* warpNode;
    struct Object* children = (struct Object*)gObjParentGraphNode.children;

    do {
        struct Object* obj = children;

        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && get_mario_spawn_type(obj) != 0) {
            warpNode = area_get_warp_node_from_params(obj);
            if (warpNode != NULL) {
                warpNode->object = obj;
            }
        }
    } while ((children = (struct Object*)children->header.gfx.node.next) != (struct Object*)gObjParentGraphNode.children);
}

void clear_areas(void) {
    s32 i, j;

    gCurrentArea = NULL;
    gWarpTransition.isActive = FALSE;
    gWarpTransition.pauseRendering = FALSE;
    gMarioSpawnInfo->areaIndex = -1;

    for (i = 0; i < ARRAY_COUNT(gAreaData); i++) {
        gAreaData[i].index = i;
        gAreaData[i].flags = AREA_FLAG_UNLOAD;
        gAreaData[i].terrainType = TERRAIN_GRASS;
        gAreaData[i].graphNode = NULL;
        gAreaData[i].terrainData = NULL;
        gAreaData[i].surfaceRooms = NULL;
        gAreaData[i].macroObjects = NULL;
        gAreaData[i].warpNodes = NULL;
        gAreaData[i].instantWarps = NULL;
        gAreaData[i].objectSpawnInfos = NULL;
        gAreaData[i].camera = NULL;
        gAreaData[i].unused = NULL;
        for (j = 0; j < ARRAY_COUNT(gAreaData[i].whirlpools); j++) {
            gAreaData[i].whirlpools[j] = NULL;
        }
        for (j = 0; j < ARRAY_COUNT(gAreaData[i].dialog); j++) {
            gAreaData[i].dialog[j] = DIALOG_NONE;
        }
        gAreaData[i].musicSettingsPreset = SEQ_SETTINGS_PRESET_DEFAULT;
        gAreaData[i].musicSeqId = SEQ_SOUND_PLAYER;
        gAreaData[i].useEchoOverride = FALSE;
        gAreaData[i].echoOverride = 0;
#ifdef BETTER_REVERB
        gAreaData[i].betterReverbPreset = 0;
#endif
    }
}

void clear_area_graph_nodes(void) {
    s32 i;

    if (gCurrentArea != NULL) {
        geo_call_global_function_nodes(&gCurrentArea->graphNode->node, GEO_CONTEXT_AREA_UNLOAD);
        gCurrentArea = NULL;
        gWarpTransition.isActive = FALSE;
    }

    for (i = 0; i < ARRAY_COUNT(gAreaData); i++) {
        if (gAreaData[i].graphNode != NULL) {
            geo_call_global_function_nodes(&gAreaData[i].graphNode->node, GEO_CONTEXT_AREA_INIT);
            gAreaData[i].graphNode = NULL;
        }
    }
}

void load_area(s32 index) {
    if (gCurrentArea == NULL && gAreaData[index].graphNode != NULL) {
        gCurrentArea = &gAreaData[index];
        gCurrAreaIndex = gCurrentArea->index;
        main_pool_pop_state();
        main_pool_push_state();

        gMarioCurrentRoom = 0;

        if (gCurrentArea->terrainData != NULL) {
            load_area_terrain(index,
                gCurrentArea->terrainData,
                gCurrentArea->surfaceRooms,
                gCurrentArea->macroObjects
            );
        }

        if (gCurrentArea->objectSpawnInfos != NULL) {
            spawn_objects_from_info(gCurrentArea->objectSpawnInfos);
        }

        load_obj_warp_nodes();
        geo_call_global_function_nodes(&gCurrentArea->graphNode->node, GEO_CONTEXT_AREA_LOAD);
    }
}

void unload_area(void) {
    if (gCurrentArea != NULL) {
        unload_objects_from_area(gCurrentArea->index);
        geo_call_global_function_nodes(&gCurrentArea->graphNode->node, GEO_CONTEXT_AREA_UNLOAD);

        gCurrentArea->flags = AREA_FLAG_UNLOAD;
        gCurrentArea = NULL;
        gWarpTransition.isActive = FALSE;
    }
}

void load_mario_area(void) {
    stop_sounds_in_continuous_banks();
    load_area(gMarioSpawnInfo->areaIndex);

    if (gCurrentArea->index == gMarioSpawnInfo->areaIndex) {
        gCurrentArea->flags |= AREA_FLAG_LOAD;
        spawn_objects_from_info(gMarioSpawnInfo);
    }

    if (gAreaSkyboxStart[gCurrAreaIndex - 1]) {
        load_segment_decompress(SEGMENT_SKYBOX, gAreaSkyboxStart[gCurrAreaIndex - 1], gAreaSkyboxEnd[gCurrAreaIndex - 1]);
    }
}

void unload_mario_area(void) {
    if ((gCurrentArea != NULL) && (gCurrentArea->flags & AREA_FLAG_LOAD)) {
        unload_objects_from_area(gMarioSpawnInfo->activeAreaIndex);

        gCurrentArea->flags &= ~AREA_FLAG_LOAD;
        if (gCurrentArea->flags == AREA_FLAG_UNLOAD) {
            unload_area();
        }
    }
}

void change_area(s32 index) {
    s32 areaFlags = gCurrentArea->flags;

    if (gCurrAreaIndex != index) {
        unload_area();
        load_area(index);

        gCurrentArea->flags = areaFlags;
        gMarioObject->oActiveParticleFlags = ACTIVE_PARTICLE_NONE;
    }

    if (areaFlags & AREA_FLAG_LOAD) {
        gMarioObject->header.gfx.areaIndex = index;
        gMarioSpawnInfo->areaIndex = index;
    }
}

void area_update_objects(void) {
    gAreaUpdateCounter++;
    update_objects();
}

/*
 * Sets up the information needed to play a warp transition, including the
 * transition type, time in frames, and the RGB color that will fill the screen.
 */
void play_transition(s16 transType, s16 time, Color red, Color green, Color blue) {
#ifndef L3DEX2_ALONE
    gWarpTransition.isActive = TRUE;
    gWarpTransition.type = transType;
    gWarpTransition.time = time;
    gWarpTransition.pauseRendering = FALSE;

    // The lowest bit of transType determines if the transition is fading in or out.
    if (transType & WARP_TRANSITION_FADE_INTO) {
        set_warp_transition_rgb(red, green, blue);
    } else {
        red   = gWarpTransRed;
        green = gWarpTransGreen;
        blue  = gWarpTransBlue;
    }

    if (transType < WARP_TRANSITION_TYPE_STAR) { // if transition is WARP_TRANSITION_TYPE_COLOR
        gWarpTransition.data.red   = red;
        gWarpTransition.data.green = green;
        gWarpTransition.data.blue  = blue;
    } else { // if transition is textured
        gWarpTransition.data.red   = red;
        gWarpTransition.data.green = green;
        gWarpTransition.data.blue  = blue;

        // Both the start and end textured transition are always located in the middle of the screen.
        // If you really wanted to, you could place the start at one corner and the end at
        // the opposite corner. This will make the transition image look like it is moving
        // across the screen.
        gWarpTransition.data.startTexX = SCREEN_CENTER_X;
        gWarpTransition.data.startTexY = SCREEN_CENTER_Y;
        gWarpTransition.data.endTexX   = SCREEN_CENTER_X;
        gWarpTransition.data.endTexY   = SCREEN_CENTER_Y;

        gWarpTransition.data.texTimer = 0;

        if (transType & WARP_TRANSITION_FADE_INTO) { // Is the image fading in?
            gWarpTransition.data.startTexRadius = GFX_DIMENSIONS_FULL_RADIUS;
            if (transType >= WARP_TRANSITION_FADES_INTO_LARGE) {
                gWarpTransition.data.endTexRadius = 16;
            } else {
                gWarpTransition.data.endTexRadius = 0;
            }
        } else { // The image is fading out. (Reverses start & end circles)
            if (transType >= WARP_TRANSITION_FADES_FROM_LARGE) {
                gWarpTransition.data.startTexRadius = 16;
            } else {
                gWarpTransition.data.startTexRadius = 0;
            }
            gWarpTransition.data.endTexRadius = GFX_DIMENSIONS_FULL_RADIUS;
        }
    }
#endif
}

/*
 * Sets up the information needed to play a warp transition, including the
 * transition type, time in frames, and the RGB color that will fill the screen.
 * The transition will play only after a number of frames specified by 'delay'
 */
void play_transition_after_delay(s16 transType, s16 time, Color red, Color green, Color blue, s16 delay) {
    gWarpTransDelay = delay; // Number of frames to delay playing the transition.
    play_transition(transType, time, red, green, blue);
}

#ifdef ENABLE_STATUS_REPOLLING_GUI
 #if (MAX_NUM_PLAYERS > 1)
// Button names, in print order.
ALIGNED8 static const struct ButtonName sButtonNames[16] = {
    { .mask = U_CBUTTONS,   .name = "C UP",    },
    { .mask = D_CBUTTONS,   .name = "C DOWN",  },
    { .mask = L_CBUTTONS,   .name = "C LEFT",  },
    { .mask = R_CBUTTONS,   .name = "C RIGHT", },
    { .mask = U_JPAD,       .name = "D UP",    },
    { .mask = D_JPAD,       .name = "D DOWN",  },
    { .mask = L_JPAD,       .name = "D LEFT",  },
    { .mask = R_JPAD,       .name = "D RIGHT", },
    { .mask = Z_TRIG,       .name = "Z",       },
    { .mask = L_TRIG,       .name = "L",       },
    { .mask = R_TRIG,       .name = "R",       },
    { .mask = A_BUTTON,     .name = "A",       },
    { .mask = B_BUTTON,     .name = "B",       },
    { .mask = X_BUTTON,     .name = "X",       },
    { .mask = Y_BUTTON,     .name = "Y",       },
    { .mask = START_BUTTON, .name = "START",   },
};

/**
 * @brief Creates a string from a combination of buttons and adds it to 'strp'.
 */
static size_t button_combo_to_string(char* strp, u16 buttons) {
    size_t count = 0;

    for (int i = 0; i < ARRAY_COUNT(sButtonNames); i++) {
        const struct ButtonName* buttonName = &sButtonNames[i];

        if (buttons & buttonName->mask) {
            buttons &= ~buttonName->mask;

            if (count) {
                strcat(strp, "+");
                count += strlen("+");
            }

            strcat(strp, buttonName->name);
            count += strlen(buttonName->name);
        }
    }

    return count;
}
 #endif // (MAX_NUM_PLAYERS > 1)

#define CONT_ICON_W 32
#define CONT_ICON_H 32

#ifdef CONTROLLERS_INPUT_DISPLAY

// This is 4 32x32 RGBA16 textures, 2048 bytes each, 8192 bytes total.
ALIGNED8 RGBA16 sInputOverlayTextures[MAXCONTROLLERS][CONT_ICON_W * CONT_ICON_H];


ALIGNED4 const ButtonHighlight buttons_display_controller_n64_normal[] = {
    { .mask = CONT_A,           .x = 22, .y = 13, .w = 2, .h = 2, },
    { .mask = CONT_B,           .x = 20, .y = 11, .w = 2, .h = 2, },
    { .mask = CONT_L,           .x =  6, .y =  5, .w = 4, .h = 1, },
    { .mask = CONT_R,           .x = 22, .y =  5, .w = 4, .h = 1, },
    { .mask = CONT_G,           .x = 15, .y =  7, .w = 2, .h = 3, },
    { .mask = CONT_START,       .x = 15, .y = 11, .w = 2, .h = 2, },
    { .mask = CONT_UP,          .x =  7, .y =  8, .w = 2, .h = 2, },
    { .mask = CONT_DOWN,        .x =  7, .y = 12, .w = 2, .h = 2, },
    { .mask = CONT_LEFT,        .x =  5, .y = 10, .w = 2, .h = 2, },
    { .mask = CONT_RIGHT,       .x =  9, .y = 10, .w = 2, .h = 2, },
    { .mask = CONT_E,           .x = 24, .y =  7, .w = 2, .h = 2, },
    { .mask = CONT_D,           .x = 24, .y = 11, .w = 2, .h = 2, },
    { .mask = CONT_C,           .x = 22, .y =  9, .w = 2, .h = 2, },
    { .mask = CONT_F,           .x = 26, .y =  9, .w = 2, .h = 2, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_n64_mouse[] = {
    { .mask = CONT_A,           .x = 10, .y =  8, .w = 5, .h = 5, },
    { .mask = CONT_B,           .x = 17, .y =  8, .w = 5, .h = 5, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_gba[] = {
    { .mask = CONT_A,           .x = 27, .y = 13, .w = 2, .h = 2, },
    { .mask = CONT_B,           .x = 24, .y = 14, .w = 2, .h = 2, },
    { .mask = CONT_L,           .x =  3, .y =  9, .w = 3, .h = 1, },
    { .mask = CONT_R,           .x = 26, .y =  9, .w = 3, .h = 1, },
    { .mask = CONT_G,           .x =  7, .y = 20, .w = 1, .h = 1, },
    { .mask = CONT_START,       .x =  7, .y = 10, .w = 1, .h = 1, },
    { .mask = CONT_UP,          .x =  5, .y = 13, .w = 1, .h = 1, },
    { .mask = CONT_DOWN,        .x =  5, .y = 15, .w = 1, .h = 1, },
    { .mask = CONT_LEFT,        .x =  4, .y = 14, .w = 1, .h = 1, },
    { .mask = CONT_RIGHT,       .x =  6, .y = 14, .w = 1, .h = 1, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_gcn_normal[] = {
    { .mask = CONT_GCN_START,   .x = 15, .y = 12, .w = 2, .h = 2, },
    { .mask = CONT_GCN_Y,       .x = 21, .y =  9, .w = 2, .h = 2, },
    { .mask = CONT_GCN_X,       .x = 25, .y = 10, .w = 2, .h = 2, },
    { .mask = CONT_GCN_B,       .x = 20, .y = 14, .w = 2, .h = 2, },
    { .mask = CONT_GCN_A,       .x = 22, .y = 11, .w = 3, .h = 3, },
    { .mask = CONT_GCN_L,       .x =  7, .y =  6, .w = 3, .h = 1, },
    { .mask = CONT_GCN_R,       .x = 22, .y =  6, .w = 3, .h = 1, },
    { .mask = CONT_GCN_Z,       .x = 22, .y =  7, .w = 3, .h = 1, },
    { .mask = CONT_GCN_UP,      .x = 11, .y = 17, .w = 1, .h = 1, },
    { .mask = CONT_GCN_DOWN,    .x = 11, .y = 19, .w = 1, .h = 1, },
    { .mask = CONT_GCN_LEFT,    .x = 10, .y = 18, .w = 1, .h = 1, },
    { .mask = CONT_GCN_RIGHT,   .x = 12, .y = 18, .w = 1, .h = 1, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_gcn_wheel[] = {
    { .mask = CONT_GCN_START,   .x = 15, .y = 22, .w = 2, .h = 2, },
    { .mask = CONT_GCN_Y,       .x = 21, .y = 15, .w = 1, .h = 2, },
    { .mask = CONT_GCN_X,       .x = 22, .y = 14, .w = 2, .h = 1, },
    { .mask = CONT_GCN_B,       .x = 23, .y = 17, .w = 1, .h = 1, },
    { .mask = CONT_GCN_A,       .x = 22, .y = 15, .w = 2, .h = 2, },
    { .mask = CONT_GCN_L,       .x =  8, .y = 18, .w = 3, .h = 2, },
    { .mask = CONT_GCN_R,       .x = 21, .y = 18, .w = 3, .h = 2, },
    { .mask = CONT_GCN_Z,       .x =  9, .y = 14, .w = 1, .h = 1, },
    { .mask = CONT_GCN_UP,      .x =  9, .y = 14, .w = 1, .h = 1, },
    { .mask = CONT_GCN_DOWN,    .x =  9, .y = 17, .w = 1, .h = 1, },
    { .mask = CONT_GCN_LEFT,    .x =  8, .y = 16, .w = 1, .h = 1, },
    { .mask = CONT_GCN_RIGHT,   .x = 10, .y = 16, .w = 1, .h = 1, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_gcn_keyboard[] = {
    { .mask = CONT_GCN_START,   .x =  7, .y = 14, .w = 1, .h = 1, },
    { .mask = CONT_GCN_Y,       .x = 27, .y = 13, .w = 1, .h = 1, },
    { .mask = CONT_GCN_X,       .x = 28, .y = 14, .w = 1, .h = 1, },
    { .mask = CONT_GCN_B,       .x = 26, .y = 15, .w = 1, .h = 1, },
    { .mask = CONT_GCN_A,       .x = 27, .y = 14, .w = 1, .h = 1, },
    { .mask = CONT_GCN_L,       .x =  3, .y = 11, .w = 3, .h = 1, },
    { .mask = CONT_GCN_R,       .x = 26, .y = 11, .w = 3, .h = 1, },
    { .mask = CONT_GCN_Z,       .x = 25, .y = 12, .w = 3, .h = 1, },
    { .mask = CONT_GCN_UP,      .x =  6, .y = 16, .w = 1, .h = 1, },
    { .mask = CONT_GCN_DOWN,    .x =  6, .y = 18, .w = 1, .h = 1, },
    { .mask = CONT_GCN_LEFT,    .x =  5, .y = 17, .w = 1, .h = 1, },
    { .mask = CONT_GCN_RIGHT,   .x =  7, .y = 17, .w = 1, .h = 1, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_gcn_dancepad[] = {
    { .mask = CONT_GCN_START,   .x = 21, .y =  5, .w = 3, .h = 1, },
    { .mask = CONT_GCN_Y,       .x =  8, .y = 22, .w = 3, .h = 3, },
    { .mask = CONT_GCN_X,       .x = 21, .y = 22, .w = 3, .h = 3, },
    { .mask = CONT_GCN_B,       .x =  8, .y =  9, .w = 3, .h = 3, },
    { .mask = CONT_GCN_A,       .x = 21, .y =  9, .w = 3, .h = 3, },
    { .mask = CONT_GCN_Z,       .x =  8, .y =  5, .w = 3, .h = 1, },
    { .mask = CONT_GCN_UP,      .x = 13, .y =  8, .w = 6, .h = 5, },
    { .mask = CONT_GCN_DOWN,    .x = 13, .y = 21, .w = 6, .h = 5, },
    { .mask = CONT_GCN_LEFT,    .x =  7, .y = 14, .w = 5, .h = 6, },
    { .mask = CONT_GCN_RIGHT,   .x = 20, .y = 14, .w = 5, .h = 6, },
    { .mask = (u16)-1, },
};

ALIGNED4 const ButtonHighlight buttons_display_controller_null[] = {
    { .mask = (u16)-1, },
};

/**
 * @brief Draws a red rectangle on sInputOverlayTextures using the ButtonHighlight data.
 *
 * @param port The port index to use when writing to sInputOverlayTextures.
 * @param buttonHighlight The ButtonHighlight data.
 */
void apply_to_overlay_texture(int port, const ButtonHighlight* buttonHighlight) {
    int startX = buttonHighlight->x;
    int startY = buttonHighlight->y;
    int w = buttonHighlight->w;
    int h = buttonHighlight->h;

    RGBA16* dst = (sInputOverlayTextures[port] + (CONT_ICON_W * startY) + startX);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            *dst++ = RGBA16_COMPOSITE(31, 0, 0, 1);
        }

        dst += (CONT_ICON_W - w);
    }
}

/**
 * @brief Loop through a ButtonHighlight list and apply all active highlights for a given port based on controller input.
 * 
 * @param port The port to use input from and the port index to use when writing to sInputOverlayTextures.
 * @param buttonHighlightList Pointer to a list of ButtonHighlight data.
 */
void set_overlay_texture(int port, const ButtonHighlight (*buttonHighlightList)[]) {
    const ButtonHighlight* buttonHighlight = *buttonHighlightList;
    u16 buttons = gControllerPads[port].rawContButtons;

    while (buttonHighlight->mask != (u16)-1) {
        if ((buttons & buttonHighlight->mask) != 0) {
            apply_to_overlay_texture(port, buttonHighlight);
        }

        buttonHighlight++;
    }
}

#define CONT_BUTTON_LIST(type) .buttonHighlightList = &buttons_display_controller_##type
#else // !CONTROLLERS_INPUT_DISPLAY
#define CONT_BUTTON_LIST(type)
#endif // !CONTROLLERS_INPUT_DISPLAY
#define CONT_TEXTURE(type) .texture = texture_controller_##type

// Controller icons (see segment2.c).
ALIGNED8 static const struct ControllerIcon sControllerIcons[] = {
    { .type = CONT_NONE,              CONT_TEXTURE(port        ), CONT_BUTTON_LIST(null        ), },
    { .type = CONT_TYPE_NORMAL,       CONT_TEXTURE(n64_normal  ), CONT_BUTTON_LIST(n64_normal  ), },
    { .type = CONT_TYPE_MOUSE,        CONT_TEXTURE(n64_mouse   ), CONT_BUTTON_LIST(n64_mouse   ), },
    { .type = CONT_TYPE_VOICE,        CONT_TEXTURE(n64_voice   ), CONT_BUTTON_LIST(null        ), },
    { .type = CONT_TYPE_KEYBOARD,     CONT_TEXTURE(n64_keyboard), CONT_BUTTON_LIST(null        ), },
    { .type = CONT_TYPE_GBA,          CONT_TEXTURE(gba         ), CONT_BUTTON_LIST(gba         ), },
    { .type = CONT_TYPE_GCN_NORMAL,   CONT_TEXTURE(gcn_normal  ), CONT_BUTTON_LIST(gcn_normal  ), },
    { .type = CONT_TYPE_GCN_RECEIVER, CONT_TEXTURE(gcn_receiver), CONT_BUTTON_LIST(null        ), },
    { .type = CONT_TYPE_GCN_WAVEBIRD, CONT_TEXTURE(gcn_wavebird), CONT_BUTTON_LIST(gcn_normal  ), },
    { .type = CONT_TYPE_GCN_WHEEL,    CONT_TEXTURE(gcn_wheel   ), CONT_BUTTON_LIST(gcn_wheel   ), },
    { .type = CONT_TYPE_GCN_KEYBOARD, CONT_TEXTURE(gcn_keyboard), CONT_BUTTON_LIST(gcn_keyboard), },
    { .type = CONT_TYPE_GCN_DANCEPAD, CONT_TEXTURE(gcn_dancepad), CONT_BUTTON_LIST(gcn_dancepad), },
    { .type = (u16)-1,                CONT_TEXTURE(unknown     ), CONT_BUTTON_LIST(null        ), },
};

/**
 * @brief Loop through sControllerIcons to get the port's controller's corresponding texture.
 *
 * @param[in] port The port with the controller to check.
 * @return const struct ControllerIcon* A pointer to the icon data in sControllerIcons.
 */
const struct ControllerIcon* get_controller_icon(int port) {
    const struct ControllerIcon* icon = &sControllerIcons[0];
    u16 type = gControllerStatuses[port].type;

    while (icon->type != (u16)-1) {
        if (icon->type == type) {
            break;
        }

        icon++;
    }

    return icon;
}

static const Gfx dl_controller_icons_begin[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_COPY),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

static const Gfx dl_controller_icons_end[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

/**
 * @brief Displays controller info (eg. type and player number) while polling for controller statuses.
 * TODO: Fix this not rendering/updating during transitions.
 */
void render_controllers_overlay(void) {
    const s32 w = CONT_ICON_W;
    const s32 h = CONT_ICON_H;
    s32 x = SCREEN_CENTER_X;
    const s32 y = (SCREEN_CENTER_Y - (h / 2));
    const s32 texW = CONT_ICON_W;
    const s32 texH = CONT_ICON_H;
    const s32 spacing = 2;
    const s32 spacedW = (w + spacing);
    const s32 iconStartX = (SCREEN_CENTER_X - (((spacedW * MAXCONTROLLERS) - spacing) / 2));
    char text_buffer[32] = "";
    int port;

    // Only show UI when status polling and not in boot mode.
    if (!gContStatusPolling || gContStatusPollingIsBootMode) {
        return;
    }

    Color col = remap(get_cycle(((f32)CONT_STATUS_POLLING_TIME / 30.0f), 1.0f, gContStatusPollTimer), -1, 1, 127, 255);

    // Darken the screen while polling controller status, similar to pausing the game.
    shade_screen();

 #ifdef CONTROLLERS_INPUT_DISPLAY
    bzero(sInputOverlayTextures, sizeof(sInputOverlayTextures));
 #endif // CONTROLLERS_INPUT_DISPLAY

    Gfx* dlHead = gDisplayListHead;

    // Allow drawing outside the screen borders.
    gDPSetScissor(dlHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    gSPDisplayList(dlHead++, dl_controller_icons_begin);

    // Draw the port icons:
    for (port = 0; port < MAXCONTROLLERS; port++) {
        const struct ControllerIcon* icon = get_controller_icon(port);
        x = (iconStartX + (spacedW * port));

        gDPLoadTextureTile(dlHead++,
            icon->texture, G_IM_FMT_RGBA, G_IM_SIZ_16b,
            texW, texH, 0, 0,
            (texW - 1), (texH - 1), 0,
            (G_TX_NOMIRROR | G_TX_CLAMP),
            (G_TX_NOMIRROR | G_TX_CLAMP),
            G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD,  G_TX_NOLOD
        );
        gSPTextureRectangle(dlHead++,
            (x << 2), (y << 2),
            (((x + w) - 1) << 2),
            (((y + h) - 1) << 2),
            G_TX_RENDERTILE, 0, 0,
            (4 << 10), (1 << 10)
        );

 #ifdef CONTROLLERS_INPUT_DISPLAY
        const ButtonHighlight (*buttonHighlightList)[] = icon->buttonHighlightList;
        set_overlay_texture(port, buttonHighlightList);
        gDPLoadTextureTile(dlHead++,
            sInputOverlayTextures[port], G_IM_FMT_RGBA, G_IM_SIZ_16b,
            texW, texH, 0, 0,
            (texW - 1), (texH - 1), 0,
            (G_TX_NOMIRROR | G_TX_CLAMP),
            (G_TX_NOMIRROR | G_TX_CLAMP),
            G_TX_NOMASK, G_TX_NOMASK,
            G_TX_NOLOD,  G_TX_NOLOD
        );
        gSPTextureRectangle(dlHead++,
            (x << 2), (y << 2),
            (((x + w) - 1) << 2),
            (((y + h) - 1) << 2),
            G_TX_RENDERTILE, 0, 0,
            (4 << 10), (1 << 10)
        );
 #endif // CONTROLLERS_INPUT_DISPLAY
    }

    gSPDisplayList(dlHead++, dl_controller_icons_end);
    gSPDisplayList(dlHead++, dl_fasttext_begin);

    drawSmallStringCol(&dlHead, (SCREEN_CENTER_X - 79), (SCREEN_CENTER_Y - 40), "WAITING FOR CONTROLLERS...", col, col, col);

    // Instructions:
    if (gControllerBits) {
        if (gContStatusPollingReadyForInput) {
            sprintf(text_buffer, "PRESS BUTTON TO ASSIGN P%d", (gNumPlayers + 1));
            drawSmallStringCol(&dlHead, (SCREEN_CENTER_X - 77), (SCREEN_CENTER_Y - 28), text_buffer, col, col, col);
 #if (MAX_NUM_PLAYERS > 1)
            char comboStr[32] = "";
            size_t count = button_combo_to_string(comboStr, TOGGLE_CONT_STATUS_POLLING_COMBO);
            sprintf(text_buffer, "OR %s TO EXIT", comboStr);
            s32 xOffset = (((strlen("OR%sTOEXIT") + count) / 2) * 7); // Center the text based on char count.
            drawSmallStringCol(&dlHead, (SCREEN_CENTER_X - xOffset), (SCREEN_CENTER_Y + 28), text_buffer, col, col, col);
 #endif // (MAX_NUM_PLAYERS > 1)
        } else {
            drawSmallStringCol(&dlHead, (SCREEN_CENTER_X - 84), (SCREEN_CENTER_Y - 28), "RELEASE ALL INPUTS TO START", col, col, col);
        }
    }

    // Print the assigned player numbers.
    for (port = 0; port < MAXCONTROLLERS; port++) {
        u8 playerNum = gControllerPads[port].playerNum;

        // Print if a controller is plugged in and assigned to a player.
        if ((gControllerStatuses[port].type != CONT_NONE) && (playerNum != 0)) {
            sprintf(text_buffer, "P%d", playerNum);
            s32 playerNumX = ((iconStartX + 9) + (spacedW * port));
            drawSmallString(&dlHead, playerNumX, (SCREEN_CENTER_Y + (CONT_ICON_H / 2)), text_buffer);
        }
    }

    gSPDisplayList(dlHead++, dl_fasttext_end);

    // Disallow drawing outside the screen borders.
    gDPSetScissor(dlHead++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH, (SCREEN_HEIGHT - gBorderHeight));

    gDisplayListHead = dlHead;
}
#endif // ENABLE_STATUS_REPOLLING_GUI

void render_game(void) {
    PROFILER_GET_SNAPSHOT_TYPE(PROFILER_DELTA_COLLISION);
    if (gCurrentArea != NULL && !gWarpTransition.pauseRendering) {
        if (gCurrentArea->graphNode) {
            geo_process_root(gCurrentArea->graphNode, gViewportOverride, gViewportClip, gFBSetColor);
        }
#ifdef PUPPYPRINT
        bzero(gCurrEnvCol, sizeof(ColorRGBA));
#endif

        Gfx* dlHead = gDisplayListHead;

        gSPViewport(dlHead++, VIRTUAL_TO_PHYSICAL(&gViewport));

        gDPSetScissor(dlHead++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH,
                      (SCREEN_HEIGHT - gBorderHeight));

        gDisplayListHead = dlHead;

        render_hud();

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        render_text_labels();
#ifdef PUPPYPRINT
        puppyprint_print_deferred();
#endif
        do_cutscene_handler();
        print_displaying_credits_entry();
        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH,
                      (SCREEN_HEIGHT - gBorderHeight));
        gMenuOptSelectIndex = render_menus_and_dialogs();

        if (gMenuOptSelectIndex != 0) {
            gSaveOptSelectIndex = gMenuOptSelectIndex;
        }

        if (gViewportClip != NULL) {
            make_viewport_clip_rect(gViewportClip);
        } else {
            gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH,
                          (SCREEN_HEIGHT - gBorderHeight));
        }

        if (gWarpTransition.isActive) {
            if (gWarpTransDelay == 0) {
                gWarpTransition.isActive = !render_screen_transition(0, gWarpTransition.type, gWarpTransition.time,
                                                                     &gWarpTransition.data);
                if (!gWarpTransition.isActive) {
                    if (gWarpTransition.type & WARP_TRANSITION_FADE_INTO) {
                        gWarpTransition.pauseRendering = TRUE;
                    } else {
                        set_warp_transition_rgb(0, 0, 0);
                    }
                }
            } else {
                gWarpTransDelay--;
            }
        }
#ifdef S2DEX_TEXT_ENGINE
        s2d_init();

        // Place any custom text engine code here if not using deferred prints.

        s2d_handle_deferred();
        s2d_stop();
#endif
    } else {
        render_text_labels();
#ifdef PUPPYPRINT
        puppyprint_print_deferred();
#endif
        if (gViewportClip != NULL) {
            clear_viewport(gViewportClip, gWarpTransFBSetColor);
        } else {
            clear_framebuffer(gWarpTransFBSetColor);
        }
    }

    gViewportOverride = NULL;
    gViewportClip = NULL;

#ifdef ENABLE_STATUS_REPOLLING_GUI
    render_controllers_overlay();
#endif

    profiler_update(PROFILER_TIME_GFX, (profiler_get_delta(PROFILER_DELTA_COLLISION) - first));
    profiler_print_times();
#ifdef PUPPYPRINT_DEBUG
    puppyprint_render_profiler();
#endif
}
