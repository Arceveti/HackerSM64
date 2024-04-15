#include <PR/ultratypes.h>

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
#include "puppyprint.h"
#include "debug_box.h"
#include "engine/colors.h"
// #include "controller_select_menu.h"
#include "fasttext.h"
#ifdef S2DEX_TEXT_ENGINE
#include "s2d_engine/init.h"
#endif

struct SpawnInfo gPlayerSpawnInfos[1];
struct GraphNode *gGraphNodePointers[MODEL_ID_COUNT];
struct Area gAreaData[AREA_COUNT];

struct WarpTransition gWarpTransition;

s16 gCurrCourseNum;
s16 gCurrActNum = 1;
s16 gCurrAreaIndex;
s16 gSavedCourseNum;
s16 gMenuOptSelectIndex;
s16 gSaveOptSelectIndex;

struct SpawnInfo *gMarioSpawnInfo = &gPlayerSpawnInfos[0];
struct GraphNode **gLoadedGraphNodes = gGraphNodePointers;
struct Area *gAreas = gAreaData;
struct Area *gCurrentArea = NULL;
struct CreditsEntry *gCurrCreditsEntry = NULL;

Vp *gViewportOverride = NULL;
Vp *gViewportClip = NULL;
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
 * When looping through sWarpBhvSpawnTable, if the behavior function in the table matches
 * the spawn behavior executed, the index of that behavior is used with sSpawnTypeFromWarpBhv
*/

const BehaviorScript *sWarpBhvSpawnTable[] = {
    bhvDoorWarp,                bhvStar,                   bhvExitPodiumWarp,          bhvWarp,
    bhvWarpPipe,                bhvFadingWarp,             bhvInstantActiveWarp,       bhvAirborneWarp,
    bhvHardAirKnockBackWarp,    bhvSpinAirborneCircleWarp, bhvDeathWarp,               bhvSpinAirborneWarp,
    bhvFlyingWarp,              bhvSwimmingWarp,           bhvPaintingStarCollectWarp, bhvPaintingDeathWarp,
    bhvAirborneStarCollectWarp, bhvAirborneDeathWarp,      bhvLaunchStarCollectWarp,   bhvLaunchDeathWarp,
};

u8 sSpawnTypeFromWarpBhv[] = {
    MARIO_SPAWN_DOOR_WARP,             MARIO_SPAWN_IDLE,                 MARIO_SPAWN_PIPE,                  MARIO_SPAWN_PIPE,
    MARIO_SPAWN_PIPE,                  MARIO_SPAWN_TELEPORT,             MARIO_SPAWN_INSTANT_ACTIVE,        MARIO_SPAWN_AIRBORNE,
    MARIO_SPAWN_HARD_AIR_KNOCKBACK,    MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE, MARIO_SPAWN_DEATH,                 MARIO_SPAWN_SPIN_AIRBORNE,
    MARIO_SPAWN_FLYING,                MARIO_SPAWN_SWIMMING,             MARIO_SPAWN_PAINTING_STAR_COLLECT, MARIO_SPAWN_PAINTING_DEATH,
    MARIO_SPAWN_AIRBORNE_STAR_COLLECT, MARIO_SPAWN_AIRBORNE_DEATH,       MARIO_SPAWN_LAUNCH_STAR_COLLECT,   MARIO_SPAWN_LAUNCH_DEATH,
};

Vp gViewport = {
    .vp = {
        .vscale = { (SCREEN_WIDTH * 2), (SCREEN_HEIGHT * 2), (G_MAXZ / 2), 0 },
        .vtrans = { (SCREEN_WIDTH * 2), (SCREEN_HEIGHT * 2), (G_MAXZ / 2), 0 },
    }
};

void override_viewport_and_clip(Vp *vpOverride, Vp *vpClip, Color red, Color green, Color blue) {
    RGBA16 color = ((red >> 3) << IDX_RGBA16_R) | ((green >> 3) << IDX_RGBA16_G) | ((blue >> 3) << IDX_RGBA16_B) | MSK_RGBA16_A;

    gFBSetColor = (color << 16) | color;
    gViewportOverride = vpOverride;
    gViewportClip = vpClip;
}

void set_warp_transition_rgb(Color red, Color green, Color blue) {
    RGBA16 warpTransitionRGBA16 = ((red >> 3) << IDX_RGBA16_R) | ((green >> 3) << IDX_RGBA16_G) | ((blue >> 3) << IDX_RGBA16_B) | MSK_RGBA16_A;

    gWarpTransFBSetColor = (warpTransitionRGBA16 << 16) | warpTransitionRGBA16;
    gWarpTransRed = red;
    gWarpTransGreen = green;
    gWarpTransBlue = blue;
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

u32 get_mario_spawn_type(struct Object *obj) {
    s32 i;
    const BehaviorScript *behavior = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, obj->behavior);

    for (i = 0; i < 20; i++) {
        if (sWarpBhvSpawnTable[i] == behavior) {
            return sSpawnTypeFromWarpBhv[i];
        }
    }
    return MARIO_SPAWN_NONE;
}

extern const char* get_warp_node_name(const enum WarpNodes id);

struct ObjectWarpNode *area_get_warp_node(u8 id) {
    struct ObjectWarpNode *node = NULL;

    DEBUG_ASSERTF((gCurrentArea != NULL), ASSERT_PREFIX_LEVEL"Warp destination is in invalid or unloaded area %d!\nNode ID: 0x%02X %s",
        sWarpDest.areaIdx, id, get_warp_node_name(id)
    );

    for (node = gCurrentArea->warpNodes; node != NULL; node = node->next) {
        if (node->node.id == id) {
            break;
        }
    }

    DEBUG_ASSERTF((node != NULL), ASSERT_PREFIX_LEVEL"Couldn't find warp node!\nNode ID: 0x%02X %s",
        id, get_warp_node_name(id)
    );

    return node;
}

struct ObjectWarpNode *area_get_warp_node_from_params(struct Object *obj) {
    return area_get_warp_node(GET_BPARAM2(obj->oBehParams));
}

void load_obj_warp_nodes(void) {
    struct ObjectWarpNode *warpNode;
    struct Object *children = (struct Object *) gObjParentGraphNode.children;

    do {
        struct Object *obj = children;

        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && get_mario_spawn_type(obj) != 0) {
            warpNode = area_get_warp_node_from_params(obj);
            if (warpNode != NULL) {
                warpNode->object = obj;
            }
        }
    } while ((children = (struct Object *) children->header.gfx.node.next)
             != (struct Object *) gObjParentGraphNode.children);
}

void clear_areas(void) {
    s32 i, j;

    gCurrentArea = NULL;
    gWarpTransition.isActive = FALSE;
    gWarpTransition.pauseRendering = FALSE;
    gMarioSpawnInfo->areaIndex = -1;

    for (i = 0; i < AREA_COUNT; i++) {
        gAreaData[i].index = i;
        gAreaData[i].flags = AREA_FLAG_UNLOAD;
        gAreaData[i].terrainType = TERRAIN_GRASS;
        gAreaData[i].graphNode = NULL;
        gAreaData[i].terrainData = NULL;
        gAreaData[i].surfaceRooms = NULL;
        gAreaData[i].macroObjects = NULL;
        gAreaData[i].warpNodes = NULL;
        gAreaData[i].paintingWarpNodes = NULL;
        gAreaData[i].instantWarps = NULL;
        gAreaData[i].objectSpawnInfos = NULL;
        gAreaData[i].camera = NULL;
        gAreaData[i].unused = NULL;
        for (j = 0; j < ARRAY_COUNT(gAreaData[i].whirlpools); j++) {
            gAreaData[i].whirlpools[j] = NULL;
        }
        gAreaData[i].dialog[0] = DIALOG_NONE;
        gAreaData[i].dialog[1] = DIALOG_NONE;
        gAreaData[i].musicParam = 0;
        gAreaData[i].musicParam2 = 0;
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

    for (i = 0; i < AREA_COUNT; i++) {
        if (gAreaData[i].graphNode != NULL) {
            geo_call_global_function_nodes(&gAreaData[i].graphNode->node, GEO_CONTEXT_AREA_INIT);
            gAreaData[i].graphNode = NULL;
        }
    }
}

void load_area(s32 index) {
    // load_area() is called with index == -1 when not trying to load an area.
    // This check prevents gAreaData[index] from reading outside the array's bounds.
    if ((index < 0) || (index >= AREA_COUNT)) {
        return;
    }

    DEBUG_ASSERTF((gAreaData[index].graphNode != NULL), ASSERT_PREFIX_LEVEL"Tried to load invalid area %d", index);

    if (gCurrentArea == NULL && gAreaData[index].graphNode != NULL) {
        gCurrentArea = &gAreaData[index];
        gCurrAreaIndex = gCurrentArea->index;
        main_pool_pop_state();
        main_pool_push_state();

        gMarioCurrentRoom = 0;

        if (gCurrentArea->terrainData != NULL) {
            load_area_terrain(index, gCurrentArea->terrainData, gCurrentArea->surfaceRooms,
                              gCurrentArea->macroObjects);
        }

        if (gCurrentArea->objectSpawnInfos != NULL) {
            spawn_objects_from_info(0, gCurrentArea->objectSpawnInfos);
        }

        load_obj_warp_nodes();
        geo_call_global_function_nodes(&gCurrentArea->graphNode->node, GEO_CONTEXT_AREA_LOAD);
    }
}

void unload_area(void) {
    if (gCurrentArea != NULL) {
        unload_objects_from_area(0, gCurrentArea->index);
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
        spawn_objects_from_info(0, gMarioSpawnInfo);
    }

    if (gAreaSkyboxStart[gCurrAreaIndex - 1]) {
        load_segment_decompress(SEGMENT_SKYBOX, gAreaSkyboxStart[gCurrAreaIndex - 1], gAreaSkyboxEnd[gCurrAreaIndex - 1]);
    }
}

void unload_mario_area(void) {
    if ((gCurrentArea != NULL) && (gCurrentArea->flags & AREA_FLAG_LOAD)) {
        unload_objects_from_area(0, gMarioSpawnInfo->activeAreaIndex);

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
        gMarioObject->header.gfx.areaIndex = index, gMarioSpawnInfo->areaIndex = index;
    }
}

void area_update_objects(void) {
    gAreaUpdateCounter++;
    update_objects(0);
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
        red = gWarpTransRed, green = gWarpTransGreen, blue = gWarpTransBlue;
    }

    if (transType & WARP_TRANSITION_TYPE_COLOR) {
        gWarpTransition.data.red = red;
        gWarpTransition.data.green = green;
        gWarpTransition.data.blue = blue;
    } else { // if transition is textured
        set_and_reset_transition_fade_timer(0); // Reset transition timers by passing in 0 for time

        gWarpTransition.data.red = red;
        gWarpTransition.data.green = green;
        gWarpTransition.data.blue = blue;

        // Both the start and end textured transition are always located in the middle of the screen.
        // If you really wanted to, you could place the start at one corner and the end at
        // the opposite corner. This will make the transition image look like it is moving
        // across the screen.
        gWarpTransition.data.startTexX = SCREEN_CENTER_X;
        gWarpTransition.data.startTexY = SCREEN_CENTER_Y;
        gWarpTransition.data.endTexX = SCREEN_CENTER_X;
        gWarpTransition.data.endTexY = SCREEN_CENTER_Y;

        gWarpTransition.data.angleSpeed = DEGREES(0);

        s16 fullRadius = GFX_DIMENSIONS_FULL_RADIUS;

#ifdef POLISHED_TRANSITIONS
        switch (transType){
            case WARP_TRANSITION_TYPE_BOWSER:
            case WARP_TRANSITION_FADE_INTO_BOWSER:
                fullRadius *= 4;
            break;

            case WARP_TRANSITION_FADE_FROM_MARIO:
            case WARP_TRANSITION_FADE_INTO_MARIO:

            case WARP_TRANSITION_FADE_FROM_STAR:
            case WARP_TRANSITION_FADE_INTO_STAR:
                fullRadius *= 1.5f;
            break;
        }
#endif

        if (transType & WARP_TRANSITION_FADE_INTO) { // Is the image fading in?
            gWarpTransition.data.startTexRadius = fullRadius;
            gWarpTransition.data.endTexRadius = 0;

        } else { // The image is fading out. (Reverses start & end circles)
            gWarpTransition.data.startTexRadius = 0;
            gWarpTransition.data.endTexRadius = fullRadius;
        }
    }
#endif
}

/*
 * Sets up the information needed to play a warp transition, including the
 * transition type, time in frames, and the RGB color that will fill the screen.
 * The transition will play only after a number of frames specified by 'delay'
 */
void play_transition_after_delay(s16 transType, s16 time, u8 red, u8 green, u8 blue, s16 delay) {
    gWarpTransDelay = delay; // Number of frames to delay playing the transition.
    play_transition(transType, time, red, green, blue);
}

#include <string.h>

#define FPS_COUNT 30
#define FRAMES_TO_NESC(f)   (((OSTime)(f) * 1000000000LL) / FPS_COUNT)
#define FRAMES_TO_UESC(f)   (((OSTime)(f) * 1000000LL) / FPS_COUNT)
#define FRAMES_TO_CYCLES(f) (((OSTime)(f) * OS_CPU_COUNTER) / FPS_COUNT)
#define NSEC_TO_FRAMES(n)   (((OSTime)(n) * FPS_COUNT) / 1000000000LL)
#define USEC_TO_FRAMES(n)   (((OSTime)(n) * FPS_COUNT) / 1000000LL)
#define CYCLES_TO_FRAMES(c) (((OSTime)(c) * FPS_COUNT) / OS_CPU_COUNTER)

_Bool gSuitAcquired = FALSE;

#include "suit_dialogue.h"

// enum OnScreenDialogue
SuitDialogue sOnScreenDialogues[] = {
    [DLG_NONE] = { .txt = "", },
    // Summit
    [DLG_BOOTING] = {.txt = "BOOTING UP S-L1D3...", .speed = 0.1f, },
    [DLG_SCANNING] = {.txt = "SCANNING AREA...", .speed = 0.25f, },
    [DLG_HELLO_FRIEND] = { .txt = "HELLO FRIEND, I AM SIGMA-L1D3.", .speed = 0.5f, },
    [DLG_HELLO_2] = {.txt = "YOU CAN CALL ME S-L1DE FOR SHORT"},
    [DLG_LAST_BOOT] = { .txt = "IT HAS BEEN xxxx CYCLES SINCE LAST BOOT." },
    // [DLG_WELCOME] = { .txt = "", },
    [DLG_LIGHTS_1] = { .txt = "LOOK AT THOSE PRETTY LIGHTS IN THE DISTANCE!" },
    [DLG_HOME_1] = { .txt = "I WILL HELP YOU GO HOME", },
    // Mountain
    [DLG_START_RIGHT] = { .txt = "TURN TO THE RIGHT.", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_READY_LEFT] = { .txt = "GET READY TO GO LEFT...", .r = 0xFF, .g = 0xFF, .b = 0x01,  },
    [DLG_GO_LEFT] = { .txt = "TURN LEFT!", .r = 0xFF, .g = 0xFF, .b = 0x01,  },
    [DLG_READY_RIGHT] = {.txt = "GET READY TO GO RIGHT...", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_GO_RIGHT] = { .txt = "TURN RIGHT!", .r = 0xFF, .g = 0xFF, .b = 0x01,  },
    [DLG_READY_JUMP] = { .txt = "GET READY TO JUMP", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_GO_JUMP] = { .txt = "JUMP!", .r = 0xFF, .g = 0xFF, .b = 0x01,  },
    [DLG_JUMP_ICICLES] = {.txt = "JUMP, BUT DON'T HIT THE ICICLES!", .r = 0xFF, .g = 0xFF, .b = 0x01, .speed = 2.0f, },
    [DLG_JUMP_THEN_LEFT] = { .txt = "JUMP, THEN GO LEFT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_JUMP_THEN_RIGHT] = { .txt = "JUMP, THEN GO RIGHT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_KEEP_LEFT] = {.txt = "KEEP LEFT!", .timeout = 600, .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_KEEP_RIGHT] = {.txt = "KEEP RIGHT!", .timeout = 600, .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_LEFT_NO_RIGHT] = {.txt = "LEFT!  NO, WAIT, RIGHT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_RIGHT_NO_LEFT] = {.txt = "RIGHT!  NO, WAIT, LEFT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_LEFT_AGAIN] = {.txt = "LEFT AGAIN!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_RIGHT_AGAIN] = {.txt = "RIGHT AGAIN!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_INTO_THE_CAVE] = {.txt = "INTO THAT CAVE!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_JUMP_LEFT] = {.txt = "JUMP TO THE LEFT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_JUMP_RIGHT] = {.txt = "JUMP TO THE RIGHT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    [DLG_WRONG_WAY] = {.txt = "WRONG WAY!", .r = 0xFF, .g = 0x01, .b = 0x01, .timeout = 1800, },
    [DLG_GOOD] = {.txt = "GOOD!", .r = 0x01, .g = 0xFF, .b = 0x01, },
    [DLG_DONT_FALL] = {.txt = "DON'T FALL!"},
    [DLG_WATCH_OUT] = {.txt = "WATCH OUT!", .r = 0xFF, .g = 0xFF, .b = 0x01, },
    // Lava
    [DLG_WELCOME_HOME] = { .txt = "WELCOME TO MY HOME!", },
    [DLG_HOTTER] = {.txt = "TEMP xxxx DEGREES HOTTER THAN PREV RECORD"},
    [DLG_SHIELD] = {.txt = "ACTIVATING HEAT SHIELD..."},
    [DLG_UPHILL] = {.txt = "THE HEAT REDUCES FRICTION, YOU CAN SLIDE UPHILL"},
    // Boss
    [DLG_OFFSCREEN] = {.txt = "A LOT JUST HAPPENED OFFSCREEN (RAN OUT OF TIME)"},
    [DLG_OFFSCREEN_2] = {.txt = "WE'RE IN SPACE NOW, DON'T WORRY ABOUT IT."},
    [DLG_BLACK_HOLE] = {.txt = "FLY AROUND THE BLACK HOLE TO FINISH THE HACK"},
    // Space
    [DLG_GOODBYE_FRIEND] = { .txt = "GOODBYE FRIEND.", .speed = 0.75f },
    [DLG_LIGHTS_2] = { .txt = "LOOK AT ALL THOSE PRETTY LIGHTS" },
    [DLG_HOME_2] = { .txt = "I WILL HELP YOU GO HOME" },
    [DLG_YOUR_HOME] = {.txt = "YOUR HOME."},
    [DLG_RAINBOW_ROAD] = {.txt = "ACTIVATING RAINBOW ROAD..."},
};


const SuitDialogue defaultDLGinfo = {
    .txt = "",
    .speed = 1.0f,
    .timeout = 120,
    .r = 0xFF,
    .g = 0xFF,
    .b = 0xFF,
};

char currOnScreenDlgBuffer[48] = "";
int prevOnScreenDlgID = 0;
int currOnScreenDlgID = 0;

_Bool currOnScreenDlgDonePrinting = FALSE;
OSTime currOnScreenDlgStartTime = 0;
u32 currOnScreenDlgPrevFrameNumCharsShown = 0;
u32 timeSinceFinished = 0;

// u32 initDLG_1[] = {
//     DLG_BOOTING, DLG_HELLO_FRIEND, DLG_HELLO_2, DLG_LAST_BOOT, //DLG_HOME_1,
// };
// u32 initDLG_2[] = {
//     DLG_WELCOME_HOME, DLG_HOTTER, DLG_SHIELD, DLG_UPHILL,
// };
// u32 initDLG_3[] = {
//     DLG_OFFSCREEN, DLG_OFFSCREEN_2, DLG_BLACK_HOLE,
// };


u32 gInitialTextIndex = 0;
// // _Bool gDoingInitialDialogues = gInitialTextIndex;

// u32 get_initial_DLG(void) {
//     return DLG_BLACK_HOLE;
//     gInitialTextIndex++;
//     size_t size = 0;
//     int dlgID = DLG_NONE;
//     u32* list = NULL;
//     switch (gCurrLevelNum) {
//         case LEVEL_BOB:
//             size = ARRAY_COUNT(initDLG_1);
//             list = initDLG_1;
//             break;
//         case LEVEL_WF:
//             size = ARRAY_COUNT(initDLG_2);
//             list = initDLG_2;
//             break;
//         case LEVEL_JRB:
//             size = ARRAY_COUNT(initDLG_3);
//             list = initDLG_2;
//             break;
//         default:
//             gInitialTextIndex = 0xFFFF;
//             return DLG_NONE;
//     }

//     if (gInitialTextIndex < size) {
//         dlgID = list[gInitialTextIndex];
//     }

//     if (dlgID != currOnScreenDlgID) {
//         return dlgID;
//     }

//     return DLG_NONE;
// }


void set_default_DLG_info(SuitDialogue* dlg) {
    if (dlg->txt == NULL) {
        dlg->txt = defaultDLGinfo.txt;
    }
    if (dlg->speed == 0.0f) {
        dlg->speed = defaultDLGinfo.speed;
    }
    if (dlg->timeout == 0) {
        dlg->timeout = defaultDLGinfo.timeout;
    }
    if (dlg->r == 0x00) dlg->r = defaultDLGinfo.r;
    if (dlg->g == 0x00) dlg->g = defaultDLGinfo.g;
    if (dlg->b == 0x00) dlg->b = defaultDLGinfo.b;
}

void clear_onscreen_dlg_buf(void) {
    bzero(currOnScreenDlgBuffer, sizeof(currOnScreenDlgBuffer));
}

void onscreen_dlg_changed(void) {
    clear_onscreen_dlg_buf();
    currOnScreenDlgDonePrinting = FALSE;
    currOnScreenDlgPrevFrameNumCharsShown = 0;
    currOnScreenDlgStartTime = osGetTime();
    set_default_DLG_info(&sOnScreenDialogues[currOnScreenDlgID]);
}

void set_onscreen_dlg(int id) {
    prevOnScreenDlgID = currOnScreenDlgID;
    currOnScreenDlgID = id;
    if (id == currOnScreenDlgID) {
        return;
    }
    onscreen_dlg_changed();
}

void print_onscreen_dlg(void) {
    if (currOnScreenDlgID != prevOnScreenDlgID) {
        onscreen_dlg_changed();
    }
    OSTime time = osGetTime();
    clear_onscreen_dlg_buf();
    u32 id = currOnScreenDlgID;
    SuitDialogue* dlg = &sOnScreenDialogues[id];
    OSTime timeSinceStart = (time - currOnScreenDlgStartTime);
    
    u32 numCharsToPrint = CYCLES_TO_FRAMES(timeSinceStart) * dlg->speed;
    const char* src = dlg->txt;
    size_t srcSize = strlen(src);

    s32 sound = SOUND_OBJ_BOBOMB_BUDDY_TALK;
    // s32 sound = SOUND_OBJ_BULLY_WALK_LARGE;
    stop_sound(sound, gGlobalSoundSource);
    if (
        src[numCharsToPrint - 1] != ' ' &&
        src[numCharsToPrint - 1] != '\n' &&
        src[numCharsToPrint - 1] != '\0' &&
        !currOnScreenDlgDonePrinting &&
        (numCharsToPrint != currOnScreenDlgPrevFrameNumCharsShown)
    ) {
        play_sound(sound, gGlobalSoundSource);
    }
    // if (currOnScreenDlgDonePrinting) {
    //     stop_sound(sound, gGlobalSoundSource);
    // }
    if (numCharsToPrint > srcSize) {
        currOnScreenDlgDonePrinting = TRUE;
        numCharsToPrint = srcSize;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    strncpy(currOnScreenDlgBuffer, src, numCharsToPrint);
#pragma GCC diagnostic pop
    currOnScreenDlgPrevFrameNumCharsShown = numCharsToPrint;
    if (currOnScreenDlgDonePrinting) {
        timeSinceFinished++;
    }
    if (timeSinceFinished >= dlg->timeout) {
        timeSinceFinished = 0;
        // set_onscreen_dlg((gInitialTextIndex != 0xFFFF) ? get_initial_DLG() : DLG_NONE);
        set_onscreen_dlg(DLG_NONE);
    }
}
#include "segment2.h"

void render_suit_overlay(void) {
    // if (!gSuitAcquired) {
    //     return;
    // }

    Gfx* dlHead = gDisplayListHead;

    // Allow drawing outside the screen borders.
    gDPSetScissor(dlHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    gSPDisplayList(dlHead++, dl_texrect_rgba16_begin);
    if (currOnScreenDlgPrevFrameNumCharsShown > 0) {
        texrect_rgba16(&dlHead, texture_robot, 32, 32, 32, 32, 32, 32);
    }
    gSPDisplayList(dlHead++, dl_texrect_rgba16_end);
    gSPDisplayList(dlHead++, dl_fasttext_begin);
    print_onscreen_dlg();
    SuitDialogue* dlg = &sOnScreenDialogues[currOnScreenDlgID];
    const char* txt = dlg->txt;
    for (size_t i = 0; i < strlen(txt); i++) {
        // if (txt[i] == '*') {
        //     if (currOnScreenDlgBuffer[i] != '\0') {
        //         currOnScreenDlgBuffer[i] = 0x20 + (unsigned char)(random_float() * (0x7E - 0x20));//'0' + (unsigned char)(random_float() * 10.0f);
        //     }
        // }
        
        if (txt[i] == 'x') {
            if (currOnScreenDlgBuffer[i] != '\0') {
                currOnScreenDlgBuffer[i] = (random_u16() & 0x1) ? 'X' : '9';
            }
        }
    }
    // Color col = 255;
    
    drawSmallStringCol(&dlHead,
        64+8, 32,//(20 + random_f32_around_zero(sOnScreenDialogues[currOnScreenDlgID].speed)),
        currOnScreenDlgBuffer,
        dlg->r, dlg->g, dlg->b
    );
    gSPDisplayList(dlHead++, dl_fasttext_end);

    // Disallow drawing outside the screen borders.
    gDPSetScissor(dlHead++, G_SC_NON_INTERLACE, gBorderWidth, gBorderHeight, (SCREEN_WIDTH - gBorderWidth), (SCREEN_HEIGHT - gBorderHeight));

    gDisplayListHead = dlHead;
}

void render_game(void) {
    PROFILER_GET_SNAPSHOT_TYPE(PROFILER_DELTA_COLLISION);
    if (gCurrentArea != NULL && !gWarpTransition.pauseRendering) {
        if (gCurrentArea->graphNode) {
            geo_process_root(gCurrentArea->graphNode, gViewportOverride, gViewportClip, gFBSetColor);
        }
#ifdef PUPPYPRINT
        bzero(gCurrEnvCol, sizeof(ColorRGBA));
#endif

        gSPViewport(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(&gViewport));

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, gBorderWidth, gBorderHeight, SCREEN_WIDTH - gBorderWidth,
                      SCREEN_HEIGHT - gBorderHeight);
        render_hud();

        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        render_text_labels();
#ifdef PUPPYPRINT
        puppyprint_print_deferred();
#endif
        do_cutscene_handler();
        print_displaying_credits_entry();
        gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, gBorderWidth, gBorderHeight, SCREEN_WIDTH - gBorderWidth,
                      SCREEN_HEIGHT - gBorderHeight);
        gMenuOptSelectIndex = render_menus_and_dialogs();

        if (gMenuOptSelectIndex != 0) {
            gSaveOptSelectIndex = gMenuOptSelectIndex;
        }

        if (gViewportClip != NULL) {
            make_viewport_clip_rect(gViewportClip);
        } else {
            gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, gBorderWidth, gBorderHeight, SCREEN_WIDTH - gBorderWidth,
                          SCREEN_HEIGHT - gBorderHeight);
        }
        if (gWarpTransition.isActive) {
            if (gWarpTransDelay == 0) {
                gWarpTransition.isActive = !render_screen_transition(gWarpTransition.type, gWarpTransition.time,
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

        // place any custom text engine code here if not using deferred prints

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
    gViewportClip     = NULL;

    render_suit_overlay();

    profiler_update(PROFILER_TIME_GFX, profiler_get_delta(PROFILER_DELTA_COLLISION) - first);
    profiler_print_times();
#ifdef PUPPYPRINT_DEBUG
    puppyprint_render_profiler();
#endif
}
