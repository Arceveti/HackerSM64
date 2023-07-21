#ifndef LEVEL_UPDATE_H
#define LEVEL_UPDATE_H

#include <PR/ultratypes.h>

#include "types.h"

enum TimerControl {
    TIMER_CONTROL_SHOW,
    TIMER_CONTROL_START,
    TIMER_CONTROL_STOP,
    TIMER_CONTROL_HIDE
};

enum WarpOperation {
    WARP_OP_NONE,
    WARP_OP_LOOK_UP,
    WARP_OP_SPIN_SHRINK,
    WARP_OP_WARP_DOOR,
    WARP_OP_WARP_OBJECT,
    WARP_OP_TELEPORT,
    WARP_OP_TRIGGERS_LEVEL_SELECT = 0x10,
    WARP_OP_STAR_EXIT,
    WARP_OP_DEATH,
    WARP_OP_WARP_FLOOR,
    WARP_OP_GAME_OVER,
    WARP_OP_CREDITS_END,
    WARP_OP_DEMO_NEXT,
    WARP_OP_CREDITS_START,
    WARP_OP_CREDITS_NEXT,
    WARP_OP_DEMO_END
};

enum SpecialWarpDestinations {
    WARP_SPECIAL_LEVEL_SELECT        = -9,
    WARP_SPECIAL_INTRO_SPLASH_SCREEN = -8,
    WARP_SPECIAL_MARIO_HEAD_DIZZY    = -3,
    WARP_SPECIAL_MARIO_HEAD_REGULAR  = -2,
    WARP_SPECIAL_ENDING              = -1,
    WARP_SPECIAL_NONE                =  0,
};

enum WarpFlags {
    WARP_FLAGS_NONE           = 0x0,
    WARP_FLAG_DOOR_PULLED     = BIT(0), // 0x01
    WARP_FLAG_DOOR_FLIP_MARIO = BIT(1), // 0x02
    WARP_FLAG_DOOR_IS_WARP    = BIT(2), // 0x04
    WARP_FLAG_EXIT_COURSE     = BIT(3), // 0x08
};

enum MarioSpawnType {
    MARIO_SPAWN_NONE,
    MARIO_SPAWN_DOOR_WARP,
    MARIO_SPAWN_IDLE,
    MARIO_SPAWN_PIPE,
    MARIO_SPAWN_TELEPORT,
    MARIO_SPAWN_INSTANT_ACTIVE = 0x10,
    MARIO_SPAWN_SWIMMING,
    MARIO_SPAWN_AIRBORNE,
    MARIO_SPAWN_HARD_AIR_KNOCKBACK,
    MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE,
    MARIO_SPAWN_DEATH,
    MARIO_SPAWN_SPIN_AIRBORNE,
    MARIO_SPAWN_FLYING,
    MARIO_SPAWN_PAINTING_STAR_COLLECT = 0x20,
    MARIO_SPAWN_PAINTING_DEATH,
    MARIO_SPAWN_AIRBORNE_STAR_COLLECT,
    MARIO_SPAWN_AIRBORNE_DEATH,
    MARIO_SPAWN_LAUNCH_STAR_COLLECT,
    MARIO_SPAWN_LAUNCH_DEATH,
    MARIO_SPAWN_UNUSED_38,
    MARIO_SPAWN_FADE_FROM_BLACK
};

struct CreditsEntry {
    /*0x00*/ u8 levelNum;
    /*0x01*/ u8 areaIndex;
    /*0x03*/ u8 actNum;
    /*0x04*/ Vec3s marioPos;
    /*0x0A*/ s16 marioAngle;
    /*0x0C*/ s16 yawVel;
    /*0x0E*/ u8 isTop   : 1;
    /*0x0F*/ u8 isRight : 1;
    /*0x10*/ const char** string;
}; /*0x14*/

extern struct CreditsEntry* gCurrCreditsEntry;

extern struct MarioState gMarioStates[];
extern struct MarioState* gMarioState;

extern s16 sCurrPlayMode;
extern s16 sTransitionTimer;
extern void (*sTransitionUpdate)(s16*);
extern void load_language_text(void);

struct WarpDest {
    /*0x00*/ u8 type;
    /*0x01*/ u8 levelNum;
    /*0x02*/ u8 areaIdx;
    /*0x03*/ u8 nodeId;
    /*0x04*/ u32 arg;
}; /*0x08*/

extern struct WarpDest sWarpDest;

extern s16 sSpecialWarpDest;
extern s16 sDelayedWarpOp;
extern s16 sDelayedWarpTimer;
extern s16 sSourceWarpNodeId;
extern s32 sDelayedWarpArg;
extern _Bool sTimerRunning;

struct HudDisplay {
    /*0x00*/ s16 lives;
    /*0x02*/ s16 coins;
    /*0x04*/ s16 stars;
    /*0x06*/ s16 wedges;
    /*0x08*/ s16 keys;
    /*0x0A*/ s16 flags;
    /*0x0C*/ u16 timer;
#ifdef BREATH_METER
    /*0x0E*/ u16 breath;
#endif
}; /*0x10*/

extern struct HudDisplay gHudDisplay;
extern _Bool gNeverEnteredCastle;
extern _Bool g100CoinStarSpawned;

enum HUDDisplayFlag {
    HUD_DISPLAY_NONE                  = 0x0,
    HUD_DISPLAY_FLAG_LIVES            = BIT( 0), // 0x0001
    HUD_DISPLAY_FLAG_COIN_COUNT       = BIT( 1), // 0x0002
    HUD_DISPLAY_FLAG_STAR_COUNT       = BIT( 2), // 0x0004
    HUD_DISPLAY_FLAG_CAMERA_AND_POWER = BIT( 3), // 0x0008
    HUD_DISPLAY_FLAG_KEYS             = BIT( 4), // 0x0010
    HUD_DISPLAY_FLAG_UNKNOWN_0020     = BIT( 5), // 0x0020
    HUD_DISPLAY_FLAG_TIMER            = BIT( 6), // 0x0040
#ifdef BREATH_METER
    HUD_DISPLAY_FLAG_BREATH_METER     = BIT(14), // 0x4000
#endif
    HUD_DISPLAY_FLAG_EMPHASIZE_POWER  = BIT(15), // 0x8000
    HUD_DISPLAY_DEFAULT = (HUD_DISPLAY_FLAG_LIVES | HUD_DISPLAY_FLAG_COIN_COUNT | HUD_DISPLAY_FLAG_STAR_COUNT | HUD_DISPLAY_FLAG_CAMERA_AND_POWER | HUD_DISPLAY_FLAG_KEYS | HUD_DISPLAY_FLAG_UNKNOWN_0020),
};

enum PlayModes {
    PLAY_MODE_NORMAL,
    PLAY_MODE_UNUSED,
    PLAY_MODE_PAUSED,
    PLAY_MODE_CHANGE_AREA,
    PLAY_MODE_CHANGE_LEVEL,
    PLAY_MODE_FRAME_ADVANCE,
};

enum WarpTypes {
    WARP_TYPE_NOT_WARPING,
    WARP_TYPE_CHANGE_LEVEL,
    WARP_TYPE_CHANGE_AREA,
    WARP_TYPE_SAME_AREA,
};

enum WarpNodes {
    WARP_NODE_DEFAULT       = 0xF0,
    WARP_NODE_DEATH         = 0xF1,
    WARP_NODE_LOOK_UP       = 0xF2,
    WARP_NODE_WARP_FLOOR    = 0xF3,
    WARP_NODE_CREDITS_MIN   = 0xF8,
    WARP_NODE_CREDITS_START = 0xF8,
    WARP_NODE_CREDITS_NEXT  = 0xF9,
    WARP_NODE_CREDITS_END   = 0xFA,
};

enum WarpsBBH {
    WARP_BBH_MAIN_ENTRY = 0x0A,
};

enum WarpsCCM {
    // Area 1
    WARP_CCM_1_MAIN_ENTRY = 0x0A,
    WARP_CCM_1_FROM_CCM_2 = 0x14,
    WARP_CCM_1_TO_CCM_2   = 0x1E,
    WARP_CCM_1_FADING_A   = 0x1F,
    WARP_CCM_1_FADING_B   = 0x20,
    // Area 2
    WARP_CCM_2_MAIN_ENTRY = 0x0A,
    WARP_CCM_2_FROM_CCM_1 = 0x14,
};

enum WarpsCastleInside {
    // Area 1
    WARP_CASTLE_INSIDE_1_TO_CASTLE_GROUNDS_A   = 0x00,
    WARP_CASTLE_INSIDE_1_TO_CASTLE_GROUNDS_B   = 0x01,
    WARP_CASTLE_INSIDE_1_TO_COURTYARD          = 0x02,
    WARP_CASTLE_INSIDE_1_TO_CASTLE_INSIDE_2_A  = 0x03,
    WARP_CASTLE_INSIDE_1_TO_CASTLE_INSIDE_2_B  = 0x04,
    WARP_CASTLE_INSIDE_1_TO_CASTLE_INSIDE_3_A  = 0x05,
    WARP_CASTLE_INSIDE_1_TO_CASTLE_INSIDE_3_B  = 0x06,
    WARP_CASTLE_INSIDE_1_TO_PSS                = 0x0A,
    WARP_CASTLE_INSIDE_1_TO_BITDW              = 0x0B,
    WARP_CASTLE_INSIDE_1_TO_SA                 = 0x0C,
    WARP_CASTLE_INSIDE_1_1E                    = 0x1E,
    WARP_CASTLE_INSIDE_1_1F                    = 0x1F,
    WARP_CASTLE_INSIDE_1_FROM_SECRET_WARP      = 0x20,
    WARP_CASTLE_INSIDE_1_21                    = 0x21,
    WARP_CASTLE_INSIDE_1_22                    = 0x22,
    WARP_CASTLE_INSIDE_1_FROM_SECRET_DEATH     = 0x23,
    WARP_CASTLE_INSIDE_1_FROM_BOWSER_1_SUCCESS = 0x24,
    WARP_CASTLE_INSIDE_1_FROM_BITDW_DEATH      = 0x25,
    WARP_CASTLE_INSIDE_1_FROM_SECRET_SUCCESS   = 0x26,
    WARP_CASTLE_INSIDE_1_FROM_SA_SUCCESS       = 0x27,
    WARP_CASTLE_INSIDE_1_FROM_SA_DEATH         = 0x28,
    WARP_CASTLE_INSIDE_1_FROM_BOB_SUCCESS      = 0x32,
    WARP_CASTLE_INSIDE_1_FROM_CCM_SUCCESS      = 0x33,
    WARP_CASTLE_INSIDE_1_FROM_WF_SUCCESS       = 0x34,
    WARP_CASTLE_INSIDE_1_FROM_JRB_SUCCESS      = 0x35,
    WARP_CASTLE_INSIDE_1_FROM_BOB_DEATH        = 0x64,
    WARP_CASTLE_INSIDE_1_FROM_CCM_DEATH        = 0x65,
    WARP_CASTLE_INSIDE_1_FROM_WF_DEATH         = 0x66,
    WARP_CASTLE_INSIDE_1_FROM_JRB_DEATH        = 0x67,
    WARP_CASTLE_INSIDE_1_PAINTING_BOB          = 0x96,
    WARP_CASTLE_INSIDE_1_PAINTING_CCM          = 0x97,
    WARP_CASTLE_INSIDE_1_PAINTING_WF           = 0x98,
    WARP_CASTLE_INSIDE_1_PAINTING_JRB          = 0x99,
    // Area 2
    WARP_CASTLE_INSIDE_2_TO_CASTLE_INSIDE_1_A  = 0x00,
    WARP_CASTLE_INSIDE_2_TO_CASTLE_INSIDE_1_B  = 0x01,
    WARP_CASTLE_INSIDE_2_TO_WMOTR              = 0x0A,
    WARP_CASTLE_INSIDE_2_TO_BITS               = 0x0B,
    WARP_CASTLE_INSIDE_2_FROM_WDW_SUCCESS      = 0x32,
    WARP_CASTLE_INSIDE_2_FROM_THI_2_SUCCESS    = 0x33,
    WARP_CASTLE_INSIDE_2_FROM_TTM_SUCCESS      = 0x34,
    WARP_CASTLE_INSIDE_2_FROM_TTC_SUCCESS      = 0x35,
    WARP_CASTLE_INSIDE_2_FROM_SL_SUCCESS       = 0x36,
    WARP_CASTLE_INSIDE_2_FROM_THI_1_SUCCESS    = 0x37,
    WARP_CASTLE_INSIDE_2_FROM_WMOTR_SUCCESS    = 0x38,
    WARP_CASTLE_INSIDE_2_FROM_RR_SUCCESS       = 0x3A,
    WARP_CASTLE_INSIDE_2_FROM_WDW_DEATH        = 0x64,
    WARP_CASTLE_INSIDE_2_FROM_THI_2_DEATH      = 0x65,
    WARP_CASTLE_INSIDE_2_FROM_TTM_DEATH        = 0x66,
    WARP_CASTLE_INSIDE_2_FROM_TTC_DEATH        = 0x67,
    WARP_CASTLE_INSIDE_2_FROM_SL_DEATH         = 0x68,
    WARP_CASTLE_INSIDE_2_FROM_THI_1_DEATH      = 0x69,
    WARP_CASTLE_INSIDE_2_FROM_BITS_DEATH       = 0x6B,
    WARP_CASTLE_INSIDE_2_FROM_RR_DEATH         = 0x6C,
    WARP_CASTLE_INSIDE_2_FROM_WMOTR_DEATH      = 0x6D,
    WARP_CASTLE_INSIDE_2_PAINTING_WDW          = 0x96,
    WARP_CASTLE_INSIDE_2_PAINTING_THI_TINY     = 0x97,
    WARP_CASTLE_INSIDE_2_PAINTING_TTM          = 0x98,
    WARP_CASTLE_INSIDE_2_PAINTING_TTC          = 0x99,
    WARP_CASTLE_INSIDE_2_PAINTING_SL           = 0x9A,
    WARP_CASTLE_INSIDE_2_PAINTING_THI_HUGE     = 0x9B,
    WARP_CASTLE_INSIDE_2_PAINTING_RR           = 0x9C,
    // Area 3
    WARP_CASTLE_INSIDE_3_TO_CASTLE_INSIDE_1_A  = 0x00,
    WARP_CASTLE_INSIDE_3_TO_CASTLE_INSIDE_1_B  = 0x01,
    WARP_CASTLE_INSIDE_3_TO_CASTLE_GROUNDS     = 0x02,
    WARP_CASTLE_INSIDE_3_TO_BITFS              = 0x18,
    WARP_CASTLE_INSIDE_3_FROM_LLL_SUCCESS      = 0x32,
    WARP_CASTLE_INSIDE_3_FROM_SSL_SUCCESS      = 0x33,
    WARP_CASTLE_INSIDE_3_FROM_HMC_SUCCESS      = 0x34,
    WARP_CASTLE_INSIDE_3_FROM_DDD_SUCCESS      = 0x35,
    WARP_CASTLE_INSIDE_3_FROM_BOWSER_2_SUCCESS = 0x36,
    WARP_CASTLE_INSIDE_3_FROM_LLL_DEATH        = 0x64,
    WARP_CASTLE_INSIDE_3_FROM_SSL_DEATH        = 0x65,
    WARP_CASTLE_INSIDE_3_FROM_HMC_DEATH        = 0x66,
    WARP_CASTLE_INSIDE_3_FROM_DDD_DEATH        = 0x67,
    WARP_CASTLE_INSIDE_3_FROM_BITFS_DEATH      = 0x68,
    WARP_CASTLE_INSIDE_3_PAINTING_LLL          = 0x96,
    WARP_CASTLE_INSIDE_3_PAINTING_SSL          = 0x97,
    WARP_CASTLE_INSIDE_3_PAINTING_HMC          = 0x98,
    WARP_CASTLE_INSIDE_3_PAINTING_DDD          = 0x99,
};

enum WarpsHMC {
    WARP_HMC_MAIN_ENTRY     = 0x0A,
    WARP_HMC_TO_COTMC       = 0x0B,
    WARP_HMC_PAINTING_COTMC = 0x96,
};

enum WarpsSSL {
    // Area 1
    WARP_SSL_1_MAIN_ENTRY = 0x0A,
    WARP_SSL_1_TO_SSL_2_A = 0x14,
    WARP_SSL_1_TO_SSL_2_B = 0x1E,
    WARP_SSL_1_FADING_A   = 0x1F,
    WARP_SSL_1_FADING_B   = 0x20,
    // Area 2
    WARP_SSL_2_MAIN_ENTRY = 0x0A,
    WARP_SSL_2_FROM_SSL_1 = 0x14,
    WARP_SSL_2_FADING_A   = 0x15,
    WARP_SSL_2_FADING_B   = 0x16,
    // Area 3
    WARP_SSL_3_MAIN_ENTRY = 0x0A,
};

enum WarpsBOB {
    WARP_BOB_MAIN_ENTRY = 0x0A,
    WARP_BOB_FADING_1A  = 0x0B,
    WARP_BOB_FADING_1B  = 0x0C,
    WARP_BOB_FADING_2A  = 0x0D,
    WARP_BOB_FADING_2B  = 0x0E,
};

enum WarpsSL {
    // Area 1
    WARP_SL_1_MAIN_ENTRY = 0x0A,
    WARP_SL_1_FROM_SL_2  = 0x0B,
    WARP_SL_1_TO_SL_2    = 0x0C,
    WARP_SL_1_FADING_A   = 0x0D,
    WARP_SL_1_FADING_B   = 0x0E,
    // Area 2
    WARP_SL_2_MAIN_ENTRY = 0x0A,
    WARP_SL_2_TO_SL_1    = 0x0B,
};

enum WarpsWDW {
    // Area 1
    WARP_WDW_1_MAIN_ENTRY = 0x0A,
    WARP_WDW_1_FADING_A   = 0x0B,
    WARP_WDW_1_FADING_B   = 0x0C,
    // Area 2
    WARP_WDW_2_MAIN_ENTRY = 0x0A,
};

enum WarpsJRB {
    // Area 1
    WARP_JRB_1_MAIN_ENTRY = 0x0A,
    // Area 2
    WARP_JRB_2_MAIN_ENTRY = 0x0A,
};

enum WarpsTHI {
    // Area 1
    WARP_THI_1_MAIN_ENTRY  = 0x0A,
    WARP_THI_1_FROM_THI_3  = 0x0B,
    WARP_THI_1_TO_THI_3_A  = 0x0C,
    WARP_THI_1_TO_THI_3_B  = 0x0D,
    WARP_THI_1_WARP_PIPE_A = 0x32,
    WARP_THI_1_WARP_PIPE_B = 0x33,
    WARP_THI_1_WARP_PIPE_C = 0x34,
    // Area 2
    WARP_THI_2_MAIN_ENTRY  = 0x0A,
    WARP_THI_2_FADING_A    = 0x0B,
    WARP_THI_2_FADING_B    = 0x0C,
    WARP_THI_2_WARP_PIPE_A = 0x32,
    WARP_THI_2_WARP_PIPE_B = 0x33,
    WARP_THI_2_WARP_PIPE_C = 0x34,
    // Area 3
    WARP_THI_3_MAIN_ENTRY  = 0x0A,
    WARP_THI_3_FROM_THI_1  = 0x0B,
    WARP_THI_3_TO_THI_1    = 0x0C,
};

enum WarpsTTC {
    WARP_TTC_MAIN_ENTRY = 0x0A,
};

enum WarpsRR {
    WARP_RR_MAIN_ENTRY = 0x0A,
    WARP_RR_FADING_A   = 0x0B,
    WARP_RR_FADING_B   = 0x0C,
};

enum WarpsCastleGrounds {
    WARP_CASTLE_GROUNDS_00                 = 0x00,
    WARP_CASTLE_GROUNDS_01                 = 0x01,
    WARP_CASTLE_GROUNDS_TO_CASTLE_INSIDE_3 = 0x02,
    WARP_CASTLE_GROUNDS_DEATH_EXIT         = 0x03,
    WARP_CASTLE_GROUNDS_04                 = 0x04,
    WARP_CASTLE_GROUNDS_TO_VCUTM           = 0x05,
    WARP_CASTLE_GROUNDS_FROM_VCUTM_DEATH   = 0x06,
    WARP_CASTLE_GROUNDS_FROM_VCUTM_WARP    = 0x07,
    WARP_CASTLE_GROUNDS_FROM_VCUTM_SUCCESS = 0x08,
    WARP_CASTLE_GROUNDS_MAIN_ENTRY         = 0x0A,
    WARP_CASTLE_GROUNDS_FROM_COTMC         = 0x14,
    WARP_CASTLE_GROUNDS_FROM_DDD_DOOR      = 0x1E,
};

enum WarpsBITDW {
    WARP_BITDW_MAIN_ENTRY          = 0x0A,
    WARP_BITDW_TO_BOWSER_1         = 0x0B,
    WARP_BITDW_FROM_BOWSER_1_DEATH = 0x0C,
};

enum WarpsVCUTM {
    WARP_VCUTM_MAIN_ENTRY = 0x0A,
};

enum WarpsBITFS {
    WARP_BITFS_MAIN_ENTRY          = 0x0A,
    WARP_BITFS_TO_BOWSER_2         = 0x0B,
    WARP_BITFS_FROM_BOWSER_2_DEATH = 0x0C,
};

enum WarpsSA {
    WARP_SA_MAIN_ENTRY = 0x0A,
};

enum WarpsBITS {
    WARP_BITS_MAIN_ENTRY          = 0x0A,
    WARP_BITS_TO_BOWSER_3         = 0x0B,
    WARP_BITS_FROM_BOWSER_3_DEATH = 0x0C,
};

enum WarpsLLL {
    // Area 1
    WARP_LLL_1_MAIN_ENTRY = 0x0A,
    WARP_LLL_1_TO_LLL_2   = 0x0B,
    WARP_LLL_1_FADING_A   = 0x0C,
    WARP_LLL_1_FADING_B   = 0x0D,
    // Area 2
    WARP_LLL_2_MAIN_ENTRY = 0x0A,
};

enum WarpsDDD {
    // Area 1
    WARP_DDD_1_MAIN_ENTRY = 0x0A,
    // Area 2
    WARP_DDD_2_MAIN_ENTRY = 0x0A,
};

enum WarpsWF {
    WARP_WF_MAIN_ENTRY = 0x0A,
    WARP_WF_FADING_A   = 0x0B,
    WARP_WF_FADING_B   = 0x0C,
};

enum WarpsEnding {
    WARP_Ending_MAIN_ENTRY = 0x0A,
};

enum WarpsCourtyard {
    WARP_COURTYARD_TO_CASTLE_INSIDE = 0x01,
    WARP_COURTYARD_TO_BBH           = 0x05,
    WARP_COURTYARD_FROM_BBH_SUCCESS = 0x0A,
    WARP_COURTYARD_FROM_BBH_DEATH   = 0x0B,
};

enum WarpsPSS {
    WARP_PSS_MAIN_ENTRY = 0x0A,
};

enum WarpsCOTMC {
    WARP_COTMC_MAIN_ENTRY = 0x0A,
};

enum WarpsTOTWC {
    WARP_TOTWC_MAIN_ENTRY = 0x0A,
};

enum WarpsBowser1 {
    WARP_BOWSER_1_MAIN_ENTRY = 0x0A,
};

enum WarpsWMOTR {
    WARP_WMOTR_MAIN_ENTRY = 0x0A,
};

enum WarpsBowser2 {
    WARP_BOWSER_2_MAIN_ENTRY = 0x0A,
};

enum WarpsBowser3 {
    WARP_BOWSER_3_MAIN_ENTRY = 0x0A,
};

enum WarpsTTM {
    // Area 1
    WARP_TTM_1_MAIN_ENTRY     = 0x0A,
    WARP_TTM_1_FROM_TTM_4     = 0x14,
    WARP_TTM_1_FADING_A       = 0x15,
    WARP_TTM_1_FADING_B       = 0x16,
    WARP_TTM_1_PAINTING_SLIDE = 0x96,
    // Area 2
    WARP_TTM_2_MAIN_ENTRY     = 0x0A,
    // Area 3
    WARP_TTM_3_MAIN_ENTRY     = 0x0A,
    // Area 4
    WARP_TTM_4_MAIN_ENTRY     = 0x0A,
};

struct Controller* get_mario_controller(void);
u32 level_control_timer(s32 timerOp);
void fade_into_special_warp(u32 warpArg, u32 color);
void load_level_init_text(u32 areaDialogIdx);
s32 level_trigger_warp(struct MarioState* m, s32 warpOp);
void level_set_transition(s16 length, void (*updateFunction)());

s32 is_bowser_level(u32 level);

s32 lvl_init_or_update(                  s16 initOrUpdate, UNUSED s32 levelNum);
s32 lvl_init_from_save_file(      UNUSED s16 initOrUpdate,        s32 levelNum);
s32 lvl_set_current_level(        UNUSED s16 initOrUpdate,        s32 levelNum);
s32 lvl_play_the_end_screen_sound(UNUSED s16 initOrUpdate, UNUSED s32 levelNum);
void basic_update(void);

#endif // LEVEL_UPDATE_H
