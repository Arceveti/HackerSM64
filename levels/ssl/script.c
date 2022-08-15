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
#include "levels/ssl/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_SSL_PYRAMID_TOP, /*pos*/ -2047, 1536, -1023, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvPyramidTop),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_SSL_TOX_BOX,     /*pos*/ -1284,    0, -5895, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvToxBox),
    OBJECT(/*model*/ MODEL_SSL_TOX_BOX,     /*pos*/  1283,    0, -4865, /*angle*/ 0, 0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvToxBox),
    OBJECT(/*model*/ MODEL_SSL_TOX_BOX,     /*pos*/  4873,    0, -3335, /*angle*/ 0, 0, 0, /*behParam*/ 0x00020000, /*beh*/ bhvToxBox),
    OBJECT(/*model*/ MODEL_TWEESTER,        /*pos*/ -3600, -200,  2940, /*angle*/ 0, 0, 0, /*behParam*/ 0x00120000, /*beh*/ bhvTweester),
    OBJECT_WITH_ACTS(/*model*/ MODEL_TWEESTER,        /*pos*/  1017, -200,  3832, /*angle*/ 0, 0, 0, /*behParam*/ 0x00190000, /*beh*/ bhvTweester, /*acts*/                         ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_TWEESTER,        /*pos*/  3066, -200,   400, /*angle*/ 0, 0, 0, /*behParam*/ 0x00190000, /*beh*/ bhvTweester, /*acts*/                         ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_KLEPTO,          /*pos*/  2200, 1174, -2820, /*angle*/ 0, 0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvKlepto,   /*acts*/ ACT_1),
    OBJECT_WITH_ACTS(/*model*/ MODEL_KLEPTO,          /*pos*/ -5963,  573, -4784, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvKlepto,   /*acts*/         ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/ -2050, 1200, -580, /*angle*/ 0, 0, 0, /*behParam*/ 0x01000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE, /*pos*/  6000,  800, 3500, /*angle*/ 0, 0, 0, /*behParam*/ 0x04000000, /*beh*/ bhvHiddenRedCoinStar, /*acts*/ ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT(/*model*/ MODEL_NONE,                    /*pos*/  2867,  640,  2867, /*angle*/ 0,   0, 0, /*behParam*/ 0x004D0000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE,                    /*pos*/     0, 3200,  1331, /*angle*/ 0,   0, 0, /*behParam*/ 0x005C0000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_SSL_GRINDEL,             /*pos*/  3297,    0,    95, /*angle*/ 0,   0, 0, /*behParam*/ 0x001C0000, /*beh*/ bhvGrindel),
    OBJECT(/*model*/ MODEL_SSL_GRINDEL,             /*pos*/  -870, 3840,   105, /*angle*/ 0, 180, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHorizontalGrindel),
    OBJECT(/*model*/ MODEL_SSL_GRINDEL,             /*pos*/ -3362,    0, -1385, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvHorizontalGrindel),
    OBJECT(/*model*/ MODEL_SSL_SPINDEL,             /*pos*/ -2458, 2109, -1430, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSpindel),
    OBJECT(/*model*/ MODEL_SSL_MOVING_PYRAMID_WALL, /*pos*/   858, 1927, -2307, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSslMovingPyramidWall),
    OBJECT(/*model*/ MODEL_SSL_MOVING_PYRAMID_WALL, /*pos*/   730, 1927, -2307, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvSslMovingPyramidWall),
    OBJECT(/*model*/ MODEL_SSL_MOVING_PYRAMID_WALL, /*pos*/  1473, 2567, -2307, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvSslMovingPyramidWall),
    OBJECT(/*model*/ MODEL_SSL_MOVING_PYRAMID_WALL, /*pos*/  1345, 2567, -2307, /*angle*/ 0,   0, 0, /*behParam*/ 0x00020000, /*beh*/ bhvSslMovingPyramidWall),
    OBJECT(/*model*/ MODEL_SSL_PYRAMID_ELEVATOR,    /*pos*/     0, 4966,   256, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvPyramidElevator),
    OBJECT(/*model*/ MODEL_NONE,                    /*pos*/  1198, -133,  2396, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSandSoundLoop),
    OBJECT(/*model*/ MODEL_NONE,                    /*pos*/     7, 1229,  -708, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSandSoundLoop),
    OBJECT(/*model*/ MODEL_NONE,                    /*pos*/     7, 4317,  -708, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvSandSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/ 500, 5050, -500, /*angle*/ 0, 0, 0, /*behParam*/ 0x02000000, /*beh*/ bhvStar,       /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE, /*pos*/ 900, 1400, 2350, /*angle*/ 0, 0, 0, /*behParam*/ 0x05040000, /*beh*/ bhvHiddenStar, /*acts*/ ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_6[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ 0, -1534, -3693, /*angle*/ 0, 0, 0, /*behParam*/ 0x03000000, /*beh*/ bhvEyerokBoss),
    RETURN(),
};

const LevelScript level_ssl_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(ssl),
    LOAD_SKYBOX(ssl),
    LOAD_TEXTURE_BIN(generic),
    LOAD_GROUPA(group5),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_6),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_PALM_TREE,           palm_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,       ssl_geo_0005C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,       ssl_geo_0005D8),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_PYRAMID_TOP,         ssl_geo_000618),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_GRINDEL,             ssl_geo_000734),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_SPINDEL,             ssl_geo_000764),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_MOVING_PYRAMID_WALL, ssl_geo_000794),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_PYRAMID_ELEVATOR,    ssl_geo_0007AC),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_TOX_BOX,             ssl_geo_000630),

    AREA(/*index*/ 1, ssl_geo_000648),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   653, 1038,  6566, /*angle*/ 0,  90, 0, /*behParam*/              WARP_SSL_1_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2048,    0,    56, /*angle*/ 0,   0, 0, /*behParam*/              WARP_SSL_1_TO_SSL_2_A << 16, /*beh*/ bhvWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2048,  768, -1024, /*angle*/ 0,   0, 0, /*behParam*/ (15 << 24) | WARP_SSL_1_TO_SSL_2_B << 16, /*beh*/ bhvWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  6930,    0, -4871, /*angle*/ 0, 159, 0, /*behParam*/              WARP_SSL_1_FADING_A   << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5943,    0, -4903, /*angle*/ 0,  49, 0, /*behParam*/              WARP_SSL_1_FADING_B   << 16, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ WARP_SSL_1_MAIN_ENTRY, /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x01, /*destNode*/ WARP_SSL_1_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_1_TO_SSL_2_A, /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x02, /*destNode*/ WARP_SSL_2_MAIN_ENTRY,                 /*flags*/ WARP_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_1_TO_SSL_2_B, /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x02, /*destNode*/ WARP_SSL_2_FROM_SSL_1,                 /*flags*/ WARP_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_1_FADING_A,   /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x01, /*destNode*/ WARP_SSL_1_FADING_B,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_1_FADING_B,   /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x01, /*destNode*/ WARP_SSL_1_FADING_A,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_SSL_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_SSL_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN(/*terrainData*/ ssl_seg7_area_1_collision),
        MACRO_OBJECTS(/*objList*/ ssl_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_HOT),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_SAND),
    END_AREA(),

    AREA(/*index*/ 2, ssl_geo_0007CC),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/    0,  300,  6451, /*angle*/ 0, 180, 0, /*behParam*/ WARP_SSL_2_MAIN_ENTRY << 16, /*beh*/ bhvAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/    0, 5500,   256, /*angle*/ 0, 180, 0, /*behParam*/ WARP_SSL_2_FROM_SSL_1 << 16, /*beh*/ bhvAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 3070, 1280,  2900, /*angle*/ 0, 180, 0, /*behParam*/ WARP_SSL_2_FADING_A   << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 2546, 1150, -2647, /*angle*/ 0,  78, 0, /*behParam*/ WARP_SSL_2_FADING_B   << 16, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ WARP_SSL_2_MAIN_ENTRY, /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x02, /*destNode*/ WARP_SSL_2_MAIN_ENTRY,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_2_FROM_SSL_1, /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x02, /*destNode*/ WARP_SSL_2_FROM_SSL_1,                 /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_2_FADING_A,   /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x02, /*destNode*/ WARP_SSL_2_FADING_B,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_SSL_2_FADING_B,   /*destLevel*/ LEVEL_SSL,    /*destArea*/ 0x02, /*destNode*/ WARP_SSL_2_FADING_A,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_SSL_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_SSL_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        JUMP_LINK(script_func_local_5),
        INSTANT_WARP(/*index*/ 3, /*destArea*/ 3, /*displace*/ 0, 0, 0),
        TERRAIN(/*terrainData*/ ssl_seg7_area_2_collision),
        MACRO_OBJECTS(/*objList*/ ssl_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_CAVE, /*seq*/ SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    AREA(/*index*/ 3, ssl_geo_00088C),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_SSL_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,   /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ WARP_CASTLE_INSIDE_3_FROM_SSL_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_6),
        TERRAIN(/*terrainData*/ ssl_seg7_area_3_collision),
        MACRO_OBJECTS(/*objList*/ ssl_seg7_area_3_macro_objs),
        INSTANT_WARP(/*index*/ 2, /*destArea*/ 2, /*displace*/ 0, 0, 0),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_CAVE, /*seq*/ SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 88, /*pos*/ 653, 38, 6566),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
