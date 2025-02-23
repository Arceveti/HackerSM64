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
#include "levels/bowser_2/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_BOWSER_2_TILTING_ARENA, /*pos*/     0,    0,     0, /*angle*/ 0, 90, 0, /*behParam*/ 0x00000000, /*beh*/ bhvTiltingBowserLavaPlatform),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,            /*pos*/     4, 1329,  3598, /*angle*/ 0, 90, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,            /*pos*/  3584, 1329,     0, /*angle*/ 0, 90, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,            /*pos*/     0, 1329, -3583, /*angle*/ 0, 90, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    OBJECT(/*model*/ MODEL_BOWSER_BOMB,            /*pos*/ -3583, 1329,     0, /*angle*/ 0, 90, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBowserBomb),
    RETURN(),
};

const LevelScript level_bowser_2_entry[] = {
    INIT_LEVEL(),
    LOAD_SKYBOX(bitfs),
    LOAD_TEXTURE_BIN(fire),
    LOAD_EFFECTS(),
    LOAD_LEVEL_DATA(bitfs),
    LOAD_GROUPB(group12),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_13),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_2_TILTING_ARENA, bowser_2_geo_000170),

    AREA(/*index*/ 1, bowser_2_geo_000188),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, 2229, 0, /*angle*/ 0, 180, 0, /*behParam*/ WARP_BOWSER_2_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneCircleWarp),
        WARP_NODE(/*id*/ WARP_BOWSER_2_MAIN_ENTRY, /*destLevel*/ LEVEL_BOWSER_2, /*destArea*/ 0x01, /*destNode*/ WARP_BOWSER_2_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,        /*destLevel*/ LEVEL_CASTLE,   /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_BOWSER_2_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,          /*destLevel*/ LEVEL_BITFS,    /*destArea*/ 0x01, /*destNode*/ WARP_BITFS_FROM_BOWSER_2_DEATH,           /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        TERRAIN(/*terrainData*/ bowser_2_seg7_collision_lava),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_BOSS, /*seq*/ SEQ_LEVEL_BOSS_KOOPA),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 180, /*pos*/ 0, 1229, 0),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
