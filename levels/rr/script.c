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
#include "levels/rr/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_RR_SLIDING_PLATFORM,         /*pos*/ -3400, -2038,  6564, /*angle*/  0,   0,   0, /*behParam*/ 0x04560000, /*beh*/ bhvSlidingPlatform2),
    OBJECT(/*model*/ MODEL_RR_SLIDING_PLATFORM,         /*pos*/ -2684, -1423,   -36, /*angle*/  0,   0,   0, /*behParam*/ 0x04590000, /*beh*/ bhvSlidingPlatform2),
    OBJECT(/*model*/ MODEL_RR_FLYING_CARPET,            /*pos*/  4571, -1782,  2036, /*angle*/  0, 180,   0, /*behParam*/ 0x00000000, /*beh*/ bhvPlatformOnTrack),
    OBJECT(/*model*/ MODEL_RR_FLYING_CARPET,            /*pos*/   580,  -963, -3659, /*angle*/  0, 180,   0, /*behParam*/ 0x00010000, /*beh*/ bhvPlatformOnTrack),
    OBJECT(/*model*/ MODEL_RR_FLYING_CARPET,            /*pos*/  1567,   880,  -184, /*angle*/  0, 180,   0, /*behParam*/ 0x00070000, /*beh*/ bhvPlatformOnTrack),
    OBJECT(/*model*/ MODEL_RR_FLYING_CARPET,            /*pos*/   646,   880,  -184, /*angle*/  0,   0,   0, /*behParam*/ 0x00080000, /*beh*/ bhvPlatformOnTrack),
    OBJECT(/*model*/ MODEL_RR_OCTAGONAL_PLATFORM,       /*pos*/   644, -1321, -1301, /*angle*/  0, 180,   0, /*behParam*/ 0x03010000, /*beh*/ bhvOctagonalPlatformRotating),
    OBJECT(/*model*/ MODEL_RR_OCTAGONAL_PLATFORM,       /*pos*/  1797, -1321,   -56, /*angle*/  0,   0,   0, /*behParam*/ 0x00010000, /*beh*/ bhvOctagonalPlatformRotating),
    OBJECT(/*model*/ MODEL_RR_OCTAGONAL_PLATFORM,       /*pos*/   663, -1321,  1179, /*angle*/  0, 180,   0, /*behParam*/ 0x03010000, /*beh*/ bhvOctagonalPlatformRotating),
    OBJECT(/*model*/ MODEL_RR_OCTAGONAL_PLATFORM,       /*pos*/  -502, -1321,   -51, /*angle*/  0,   0,   0, /*behParam*/ 0x00010000, /*beh*/ bhvOctagonalPlatformRotating),
    OBJECT(/*model*/ MODEL_RR_ROTATING_BRIDGE_PLATFORM, /*pos*/  5043,  1342,   300, /*angle*/  0,   0,   0, /*behParam*/ 0x00010000, /*beh*/ bhvRrRotatingBridgePlatform),
    OBJECT(/*model*/ MODEL_RR_CRUISER_WING,             /*pos*/  3473,  2422, -1821, /*angle*/  0,   0,   0, /*behParam*/ 0x00000000, /*beh*/ bhvRrCruiserWing),
    OBJECT(/*model*/ MODEL_RR_CRUISER_WING,             /*pos*/  4084,  2431, -2883, /*angle*/ 45, 180, 180, /*behParam*/ 0x00010000, /*beh*/ bhvRrCruiserWing),
    OBJECT(/*model*/ MODEL_RR_CRUISER_WING,             /*pos*/  3470,  2420, -2869, /*angle*/ 45, 180, 180, /*behParam*/ 0x00010000, /*beh*/ bhvRrCruiserWing),
    OBJECT(/*model*/ MODEL_RR_CRUISER_WING,             /*pos*/  2856,  2410, -2855, /*angle*/ 45, 180, 180, /*behParam*/ 0x00010000, /*beh*/ bhvRrCruiserWing),
    OBJECT(/*model*/ MODEL_RR_CRUISER_WING,             /*pos*/  4101,  2435, -1813, /*angle*/  0,   0,   0, /*behParam*/ 0x00000000, /*beh*/ bhvRrCruiserWing),
    OBJECT(/*model*/ MODEL_RR_CRUISER_WING,             /*pos*/  2859,  2411, -1834, /*angle*/  0,   0,   0, /*behParam*/ 0x00000000, /*beh*/ bhvRrCruiserWing),
    OBJECT(/*model*/ MODEL_RR_SEESAW_PLATFORM,          /*pos*/ -6013, -2857,  6564, /*angle*/  0, 270,   0, /*behParam*/ 0x00050000, /*beh*/ bhvSeesawPlatform),
    OBJECT(/*model*/ MODEL_RR_SEESAW_PLATFORM,          /*pos*/   614, -3574,  6564, /*angle*/  0, 270,   0, /*behParam*/ 0x00050000, /*beh*/ bhvSeesawPlatform),
    OBJECT(/*model*/ MODEL_RR_SWINGING_PLATFORM,        /*pos*/ -3557,  -809,  4619, /*angle*/  0,   0,   0, /*behParam*/ 0x00000000, /*beh*/ bhvSwingPlatform),
    OBJECT(/*model*/ MODEL_RR_SWINGING_PLATFORM,        /*pos*/ -2213, -2582,  6257, /*angle*/  0,   0,   0, /*behParam*/ 0x00000000, /*beh*/ bhvSwingPlatform),
    OBJECT(/*model*/ MODEL_RR_ELEVATOR_PLATFORM,        /*pos*/ -2684,  1546,   -36, /*angle*/  0,   0,   0, /*behParam*/ 0x00050000, /*beh*/ bhvRrElevatorPlatform),
    OBJECT(/*model*/ MODEL_RR_TRICKY_TRIANGLES,         /*pos*/  5862, -1347,  6564, /*angle*/  0,   0,   0, /*behParam*/ 0x00020000, /*beh*/ bhvAnimatesOnFloorSwitchPress),
    OBJECT(/*model*/ MODEL_PURPLE_SWITCH,               /*pos*/  4428, -1936,  6564, /*angle*/  0,   0,   0, /*behParam*/ 0x00000000, /*beh*/ bhvFloorSwitchAnimatesObject),
    OBJECT(/*model*/ MODEL_NONE,                        /*pos*/   614, -2857,  3671, /*angle*/  0,   0,   0, /*behParam*/ 0x00CC0000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE,                        /*pos*/   621, -4598,  7362, /*angle*/  0,   0,   0, /*behParam*/ 0x00750000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE,                        /*pos*/  5119,  3819,  3325, /*angle*/  0,   0,   0, /*behParam*/ 0x00610000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE,                        /*pos*/  3554,  2891, -2327, /*angle*/  0,   0,   0, /*behParam*/ 0x00C10000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE,                        /*pos*/  2680,   214,   295, /*angle*/  0,   0,   0, /*behParam*/ 0x00620000, /*beh*/ bhvPoleGrabbing),
    OBJECT(/*model*/ MODEL_NONE,                        /*pos*/  3811,  1033,   295, /*angle*/  0,   0,   0, /*behParam*/ 0x00620000, /*beh*/ bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4887, -2908,  6564, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4376, -2089,  6564, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  -179, -2089,  6564, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  1356, -2089,  6564, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  2482, -2089,  6564, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -6730, -2089,  6522, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -7345, -2447,  6522, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  -169,  1229,  -173, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  1282,  2520,   -32, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1165,  2662,  -187, /*angle*/ 0, 0, 0, /*behParam*/ 0x00030000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2156,  2211,   -87, /*angle*/ 0, 0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvDonutRow),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5809, -1834,  5719, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvFlamethrower),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4838, -1015,  4081, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvFlamethrower),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  3301, -1834,  5617, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvFlamethrower),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/  6772,  -757,  -606, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvFlamethrower),
    OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4187,  3213, -6630, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvFlamethrower),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/  1450,  3400, -2352, /*angle*/ 0, 0, 0, /*behParam*/ 0x00000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/ -4200,  6700, -4450, /*angle*/ 0, 0, 0, /*behParam*/ 0x01000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE, /*pos*/ -5150, -1400,     0, /*angle*/ 0, 0, 0, /*behParam*/ 0x02000000, /*beh*/ bhvHiddenRedCoinStar, /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/ -5850,  -700,  4950, /*angle*/ 0, 0, 0, /*behParam*/ 0x03000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR, /*pos*/  3700,  -400,  6600, /*angle*/ 0, 0, 0, /*behParam*/ 0x04000000, /*beh*/ bhvStar,              /*acts*/ ALL_ACTS),
    RETURN(),
};

const LevelScript level_rr_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(rr),
    LOAD_TEXTURE_BIN(sky),
    LOAD_SKYBOX(cloud_floor),
    LOAD_GROUPA(group11),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_12),
    JUMP_LINK(script_func_global_1),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,           rr_geo_000660),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,           rr_geo_000678),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,           rr_geo_000690),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,           rr_geo_0006A8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,           rr_geo_0006C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,           rr_geo_0006D8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,           rr_geo_0006F0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,           rr_geo_000708),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,           rr_geo_000720),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,           rr_geo_000738),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,           rr_geo_000758),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,           rr_geo_000770),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,           rr_geo_000788),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_10,           rr_geo_0007A0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,           rr_geo_0007B8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_12,           rr_geo_0007D0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_13,           rr_geo_0007E8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_14,           rr_geo_000800),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_15,           rr_geo_000818),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_16,           rr_geo_000830),
    LOAD_MODEL_FROM_GEO(MODEL_RR_SLIDING_PLATFORM,         rr_geo_0008C0),
    LOAD_MODEL_FROM_GEO(MODEL_RR_FLYING_CARPET,            rr_geo_000848),
    LOAD_MODEL_FROM_GEO(MODEL_RR_OCTAGONAL_PLATFORM,       rr_geo_0008A8),
    LOAD_MODEL_FROM_GEO(MODEL_RR_ROTATING_BRIDGE_PLATFORM, rr_geo_000878),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRIANGLE_PLATFORM,        rr_geo_0008D8),
    LOAD_MODEL_FROM_GEO(MODEL_RR_CRUISER_WING,             rr_geo_000890),
    LOAD_MODEL_FROM_GEO(MODEL_RR_SEESAW_PLATFORM,          rr_geo_000908),
    LOAD_MODEL_FROM_GEO(MODEL_RR_L_SHAPED_PLATFORM,        rr_geo_000940),
    LOAD_MODEL_FROM_GEO(MODEL_RR_SWINGING_PLATFORM,        rr_geo_000860),
    LOAD_MODEL_FROM_GEO(MODEL_RR_DONUT_PLATFORM,           rr_geo_000920),
    LOAD_MODEL_FROM_GEO(MODEL_RR_ELEVATOR_PLATFORM,        rr_geo_0008F0),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES,         rr_geo_000958),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME1,  rr_geo_000970),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME2,  rr_geo_000988),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME3,  rr_geo_0009A0),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME4,  rr_geo_0009B8),

    AREA(/*index*/ 1, rr_geo_0009D0),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  2599, -833,  2071, /*angle*/ 0,  90, 0, /*behParam*/ WARP_RR_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -7092, 2364,   -63, /*angle*/ 0,  90, 0, /*behParam*/ WARP_RR_FADING_A   << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -4213, 3379, -2815, /*angle*/ 0, 180, 0, /*behParam*/ WARP_RR_FADING_B   << 16, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ WARP_RR_MAIN_ENTRY, /*destLevel*/ LEVEL_RR,     /*destArea*/ 0x01, /*destNode*/ WARP_RR_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_RR_FADING_A,   /*destLevel*/ LEVEL_RR,     /*destArea*/ 0x01, /*destNode*/ WARP_RR_FADING_B,                     /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_RR_FADING_B,   /*destLevel*/ LEVEL_RR,     /*destArea*/ 0x01, /*destNode*/ WARP_RR_FADING_A,                     /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,  /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_RR_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,    /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ WARP_CASTLE_INSIDE_2_FROM_RR_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN(/*terrainData*/ rr_seg7_collision_level),
        MACRO_OBJECTS(/*objList*/ rr_seg7_macro_objs),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 90, /*pos*/ 2599, -1833, 2071),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
