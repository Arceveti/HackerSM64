#pragma once

/******************
 * DEBUG SETTINGS *
 ******************/

/**
 * Enables most debug options, including PUPPYPRINT_DEBUG.
 */
// #define DEBUG_ALL

/**
 * Disables all debug options (except PUPPYPRINT).
 */
// #define DISABLE_ALL

/**
 * Enables a comprehensive standalone profiler. Automatically enabled by PUPPYPRINT_DEBUG.
 * If not using PUPPYPRINT_DEBUG, press L to toggle the profiler.
 */
#define USE_PROFILER

/**
 * -- TEST LEVEL --
 * Uncomment this define and set a test level in order to boot straight into said level.
 * This allows you to quickly test the level you're working on.
 * If you want the game to boot normally, just comment out the define again.
 */
// #define STARTUP_SPAWN_LEVEL LEVEL_BOB

/**
 * Mario's starting area on boot.
 * NOTE: Requires STARTUP_SPAWN_LEVEL.
 * NOTE: It is also recommended to set STARTUP_SPAWN_POS with this, otherwise the default position may cause Mario to spawn out of bounds.
 */
// #define STARTUP_SPAWN_AREA 1

/**
 * Mario's starting position on boot.
 * NOTE: Requires STARTUP_SPAWN_LEVEL.
 */
// #define STARTUP_SPAWN_POS 0, 0, 0

/**
 * Mario's starting yaw on boot.
 * NOTE: Requires STARTUP_SPAWN_LEVEL.
 */
// #define STARTUP_SPAWN_ANGLE 0x0

/**
 * Enable debug level select.
 * Hold L while the splash screen ends to turn it on.
 * Hold L + Z and press Start while on the level select menu to turn it off.
 */
#define DEBUG_LEVEL_SELECT

/**
 * Enables debug free move (D-pad up to enter, A to exit).
 */
#define ENABLE_DEBUG_FREE_MOVE

/**
 * Enables a custom, enhanced performance profiler. (Enables PUPPYPRINT by default in config_safeguards).
 */
#define PUPPYPRINT_DEBUG

/**
 * Uses cycles instead of microseconds in Puppyprint debug output.
 */
// #define PUPPYPRINT_DEBUG_CYCLES

/**
 * A vanilla style debug mode. It doesn't rely on a text engine, but it's much less powerful that PUPPYPRINT_DEBUG.
 * Press D-pad left to show the debug UI.
 */
// #define VANILLA_STYLE_CUSTOM_DEBUG

/**
 * Visual debug enables some collision visuals. Tapping Right on the D-pad will cycle between visual hitboxes, visual surfaces, both, and neither.
 * If puppyprint is enabled, then this can be cycled only while the screen is active.
 */
#define VISUAL_DEBUG

/**
 * Opens all courses and doors. Used for debugging purposes to unlock all content.
 */
#define UNLOCK_ALL

/**
 * Same as above, but also reads all save file flags as complete.
 * This will not overwrite existing save file data unless you save over it.
 */
// #define COMPLETE_SAVE_FILE

/**
 * Removes the limit on FPS.
 */
// #define UNLOCK_FPS

/**
 * Includes vanilla debug functionality.
 */
// #define VANILLA_DEBUG

/**
 * Forces a crash when the game starts. Useful for debugging the crash screen.
 */
// #define DEBUG_FORCE_CRASH_ON_BOOT

/**
 * Forces a crash when the player presses L in-game.
 */
// #define DEBUG_FORCE_CRASH_ON_L

/**
 * Enables the DEBUG_ASSERT macro.
 */
#define ENABLE_DEBUG_ASSERTS

/**
 * Includes function map data in the ROM.
 */
#define INCLUDE_DEBUG_MAP

/**
 * Include all MIPS III instructions in the disassembler, rather than just the ones that can generate normally.
 */
#define DISASM_INCLUDE_ALL_INSTRUCTIONS

/**
 * Includes an image which displays when the crash screen crashes.
 * Uses 38.4 KB of rom space.
 */
#define CRASH_SCREEN_CRASH_SCREEN

/**
 * Plays a Mario "WAAAOOOW" sound effect when the game crashes.
 */
// #define FUNNY_CRASH_SOUND
