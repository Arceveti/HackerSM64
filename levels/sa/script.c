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
#include "levels/sa/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, -1000, 0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvFishSpawner),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, -1000, 0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00020000, /*beh*/ bhvFishSpawner),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, -4250, 0, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_sa_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(sa),
    LOAD_TEXTURE_BIN(inside),
    LOAD_SKYBOX(cloud_floor),
    LOAD_EFFECTS(),
    LOAD_GROUPA(group4),
    LOAD_GROUPB(group13),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_5),
    JUMP_LINK(script_func_global_14),

    AREA(/*index*/ 1, sa_geo_000170),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, -1535, 0, /*angle*/ 0, 90, 0, /*behParam*/ WARP_SA_MAIN_ENTRY << 16, /*beh*/ bhvSwimmingWarp),
        WARP_NODE(/*id*/ WARP_SA_MAIN_ENTRY, /*destLevel*/ LEVEL_SA,     /*destArea*/ 0x01, /*destNode*/ WARP_SA_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,  /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SA_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,    /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_SA_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN(/*terrainData*/ sa_seg7_collision),
        MACRO_OBJECTS(/*objList*/ sa_seg7_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_WATER, /*seq*/ (SEQ_LEVEL_WATER | SEQ_FLAG_VARIATION)),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_WATER),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 90, /*pos*/ 0, -1535, 0),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
