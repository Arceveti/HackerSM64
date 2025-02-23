#pragma once

/****************************
 * SPECIFIC OBJECT SETTINGS *
 ****************************/


/*********************
 * -- MARIO'S CAP --
 *********************/

/**
 * The time each type of cap power-up lasts
 */
#define VANISH_CAP_TIME  600
#define  METAL_CAP_TIME  600
#define   WING_CAP_TIME 1800


/**************
 * -- COIN --
 **************/

/**
 * The distance from Mario at which coin formations spawn their coins. Vanilla is 2000.
 * Set this to o->oDrawingDistance for a more reasonable range at the cost of performance.
 */
#define COIN_FORMATION_DISTANCE 2000

/**
 * Moving Coins flicker and disappear when they hit lava instead of being instantly deleted.
 */
#define COIN_LAVA_FLICKER

/**
 * Allows for retries on collecting the remaining blue coins from a blue coin switch.
 */
#define BLUE_COIN_SWITCH_RETRY

/**
 * The radius of coin ring formations.
 */
#define COIN_FORMATION_RING_RADIUS_HORIZONTAL 300.0f
#define COIN_FORMATION_RING_RADIUS_VERTICAL   200.0f


/*************************
 * -- EXCLAMATION BOX --
 *************************/

/**
 * Makes all cap boxes solid regardless of whether their cap switch is pressed.
 */
#define EXCLAMATION_BOXES_ALWAYS_SOLID


/**
 * Allow previewing blue coin locations from a blue coin switch when Mario is standing on the blue coin switch.
 */
// #define BLUE_COIN_SWITCH_PREVIEW

/****************
 * -- GOOMBA --
 ****************/

/**
 * Tiny Goombas (from THI) always drop their coin.
 */
#define TINY_GOOMBA_ALWAYS_DROPS_COIN

/**
 * Floombas! These fellas sport custom behaviors (bhvFloomba, bhvFloombaTripletSpawner) and of course the iconic Floomba texture.
 * Also supports macros (macro_floomba, macro_huge_floomba, macro_tiny_floomba, macro_floomba_triplet_spawner, macro_floomba_quintuplet_spawner).
 */
#define FLOOMBAS


/**************
 * -- HOOT --
 **************/

/**
 * Uses intendedYaw to control Hoot instead of the raw left and right inputs.
 */
#define HOOT_YAW_FIX

/**
 * Causes leaf particles to occasionally fall from trees which contain Hoot.
 */
#define HOOT_TREE_PARTICLES


/**************
 * -- MR I --
 **************/

/**
 * Enables Mr I to shoot his particle in any direction.
 */
#define MR_I_PITCH_SHOOTING


/********************
 * -- WATER RING --
 ********************/

/**
 * Fix the water rings in DDD by checking for interaction rather than normals.
 */
#define FIX_WATER_RINGS


/********************
 * -- POWER STAR --
 ********************/

/**
 * Collecting a Power Star will fully heal Mario.
 */
#define POWER_STARS_HEAL


/***********************
 * -- 1-UP MUSHROOM --
 ***********************/

/**
 * Collecting a 1-Up Mushroom will fully heal Mario.
 */
#define MUSHROOMS_HEAL


/***************************
 * -- PLATFORM ON TRACK --
 ***************************/

/**
 * Allows the speed of a platform on a track to be controlled by standing near the front or back of it.
 */
// #define CONTROLLABLE_PLATFORM_SPEED


/*********************
 * -- CHAIN CHOMP --
 *********************/

/**
 * The number of chain balls the Chain Chomp has. Vanilla is 5.
 */
#define CHAIN_CHOMP_NUM_SEGMENTS 5


/***************
 * -- POKEY --
 ***************/

/**
 * The number of parts Pokey has, including the head. Vanilla is 5, max is 30.
 */
#define POKEY_NUM_SEGMENTS       5


/*****************
 * -- WIGGLER --
 *****************/

/**
 * The number of segments Wiggler has, not including the head. Vanilla is 4.
 */
#define WIGGLER_NUM_SEGMENTS     4


/***************************
 * -- WF BREAKABLE WALL --
 ***************************/

/**
 * Makes the breakable wall in WF spawn its star instead of the star existing in the level from the start.
 */
// #define WF_BREAKABLE_WALL_SPAWNS_STAR


/******************
 * -- JRB MIST --
 ******************/

/**
 * Makes the JRB mist always appear on act 1.
 */
#define JRB_ACT_SPECIFIC_MIST


/*********************
 * -- DDD OBJECTS --
 *********************/

/**
 * Makes certain objects in DDD (Bowser's sub, sub door, & poles) act-specific instead of being dependent on Bowser 2 being beaten.
 */
#define DDD_ACT_SPECIFIC_OBJECTS
