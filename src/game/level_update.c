#include <ultra64.h>

#include "sm64.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "audio/synthesis.h"
#include "level_update.h"
#include "game_init.h"
#include "main.h"
#include "engine/math_util.h"
#include "engine/graph_node.h"
#include "area.h"
#include "save_file.h"
#include "sound_init.h"
#include "mario.h"
#include "camera.h"
#include "object_list_processor.h"
#include "ingame_menu.h"
#include "obj_behaviors.h"
#include "save_file.h"
#if MULTILANG
#include "memory.h"
#include "eu_translation.h"
#include "segment_symbols.h"
#endif
#include "level_table.h"
#include "course_table.h"
#include "puppycam2.h"
#include "puppyprint.h"
#include "puppylights.h"
#include "level_commands.h"
#include "behavior_data.h"
#include "paintings.h"

#include "config.h"

// TODO: Make these ifdefs better
const char* credits01[] = { "1GAME DIRECTOR", "SHIGERU MIYAMOTO" };
const char* credits02[] = { "2ASSISTANT DIRECTORS", "YOSHIAKI KOIZUMI", "TAKASHI TEZUKA" };
const char* credits03[] = { "2SYSTEM PROGRAMMERS", "YASUNARI NISHIDA", "YOSHINORI TANIMOTO" };
const char* credits04[] = { "3PROGRAMMERS", "HAJIME YAJIMA", "DAIKI IWAMOTO", "TOSHIO IWAWAKI" };

#if defined(VERSION_JP) || defined(VERSION_SH)

const char* credits05[] = { "1CAMERA PROGRAMMER", "TAKUMI KAWAGOE" };
const char* credits06[] = { "1MARIO FACE PROGRAMMER", "GILES GODDARD" };
const char* credits07[] = { "2COURSE DIRECTORS", "YOICHI YAMADA", "YASUHISA YAMAMURA" };
const char* credits08[] = { "2COURSE DESIGNERS", "KENTA USUI", "NAOKI MORI" };
const char* credits09[] = { "3COURSE DESIGNERS", "YOSHIKI HARUHANA", "MAKOTO MIYANAGA", "KATSUHIKO KANNO" };
const char* credits10[] = { "1SOUND COMPOSER", "KOJI KONDO" };

// Shindou combines sound effects and sound programmer in order to make room for Mario voice and Peach voice
const char* credits11[] = { "4SOUND EFFECTS", "SOUND PROGRAMMER", "YOJI INAGAKI", "HIDEAKI SHIMIZU" };
const char* credits12[] = { "23D ANIMATORS", "YOSHIAKI KOIZUMI", "SATORU TAKIZAWA" };
const char* credits13[] = { "1CG DESIGNER", "MASANAO ARIMOTO" };
const char* credits14[] = { "3TECHNICAL SUPPORT", "TAKAO SAWANO", "HIROHITO YOSHIMOTO", "HIROTO YADA" };
const char* credits15[] = { "1TECHNICAL SUPPORT", "SGI. 64PROJECT STAFF" };
const char* credits16[] = { "2PROGRESS MANAGEMENT", "KIMIYOSHI FUKUI", "KEIZO KATO" };

#else // (VERSION_US || VERSION_EU)

// US and EU combine camera programmer and Mario face programmer...
const char* credits05[] = { "4CAMERA PROGRAMMER", "MARIO FACE PROGRAMMER", "TAKUMI KAWAGOE", "GILES GODDARD" };
const char* credits06[] = { "2COURSE DIRECTORS", "YOICHI YAMADA", "YASUHISA YAMAMURA" };
const char* credits07[] = { "2COURSE DESIGNERS", "KENTA USUI", "NAOKI MORI" };
const char* credits08[] = { "3COURSE DESIGNERS", "YOSHIKI HARUHANA", "MAKOTO MIYANAGA", "KATSUHIKO KANNO" };

#ifdef VERSION_US
const char* credits09[] = { "1SOUND COMPOSER", "KOJI KONDO" };
// ...as well as sound effects and sound programmer in order to make room for screen text writer, Mario voice, and Peach voice
const char* credits10[] = { "4SOUND EFFECTS", "SOUND PROGRAMMER", "YOJI INAGAKI", "HIDEAKI SHIMIZU" };
const char* credits11[] = { "23-D ANIMATORS", "YOSHIAKI KOIZUMI", "SATORU TAKIZAWA" };
const char* credits12[] = { "1ADDITIONAL GRAPHICS", "MASANAO ARIMOTO" };
const char* credits13[] = { "3TECHNICAL SUPPORT", "TAKAO SAWANO", "HIROHITO YOSHIMOTO", "HIROTO YADA" };
const char* credits14[] = { "1TECHNICAL SUPPORT", "SGI N64 PROJECT STAFF" };
const char* credits15[] = { "2PROGRESS MANAGEMENT", "KIMIYOSHI FUKUI", "KEIZO KATO" };
const char* credits16[] = { "5SCREEN TEXT WRITER", "TRANSLATION", "LESLIE SWAN", "MINA AKINO", "HIRO YAMADA" };
#else // VERSION_EU
// ...as well as sound composer, sound effects, and sound programmer, and...
const char* credits09[] = { "7SOUND COMPOSER", "SOUND EFFECTS", "SOUND PROGRAMMER", "KOJI KONDO", "YOJI INAGAKI", "HIDEAKI SHIMIZU" };
// ...3D animators and additional graphics in order to make room for screen text writer(s), Mario voice, and Peach voice
const char* credits10[] = { "63-D ANIMATORS", "ADDITIONAL GRAPHICS", "YOSHIAKI KOIZUMI", "SATORU TAKIZAWA", "MASANAO ARIMOTO" };
const char* credits11[] = { "3TECHNICAL SUPPORT", "TAKAO SAWANO", "HIROHITO YOSHIMOTO", "HIROTO YADA" };
const char* credits12[] = { "1TECHNICAL SUPPORT", "SGI N64 PROJECT STAFF" };
const char* credits13[] = { "2PROGRESS MANAGEMENT", "KIMIYOSHI FUKUI", "KEIZO KATO" };
const char* credits14[] = { "5SCREEN TEXT WRITER", "ENGLISH TRANSLATION", "LESLIE SWAN", "MINA AKINO", "HIRO YAMADA" };
const char* credits15[] = { "4SCREEN TEXT WRITER", "FRENCH TRANSLATION", "JULIEN BARDAKOFF", "KENJI HARAGUCHI" };
const char* credits16[] = { "4SCREEN TEXT WRITER", "GERMAN TRANSLATION", "THOMAS GOERG", "THOMAS SPINDLER" };
#endif

#endif

const char* credits17[] = { "4MARIO VOICE", "PEACH VOICE", "CHARLES MARTINET", "LESLIE SWAN" };

#if defined(VERSION_JP) || defined(VERSION_SH)
const char* credits18[] = { "3SPECIAL THANKS TO", "JYOHO KAIHATUBU", "ALL NINTENDO", "MARIO CLUB STAFF" };
#else // (VERSION_US || VERSION_EU)
const char* credits18[] = { "3SPECIAL THANKS TO", "EAD STAFF", "ALL NINTENDO PERSONNEL", "SUPER MARIO CLUB STAFF" };
#endif

const char* credits19[] = { "1PRODUCER", "SHIGERU MIYAMOTO" };
const char* credits20[] = { "1EXECUTIVE PRODUCER", "HIROSHI YAMAUCHI" };


struct CreditsEntry sCreditsSequence[] = {
    { .levelNum = LEVEL_CASTLE_GROUNDS, .areaIndex = 1, .actNum = 1, .marioPos = {     0,  8000,     0 }, .marioAngle = 0x8000, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = NULL      },
    { .levelNum = LEVEL_BOB,            .areaIndex = 1, .actNum = 1, .marioPos = {   713,  3918, -3889 }, .marioAngle = 0x7500, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = credits01 },
    { .levelNum = LEVEL_WF,             .areaIndex = 1, .actNum = 2, .marioPos = {   347,  5376,   326 }, .marioAngle = 0x2E00, .yawVel = 0x0000, .isTop =  TRUE, .isRight =  TRUE, .string = credits02 },
    { .levelNum = LEVEL_JRB,            .areaIndex = 1, .actNum = 2, .marioPos = {  3800, -4840,  2727 }, .marioAngle = 0x1600, .yawVel = 0x0000, .isTop = FALSE, .isRight =  TRUE, .string = credits03 },
    { .levelNum = LEVEL_CCM,            .areaIndex = 2, .actNum = 2, .marioPos = { -5464,  6656, -6575 }, .marioAngle = 0x1900, .yawVel = 0x0000, .isTop =  TRUE, .isRight = FALSE, .string = credits04 },
    { .levelNum = LEVEL_BBH,            .areaIndex = 1, .actNum = 1, .marioPos = {   257,  1922,  2580 }, .marioAngle = 0x3C00, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = credits05 },
    { .levelNum = LEVEL_HMC,            .areaIndex = 1, .actNum = 1, .marioPos = { -6469,  1616, -6054 }, .marioAngle = 0x7B00, .yawVel = 0xC000, .isTop =  TRUE, .isRight =  TRUE, .string = credits06 },
    { .levelNum = LEVEL_THI,            .areaIndex = 3, .actNum = 1, .marioPos = {   508,  1024,  1942 }, .marioAngle = 0xE000, .yawVel = 0x0000, .isTop = FALSE, .isRight =  TRUE, .string = credits07 },
    { .levelNum = LEVEL_LLL,            .areaIndex = 2, .actNum = 1, .marioPos = {   -73,    82, -1467 }, .marioAngle = 0x7C00, .yawVel = 0x0000, .isTop =  TRUE, .isRight = FALSE, .string = credits08 },
    { .levelNum = LEVEL_SSL,            .areaIndex = 1, .actNum = 1, .marioPos = { -5906,  1024, -2576 }, .marioAngle = 0x6200, .yawVel = 0x4000, .isTop = FALSE, .isRight = FALSE, .string = credits09 },
    { .levelNum = LEVEL_DDD,            .areaIndex = 1, .actNum = 2, .marioPos = { -4884, -4607,  -272 }, .marioAngle = 0x2F00, .yawVel = 0x0000, .isTop =  TRUE, .isRight =  TRUE, .string = credits10 },
    { .levelNum = LEVEL_SL,             .areaIndex = 1, .actNum = 1, .marioPos = {  1925,  3328,   563 }, .marioAngle = 0xDE00, .yawVel = 0x0000, .isTop = FALSE, .isRight =  TRUE, .string = credits11 },
    { .levelNum = LEVEL_WDW,            .areaIndex = 1, .actNum = 1, .marioPos = {  -537,  1850,  1818 }, .marioAngle = 0x6900, .yawVel = 0x0000, .isTop =  TRUE, .isRight = FALSE, .string = credits12 },
    { .levelNum = LEVEL_TTM,            .areaIndex = 1, .actNum = 2, .marioPos = {  2613,   313,  1074 }, .marioAngle = 0xDF00, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = credits13 },
    { .levelNum = LEVEL_THI,            .areaIndex = 1, .actNum = 3, .marioPos = { -2609,   512,   856 }, .marioAngle = 0x3600, .yawVel = 0x0000, .isTop =  TRUE, .isRight =  TRUE, .string = credits14 },
    { .levelNum = LEVEL_TTC,            .areaIndex = 1, .actNum = 1, .marioPos = { -1304,   -71,  -967 }, .marioAngle = 0xB800, .yawVel = 0x0000, .isTop = FALSE, .isRight =  TRUE, .string = credits15 },
    { .levelNum = LEVEL_RR,             .areaIndex = 1, .actNum = 1, .marioPos = {  1565,  1024,  -148 }, .marioAngle = 0x4000, .yawVel = 0x0000, .isTop =  TRUE, .isRight = FALSE, .string = credits16 },
    { .levelNum = LEVEL_SA,             .areaIndex = 1, .actNum = 1, .marioPos = { -1050, -1330, -1559 }, .marioAngle = 0x1800, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = credits17 },
    { .levelNum = LEVEL_COTMC,          .areaIndex = 1, .actNum = 1, .marioPos = {  -254,   415, -6045 }, .marioAngle = 0xF000, .yawVel = 0x0000, .isTop =  TRUE, .isRight =  TRUE, .string = credits18 },
    { .levelNum = LEVEL_DDD,            .areaIndex = 2, .actNum = 1, .marioPos = {  3948,  1185,  -104 }, .marioAngle = 0xC000, .yawVel = 0xC000, .isTop = FALSE, .isRight = FALSE, .string = credits19 },
    { .levelNum = LEVEL_CCM,            .areaIndex = 1, .actNum = 1, .marioPos = {  3169, -4607,  5240 }, .marioAngle = 0x1F00, .yawVel = 0x0000, .isTop =  TRUE, .isRight = FALSE, .string = credits20 },
    { .levelNum = LEVEL_CASTLE_GROUNDS, .areaIndex = 1, .actNum = 1, .marioPos = {     0,   906, -1200 }, .marioAngle = 0x8000, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = NULL      },
    { .levelNum = LEVEL_NONE,           .areaIndex = 0, .actNum = 1, .marioPos = {     0,     0,     0 }, .marioAngle = 0x0000, .yawVel = 0x0000, .isTop = FALSE, .isRight = FALSE, .string = NULL      },
};

struct MarioState gMarioStates[1];
struct HudDisplay gHudDisplay;
s16 sCurrPlayMode;
s16 sTransitionTimer;
void (*sTransitionUpdate)(s16*);
struct WarpDest sWarpDest;
s16 sSpecialWarpDest;
s16 sDelayedWarpOp;
s16 sDelayedWarpTimer;
s16 sSourceWarpNodeId;
s32 sDelayedWarpArg;
_Bool sTimerRunning;
_Bool gNeverEnteredCastle;
// Prevent multiple 100 coin stars from spawning
_Bool g100CoinStarSpawned = FALSE;

struct MarioState* gMarioState = &gMarioStates[0];
_Bool sWarpCheckpointActive = FALSE;

struct Controller* get_mario_controller(void) {
    return (gMarioState ? gMarioState->controller : gPlayer1Controller);
}

u32 level_control_timer(s32 timerOp) {
    switch (timerOp) {
        case TIMER_CONTROL_SHOW:
            gHudDisplay.flags |= HUD_DISPLAY_FLAG_TIMER;
            sTimerRunning = FALSE;
            gHudDisplay.timer = 0;
            break;

        case TIMER_CONTROL_START:
            sTimerRunning = TRUE;
            break;

        case TIMER_CONTROL_STOP:
            sTimerRunning = FALSE;
            break;

        case TIMER_CONTROL_HIDE:
            gHudDisplay.flags &= ~HUD_DISPLAY_FLAG_TIMER;
            sTimerRunning = FALSE;
            gHudDisplay.timer = 0;
            break;
    }

    return gHudDisplay.timer;
}

u32 pressed_pause(void) {
    u32 dialogActive = (gDialogID >= 0);
    u32 intangible = ((gMarioState->action & ACT_FLAG_INTANGIBLE) != 0);

#ifdef PUPPYPRINT_DEBUG
    if (
        sPPDebugPage == PUPPYPRINT_PAGE_RAM
     || sPPDebugPage == PUPPYPRINT_PAGE_LEVEL_SELECT
 #ifdef BETTER_REVERB
     || sPPDebugPage == PUPPYPRINT_PAGE_BETTER_REVERB
 #endif
    ) {
        return FALSE;
    }
#endif

    return (
        !intangible                                        &&
        !dialogActive                                      &&
        !gWarpTransition.isActive                          &&
        sDelayedWarpOp == WARP_OP_NONE                     &&
        (gPlayer1Controller->buttonPressed & START_BUTTON)
    );
}

void set_play_mode(s16 playMode) {
    sCurrPlayMode = playMode;
}

void warp_special(s32 specialWarpDest) {
    sCurrPlayMode = PLAY_MODE_CHANGE_LEVEL;
    sSpecialWarpDest = specialWarpDest;
}

void fade_into_special_warp(u32 warpArg, u32 color) {
    if (color != 0) {
        color = 0xFF;
    }

    fadeout_music(190);
    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 16, color, color, color);
    level_set_transition(30, NULL);

    warp_special(warpArg);
}

void load_level_init_text(u32 areaDialogIdx) {
    s32 gotAchievement;
    u32 dialogID = gCurrentArea->dialog[areaDialogIdx];

    switch (dialogID) {
        case DIALOG_129:
            gotAchievement = (save_file_get_flags() & SAVE_FLAG_HAVE_VANISH_CAP);
            break;

        case DIALOG_130:
            gotAchievement = (save_file_get_flags() & SAVE_FLAG_HAVE_METAL_CAP);
            break;

        case DIALOG_131:
            gotAchievement = (save_file_get_flags() & SAVE_FLAG_HAVE_WING_CAP);
            break;

        case (u8)DIALOG_NONE: // 255, cast value to u8 to match (-1)
            gotAchievement = TRUE;
            break;

        default:
            gotAchievement = save_file_get_star_flags(
                SAVE_NUM_TO_INDEX(gCurrSaveFileNum),
                COURSE_NUM_TO_INDEX(gCurrCourseNum)
            );
            break;
    }

    if (!gotAchievement) {
        level_set_transition(-1, NULL);
        create_dialog_box(dialogID);
    }
}

void init_door_warp(struct SpawnInfo* spawnInfo, u32 warpDestFlags) {
    if (warpDestFlags & WARP_FLAG_DOOR_FLIP_MARIO) {
        spawnInfo->startAngle[1] += 0x8000;
    }

    spawnInfo->startPos[0] += 300.0f * sins(spawnInfo->startAngle[1]);
    spawnInfo->startPos[2] += 300.0f * coss(spawnInfo->startAngle[1]);
}

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
void set_mario_initial_cap_powerup(struct MarioState* m) {
    u32 capCourseIndex = gCurrCourseNum - COURSE_CAP_COURSES;

    switch (capCourseIndex) {
        case (COURSE_COTMC - COURSE_CAP_COURSES):
            m->flags |= (MARIO_METAL_CAP | MARIO_CAP_ON_HEAD);
            m->capTimer = METAL_CAP_TIME;
            break;

        case (COURSE_TOTWC - COURSE_CAP_COURSES):
            m->flags |= (MARIO_WING_CAP | MARIO_CAP_ON_HEAD);
            m->capTimer = (WING_CAP_TIME * 2 / 3);
            break;

        case (COURSE_VCUTM - COURSE_CAP_COURSES):
            m->flags |= (MARIO_VANISH_CAP | MARIO_CAP_ON_HEAD);
            m->capTimer = VANISH_CAP_TIME;
            break;
    }
}
#endif

void set_mario_initial_action(struct MarioState* m, u32 spawnType, u32 actionArg) {
    switch (spawnType) {
        case MARIO_SPAWN_DOOR_WARP:             set_mario_action(m, ACT_WARP_DOOR_SPAWN, actionArg); break;
        case MARIO_SPAWN_IDLE:                  set_mario_action(m, ACT_IDLE,                    0); break;
        case MARIO_SPAWN_PIPE:                  set_mario_action(m, ACT_EMERGE_FROM_PIPE,        0); break;
        case MARIO_SPAWN_TELEPORT:              set_mario_action(m, ACT_TELEPORT_FADE_IN,        0); break;
        case MARIO_SPAWN_INSTANT_ACTIVE:        set_mario_action(m, ACT_IDLE,                    0); break;
        case MARIO_SPAWN_SWIMMING:              set_mario_action(m, ACT_WATER_IDLE,              1); break;
        case MARIO_SPAWN_AIRBORNE:              set_mario_action(m, ACT_SPAWN_NO_SPIN_AIRBORNE,  0); break;
        case MARIO_SPAWN_HARD_AIR_KNOCKBACK:    set_mario_action(m, ACT_HARD_BACKWARD_AIR_KB,    0); break;
        case MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE:  set_mario_action(m, ACT_SPAWN_SPIN_AIRBORNE,     0); break;
        case MARIO_SPAWN_DEATH:                 set_mario_action(m, ACT_FALLING_DEATH_EXIT,      0); break;
        case MARIO_SPAWN_SPIN_AIRBORNE:         set_mario_action(m, ACT_SPAWN_SPIN_AIRBORNE,     0); break;
        case MARIO_SPAWN_FLYING:                set_mario_action(m, ACT_FLYING,                  2); break;
        case MARIO_SPAWN_PAINTING_STAR_COLLECT: set_mario_action(m, ACT_EXIT_AIRBORNE,           0); break;
        case MARIO_SPAWN_PAINTING_DEATH:        set_mario_action(m, ACT_DEATH_EXIT,              0); break;
        case MARIO_SPAWN_AIRBORNE_STAR_COLLECT: set_mario_action(m, ACT_FALLING_EXIT_AIRBORNE,   0); break;
        case MARIO_SPAWN_AIRBORNE_DEATH:        set_mario_action(m, ACT_UNUSED_DEATH_EXIT,       0); break;
        case MARIO_SPAWN_LAUNCH_STAR_COLLECT:   set_mario_action(m, ACT_SPECIAL_EXIT_AIRBORNE,   0); break;
        case MARIO_SPAWN_LAUNCH_DEATH:          set_mario_action(m, ACT_SPECIAL_DEATH_EXIT,      0); break;
    }

#ifdef PREVENT_DEATH_LOOP
    if (m->isDead) {
        m->health = 0x880;
        m->isDead = FALSE;
    }
#endif

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    set_mario_initial_cap_powerup(m);
#endif
}

void init_mario_after_warp(void) {
    struct ObjectWarpNode* spawnNode = area_get_warp_node(sWarpDest.nodeId);
    u32 marioSpawnType = get_mario_spawn_type(spawnNode->object);

    if (gMarioState->action != ACT_UNINITIALIZED) {
        vec3f_to_vec3s(gPlayerSpawnInfos[0].startPos, &spawnNode->object->oPosVec);

        vec3s_set(gPlayerSpawnInfos[0].startAngle, 0x0, spawnNode->object->oMoveAngleYaw, 0x0);

        if (marioSpawnType == MARIO_SPAWN_DOOR_WARP) {
            init_door_warp(&gPlayerSpawnInfos[0], sWarpDest.arg);
        }

        if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL
         || sWarpDest.type == WARP_TYPE_CHANGE_AREA) {
            gPlayerSpawnInfos[0].areaIndex = sWarpDest.areaIdx;
            load_mario_area();
        }

        init_mario(gMarioState);
        set_mario_initial_action(gMarioState, marioSpawnType, sWarpDest.arg);

        gMarioState->interactObj = spawnNode->object;
        gMarioState->usedObj = spawnNode->object;
    }

    reset_camera(gCurrentArea->camera);
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;

    switch (marioSpawnType) {
        case MARIO_SPAWN_PIPE:                  play_transition(WARP_TRANSITION_FADE_FROM_STAR,   0x10, 0x00, 0x00, 0x00); break;
        case MARIO_SPAWN_DOOR_WARP:             play_transition(WARP_TRANSITION_FADE_FROM_CIRCLE, 0x10, 0x00, 0x00, 0x00); break;
        case MARIO_SPAWN_TELEPORT:              play_transition(WARP_TRANSITION_FADE_FROM_COLOR,  0x14, 0xFF, 0xFF, 0xFF); break;
        case MARIO_SPAWN_SPIN_AIRBORNE:         play_transition(WARP_TRANSITION_FADE_FROM_COLOR,  0x1A, 0xFF, 0xFF, 0xFF); break;
        case MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE:  play_transition(WARP_TRANSITION_FADE_FROM_CIRCLE, 0x10, 0x00, 0x00, 0x00); break;
        case MARIO_SPAWN_FADE_FROM_BLACK:       play_transition(WARP_TRANSITION_FADE_FROM_COLOR,  0x10, 0x00, 0x00, 0x00); break;
        default:                                play_transition(WARP_TRANSITION_FADE_FROM_STAR,   0x10, 0x00, 0x00, 0x00); break;
    }

    if (gCurrDemoInput == NULL) {
#ifdef BETTER_REVERB
        gBetterReverbPresetValue = gCurrentArea->betterReverbPreset;
#endif
        set_background_music(gCurrentArea->musicSettingsPreset, gCurrentArea->musicSeqId, 0);

        if (gMarioState->flags & MARIO_METAL_CAP) {
            play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP));
        }

        if (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_WING_CAP)) {
            play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP));
        }

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        if (
            gCurrLevelNum == LEVEL_BOB &&
            get_current_background_music() != SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE) &&
            sTimerRunning
        ) {
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE), 0);
        }

        if (
            sWarpDest.levelNum == LEVEL_CASTLE &&
            sWarpDest.areaIdx  == 1            &&
            sWarpDest.nodeId   == WARP_CASTLE_INSIDE_1_FROM_SECRET_WARP
        ) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        }

        if (
            sWarpDest.levelNum == LEVEL_CASTLE_GROUNDS &&
            sWarpDest.areaIdx == 1 &&
            (
                sWarpDest.nodeId == WARP_CASTLE_GROUNDS_FROM_VCUTM_WARP ||
                sWarpDest.nodeId == WARP_CASTLE_GROUNDS_MAIN_ENTRY      ||
                sWarpDest.nodeId == WARP_CASTLE_GROUNDS_FROM_COTMC      ||
                sWarpDest.nodeId == WARP_CASTLE_GROUNDS_FROM_DDD_DOOR
            )
        ) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        }
#endif
#ifndef DISABLE_EXIT_COURSE
       if (sWarpDest.arg == WARP_FLAG_EXIT_COURSE) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        }
#endif
    }
#ifdef PUPPYPRINT_DEBUG
    gPuppyWarp = 0;
    gLastWarpID = sWarpDest.nodeId;
    gPuppyWarpArea = 0;
#endif
}

// used for warps inside one level
void warp_area(void) {
    if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        if (sWarpDest.type == WARP_TYPE_CHANGE_AREA) {
            level_control_timer(TIMER_CONTROL_HIDE);
            unload_mario_area();
            load_area(sWarpDest.areaIdx);
        }

        init_mario_after_warp();
    }
}

// used for warps between levels
void warp_level(void) {
    gCurrLevelNum = sWarpDest.levelNum;

    level_control_timer(TIMER_CONTROL_HIDE);

    load_area(sWarpDest.areaIdx);
    init_mario_after_warp();
}

void warp_credits(void) {
    s32 marioAction = ACT_UNINITIALIZED;

    switch (sWarpDest.nodeId) {
        case WARP_NODE_CREDITS_START:
            marioAction = ACT_END_PEACH_CUTSCENE;
            break;

        case WARP_NODE_CREDITS_NEXT:
            sEndCutsceneVp.vp.vscale[0] = (SCREEN_WIDTH  * 2);
            sEndCutsceneVp.vp.vscale[1] = (SCREEN_HEIGHT * 3 / 2);
            sEndCutsceneVp.vp.vtrans[0] = (SCREEN_WIDTH  * 2);
            sEndCutsceneVp.vp.vtrans[1] = (SCREEN_HEIGHT * 2);
            marioAction = ACT_CREDITS_CUTSCENE;
            break;

        case WARP_NODE_CREDITS_END:
            marioAction = ACT_END_WAVING_CUTSCENE;
            break;
    }

    gCurrLevelNum = sWarpDest.levelNum;

    load_area(sWarpDest.areaIdx);

    vec3s_copy(gPlayerSpawnInfos[0].startPos, gCurrCreditsEntry->marioPos);

    vec3s_set(gPlayerSpawnInfos[0].startAngle, 0x0, gCurrCreditsEntry->marioAngle, 0x0);

    gPlayerSpawnInfos[0].areaIndex = sWarpDest.areaIdx;

    load_mario_area();
    init_mario(gMarioState);

    set_mario_action(gMarioState, marioAction, 0);

    reset_camera(gCurrentArea->camera);

    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;

    play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x14, 0x00, 0x00, 0x00);

    if (gCurrCreditsEntry == NULL || gCurrCreditsEntry == sCreditsSequence) {
#ifdef BETTER_REVERB
        gBetterReverbPresetValue = gCurrentArea->betterReverbPreset;
#endif
        set_background_music(gCurrentArea->musicSettingsPreset, gCurrentArea->musicSeqId, 0);
    }
}

void check_instant_warp(void) {
    s16 cameraAngle;
    struct Surface* floor = gMarioState->floor;

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelNum == LEVEL_CASTLE
 #ifndef UNLOCK_ALL
     && (save_file_get_total_star_count(
            SAVE_NUM_TO_INDEX(gCurrSaveFileNum),
            COURSE_NUM_TO_INDEX(COURSE_MIN),
            COURSE_NUM_TO_INDEX(COURSE_MAX)
         ) >= 70)
 #endif // UNLOCK_ALL
    ) {
        return;
    }
#endif // ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS

    if (floor != NULL) {
        s32 index = floor->type - SURFACE_INSTANT_WARP_1B;
        if (
            index >= INSTANT_WARP_INDEX_START &&
            index <  INSTANT_WARP_INDEX_STOP  &&
            gCurrentArea->instantWarps != NULL
        ) {
            struct InstantWarp* warp = &gCurrentArea->instantWarps[index];

            if (warp->id != 0) {
                vec3_add(gMarioState->pos, warp->displacement);
                vec3f_copy(&gMarioState->marioObj->oPosVec, gMarioState->pos);

                // Fix instant warp offset not working when warping across different areas
                vec3f_copy(gMarioObject->header.gfx.pos, gMarioState->pos);

                cameraAngle = gMarioState->area->camera->yaw;

                change_area(warp->area);
                gMarioState->area = gCurrentArea;

                warp_camera(warp->displacement[0], warp->displacement[1], warp->displacement[2]);

                gMarioState->area->camera->yaw = cameraAngle;
            }
        }
    }
}

s32 music_unchanged_through_warp(s16 id) {
    struct ObjectWarpNode* warpNode = area_get_warp_node(id);
    s16 levelNum = (warpNode->node.destLevel & WARP_DEST_LEVEL_NUM_MASK);

    s16 destArea = warpNode->node.destArea;
    _Bool unchanged = TRUE;

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (
        levelNum == LEVEL_BOB      &&
        levelNum == gCurrLevelNum  &&
        destArea == gCurrAreaIndex
    ) {
        s16 currBgMusic = get_current_background_music();
        if (
            currBgMusic == SEQUENCE_ARGS(4, (SEQ_EVENT_POWERUP | SEQ_FLAG_VARIATION)) ||
            currBgMusic == SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP)
        ) {
            unchanged = FALSE;
        }
    } else {
#endif
        u16 destParam1 = gAreas[destArea].musicSettingsPreset;
        u16 destParam2 = gAreas[destArea].musicSeqId;
#ifdef BETTER_REVERB
        u16 destParam3 = gAreas[destArea].betterReverbPreset;
        unchanged = (
            levelNum == gCurrLevelNum &&
            destParam1 == gCurrentArea->musicSettingsPreset &&
            destParam2 == gCurrentArea->musicSeqId &&
            destParam3 == gCurrentArea->betterReverbPreset
        );
#else
        unchanged = (
            levelNum == gCurrLevelNum &&
            destParam1 == gCurrentArea->musicSettingsPreset &&
            destParam2 == gCurrentArea->musicSeqId
        );
#endif

        if (get_current_background_music() != destParam2) {
            unchanged = FALSE;
        }
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    }
#endif
    return unchanged;
}

/**
 * Set the current warp type and destination level/area/node.
 */
void initiate_warp(s16 destLevel, s16 destArea, s16 destWarpNode, s32 warpFlags) {
    if (destWarpNode >= WARP_NODE_CREDITS_MIN) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (warpFlags == WARP_FLAG_EXIT_COURSE) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (destLevel != gCurrLevelNum) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (destArea != gCurrentArea->index) {
        sWarpDest.type = WARP_TYPE_CHANGE_AREA;
    } else {
        sWarpDest.type = WARP_TYPE_SAME_AREA;
    }

    sWarpDest.levelNum = destLevel;
    sWarpDest.areaIdx = destArea;
    sWarpDest.nodeId = destWarpNode;
    sWarpDest.arg = warpFlags;
#if defined(PUPPYCAM) || defined(PUPPYLIGHTS)
    s32 i = 0;
#endif
#ifdef PUPPYCAM
    if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL) {
        for (i = 0; i < gPuppyVolumeCount; i++) {
            mem_pool_free(gPuppyMemoryPool, sPuppyVolumeStack[i]);
        }
        gPuppyVolumeCount = 0;
    }
#endif
#ifdef PUPPYLIGHTS
    if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL) {
        for (i = 0; i < gNumLights; i++) {
            mem_pool_free(gLightsPool, gPuppyLights[i]);
        }
        gNumLights = 0;
        levelAmbient = FALSE;
    }
#endif
}

_Bool gPaintingEjectSoundPlayed = FALSE;

/**
 * Check is Mario has entered a painting, and if so, initiate a warp.
 */
void initiate_painting_warp(void) {
    struct Object* paintingObj = gMarioState->paintingObj;

    if (paintingObj != NULL) {
        struct ObjectWarpNode* warpNode = area_get_warp_node(GET_BPARAM2(paintingObj->oBehParams));

        if (warpNode != NULL) {
            if (gMarioState->action & ACT_FLAG_INTANGIBLE) {
                // Plays the painting eject sound effect if it has not already been played.
                if (!gPaintingEjectSoundPlayed) {
                    gPaintingEjectSoundPlayed = TRUE;
                    play_sound(SOUND_GENERAL_PAINTING_EJECT,
                               gMarioState->marioObj->header.gfx.cameraToObject);
                }
            } else {
                struct WarpNode* node = &warpNode->node;

                if (!(node->destLevel & WARP_NO_CHECKPOINT)) {
                    sWarpCheckpointActive = check_warp_checkpoint(node);
                }

                initiate_warp((node->destLevel & WARP_DEST_LEVEL_NUM_MASK), node->destArea, node->destNode, WARP_FLAGS_NONE);
                check_if_should_set_warp_checkpoint(node);

                play_transition_after_delay(WARP_TRANSITION_FADE_INTO_COLOR, 30, 255, 255, 255, 45);
                level_set_transition(74, basic_update);

                set_mario_action(gMarioState, ACT_DISAPPEARED, 0);

                gMarioState->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;

                play_sound(SOUND_MENU_STAR_SOUND, gMarioState->marioObj->header.gfx.cameraToObject);
                fadeout_music(398);
                queue_rumble_data(gMarioState->controller, 80, 70, 1);

                cutscene_object(CUTSCENE_ENTER_PAINTING, paintingObj);
            }
        }
    } else {
        gPaintingEjectSoundPlayed = FALSE;
    }
}

/**
 * If there is not already a delayed warp, schedule one. The source node is
 * based on the warp operation and sometimes Mario's used object.
 * Return the time left until the delayed warp is initiated.
 */
s32 level_trigger_warp(struct MarioState* m, s32 warpOp) {
    _Bool fadeMusic = TRUE;

    if (sDelayedWarpOp == WARP_OP_NONE) {
        m->invincTimer = -1;
        sDelayedWarpArg = WARP_FLAGS_NONE;
        sDelayedWarpOp = warpOp;

        switch (warpOp) {
            case WARP_OP_DEMO_NEXT:
            case WARP_OP_DEMO_END:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = WARP_NODE_DEFAULT;
                gSavedCourseNum = COURSE_NONE;
                fadeMusic = FALSE;
                play_transition(WARP_TRANSITION_FADE_INTO_STAR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_END:
                sDelayedWarpTimer = 60;
                sSourceWarpNodeId = WARP_NODE_DEFAULT;
                fadeMusic = FALSE;
                gSavedCourseNum = COURSE_NONE;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_STAR_EXIT:
                sDelayedWarpTimer = 32;
                sSourceWarpNodeId = WARP_NODE_DEFAULT;
                gSavedCourseNum = COURSE_NONE;
                play_transition(WARP_TRANSITION_FADE_INTO_MARIO, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_DEATH:
#ifdef ENABLE_LIVES
                if (m->numLives == 0) {
                    sDelayedWarpOp = WARP_OP_GAME_OVER;
                }
#endif
                sDelayedWarpTimer = 48;
                sSourceWarpNodeId = WARP_NODE_DEATH;
                play_transition(WARP_TRANSITION_FADE_INTO_BOWSER, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                play_sound(SOUND_MENU_BOWSER_LAUGH, gGlobalSoundSource);
#ifdef PREVENT_DEATH_LOOP
                m->isDead = TRUE;
#endif
                break;

            case WARP_OP_WARP_FLOOR:
                if ((m->floor != NULL) && (m->floor->force & BITMASK(8))) {
                    sSourceWarpNodeId = (m->floor->force & BITMASK(8));
                } else {
                    sSourceWarpNodeId = WARP_NODE_WARP_FLOOR;
                    if (area_get_warp_node(sSourceWarpNodeId) == NULL) {
#ifdef ENABLE_LIVES
                        if (m->numLives == 0) {
                            sDelayedWarpOp = WARP_OP_GAME_OVER;
                        } else {
                            sSourceWarpNodeId = WARP_NODE_DEATH;
                        }
#else
                        sSourceWarpNodeId = WARP_NODE_DEATH;
#endif
                    }
                }

                sDelayedWarpTimer = 20;
                play_transition(WARP_TRANSITION_FADE_INTO_CIRCLE, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_LOOK_UP: // enter totwc
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = WARP_NODE_LOOK_UP;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0xFF, 0xFF, 0xFF);
                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
                break;

            case WARP_OP_SPIN_SHRINK: // enter bbh
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0xFF, 0xFF, 0xFF);
                break;

            case WARP_OP_TELEPORT:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                fadeMusic = !music_unchanged_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0xFF, 0xFF, 0xFF);
                break;

            case WARP_OP_WARP_DOOR:
                sDelayedWarpTimer = 20;
                sDelayedWarpArg = m->actionArg;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                fadeMusic = !music_unchanged_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_CIRCLE, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_WARP_OBJECT:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                fadeMusic = !music_unchanged_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_STAR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_START:
                sDelayedWarpTimer = 30;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_NEXT:
                if (gCurrCreditsEntry == &sCreditsSequence[0]) {
                    sDelayedWarpTimer = 60;
                    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                } else {
                    sDelayedWarpTimer = 20;
                    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                }
                fadeMusic = FALSE;
                break;
        }

        if (fadeMusic && gCurrDemoInput == NULL) {
            fadeout_music(((3 * sDelayedWarpTimer / 2) * 8) - 2);
        }
    }

    return sDelayedWarpTimer;
}

/**
 * If a delayed warp is ready, initiate it.
 */
void initiate_delayed_warp(void) {
    struct ObjectWarpNode* warpNode;
    s32 destWarpNode;

#ifdef PUPPYPRINT_DEBUG
    if (gPuppyWarp) {
        initiate_warp(gPuppyWarp, gPuppyWarpArea, 0x0A, 0);
    }
#endif

    if (sDelayedWarpOp != WARP_OP_NONE && --sDelayedWarpTimer == 0) {
        reset_dialog_render_state();

        if (gDebugLevelSelect && (sDelayedWarpOp & WARP_OP_TRIGGERS_LEVEL_SELECT)) {
            warp_special(WARP_SPECIAL_LEVEL_SELECT);
        } else if (gCurrDemoInput != NULL) {
            if (sDelayedWarpOp == WARP_OP_DEMO_END) {
                warp_special(WARP_SPECIAL_INTRO_SPLASH_SCREEN);
            } else {
                warp_special(WARP_SPECIAL_MARIO_HEAD_REGULAR);
            }
        } else {
            switch (sDelayedWarpOp) {
                case WARP_OP_GAME_OVER:
                    save_file_reload();
                    warp_special(WARP_SPECIAL_MARIO_HEAD_DIZZY);
                    break;

                case WARP_OP_CREDITS_END:
                    warp_special(WARP_SPECIAL_ENDING);
                    sound_banks_enable(SEQ_PLAYER_SFX, (SOUND_BANKS_ALL & ~SOUND_BANKS_DISABLED_AFTER_CREDITS));
                    break;

                case WARP_OP_DEMO_NEXT:
                    warp_special(WARP_SPECIAL_MARIO_HEAD_REGULAR);
                    break;

                case WARP_OP_CREDITS_START:
                    gCurrCreditsEntry = &sCreditsSequence[0];
                    initiate_warp(gCurrCreditsEntry->levelNum, gCurrCreditsEntry->areaIndex,
                                  WARP_NODE_CREDITS_START, WARP_FLAGS_NONE);
                    break;

                case WARP_OP_CREDITS_NEXT:
                    sound_banks_disable(SEQ_PLAYER_SFX, SOUND_BANKS_ALL);

                    gCurrCreditsEntry++;
                    gCurrActNum = gCurrCreditsEntry->actNum;
                    if ((gCurrCreditsEntry + 1)->levelNum == LEVEL_NONE) {
                        destWarpNode = WARP_NODE_CREDITS_END;
                    } else {
                        destWarpNode = WARP_NODE_CREDITS_NEXT;
                    }

                    initiate_warp(gCurrCreditsEntry->levelNum, gCurrCreditsEntry->areaIndex, destWarpNode, WARP_FLAGS_NONE);
                    break;

                default:
                    warpNode = area_get_warp_node(sSourceWarpNodeId);

                    initiate_warp((warpNode->node.destLevel & WARP_DEST_LEVEL_NUM_MASK), warpNode->node.destArea,
                                  warpNode->node.destNode, sDelayedWarpArg);

                    check_if_should_set_warp_checkpoint(&warpNode->node);
                    if (sWarpDest.type != WARP_TYPE_CHANGE_LEVEL) {
                        level_set_transition(2, NULL);
                    }
                    break;
            }
        }
    }
}

void update_hud_values(void) {
    if (gCurrCreditsEntry == NULL) {
        s16 numHealthWedges = ((gMarioState->health > 0) ? (gMarioState->health >> 8) : 0);
#ifdef BREATH_METER
        s16 numBreathWedges = ((gMarioState->breath > 0) ? (gMarioState->breath >> 8) : 0);
#endif
        gHudDisplay.flags = COND_BIT(gHudDisplay.flags, HUD_DISPLAY_FLAG_COIN_COUNT, (gCurrCourseNum >= COURSE_MIN));

        if (gHudDisplay.coins < gMarioState->numCoins) {
            if (gGlobalTimer & 1) {
                u32 coinSound;
                if (gMarioState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
                    coinSound = SOUND_GENERAL_COIN_WATER;
                } else {
                    coinSound = SOUND_GENERAL_COIN;
                }

                gHudDisplay.coins++;
                play_sound(coinSound, gMarioState->marioObj->header.gfx.cameraToObject);
            }
        }

#ifdef ENABLE_LIVES
        if (gMarioState->numLives > MAX_NUM_LIVES) {
            gMarioState->numLives = MAX_NUM_LIVES;
        }
#endif

        if (gMarioState->numCoins > MAX_NUM_COINS) {
            gMarioState->numCoins = MAX_NUM_COINS;
        }

        if (gHudDisplay.coins > MAX_NUM_COINS) {
            gHudDisplay.coins = MAX_NUM_COINS;
        }

        gHudDisplay.stars = gMarioState->numStars;
        gHudDisplay.lives = gMarioState->numLives;
        gHudDisplay.keys  = gMarioState->numKeys;

        if (numHealthWedges > gHudDisplay.wedges) {
            play_sound(SOUND_MENU_POWER_METER, gGlobalSoundSource);
        }
        gHudDisplay.wedges = numHealthWedges;

        gHudDisplay.flags = COND_BIT(gHudDisplay.flags, HUD_DISPLAY_FLAG_EMPHASIZE_POWER, (gMarioState->hurtCounter > 0));
#ifdef BREATH_METER
        gHudDisplay.breath = numBreathWedges;
        gHudDisplay.flags = COND_BIT(gHudDisplay.flags, HUD_DISPLAY_FLAG_BREATH_METER, (gMarioState->breath > 0));
#endif
    }
}

/**
 * Update objects, HUD, and camera. This update function excludes things like
 * endless staircase, warps, pausing, etc. This is used when entering a painting,
 * presumably to allow painting and camera updating while avoiding triggering the
 * warp twice.
 */
void basic_update(void) {
    area_update_objects();
    update_hud_values();
#ifdef PUPPYLIGHTS
    delete_lights();
#endif

    if (gCurrentArea != NULL) {
        update_camera(gCurrentArea->camera);
    }
}

s32 play_mode_normal(void) {
#ifndef DISABLE_DEMO
    if (gCurrDemoInput != NULL) {
        gMarioState->controller = gDemoController;
        print_intro_text();
        if (gDemoController->buttonPressed & INPUT_END_DEMO) {
            level_trigger_warp(gMarioState, ((gDemoInputListID == gDemoInputsBuf.dmaTable->count) ? WARP_OP_DEMO_END : WARP_OP_DEMO_NEXT));
        } else if (
            !gWarpTransition.isActive &&
            sDelayedWarpOp == WARP_OP_NONE &&
            (gPlayer1Controller->buttonPressed & START_BUTTON)
        ) {
            level_trigger_warp(gMarioState, WARP_OP_DEMO_NEXT);
        }
    } else {
        gMarioState->controller = &gControllers[0];
    }
#endif

    warp_area();
    check_instant_warp();

#ifdef PUPPYPRINT_DEBUG
    if (
        sPPDebugPage != PUPPYPRINT_PAGE_RAM &&
        sPPDebugPage != PUPPYPRINT_PAGE_LEVEL_SELECT
 #ifdef BETTER_REVERB
        && sPPDebugPage != PUPPYPRINT_PAGE_BETTER_REVERB
 #endif // BETTER_REVERB
    ) {
        if (sTimerRunning && gHudDisplay.timer < 17999) {
            gHudDisplay.timer++;
        }
        area_update_objects();
    }
#else // !PUPPYPRINT_DEBUG
    if (sTimerRunning && gHudDisplay.timer < 17999) {
        gHudDisplay.timer++;
    }
    area_update_objects();
#endif // !PUPPYPRINT_DEBUG
    update_hud_values();
#ifdef PUPPYLIGHTS
    delete_lights();
#endif // PUPPYLIGHTS
    if (gCurrentArea != NULL) {
#ifdef PUPPYPRINT_DEBUG

    if (
        sPPDebugPage != PUPPYPRINT_PAGE_RAM &&
        sPPDebugPage != PUPPYPRINT_PAGE_LEVEL_SELECT
 #ifdef BETTER_REVERB
        && sPPDebugPage != PUPPYPRINT_PAGE_BETTER_REVERB
 #endif // BETTER_REVERB
    ) {
            update_camera(gCurrentArea->camera);
        }
#else // !PUPPYPRINT_DEBUG
        update_camera(gCurrentArea->camera);
#endif // !PUPPYPRINT_DEBUG
    }

    initiate_painting_warp();
    initiate_delayed_warp();

    // If either initiate_painting_warp or initiate_delayed_warp initiated a
    // warp, change play mode accordingly.
    if (sCurrPlayMode == PLAY_MODE_NORMAL) {
        if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL) {
            set_play_mode(PLAY_MODE_CHANGE_LEVEL);
        } else if (sTransitionTimer != 0) {
            set_play_mode(PLAY_MODE_CHANGE_AREA);
        } else if (pressed_pause()) {
            lower_background_noise(BG_NOISE_OP_TOGGLE_MUTE);
            cancel_rumble();
            gCameraMovementFlags |= CAM_MOVE_PAUSE_SCREEN;
            set_play_mode(PLAY_MODE_PAUSED);
        }
    }

    return FALSE;
}

s32 play_mode_paused(void) {
    if (gMenuOptSelectIndex == MENU_OPT_NONE) {
        set_menu_mode(MENU_MODE_RENDER_PAUSE_SCREEN);
    } else if (gMenuOptSelectIndex == MENU_OPT_DEFAULT) {
        raise_background_noise(BG_NOISE_OP_TOGGLE_MUTE);
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        set_play_mode(PLAY_MODE_NORMAL);
#ifndef DISABLE_EXIT_COURSE
    } else { // MENU_OPT_EXIT_COURSE
        if (gDebugLevelSelect) {
            fade_into_special_warp(WARP_SPECIAL_LEVEL_SELECT, 1);
        } else {
#ifdef DEATH_ON_EXIT_COURSE
            raise_background_noise(1);
            gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
            set_play_mode(PLAY_MODE_NORMAL);
            level_trigger_warp(gMarioState, WARP_OP_DEATH);
#else
            initiate_warp(EXIT_COURSE_LEVEL, EXIT_COURSE_AREA, EXIT_COURSE_NODE, WARP_FLAG_EXIT_COURSE);
            fade_into_special_warp(WARP_SPECIAL_NONE, 0);
            gSavedCourseNum = COURSE_NONE;
#endif
        }

        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
#endif
    }

    return FALSE;
}

/**
 * Debug mode that lets you frame advance by pressing D-pad down. Unfortunately
 * it uses the pause camera, making it basically unusable in most levels.
 */
s32 play_mode_frame_advance(void) {
    if (gPlayer1Controller->buttonPressed & D_JPAD) {
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        play_mode_normal();
    } else if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        raise_background_noise(BG_NOISE_OP_TOGGLE_MUTE);
        set_play_mode(PLAY_MODE_NORMAL);
    } else {
        gCameraMovementFlags |= CAM_MOVE_PAUSE_SCREEN;
    }

    return FALSE;
}

/**
 * Set the transition, which is a period of time after the warp is initiated
 * but before it actually occurs. If updateFunction is not NULL, it will be
 * called each frame during the transition.
 */
void level_set_transition(s16 length, void (*updateFunction)()) {
    sTransitionTimer = length;
    sTransitionUpdate = updateFunction;
}

/**
 * Play the transition and then return to normal play mode.
 */
s32 play_mode_change_area(void) {
    // sm64ex-alo
    // Change function to have similar change_level defines
    if (sTransitionUpdate != NULL) {
        sTransitionUpdate(&sTransitionTimer);
    }

    if (--sTransitionTimer == -1) {
        update_camera(gCurrentArea->camera);
        sTransitionTimer = 0;
        sTransitionUpdate = NULL;
        set_play_mode(PLAY_MODE_NORMAL);
    }

    return FALSE;
}

/**
 * Play the transition and then return to normal play mode.
 */
s32 play_mode_change_level(void) {
    if (sTransitionUpdate != NULL) {
        sTransitionUpdate(&sTransitionTimer);
    }

    if (--sTransitionTimer == -1) {
        gHudDisplay.flags = HUD_DISPLAY_NONE;
        sTransitionTimer = 0;
        sTransitionUpdate = NULL;

        if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
            return sWarpDest.levelNum;
        } else {
            return sSpecialWarpDest;
        }
    }

    return FALSE;
}

/**
 * Unused play mode. Doesn't call transition update and doesn't reset transition at the end.
 */
UNUSED static s32 play_mode_unused(void) {
    if (--sTransitionTimer == -1) {
        gHudDisplay.flags = HUD_DISPLAY_NONE;

        if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
            return sWarpDest.levelNum;
        } else {
            return sSpecialWarpDest;
        }
    }

    return FALSE;
}

s32 update_level(void) {
    _Bool changeLevel = FALSE;

    switch (sCurrPlayMode) {
        case PLAY_MODE_NORMAL:
            changeLevel = play_mode_normal();
            break;
        case PLAY_MODE_PAUSED:
            changeLevel = play_mode_paused();
            break;
        case PLAY_MODE_CHANGE_AREA:
            changeLevel = play_mode_change_area();
            break;
        case PLAY_MODE_CHANGE_LEVEL:
            changeLevel = play_mode_change_level();
            break;
        case PLAY_MODE_FRAME_ADVANCE:
            changeLevel = play_mode_frame_advance();
            break;
    }

    if (changeLevel) {
        reset_volume();
        enable_background_sound();
    }

    return changeLevel;
}

s32 init_level(void) {
    _Bool fadeFromColor = FALSE;
#ifdef PUPPYPRINT_DEBUG
    OSTime first = osGetTime();
#endif

    set_play_mode(PLAY_MODE_NORMAL);

    sDelayedWarpOp = WARP_OP_NONE;
    sTransitionTimer = 0;
    sSpecialWarpDest = WARP_SPECIAL_NONE;

    g100CoinStarSpawned = FALSE;

    // NOTE: gStarModelLastCollected reset here as a safety to prevent possible UB if assigned a model used
    // in a non-global group. This checked can be removed as needed.
    if (gStarModelLastCollected != MODEL_BOWSER_KEY
#ifdef STAR_DANCE_USES_STARS_MODEL
         || gStarModelLastCollected != MODEL_TRANSPARENT_STAR
#endif
    ) {
        gStarModelLastCollected = MODEL_STAR;
    }

    if (gCurrCreditsEntry == NULL) {
        gHudDisplay.flags = HUD_DISPLAY_DEFAULT;
    } else {
        gHudDisplay.flags = HUD_DISPLAY_NONE;
    }

    sTimerRunning = FALSE;

    if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        if (sWarpDest.nodeId >= WARP_NODE_CREDITS_MIN) {
            warp_credits();
        } else {
            warp_level();
        }
    } else {
        if (gPlayerSpawnInfos[0].areaIndex >= 0) {
            load_mario_area();
            init_mario(gMarioState);
        }

        if (gCurrentArea != NULL) {
            reset_camera(gCurrentArea->camera);
#ifdef PEACH_SKIP
            if (gCurrDemoInput != NULL) {
                set_mario_action(gMarioState, ACT_IDLE, 0);
            } else if (!gDebugLevelSelect) {
                if (gMarioState->action != ACT_UNINITIALIZED) {
                    set_mario_action(gMarioState, ACT_IDLE, 0);
                }
            }
#else
            if (gCurrDemoInput != NULL) {
                set_mario_action(gMarioState, ACT_IDLE, 0);
            } else if (!gDebugLevelSelect) {
                if (gMarioState->action != ACT_UNINITIALIZED) {
                    if (save_file_exists(SAVE_NUM_TO_INDEX(gCurrSaveFileNum))) {
                        set_mario_action(gMarioState, ACT_IDLE, 0);
                    } else {
                        set_mario_action(gMarioState, ACT_INTRO_CUTSCENE, 0);
                        fadeFromColor = TRUE;
                    }
                }
            }
#endif
        }

        if (fadeFromColor) {
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x5A, 0xFF, 0xFF, 0xFF);
        } else {
            play_transition(WARP_TRANSITION_FADE_FROM_STAR, 0x10, 0xFF, 0xFF, 0xFF);
        }

        if (gCurrDemoInput == NULL) {
#ifdef BETTER_REVERB
            gBetterReverbPresetValue = gCurrentArea->betterReverbPreset;
#endif
            set_background_music(gCurrentArea->musicSettingsPreset, gCurrentArea->musicSeqId, 0);
        }
    }

    if (gCurrDemoInput == NULL) {
        cancel_rumble();
    }

    if (gMarioState->action == ACT_INTRO_CUTSCENE) {
        sound_banks_disable(SEQ_PLAYER_SFX, SOUND_BANKS_DISABLED_DURING_INTRO_CUTSCENE);
    }

#ifdef PUPPYLIGHTS
    puppylights_allocate();
#endif

    append_puppyprint_log("Level loaded in %d" PP_CYCLE_STRING ".", (s32)(PP_CYCLE_CONV(osGetTime() - first)));
    return TRUE;
}

s32 is_bowser_level(u32 level) {
#ifndef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    return FALSE;
#endif
    return (
        level == LEVEL_BOWSER_1 ||
        level == LEVEL_BOWSER_2 ||
        level == LEVEL_BOWSER_3
    );
}

/**
 * Initialize the current level if initOrUpdate is 0, or update the level if it is 1.
 */
s32 lvl_init_or_update(s16 initOrUpdate, UNUSED s32 unused) {
    return (initOrUpdate ? update_level() : init_level());
}

#if MULTILANG
void load_language_text(void) {
    switch (gInGameLanguage - 1) {
        case LANGUAGE_ENGLISH:
            load_segment_decompress(SEGMENT_EU_TRANSLATION, _translation_en_yay0SegmentRomStart, _translation_en_yay0SegmentRomEnd);
            break;
        case LANGUAGE_FRENCH:
            load_segment_decompress(SEGMENT_EU_TRANSLATION, _translation_fr_yay0SegmentRomStart, _translation_fr_yay0SegmentRomEnd);
            break;
        case LANGUAGE_GERMAN:
            load_segment_decompress(SEGMENT_EU_TRANSLATION, _translation_de_yay0SegmentRomStart, _translation_de_yay0SegmentRomEnd);
            break;
    }
}
#endif

extern _Bool sYoshiDead;

s32 lvl_init_from_save_file(UNUSED s16 initOrUpdate, s32 levelNum) {
#if MULTILANG
    gInGameLanguage = eu_get_language() + 1;
    load_language_text();
#endif
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    gNeverEnteredCastle = !save_file_exists(SAVE_NUM_TO_INDEX(gCurrSaveFileNum));
#else
    gNeverEnteredCastle = FALSE;
#endif
    gCurrLevelNum = levelNum;
    gCurrCourseNum = COURSE_NONE;
    gSavedCourseNum = COURSE_NONE;
    gCurrCreditsEntry = NULL;
    gSpecialTripleJump = FALSE;

    init_mario_from_save_file(gMarioState);
    disable_warp_checkpoint();
    save_file_move_cap_to_default_location();
    select_mario_cam_mode();
    // Resets yoshi as spawned/despawned upon new file select.
    sYoshiDead = FALSE;

    return levelNum;
}

s32 lvl_set_current_level(UNUSED s16 initOrUpdate, s32 levelNum) {
    s32 warpCheckpointActive = sWarpCheckpointActive;

    sWarpCheckpointActive = FALSE;
    gCurrLevelNum = levelNum;
    gCurrCourseNum = gLevelToCourseNumTable[levelNum - 1];

    if (
        gCurrDemoInput != NULL    ||
        gCurrCreditsEntry != NULL ||
        gCurrCourseNum == COURSE_NONE
    ) {
        return FALSE;
    }

    if (is_bowser_level(gCurrLevelNum)) {
        gMarioState->numCoins = 0;
        gHudDisplay.coins = 0;
        gCurrCourseStarFlags = save_file_get_star_flags(
            SAVE_NUM_TO_INDEX(gCurrSaveFileNum),
            COURSE_NUM_TO_INDEX(gCurrCourseNum)
        );
    }

    if (gSavedCourseNum != gCurrCourseNum) {
        gSavedCourseNum = gCurrCourseNum;
        disable_warp_checkpoint();
    }

    if (gCurrCourseNum > COURSE_STAGES_MAX || warpCheckpointActive) {
        return FALSE;
    }

    return !gDebugLevelSelect;
}

/**
 * Play the "thank you so much for to playing my game" sound.
 */
s32 lvl_play_the_end_screen_sound(UNUSED s16 initOrUpdate, UNUSED s32 levelNum) {
    play_sound(SOUND_MENU_THANK_YOU_PLAYING_MY_GAME, gGlobalSoundSource);
    return TRUE;
}
