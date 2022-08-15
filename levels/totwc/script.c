#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/totwc/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_CAP_SWITCH, /*pos*/   0, -2047, 10, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvCapSwitch),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_NONE,       /*pos*/ 800, -1700,  0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_totwc_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(totwc),
    LOAD_SKYBOX(cloud_floor),
    LOAD_TEXTURE_BIN(sky),
    LOAD_GROUPA(group8),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_9),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, totwc_geo_000160),

    AREA(/*index*/ 1, totwc_geo_000188),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4095, 2935, 0, /*angle*/ 0, 90, 0, /*behParam*/ WARP_TOTWC_MAIN_ENTRY << 16, /*beh*/ bhvFlyingWarp),
        WARP_NODE(/*id*/ WARP_TOTWC_MAIN_ENTRY, /*destLevel*/ LEVEL_TOTWC,  /*destArea*/ 0x01, /*destNode*/ WARP_TOTWC_MAIN_ENTRY,                    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_WARP_FLOOR,  /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SECRET_WARP,    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SECRET_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SECRET_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_1),
        TERRAIN(/*terrainData*/ totwc_seg7_collision),
        MACRO_OBJECTS(/*objList*/ totwc_seg7_macro_objs),
        SHOW_DIALOG(/*index*/ 0x00, DIALOG_131),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 90, /*pos*/ -4095, 2935, 0),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
