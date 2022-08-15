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
#include "levels/cotmc/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_CAP_SWITCH, /*pos*/ 0,  363, -6144, /*angle*/ 0, 0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvCapSwitch),
    OBJECT(/*model*/ MODEL_NONE,       /*pos*/ 0,  500, -7373, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvWaterfallSoundLoop),
    OBJECT(/*model*/ MODEL_NONE,       /*pos*/ 0,  500,  3584, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvWaterfallSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_NONE,       /*pos*/ 0, -200, -7000, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_cotmc_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(cotmc),
    LOAD_TEXTURE_BIN(cave),
    LOAD_GROUPA(group8),
    LOAD_GROUPB(group17),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_9),
    JUMP_LINK(script_func_global_18),
    JUMP_LINK(script_func_global_1),

    AREA(/*index*/ 1, cotmc_geo_0001A0),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4185, 1020, -47, /*angle*/ 0, 90, 0, /*behParam*/ WARP_COTMC_MAIN_ENTRY << 16, /*beh*/ bhvAirborneWarp),
        WARP_NODE(/*id*/ WARP_COTMC_MAIN_ENTRY, /*destLevel*/ LEVEL_COTMC,          /*destArea*/ 0x01, /*destNode*/ WARP_COTMC_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE,         /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_HMC_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE,         /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_HMC_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_WARP_FLOOR,  /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_GROUNDS_FROM_COTMC,        /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_1),
        TERRAIN(/*terrainData*/ cotmc_seg7_collision_level),
        MACRO_OBJECTS(/*objList*/ cotmc_seg7_macro_objs),
        SHOW_DIALOG(/*index*/ 0x00, DIALOG_130),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_CAVE, /*seq*/ SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 90, /*pos*/ -4185, 20, -47),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
