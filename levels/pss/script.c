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
#include "levels/pss/header.h"

const LevelScript level_pss_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(pss),
    LOAD_TEXTURE_BIN(mountain),
    LOAD_GROUPA(group8),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_9),

    AREA(/*index*/ 1, pss_geo_000100),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 5632, 6751, -5631, /*angle*/ 0, 270, 0, /*behParam*/ WARP_PSS_MAIN_ENTRY << 16, /*beh*/ bhvAirborneWarp),
        WARP_NODE(/*id*/ WARP_PSS_MAIN_ENTRY,  /*destLevel*/ LEVEL_PSS,    /*destArea*/ 0x01, /*destNode*/ WARP_PSS_MAIN_ENTRY,                      /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_WARP_FLOOR, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SECRET_WARP,    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,    /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SECRET_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,      /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SECRET_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ pss_seg7_collision),
        MACRO_OBJECTS(/*objList*/ pss_seg7_macro_objs),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_SLIDE),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_CASTLE, /*seq*/ SEQ_LEVEL_SLIDE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 270, /*pos*/ 5632, 6451, -5631),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
