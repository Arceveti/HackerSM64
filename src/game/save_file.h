#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#include <PR/ultratypes.h>

#include "types.h"
#include "area.h"
#include "puppycam2.h"

#include "course_table.h"

#if defined(SRAM)
    #define EEPROM_SIZE 0x8000
#elif defined(EEP16K)
    #define EEPROM_SIZE 0x800
#else
    #define EEPROM_SIZE 0x200
#endif

#define NUM_SAVE_FILES 4

struct SaveBlockSignature {
    u16 magic;
    u16 chksum;
};

struct SaveFile {
    // Location of lost cap.
    // Note: the coordinates get set, but are never actually used, since the
    // cap can always be found in a fixed spot within the course
    u8 capLevel;
    u8 capArea;
    Vec3s capPos; // 48 bits

    u32 flags;

    // Star flags for each course.
    // The most significant bit of the byte *following* each course is set if the
    // cannon is open.
    u8 courseStars[COURSE_COUNT]; // 200 bits

    u8 courseCoinScores[COURSE_STAGES_COUNT]; // 120 bits

    struct SaveBlockSignature signature; // 32 bits
};

enum SaveFileIndex {
    SAVE_FILE_A,
    SAVE_FILE_B,
    SAVE_FILE_C,
    SAVE_FILE_D,
};

struct MainMenuSaveData {
    // Each save file has a 2 bit "age" for each course. The higher this value,
    // the older the high score is. This is used for tie-breaking when displaying
    // on the high score screen.
    u32 coinScoreAges[NUM_SAVE_FILES];
    u8 soundMode: 2;
#ifdef WIDE
    u8 wideMode: 1;
#endif

#if MULTILANG
    u8 language: 2;
#define SUBTRAHEND 8
#else
#define SUBTRAHEND 6
#endif
    u8 firstBoot;

#ifdef PUPPYCAM
    struct gPuppyOptions saveOptions;
#endif
    struct SaveBlockSignature signature;
};

struct SaveBuffer {
    // Each of the four save files has two copies. If one is bad, the other is used as a backup.
    struct SaveFile files[NUM_SAVE_FILES][2];
    // Main menu data, storing config options.
    struct MainMenuSaveData menuData;
};

#ifdef PUPPYCAM
extern void puppycam_set_save(void);
extern void puppycam_get_save(void);
extern void puppycam_check_save(void);
#endif

STATIC_ASSERT((sizeof(struct SaveBuffer) <= EEPROM_SIZE), "ERROR: Save struct too big for specified save type");

extern u8 gLastCompletedCourseNum;
extern u8 gLastCompletedStarNum;
extern _Bool sUnusedGotGlobalCoinHiScore;
extern _Bool gGotFileCoinHiScore;
extern u8 gCurrCourseStarFlags;
extern _Bool gSpecialTripleJump;
extern s8 gLevelToCourseNumTable[];

enum CourseFlags {
    COURSE_FLAG_CANNON_UNLOCKED      = BIT(7), /* 0x00000080 */
};

// game progress flags
enum SaveProgressFlags {
    SAVE_FLAGS_NONE                  = 0x0,
    SAVE_FLAG_FILE_EXISTS            = BIT( 0), /* 0x00000001 */
    SAVE_FLAG_HAVE_WING_CAP          = BIT( 1), /* 0x00000002 */
    SAVE_FLAG_HAVE_METAL_CAP         = BIT( 2), /* 0x00000004 */
    SAVE_FLAG_HAVE_VANISH_CAP        = BIT( 3), /* 0x00000008 */
    SAVE_FLAG_HAVE_KEY_1             = BIT( 4), /* 0x00000010 */
    SAVE_FLAG_HAVE_KEY_2             = BIT( 5), /* 0x00000020 */
    SAVE_FLAG_UNLOCKED_BASEMENT_DOOR = BIT( 6), /* 0x00000040 */
    SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR = BIT( 7), /* 0x00000080 */
    SAVE_FLAG_UNUSED_8               = BIT( 8), /* 0x00000100 */
    SAVE_FLAG_MOAT_DRAINED           = BIT( 9), /* 0x00000200 */
    SAVE_FLAG_UNLOCKED_PSS_DOOR      = BIT(10), /* 0x00000400 */
    SAVE_FLAG_UNLOCKED_WF_DOOR       = BIT(11), /* 0x00000800 */
    SAVE_FLAG_UNLOCKED_CCM_DOOR      = BIT(12), /* 0x00001000 */
    SAVE_FLAG_UNLOCKED_JRB_DOOR      = BIT(13), /* 0x00002000 */
    SAVE_FLAG_UNLOCKED_BITDW_DOOR    = BIT(14), /* 0x00004000 */
    SAVE_FLAG_UNLOCKED_BITFS_DOOR    = BIT(15), /* 0x00008000 */
    SAVE_FLAG_CAP_ON_GROUND          = BIT(16), /* 0x00010000 */
    SAVE_FLAG_CAP_ON_KLEPTO          = BIT(17), /* 0x00020000 */
    SAVE_FLAG_CAP_ON_UKIKI           = BIT(18), /* 0x00040000 */
    SAVE_FLAG_CAP_ON_MR_BLIZZARD     = BIT(19), /* 0x00080000 */
    SAVE_FLAG_UNLOCKED_50_STAR_DOOR  = BIT(20), /* 0x00100000 */
    SAVE_FLAG_COLLECTED_TOAD_STAR_1  = BIT(24), /* 0x01000000 */
    SAVE_FLAG_COLLECTED_TOAD_STAR_2  = BIT(25), /* 0x02000000 */
    SAVE_FLAG_COLLECTED_TOAD_STAR_3  = BIT(26), /* 0x04000000 */
    SAVE_FLAG_COLLECTED_MIPS_STAR_1  = BIT(27), /* 0x08000000 */
    SAVE_FLAG_COLLECTED_MIPS_STAR_2  = BIT(28), /* 0x10000000 */
};

enum StarFlags {
    STAR_FLAGS_NONE         = 0x0,
    STAR_FLAG_ACT_1         = BIT(0), // 0x01
    STAR_FLAG_ACT_2         = BIT(1), // 0x02
    STAR_FLAG_ACT_3         = BIT(2), // 0x04
    STAR_FLAG_ACT_4         = BIT(3), // 0x08
    STAR_FLAG_ACT_5         = BIT(4), // 0x10
    STAR_FLAG_ACT_6         = BIT(5), // 0x20
    STAR_FLAG_ACT_100_COINS = BIT(6), // 0x40
    STAR_FLAG_LAST          = STAR_FLAG_ACT_100_COINS,
};

#define SAVE_FLAG_TO_STAR_FLAG(cmd) (((cmd) >> 24) & BITMASK(7))
#define STAR_FLAG_TO_SAVE_FLAG(cmd) ((cmd) << 24)

// Variable for setting a warp checkpoint.

// possibly a WarpDest struct where arg is a union. TODO: Check?
struct WarpCheckpoint {
    /*0x00*/ u8 actNum;
    /*0x01*/ u8 courseNum;
    /*0x02*/ u8 levelID;
    /*0x03*/ u8 areaNum;
    /*0x04*/ u8 warpNode;
};

extern struct WarpCheckpoint gWarpCheckpoint;

extern _Bool gMainMenuDataModified;
extern _Bool gSaveFileModified;

void save_file_do_save(s32 fileIndex);
void save_file_erase(s32 fileIndex);
void save_file_copy(s32 srcFileIndex, s32 destFileIndex);
void save_file_load_all(void);
void save_file_reload(void);
void save_file_collect_star_or_key(s16 coinScore, s16 starIndex);
s32 save_file_exists(s32 fileIndex);
u32 save_file_get_max_coin_score(s32 courseIndex);
s32 save_file_get_course_star_count(s32 fileIndex, s32 courseIndex);
s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse);
void save_file_set_flags(u32 flags);
void save_file_clear_flags(u32 flags);
u32 save_file_get_flags(void);
u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex);
void save_file_set_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags);
s32 save_file_get_course_coin_score(s32 fileIndex, s32 courseIndex);
s32 save_file_is_cannon_unlocked(void);
void save_file_set_cannon_unlocked(void);
void save_file_set_cap_pos(s16 x, s16 y, s16 z);
s32 save_file_get_cap_pos(Vec3s capPos);
void save_file_set_sound_mode(u16 mode);
u32 save_file_get_sound_mode(void);
#ifdef WIDE
u32 save_file_get_widescreen_mode(void);
void save_file_set_widescreen_mode(u8 mode);
#endif
void save_file_move_cap_to_default_location(void);

void disable_warp_checkpoint(void);
void check_if_should_set_warp_checkpoint(struct WarpNode* warpNode);
s32 check_warp_checkpoint(struct WarpNode* warpNode);

#if MULTILANG
enum EuLanguages {
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN,
};

void eu_set_language(u16 language);
u32 eu_get_language(void);
#else
#define LANGUAGE_ENGLISH 0
#endif

#endif // SAVE_FILE_H
