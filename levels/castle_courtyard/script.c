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
#include "levels/castle_courtyard/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/     0, 200, -1652, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvAmbientSounds),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2700,   0, -1652, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBirdsSoundLoop),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  2700,   0, -1652, /*angle*/ 0, 0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvBirdsSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_BOO, /*pos*/ -3217, 100,  -101, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvCourtyardBooTriplet),
    OBJECT(/*model*/ MODEL_BOO, /*pos*/  3317, 100, -1701, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvCourtyardBooTriplet),
    OBJECT(/*model*/ MODEL_BOO, /*pos*/   -71,   1, -1387, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvCourtyardBooTriplet),
    RETURN(),
};

const LevelScript level_castle_courtyard_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(castle_courtyard),
    LOAD_SKYBOX(water),
    LOAD_TEXTURE_BIN(outside),
    LOAD_GROUPA(group9),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_10),
    LOAD_MODEL_FROM_GEO(MODEL_COURTYARD_SPIKY_TREE,  spiky_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_COURTYARD_WOODEN_DOOR, wooden_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,     castle_courtyard_geo_000200),

    AREA(/*index*/ 1, castle_courtyard_geo_000218),
        OBJECT(/*model*/ MODEL_BOO,  /*pos*/ -2360, -100, -2712, /*angle*/ 0,   0, 0, /*behParam*/ (1 << 24) | WARP_COURTYARD_TO_BBH           << 16, /*beh*/ bhvBooWithCage),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/     0,   51, -1000, /*angle*/ 0, 180, 0, /*behParam*/             WARP_COURTYARD_FROM_BBH_SUCCESS << 16, /*beh*/ bhvLaunchStarCollectWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/     0,   51, -1000, /*angle*/ 0, 180, 0, /*behParam*/             WARP_COURTYARD_FROM_BBH_DEATH   << 16, /*beh*/ bhvLaunchDeathWarp),
        WARP_NODE(/*id*/ WARP_COURTYARD_TO_BBH,           /*destLevel*/ LEVEL_BBH,              /*destArea*/ 0x01, /*destNode*/ WARP_BBH_MAIN_ENTRY,             /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_COURTYARD_FROM_BBH_SUCCESS, /*destLevel*/ LEVEL_CASTLE_COURTYARD, /*destArea*/ 0x01, /*destNode*/ WARP_COURTYARD_FROM_BBH_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_COURTYARD_FROM_BBH_DEATH,   /*destLevel*/ LEVEL_CASTLE_COURTYARD, /*destArea*/ 0x01, /*destNode*/ WARP_COURTYARD_FROM_BBH_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_COURTYARD_TO_CASTLE_INSIDE, /*destLevel*/ LEVEL_CASTLE,           /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_TO_COURTYARD, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,                 /*destLevel*/ LEVEL_CASTLE_GROUNDS,   /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_GROUNDS_DEATH_EXIT,  /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN(/*terrainData*/ castle_courtyard_seg7_collision),
        MACRO_OBJECTS(/*objList*/ castle_courtyard_seg7_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_SOUND_PLAYER),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 0, /*pos*/ -14, 0, -201),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
