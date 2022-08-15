#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/wmotr/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  3996, -2739,  5477, /*angle*/ 0, 0, 0, /*behParam*/ 0x00520000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2911,  3564, -3967, /*angle*/ 0, 0, 0, /*behParam*/ 0x00540000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -3258,  3359, -3946, /*angle*/ 0, 0, 0, /*behParam*/ 0x00690000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2639,  3154, -4369, /*angle*/ 0, 0, 0, /*behParam*/ 0x007D0000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2980,  4048, -4248, /*angle*/ 0, 0, 0, /*behParam*/ 0x00240000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -3290,  3636, -4477, /*angle*/ 0, 0, 0, /*behParam*/ 0x004D0000, /*beh*/ bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -160, 1950, -470, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_wmotr_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(wmotr),
    LOAD_SKYBOX(cloud_floor),
    LOAD_TEXTURE_BIN(sky),
    LOAD_GROUPA(group2),
    LOAD_GROUPB(group17),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_3),
    JUMP_LINK(script_func_global_18),

    AREA(/*index*/ 1, wmotr_geo_0001F0),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -67, 2669, -16, /*angle*/ 0, 270, 0, /*behParam*/ WARP_WMOTR_MAIN_ENTRY << 16, /*beh*/ bhvAirborneWarp),
        WARP_NODE(/*id*/ WARP_WMOTR_MAIN_ENTRY, /*destLevel*/ LEVEL_WMOTR,          /*destArea*/ 0x01, /*destNode*/ WARP_WMOTR_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE,         /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_WMOTR_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE,         /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_WMOTR_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_WARP_FLOOR,  /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_GROUNDS_MAIN_ENTRY,          /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN(/*terrainData*/ wmotr_seg7_collision),
        MACRO_OBJECTS(/*objList*/ wmotr_seg7_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_SNOW),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 270, /*pos*/ -67, 1669, -16),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
