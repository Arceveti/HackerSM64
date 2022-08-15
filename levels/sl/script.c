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
#include "levels/sl/header.h"


static const LevelScript script_func_local_1[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/  700, 4500,  690, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/ 4350, 1350, 4350, /*angle*/ 0, 0, 0, /*behParam*/ 0x02000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE, /*pos*/ 5000, 1200,    0, /*angle*/ 0, 0, 0, /*behParam*/ 0x04000000, /*beh*/ bhvHiddenRedCoinStar, /*acts*/ ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  977, 1024, 2075, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSnowMoundSpawn),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT(/*model*/ MODEL_PENGUIN,            /*pos*/ 1715, 3328,   518, /*angle*/ 0, -51, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSLWalkingPenguin),
    OBJECT(/*model*/ MODEL_NONE,               /*pos*/  700, 3428,   700, /*angle*/ 0,  30, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSLSnowmanWind),
    OBJECT(/*model*/ MODEL_NONE,               /*pos*/  480, 2300,  1370, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvIgloo),
    OBJECT(/*model*/ MODEL_BIG_CHILL_BULLY,    /*pos*/  315, 1331, -4852, /*angle*/ 0,   0, 0, /*behParam*/ 0x01000000, /*beh*/ bhvBigChillBully),
    OBJECT(/*model*/ MODEL_MR_BLIZZARD_HIDDEN, /*pos*/ 2954,  970,   750, /*angle*/ 0,   0, 0, /*behParam*/ 0x00020000, /*beh*/ bhvMrBlizzard),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/ 0, 500, 1000, /*angle*/ 0, 0, 0, /*behParam*/ 0x05000000, /*beh*/ bhvStar, /*acts*/ ALL_ACTS),
    RETURN(),
};

const LevelScript level_sl_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(sl),
    LOAD_TEXTURE_BIN(snow),
    LOAD_EFFECTS(),
    LOAD_SKYBOX(ccm),
    LOAD_GROUPA(group7),
    LOAD_GROUPB(group16),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_8),
    JUMP_LINK(script_func_global_17),
    LOAD_MODEL_FROM_GEO(MODEL_SL_SNOW_TRIANGLE,      sl_geo_000390),
    LOAD_MODEL_FROM_GEO(MODEL_SL_CRACKED_ICE,        sl_geo_000360),
    LOAD_MODEL_FROM_GEO(MODEL_SL_CRACKED_ICE_CHUNK,  sl_geo_000378),
    LOAD_MODEL_FROM_GEO(MODEL_SL_SNOW_TREE,          snow_tree_geo),

    AREA(/*index*/ 1, sl_geo_0003A8),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  5541, 2024,   443, /*angle*/ 0, 270, 0, /*behParam*/             WARP_SL_1_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   257, 2150,  1399, /*angle*/ 0, 290, 0, /*behParam*/             WARP_SL_1_FROM_SL_2  << 16, /*beh*/ bhvInstantActiveWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   569, 2150,  1336, /*angle*/ 0,   0, 0, /*behParam*/ (6 << 24) | WARP_SL_1_TO_SL_2    << 16, /*beh*/ bhvWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  5468, 1056, -5400, /*angle*/ 0, -20, 0, /*behParam*/             WARP_SL_1_FADING_A   << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -3698, 1024, -1237, /*angle*/ 0,   6, 0, /*behParam*/             WARP_SL_1_FADING_B   << 16, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ WARP_SL_1_MAIN_ENTRY, /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x01, /*destNode*/ WARP_SL_1_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SL_1_FROM_SL_2,  /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x01, /*destNode*/ WARP_SL_1_FROM_SL_2,                  /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SL_1_TO_SL_2,    /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x02, /*destNode*/ WARP_SL_2_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SL_1_FADING_A,   /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x01, /*destNode*/ WARP_SL_1_FADING_B,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SL_1_FADING_B,   /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x01, /*destNode*/ WARP_SL_1_FADING_A,                   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,    /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_SL_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,      /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_SL_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ sl_seg7_area_1_collision),
        MACRO_OBJECTS(/*objList*/ sl_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_SNOW),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_SNOW),
    END_AREA(),

    AREA(/*index*/ 2, sl_geo_000484),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, 0, 2867, /*angle*/ 0, 180, 0, /*behParam*/              WARP_SL_2_MAIN_ENTRY << 16, /*beh*/ bhvInstantActiveWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, 0, 3277, /*angle*/ 0,   0, 0, /*behParam*/ (20 << 24) | WARP_SL_2_TO_SL_1    << 16, /*beh*/ bhvWarp),
        WARP_NODE(/*id*/ WARP_SL_2_MAIN_ENTRY, /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x02, /*destNode*/ WARP_SL_2_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SL_2_TO_SL_1,    /*destLevel*/ LEVEL_SL,     /*destArea*/ 0x01, /*destNode*/ WARP_SL_1_FROM_SL_2,                  /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,    /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_SL_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,      /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_SL_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ sl_seg7_area_2_collision),
        MACRO_OBJECTS(/*objList*/ sl_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_CAVE, /*seq*/ SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_SNOW),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 270, /*pos*/ 5541, 1024, 443),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
