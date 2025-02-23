#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/bob/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT(/*model*/ MODEL_BOB_CHAIN_CHOMP_GATE,  /*pos*/  1456,   768,   446, /*angle*/ 0, 326, 0,  /*behParam*/ 0x00000000, /*beh*/ bhvChainChompGate),
    OBJECT(/*model*/ MODEL_BOB_SEESAW_PLATFORM,   /*pos*/ -2303,   717,  1024, /*angle*/ 0, 45, 0,   /*behParam*/ 0x00030000, /*beh*/ bhvSeesawPlatform),
    OBJECT(/*model*/ MODEL_NONE,                  /*pos*/ -2050,     0, -3069, /*angle*/ 0, 25, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvOpenableGrill),
    OBJECT(/*model*/ MODEL_PURPLE_SWITCH,         /*pos*/ -2283,     0, -3682, /*angle*/ 0, 27, 0,   /*behParam*/ 0x00000000, /*beh*/ bhvFloorSwitchGrills),
    OBJECT(/*model*/ MODEL_CHECKERBOARD_PLATFORM, /*pos*/  1612,   300,  4611, /*angle*/ 0, 0, 0,    /*behParam*/ 0x00280000, /*beh*/ bhvCheckerboardElevatorGroup),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/  1535, 3840, -5561, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvBobBowlingBallSpawner,  /*acts*/ ACT_1 | ACT_2),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/  1535, 3840, -5561, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvTtmBowlingBallSpawner,  /*acts*/                 ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/   524, 2825, -5400, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00020000,       /*beh*/ bhvBobBowlingBallSpawner,  /*acts*/ ACT_1 | ACT_2),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/   524, 2825, -5400, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00020000,       /*beh*/ bhvTtmBowlingBallSpawner,  /*acts*/                 ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT          (/*model*/ MODEL_BOWLING_BALL,          /*pos*/  -993,  886, -3565, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvPitBowlingBall),
    OBJECT          (/*model*/ MODEL_BOWLING_BALL,          /*pos*/  -785,  886, -4301, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvPitBowlingBall),
    OBJECT_WITH_ACTS(/*model*/ MODEL_BOWLING_BALL,          /*pos*/   -93,  886, -3414, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvPitBowlingBall,         /*acts*/         ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_BOBOMB_BUDDY,          /*pos*/ -5723,  140,  6017, /*angle*/ 0, 0, 0,   /*behParam*/ DIALOG_002 << 16, /*beh*/ bhvBobombBuddy,            /*acts*/ ACT_1),
    OBJECT_WITH_ACTS(/*model*/ MODEL_BOBOMB_BUDDY,          /*pos*/ -6250,    0,  6680, /*angle*/ 0, 0, 0,   /*behParam*/ DIALOG_001 << 16, /*beh*/ bhvBobombBuddy,            /*acts*/ ACT_1),
    OBJECT_WITH_ACTS(/*model*/ MODEL_BOBOMB_BUDDY,          /*pos*/ -5723,  140,  6017, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvBobombBuddyOpensCannon, /*acts*/         ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_BOBOMB_BUDDY,          /*pos*/ -6250,    0,  6680, /*angle*/ 0, 0, 0,   /*behParam*/ DIALOG_003 << 16, /*beh*/ bhvBobombBuddy,            /*acts*/         ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_CANNON_BASE,           /*pos*/ -5694,  128,  5600, /*angle*/ 0, 135, 0, /*behParam*/ 0x00010000,       /*beh*/ bhvWaterBombCannon,        /*acts*/ ACT_1),
    OBJECT_WITH_ACTS(/*model*/ MODEL_DL_CANNON_LID,         /*pos*/ -5694,  128,  5600, /*angle*/ 0, 180, 0, /*behParam*/ 0x00000000,       /*beh*/ bhvCannonClosed,           /*acts*/         ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/  3304, 4242, -4603, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00000000,       /*beh*/ bhvKoopaRaceEndpoint,      /*acts*/ ACT_2),
    OBJECT_WITH_ACTS(/*model*/ MODEL_KOOPA_WITH_SHELL,      /*pos*/  3400,  770,  6500, /*angle*/ 0, 0, 0,   /*behParam*/ 0x00010000,       /*beh*/ bhvKoopa,                  /*acts*/                 ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS(/*model*/ MODEL_KING_BOBOMB,           /*pos*/  1636, 4242, -5567, /*angle*/ 0, -147, 0, /*behParam*/ 0x00000000, /*beh*/ bhvKingBobomb,            /*acts*/ ACT_1),
    OBJECT_WITH_ACTS(/*model*/ MODEL_KOOPA_WITH_SHELL,      /*pos*/ -4004,    0,  5221, /*angle*/ 0, 0, 0,    /*behParam*/ 0x01020000, /*beh*/ bhvKoopa,                 /*acts*/ ACT_2),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/ -6000, 1000,  2400, /*angle*/ 0, 0, 0,    /*behParam*/ 0x03000000, /*beh*/ bhvHiddenRedCoinStar,     /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_NONE,                  /*pos*/ -6600, 1000,  1250, /*angle*/ 0, 0, 0,    /*behParam*/ 0x04040000, /*beh*/ bhvHiddenStar,            /*acts*/ ALL_ACTS),
    OBJECT_WITH_ACTS(/*model*/ MODEL_STAR,                  /*pos*/  1550, 1200,   300, /*angle*/ 0, 0, 0,    /*behParam*/ 0x05000000, /*beh*/ bhvStar,                  /*acts*/ ALL_ACTS),
    RETURN(),
};

const LevelScript level_bob_entry[] = {
    INIT_LEVEL(),
    LOAD_LEVEL_DATA(bob),
    LOAD_TEXTURE_BIN(generic),
    LOAD_SKYBOX(water),
    LOAD_GROUPA(group3),
    LOAD_GROUPB(group14),
    LOAD_COMMON0(),
    ALLOC_LEVEL_POOL(),
    MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_4),
    JUMP_LINK(script_func_global_15),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_BUBBLY_TREE,      bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_CHAIN_CHOMP_GATE, bob_geo_000440),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_SEESAW_PLATFORM,  bob_geo_000458),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_BARS_GRILLS,      bob_geo_000470),

    AREA(/*index*/ 1, bob_geo_000488),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -6558,  1000,  6464, /*angle*/ 0,  135, 0, /*behParam*/ WARP_BOB_MAIN_ENTRY << 16, /*beh*/ bhvSpinAirborneWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/   583,  2683, -5387, /*angle*/ 0, -154, 0, /*behParam*/ WARP_BOB_FADING_1A  << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  1680,  3835, -5523, /*angle*/ 0, -153, 0, /*behParam*/ WARP_BOB_FADING_1B  << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/ -6612,  1024, -3351, /*angle*/ 0,  107, 0, /*behParam*/ WARP_BOB_FADING_2A  << 16, /*beh*/ bhvFadingWarp),
        OBJECT(/*model*/ MODEL_NONE, /*pos*/  1980,   768,  6618, /*angle*/ 0, -151, 0, /*behParam*/ WARP_BOB_FADING_2B  << 16, /*beh*/ bhvFadingWarp),
        WARP_NODE(/*id*/ WARP_BOB_MAIN_ENTRY, /*destLevel*/ LEVEL_BOB,    /*destArea*/ 0x01, /*destNode*/ WARP_BOB_MAIN_ENTRY,                   /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_BOB_FADING_1A,  /*destLevel*/ LEVEL_BOB,    /*destArea*/ 0x01, /*destNode*/ WARP_BOB_FADING_1B,                    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_BOB_FADING_1B,  /*destLevel*/ LEVEL_BOB,    /*destArea*/ 0x01, /*destNode*/ WARP_BOB_FADING_1A,                    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_BOB_FADING_2A,  /*destLevel*/ LEVEL_BOB,    /*destArea*/ 0x01, /*destNode*/ WARP_BOB_FADING_2B,                    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_BOB_FADING_2B,  /*destLevel*/ LEVEL_BOB,    /*destArea*/ 0x01, /*destNode*/ WARP_BOB_FADING_2A,                    /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEFAULT,   /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_BOB_SUCCESS, /*flags*/ WARP_NO_CHECKPOINT),
        WARP_NODE(/*id*/ WARP_NODE_DEATH,     /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ WARP_CASTLE_INSIDE_1_FROM_BOB_DEATH,   /*flags*/ WARP_NO_CHECKPOINT),
        TERRAIN(/*terrainData*/ bob_seg7_collision_level),
        MACRO_OBJECTS(/*objList*/ bob_seg7_macro_objs),
        SHOW_DIALOG(/*index*/ 0x00, DIALOG_000),
        SET_BACKGROUND_MUSIC(/*settingsPreset*/ SEQ_SETTINGS_PRESET_DEFAULT, /*seq*/ SEQ_LEVEL_GRASS),
        TERRAIN_TYPE(/*terrainType*/ TERRAIN_GRASS),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS(/*area*/ 1, /*yaw*/ 135, /*pos*/ -6558, 0, 6464),
    CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
    CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(/*frames*/ 1),
    EXIT(),
};
