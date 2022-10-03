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
#include "actors/group12.h"

#include "make_const_nonconst.h"
#include "levels/bowser_1/header.h"

const LevelScript level_bowser_1_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(bowser_1),
    LOAD_SKYBOX(bidw),
    LOAD_GROUPB(group12),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_13),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, bowser_1_yellow_sphere_geo),

    AREA(/*index*/ 1, bowser_1_geo_0000D0),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, 1307, 0, /*angle*/ 0, 180, 0, /*behParam*/ WARP_BOWSER_1_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneCircleWarp),
        WARP_NODE(/*id*/ WARP_BOWSER_1_MAIN_ENTRY, /*destLevel*/ LEVEL_BOWSER_1, /*destArea*/ 0x01, /*destNode*/ WARP_BOWSER_1_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,        /*destLevel*/ LEVEL_CASTLE,   /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_BOWSER_1_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,          /*destLevel*/ LEVEL_BITDW,    /*destArea*/ 0x01, /*destNode*/ WARP_BITDW_FROM_BOWSER_1_DEATH,             /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ bowser_1_seg7_collision_level),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_BOSS, /*seq*/ SEQ_LEVEL_BOSS_KOOPA),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 180, /*pos*/ 0, 307, 0),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
