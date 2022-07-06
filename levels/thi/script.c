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
#include "levels/thi/header.h"

static const LevelScript script_func_local_1[] = {
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,             /*pos*/     0,  -700, -4500, /*angle*/ 0,   0, 0, /*behParam*/ 0x03000000, /*beh*/ bhvHiddenStar,        /*acts*/ ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,             /*pos*/ -1800,   800, -1500, /*angle*/ 0,   0, 0, /*behParam*/ 0x04000000, /*beh*/ bhvHiddenRedCoinStar, /*acts*/ ALL_ACTS),
    OBJECT(/*model*/ MODEL_WIGGLER_HEAD,     /*pos*/    17,  1843,   -62, /*angle*/ 0,   0, 0, /*behParam*/ 0x05000000, /*beh*/ bhvWigglerHead),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_KOOPA_WITH_SHELL, /*pos*/ -1900,  -511,  2400, /*angle*/ 0, -30, 0, /*behParam*/ 0x02030000, /*beh*/ bhvKoopa,             /*acts*/ ACT_3),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,             /*pos*/  7400, -1537, -6300, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvKoopaRaceEndpoint, /*acts*/ ACT_3),
    OBJECT(/*model*/ MODEL_NONE,             /*pos*/ -6556, -2969,  6565, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvGoombaTripletSpawner),
    OBJECT(/*model*/ MODEL_GOOMBA,           /*pos*/  6517, -2559,  4327, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvGoomba),
    OBJECT(/*model*/ MODEL_PIRANHA_PLANT,    /*pos*/ -6336, -2047, -3861, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvFirePiranhaPlant),
    OBJECT(/*model*/ MODEL_PIRANHA_PLANT,    /*pos*/ -5740, -2047, -6578, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvFirePiranhaPlant),
    OBJECT(/*model*/ MODEL_PIRANHA_PLANT,    /*pos*/ -6481, -2047, -5998, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvFirePiranhaPlant),
    OBJECT(/*model*/ MODEL_PIRANHA_PLANT,    /*pos*/ -5577, -2047, -4961, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvFirePiranhaPlant),
    OBJECT(/*model*/ MODEL_PIRANHA_PLANT,    /*pos*/ -6865, -2047, -4568, /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvFirePiranhaPlant),
    OBJECT(/*model*/ MODEL_NONE,             /*pos*/ -4413,   204, -2140, /*angle*/ 0,   0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvThiBowlingBallSpawner),
    OBJECT(/*model*/ MODEL_BUBBA,            /*pos*/ -6241, -3300,  -716, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBubba),
    OBJECT(/*model*/ MODEL_BUBBA,            /*pos*/  1624, -3300,  8142, /*angle*/ 0,   0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvBubba),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT(/*model*/ MODEL_THI_HUGE_ISLAND_TOP, /*pos*/     0, 3891, -1533, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvThiHugeIslandTop),
    RETURN(),
};

static const LevelScript script_func_local_6[] = {
    OBJECT(/*model*/ MODEL_THI_TINY_ISLAND_TOP, /*pos*/     0, 1167,  -460, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvThiTinyIslandTop),
    OBJECT(/*model*/ MODEL_NONE,                /*pos*/ -1382,   80,  -649, /*angle*/ 0, 0, 0, /*behParam*/ 0x00040000, /*beh*/ bhvThiBowlingBallSpawner),
    RETURN(),
};

static const LevelScript script_func_local_7[] = {
    OBJECT(/*model*/ MODEL_THI_WARP_PIPE, /*pos*/  6656, -1536, -5632, /*angle*/ 0, 0, 0, /*behParam*/ WARP_THI_1_WARP_PIPE_A << 16, /*beh*/ bhvWarpPipe),
    OBJECT(/*model*/ MODEL_THI_WARP_PIPE, /*pos*/ -5888, -2048, -5888, /*angle*/ 0, 0, 0, /*behParam*/ WARP_THI_1_WARP_PIPE_B << 16, /*beh*/ bhvWarpPipe),
    OBJECT(/*model*/ MODEL_THI_WARP_PIPE, /*pos*/ -3072,   512, -3840, /*angle*/ 0, 0, 0, /*behParam*/ WARP_THI_1_WARP_PIPE_C << 16, /*beh*/ bhvWarpPipe),
    WARP_NODE(/*id*/ WARP_THI_1_WARP_PIPE_A, /*destLevel*/ LEVEL_THI, /*destArea*/ 0x02, /*destNode*/ WARP_THI_2_WARP_PIPE_A, /*flags*/ WARP_NO_CHECKPOINT),
    WARP_NODE(/*id*/ WARP_THI_1_WARP_PIPE_B, /*destLevel*/ LEVEL_THI, /*destArea*/ 0x02, /*destNode*/ WARP_THI_2_WARP_PIPE_B, /*flags*/ WARP_NO_CHECKPOINT),
    WARP_NODE(/*id*/ WARP_THI_1_WARP_PIPE_C, /*destLevel*/ LEVEL_THI, /*destArea*/ 0x02, /*destNode*/ WARP_THI_2_WARP_PIPE_C, /*flags*/ WARP_NO_CHECKPOINT),
    RETURN(),
};

static const LevelScript script_func_local_8[] = {
    OBJECT(/*model*/ MODEL_THI_WARP_PIPE, /*pos*/  1997,  -461, -1690, /*angle*/ 0, 0, 0, /*behParam*/ WARP_THI_2_WARP_PIPE_A << 16, /*beh*/ bhvWarpPipe),
    OBJECT(/*model*/ MODEL_THI_WARP_PIPE, /*pos*/ -1766,  -614, -1766, /*angle*/ 0, 0, 0, /*behParam*/ WARP_THI_2_WARP_PIPE_B << 16, /*beh*/ bhvWarpPipe),
    OBJECT(/*model*/ MODEL_THI_WARP_PIPE, /*pos*/  -922,   154, -1152, /*angle*/ 0, 0, 0, /*behParam*/ WARP_THI_2_WARP_PIPE_C << 16, /*beh*/ bhvWarpPipe),
    WARP_NODE(/*id*/ WARP_THI_2_WARP_PIPE_A, /*destLevel*/ LEVEL_THI, /*destArea*/ 0x01, /*destNode*/ WARP_THI_1_WARP_PIPE_A, /*flags*/ WARP_NO_CHECKPOINT),
    WARP_NODE(/*id*/ WARP_THI_2_WARP_PIPE_B, /*destLevel*/ LEVEL_THI, /*destArea*/ 0x01, /*destNode*/ WARP_THI_1_WARP_PIPE_B, /*flags*/ WARP_NO_CHECKPOINT),
    WARP_NODE(/*id*/ WARP_THI_2_WARP_PIPE_C, /*destLevel*/ LEVEL_THI, /*destArea*/ 0x01, /*destNode*/ WARP_THI_1_WARP_PIPE_C, /*flags*/ WARP_NO_CHECKPOINT),
    RETURN(),
};

const LevelScript level_thi_entry[] = {
    INIT_LEVEL(),
    LOAD_YAY0(        /*seg*/ 0x07, _thi_segment_7SegmentRomStart, _thi_segment_7SegmentRomEnd),
    LOAD_YAY0_TEXTURE(/*seg*/ 0x09, _grass_yay0SegmentRomStart, _grass_yay0SegmentRomEnd),
    LOAD_YAY0(        /*seg*/ 0x0A, _water_skybox_yay0SegmentRomStart, _water_skybox_yay0SegmentRomEnd),
    LOAD_YAY0(        /*seg*/ 0x05, _group11_yay0SegmentRomStart, _group11_yay0SegmentRomEnd),
    LOAD_RAW(         /*seg*/ 0x0C, _group11_geoSegmentRomStart,  _group11_geoSegmentRomEnd),
    LOAD_YAY0(        /*seg*/ 0x06, _group14_yay0SegmentRomStart, _group14_yay0SegmentRomEnd),
    LOAD_RAW(         /*seg*/ 0x0D, _group14_geoSegmentRomStart,  _group14_geoSegmentRomEnd),
    LOAD_YAY0(        /*seg*/ 0x08, _common0_yay0SegmentRomStart, _common0_yay0SegmentRomEnd),
    LOAD_RAW(         /*seg*/ 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_12),
    JUMP_LINK(script_func_global_15),
    LOAD_MODEL_FROM_GEO(MODEL_THI_BUBBLY_TREE,     bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,   thi_geo_0005F0),
    LOAD_MODEL_FROM_GEO(MODEL_THI_WARP_PIPE,       warp_pipe_geo),
    LOAD_MODEL_FROM_GEO(MODEL_THI_HUGE_ISLAND_TOP, thi_geo_0005B0),
    LOAD_MODEL_FROM_GEO(MODEL_THI_TINY_ISLAND_TOP, thi_geo_0005C8),

    AREA(/*index*/ 1, thi_geo_000608),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -7372, -1969,  7373, /*angle*/ 0, 149, 0, /*behParam*/              WARP_THI_1_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   410,  -512,   922, /*angle*/ 0,   0, 0, /*behParam*/              WARP_THI_1_FROM_THI_3 << 16, /*beh*/ bhvInstantActiveWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   410,  -512,   717, /*angle*/ 0,   0, 0, /*behParam*/ ( 5 << 24) | WARP_THI_1_TO_THI_3_A << 16, /*beh*/ bhvWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/     0,  3170, -1570, /*angle*/ 0,   0, 0, /*behParam*/ (10 << 24) | WARP_THI_1_TO_THI_3_B << 16, /*beh*/ bhvWarp),
        WARP_NODE(/*id*/ WARP_THI_1_MAIN_ENTRY, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x01, /*destNode*/ WARP_THI_1_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_1_FROM_THI_3, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x01, /*destNode*/ WARP_THI_1_FROM_THI_3,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_1_TO_THI_3_A, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x03, /*destNode*/ WARP_THI_3_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_1_TO_THI_3_B, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x03, /*destNode*/ WARP_THI_3_FROM_THI_1,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_THI_1_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_THI_1_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_7),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_5),
        JUMP_LINK(script_func_local_4),
        TERRAIN(/*terrainData*/ thi_seg7_area_1_collision),
        MACRO_OBJECTS(/*objList*/ thi_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ 0x0000, /*seq*/ SEQ_LEVEL_GRASS),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_GRASS),
    END_AREA(),

    AREA(/*index*/ 2, thi_geo_0006D4),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2211,  110,  2212, /*angle*/ 0,  149, 0, /*behParam*/ WARP_THI_2_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   280, -767, -4180, /*angle*/ 0,    0, 0, /*behParam*/ WARP_THI_2_FADING_A   << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1638,    0, -1988, /*angle*/ 0, -126, 0, /*behParam*/ WARP_THI_2_FADING_B   << 16, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ WARP_THI_2_MAIN_ENTRY, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x02, /*destNode*/ WARP_THI_2_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_2_FADING_A,   /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x02, /*destNode*/ WARP_THI_2_FADING_B,                     /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_2_FADING_B,   /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x02, /*destNode*/ WARP_THI_2_FADING_A,                     /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_THI_2_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_THI_2_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_8),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_6),
        TERRAIN(/*terrainData*/ thi_seg7_area_2_collision),
        MACRO_OBJECTS(/*objList*/ thi_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ 0x0000, /*seq*/ SEQ_LEVEL_GRASS),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_GRASS),
    END_AREA(),

    AREA(/*index*/ 3, thi_geo_00079C),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 512, 1024, 2150, /*angle*/ 0, 180, 0, /*behParam*/             WARP_THI_3_MAIN_ENTRY << 16, /*beh*/ bhvInstantActiveWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   0, 3277,    0, /*angle*/ 0,   0, 0, /*behParam*/             WARP_THI_3_FROM_THI_1 << 16, /*beh*/ bhvAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ 512, 1024, 2355, /*angle*/ 0,   0, 0, /*behParam*/ (5 << 24) | WARP_THI_3_TO_THI_1   << 16, /*beh*/ bhvWarp),
        WARP_NODE(/*id*/ WARP_THI_3_MAIN_ENTRY, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x03, /*destNode*/ WARP_THI_3_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_3_FROM_THI_1, /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x03, /*destNode*/ WARP_THI_3_FROM_THI_1,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_THI_3_TO_THI_1,   /*destLevel*/ LEVEL_THI,    /*destArea*/ 0x01, /*destNode*/ WARP_THI_1_FROM_THI_3,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_THI_1_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,       /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_THI_1_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_3),
        TERRAIN(/*terrainData*/ thi_seg7_area_3_collision),
        MACRO_OBJECTS(/*objList*/ thi_seg7_area_3_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ 0x0004, /*seq*/ SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_GRASS),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 149, /*pos*/ -7372, -2969, 7373),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
