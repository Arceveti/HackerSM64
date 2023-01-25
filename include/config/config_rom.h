#pragma once

/****************
 * ROM SETTINGS *
 ****************/

/**
 * Internal ROM name. NEEDS TO BE **EXACTLY** 20 CHARACTERS. Can't be 19 characters, can't be 21 characters. You can fill it with spaces.
 * The end quote should be here:               "
 */
#define INTERNAL_ROM_NAME "HackerSM64          "

/**
 * Enables Rumble Pak Support.
 * Currently not recommended, as it may cause random crashes.
 */
// #define ENABLE_RUMBLE (1 || VERSION_SH)

/**
 * The maximum number of supported controllers. 1-4.
 * This will save performance if the player has extra unused controllers plugged in.
 * NOTE: Default is 2, maximum is 4.
 * NOTE: Some flashcarts (eg. ED64p) don't let you start a ROM with a GameCube controller in port 1,
 *       so ports 1 and 2 get swapped if port 1 is an N64 controller and port 2 is a GC controller.
 *       This define should be at least 2 if you want to allow GC controllers on those flashcarts.
 */
#define NUM_SUPPORTED_CONTROLLERS 2

/**
 * Informs supported emulators to default to gamecube controller inputs.
 */
// #define USE_GAMECUBE_CONTROLLER

/**
 * RCVI hack. Increases performance on emulator, and does nothing on console.
 * Might break on some emulators. Use at your own risk, and don't use it unless you actually need the extra performance.
 */
// #define RCVI_HACK

/**
 * Height of the black borders at the top and bottom of the screen for NTSC Versions. You can set it to different values for console and emulator.
 * There is generally no reason to have a value other than 0 for emulator. As for console, it provides a (small) performance boost.
 */
#define BORDER_HEIGHT_CONSOLE  0
#define BORDER_HEIGHT_EMULATOR 0

/**
 * !EXPERIMENTAL!
 * This should be either 1 or 2.
 * 1 = 240p (Default)
 * 2 = 420i
 * NOTE: DISABLE_AA is recommended for performance.
 * NOTE: Some UI elements don't render correctly.
 */
#define RESOLUTION_MULTIPLIER 1

/**
 * Screen Size Defines.
 */
#define SCREEN_WIDTH  (320 * RESOLUTION_MULTIPLIER)
#define SCREEN_HEIGHT (240 * RESOLUTION_MULTIPLIER)

/**
 * Don't change these.
 */
#define SCREEN_CENTER_X (SCREEN_WIDTH  / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
