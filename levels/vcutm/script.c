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
#include "levels/vcutm/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_VCUTM_SEESAW_PLATFORM, /*pos*/   154, -1919, -6256, /*angle*/ 0, 270, 0, /*behParam*/ 0x00070000, /*beh*/ bhvSeesawPlatform),
    OBJECT(/*model*/ MODEL_VCUTM_SEESAW_PLATFORM, /*pos*/ -2047, -3378, -2047, /*angle*/ 0,   0, 0, /*behParam*/ 0x00070000, /*beh*/ bhvSeesawPlatform),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/  3251, -1082, -6256, /*angle*/ 0, 270, 0, /*behParam*/ 0x014B0000, /*beh*/ bhvCheckerboardElevatorGroup),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/  2355, -1901, -6256, /*angle*/ 0, 270, 0, /*behParam*/ 0x014B0000, /*beh*/ bhvCheckerboardElevatorGroup),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/  1459, -1594, -6256, /*angle*/ 0,  90, 0, /*behParam*/ 0x014B0000, /*beh*/ bhvCheckerboardElevatorGroup),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/ -1151, -2413, -6256, /*angle*/ 0, 270, 0, /*behParam*/ 0x014B0000, /*beh*/ bhvCheckerboardElevatorGroup),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_CAP_SWITCH, /*pos*/ 4506,  26, -6246, /*angle*/ 0, 0, 0, /*behParam*/ 0x00020000, /*beh*/ bhvCapSwitch),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT(/*model*/ MODEL_NONE,       /*pos*/ 4600, 250, -4500, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_vcutm_entry[] = {
    INIT_LEVEL(),
    LOAD_TEXTURE_BIN(outside),
    LOAD_LEVEL_DATA(vcutm),
    LOAD_GROUPA(group8),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_9),
    LOAD_MODEL_FROM_GEO(MODEL_VCUTM_SEESAW_PLATFORM, vcutm_geo_0001F0),
    LOAD_MODEL_FROM_GEO(MODEL_VCUTM_WARP_PIPE,       warp_pipe_geo),

    AREA(/*index*/ 1, vcutm_geo_000208),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -6143, 6734, -6143, /*angle*/ 0, 0, 0, /*behParam*/ WARP_VCUTM_MAIN_ENTRY << 16, /*beh*/ bhvAirborneWarp),
        WARP_NODE(/*id*/ WARP_VCUTM_MAIN_ENTRY, /*destLevel*/ LEVEL_VCUTM,          /*destArea*/ 0x01, /*destNode*/ WARP_VCUTM_MAIN_ENTRY,                  /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_WARP_FLOOR,  /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_GROUNDS_FROM_VCUTM_WARP,    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_GROUNDS_FROM_VCUTM_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_GROUNDS_FROM_VCUTM_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN(/*terrainData*/ vcutm_seg7_collision),
        MACRO_OBJECTS(/*objList*/ vcutm_seg7_macro_objs),
        SHOW_DIALOG(/*index*/ 0x00, DIALOG_129),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 0, /*pos*/ -6143, 5734, -6143),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
