#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "audio/external.h"
#include "behavior_data.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "file_select.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/print.h"
#include "game/save_file.h"
#include "game/segment2.h"
#include "game/segment7.h"
#include "game/spawn_object.h"
#include "game/rumble_init.h"
#include "sm64.h"
#include "text_strings.h"

#include "eu_translation.h"
#if MULTILANG
#undef LANGUAGE_FUNCTION
#define LANGUAGE_FUNCTION sLanguageMode
s8 sLanguageMode = LANGUAGE_ENGLISH;
#endif

#define MAX_BUTTONS_PER_PAGE 16

struct MenuButton {
    s16 id;
    ModelID16 model;
    s16 x;
    s16 y;
};

static const struct MenuButton sMainMenuButtonsList[] = {
    // File buttons
    { MENU_BUTTON_FILE_A,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE, -6400,  2800 },
    { MENU_BUTTON_FILE_B,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE,  1500,  2800 },
    { MENU_BUTTON_FILE_C,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE, -6400,     0 },
    { MENU_BUTTON_FILE_D,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE,  1500,     0 },

    { MENU_BUTTON_SCORE,            MODEL_MAIN_MENU_GREEN_SCORE_BUTTON,    -6400, -3500 }, // Score menu button
    { MENU_BUTTON_COPY,             MODEL_MAIN_MENU_BLUE_COPY_BUTTON,      -2134, -3500 }, // Copy menu button
    { MENU_BUTTON_ERASE,            MODEL_MAIN_MENU_RED_ERASE_BUTTON,       2134, -3500 }, // Erase menu button
    { MENU_BUTTON_OPTIONS,          MODEL_MAIN_MENU_PURPLE_SOUND_BUTTON,    6400, -3500 }, // Sound mode menu button (Option Mode in EU)
    { MENU_BUTTON_NULL,             MODEL_NONE,                                0,     0 }, // Null terminator
};
static const struct MenuButton sScoreMenuButtonsList[] = {
    // File buttons
    { MENU_BUTTON_FILE_A,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,      -6400,  2800 },
    { MENU_BUTTON_FILE_B,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       1500,  2800 },
    { MENU_BUTTON_FILE_C,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,      -6400,     0 },
    { MENU_BUTTON_FILE_D,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       1500,     0 },

    { MENU_BUTTON_RETURN,           MODEL_MAIN_MENU_YELLOW_FILE_BUTTON,    -6400, -3500 }, // Return to main menu button
    { MENU_BUTTON_COPY,             MODEL_MAIN_MENU_BLUE_COPY_BUTTON,          0, -3500 }, // Switch to copy menu button
    { MENU_BUTTON_ERASE,            MODEL_MAIN_MENU_RED_ERASE_BUTTON,       6400, -3500 }, // Switch to erase menu button
    { MENU_BUTTON_NULL,             MODEL_NONE,                                0,     0 }, // Null terminator
};
static const struct MenuButton sCopyMenuButtonsList[] = {
    // File buttons
    { MENU_BUTTON_FILE_A,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,      -6400,  2800 },
    { MENU_BUTTON_FILE_B,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       1500,  2800 },
    { MENU_BUTTON_FILE_C,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,      -6400,     0 },
    { MENU_BUTTON_FILE_D,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       1500,     0 },

    { MENU_BUTTON_RETURN,           MODEL_MAIN_MENU_YELLOW_FILE_BUTTON,    -6400, -3500 }, // Return to main menu button
    { MENU_BUTTON_SCORE,            MODEL_MAIN_MENU_GREEN_SCORE_BUTTON,        0, -3500 }, // Switch to score menu button
    { MENU_BUTTON_ERASE,            MODEL_MAIN_MENU_RED_ERASE_BUTTON,       6400, -3500 }, // Switch to erase menu button
    { MENU_BUTTON_NULL,             MODEL_NONE,                                0,     0 }, // Null terminator
};
static const struct MenuButton sEraseMenuButtonsList[] = {
    // File buttons
    { MENU_BUTTON_FILE_A,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,      -6400,  2800 },
    { MENU_BUTTON_FILE_B,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       1500,  2800 },
    { MENU_BUTTON_FILE_C,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,      -6400,     0 },
    { MENU_BUTTON_FILE_D,           MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       1500,     0 },

    { MENU_BUTTON_RETURN,           MODEL_MAIN_MENU_YELLOW_FILE_BUTTON,    -6400, -3500 }, // Return to main menu button
    { MENU_BUTTON_SCORE,            MODEL_MAIN_MENU_GREEN_SCORE_BUTTON,        0, -3500 }, // Switch to score menu button
    { MENU_BUTTON_COPY,             MODEL_MAIN_MENU_BLUE_COPY_BUTTON,       6400, -3500 }, // Switch to erase menu button
    { MENU_BUTTON_NULL,             MODEL_NONE,                                0,     0 }, // Null terminator
};

static const struct MenuButton sOptionsMenuButtonsList[] = {
#if MULTILANG
    { MENU_BUTTON_STEREO,           MODEL_MAIN_MENU_GENERIC_BUTTON,        -4800,   388 }, // Stereo option button
    { MENU_BUTTON_MONO,             MODEL_MAIN_MENU_GENERIC_BUTTON,            0,   388 }, // Mono option button
    { MENU_BUTTON_HEADSET,          MODEL_MAIN_MENU_GENERIC_BUTTON,         4800,   388 }, // Headset option button

    { MENU_BUTTON_LANGUAGE_ENGLISH, MODEL_MAIN_MENU_GENERIC_BUTTON,        -4800, -1000 }, // English option button
    { MENU_BUTTON_LANGUAGE_FRENCH,  MODEL_MAIN_MENU_GENERIC_BUTTON,            0, -1000 }, // French option button
    { MENU_BUTTON_LANGUAGE_GERMAN,  MODEL_MAIN_MENU_GENERIC_BUTTON,         4800, -1000 }, // German option button

    { MENU_BUTTON_RETURN,           MODEL_MAIN_MENU_YELLOW_FILE_BUTTON,        0, -4800 }, // Return button
#else
    { MENU_BUTTON_STEREO,           MODEL_MAIN_MENU_GENERIC_BUTTON,        -4800,     0 }, // Stereo option button
    { MENU_BUTTON_MONO,             MODEL_MAIN_MENU_GENERIC_BUTTON,            0,     0 }, // Mono option button
    { MENU_BUTTON_HEADSET,          MODEL_MAIN_MENU_GENERIC_BUTTON,         4800,     0 }, // Headset option button
#endif
    { MENU_BUTTON_NULL,             MODEL_NONE,                                0,     0 }, // Null terminator
};

#undef SOUND_BUTTON_Y

static const s8 sButtonTypeDestPages[NUM_BUTTON_TYPES] = {
    // MENU_PAGE_NULL,    // MENU_BUTTON_NULL
    MENU_PAGE_NULL,    // MENU_BUTTON_FILE_A
    MENU_PAGE_NULL,    // MENU_BUTTON_FILE_B
    MENU_PAGE_NULL,    // MENU_BUTTON_FILE_C
    MENU_PAGE_NULL,    // MENU_BUTTON_FILE_D
    MENU_PAGE_SCORE,   // MENU_BUTTON_SCORE
    MENU_PAGE_COPY,    // MENU_BUTTON_COPY
    MENU_PAGE_ERASE,   // MENU_BUTTON_ERASE
    MENU_PAGE_OPTIONS, // MENU_BUTTON_OPTIONS
    MENU_PAGE_MAIN,    // MENU_BUTTON_RETURN
#if MULTILANG
    MENU_PAGE_NULL,    // MENU_BUTTON_STEREO
    MENU_PAGE_NULL,    // MENU_BUTTON_MONO
    MENU_PAGE_NULL,    // MENU_BUTTON_HEADSET
    MENU_PAGE_NULL,    // MENU_BUTTON_LANGUAGE_ENGLISH
    MENU_PAGE_NULL,    // MENU_BUTTON_LANGUAGE_FRENCH
    MENU_PAGE_NULL,    // MENU_BUTTON_LANGUAGE_GERMAN
#else
    MENU_PAGE_MAIN,    // MENU_BUTTON_STEREO
    MENU_PAGE_MAIN,    // MENU_BUTTON_MONO
    MENU_PAGE_MAIN,    // MENU_BUTTON_HEADSET
#endif
};

struct MenuPage {
    s32 id;
    // s16 menuLevel;
    // s16 parentPage;
    // s16 hasFiles;
    // const BehaviorScript *parentBehavior;
    const struct MenuButton *buttonSpawnData;
    struct Object *parentButton;
    struct Object *buttons[MAX_BUTTONS_PER_PAGE];
};

// Amount of main menu buttons defined in the code called by spawn_object_rel_with_rot.
// See file_select.h for the names in MenuButtonTypes.
struct MenuPage sMenuPages[] = {
    { MENU_PAGE_MAIN,    /*MENU_LAYER_MAIN,   */    sMainMenuButtonsList, NULL, { NULL } },
    { MENU_PAGE_SCORE,   /*MENU_LAYER_SUBMENU,*/   sScoreMenuButtonsList, NULL, { NULL } },
    { MENU_PAGE_COPY,    /*MENU_LAYER_SUBMENU,*/    sCopyMenuButtonsList, NULL, { NULL } },
    { MENU_PAGE_ERASE,   /*MENU_LAYER_SUBMENU,*/   sEraseMenuButtonsList, NULL, { NULL } },
    { MENU_PAGE_OPTIONS, /*MENU_LAYER_SUBMENU,*/ sOptionsMenuButtonsList, NULL, { NULL } },
};

struct Object *sMenuButtons[MAX_BUTTONS_PER_PAGE];

extern void *languageTable[][3];

#define CLICK_POS_NULL -10000

/**
 * @file file_select.c
 * This file implements how the file select and it's menus render and function.
 * That includes button IDs rendered as object models, strings, hand cursor,
 * special menu messages and phases, button states and button clicked checks.
 */

struct Object *sSelectedButton = NULL;

struct Object *sPageButton = NULL;

// Used to defined yes/no fade colors after a file is selected in the erase menu.
Color sYesColor;
Color sNoColor;

// Whether we are on the main menu or one of the submenus.
s8 sCurrentMenuLevel = MENU_LAYER_MAIN;

// Used for text opacifying. If it is below 250, it is constantly incremented.
Alpha sTextBaseAlpha = 0;

// 2D position of the cursor on the screen.
// sCursorPos[0]: X | sCursorPos[1]: Y
s16 sCursorPos[] = { 0, 0 };

// Determines which graphic to use for the cursor.
s16 sCursorClickingTimer = 0;

// Equal to sCursorPos if the cursor gets clicked, { CLICK_POS_NULL, CLICK_POS_NULL } otherwise.
s16 sClickPos[] = { CLICK_POS_NULL, CLICK_POS_NULL };

// Used for determining which file has been selected during copying and erasing.
s8 sSelectedFileIndex = -1;

// Whether to fade out text or not.
s8 sFadeOutText = FALSE;

// The message currently being displayed at the top of a menu.
s8 sStatusMessageID = 0;

// Used for text fading. The alpha value of text is calculated as
// sTextBaseAlpha - sTextFadeAlpha.
Alpha sTextFadeAlpha = 0;

// File select timer that keeps counting until it reaches 1000.
// Used to prevent buttons from being clickable as soon as a menu loads.
// Gets reset when you click an empty save, existing saves in copy and erase menus
// and when you click yes/no in the erase confirmation prompt.
s16 sMainMenuTimer = 0;

// Sound mode menu buttonID, has different values compared to gSoundMode in audio.
// 0: gSoundMode = 0 (Stereo) | 1: gSoundMode = 3 (Mono) | 2: gSoundMode = 1 (Headset)
s8 sSoundMode = SOUND_MODE_STEREO;

// Active language for EU arrays, values defined similar to sSoundMode
// 0: English | 1: French | 2: German

// Tracks which button will be pressed in the erase confirmation prompt (yes/no).
s8 sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;

// Used for the copy menu, defines if the game as all 4 save slots with data.
// if TRUE, it doesn't allow copying more files.
s8 sAllFilesExist = FALSE;

// Defines the value of the save slot selected in the menu.
// Mario A: 1 | Mario B: 2 | Mario C: 3 | Mario D: 4
s8 sSelectedFileNum = 0;

// Which coin score mode to use when scoring files. 0 for local
// coin high score, 1 for high score across all files.
s8 sScoreFileCoinScoreMode = 0;

// In EU, if no save file exists, open the language menu so the user can find it.

const unsigned char textReturn[] = { TEXT_RETURN };
const unsigned char textViewScore[] = { TEXT_CHECK_SCORE };
const unsigned char textCopyFileButton[] = { TEXT_COPY_FILE_BUTTON };
const unsigned char textEraseFileButton[] = { TEXT_ERASE_FILE_BUTTON };

const unsigned char textSoundModes[][8] = {
    { TEXT_STEREO  },
    { TEXT_MONO    },
    { TEXT_HEADSET },
};

#if MULTILANG
unsigned char textLanguageSelect[][17] = {
    { TEXT_LANGUAGE_SELECT }
};
#endif

const unsigned char textMarioA[] = { TEXT_FILE_MARIO_A };
const unsigned char textMarioB[] = { TEXT_FILE_MARIO_B };
const unsigned char textMarioC[] = { TEXT_FILE_MARIO_C };
const unsigned char textMarioD[] = { TEXT_FILE_MARIO_D };

const unsigned char *textMarioFiles[] = {
    textMarioA,
    textMarioB,
    textMarioC,
    textMarioD,
};

const unsigned char textScore[] = { TEXT_SCORE };
const unsigned char textCopy[]  = { TEXT_COPY  };
const unsigned char textErase[] = { TEXT_ERASE };

unsigned char textLanguage[][9] = {
    { TEXT_ENGLISH },
    { TEXT_FRENCH  },
    { TEXT_GERMAN  },
};

const unsigned char textNoSavedDataExists[] = { TEXT_NO_SAVED_DATA_EXISTS };
const unsigned char textSavedDataExists[] = { TEXT_SAVED_DATA_EXISTS };

// The scale difference between the main menu and the submenus.
#define MENU_SCALE 9.0f

/**
 * Yellow Background Menu Initial Action
 * Rotates the background at 180 grades and it's scale.
 * Although the scale is properly applied in the loop function.
 */
void beh_yellow_background_menu_init(void) {
    o->oFaceAngleYaw = 0x8000;
    o->oMenuButtonScale = MENU_SCALE;
}

/**
 * Yellow Background Menu Loop Action
 * Properly scales the background in the main menu.
 */
void beh_yellow_background_menu_loop(void) {
    cur_obj_scale(o->oMenuButtonScale);
}

#define CLICK_DEPTH_MAINMENU 200
#define CLICK_DEPTH_SUBMENU   22

/**
 * Check if a button was clicked.
 * depth = CLICK_DEPTH_MAINMENU for main menu, CLICK_DEPTH_SUBMENU for submenus.
 */
s32 check_clicked_button(s16 x, s16 y, f32 depth) {
    f32 a = 52.4213f; // sqrt(2748)?
    f32 newX = ((f32) x * 160.0f) / (a * depth);
    f32 newY = ((f32) y * 120.0f) / (a * 3 / 4 * depth);
    s16 maxX = newX + 25.0f;
    s16 minX = newX - 25.0f;
    s16 maxY = newY + 21.0f;
    s16 minY = newY - 21.0f;

    return ((sClickPos[0] < maxX)
         && (minX < sClickPos[0])
         && (sClickPos[1] < maxY)
         && (minY < sClickPos[1]));
}

s32 bhv_menu_button_timer(struct Object *button, s32 timer, s32 state) {
    button->oMenuButtonTimer++;

    if (button->oMenuButtonTimer == timer) {
        button->oMenuButtonState = state;
        button->oMenuButtonTimer = 0;

        return TRUE;
    }

    return FALSE;
}

void bhv_menu_button_growing(struct Object *button, f32 z, s32 timer) {
    s32 turnAmt = 0x8000 / timer;

    if (button->oMenuButtonTimer < timer) {
        button->oFaceAngleYaw += turnAmt;
        if (button->oMenuButtonTimer < (timer / 2)) {
            button->oFaceAnglePitch += turnAmt;
        } else {
            button->oFaceAnglePitch -= turnAmt;
        }
    }

    button->oParentRelativePosX -= button->oMenuButtonOrigPosX / (f32)timer;
    button->oParentRelativePosY -= button->oMenuButtonOrigPosY / (f32)timer;
    button->oParentRelativePosZ += z / (f32)timer;

    if (bhv_menu_button_timer(button, timer, MENU_BUTTON_STATE_FULLSCREEN)) {
        button->oParentRelativePosX = 0.0f;
        button->oParentRelativePosY = 0.0f;
    }
}

void bhv_menu_button_shrinking(struct Object *button, f32 z, s32 timer) {
    s32 turnAmt = 0x8000 / timer;

    if (button->oMenuButtonTimer < timer) {
        button->oFaceAngleYaw -= turnAmt;
        if (button->oMenuButtonTimer < (timer / 2)) {
            button->oFaceAnglePitch -= turnAmt;
        } else {
            button->oFaceAnglePitch += turnAmt;
        }
    }

    button->oParentRelativePosX += button->oMenuButtonOrigPosX / (f32)timer;
    button->oParentRelativePosY += button->oMenuButtonOrigPosY / (f32)timer;
    button->oParentRelativePosZ -= z / (f32)timer;

    if (bhv_menu_button_timer(button, timer, MENU_BUTTON_STATE_DEFAULT)) {
        button->oParentRelativePosX = button->oMenuButtonOrigPosX;
        button->oParentRelativePosY = button->oMenuButtonOrigPosY;
    }
}

/**
 * A small increase and decrease in size.
 * Used by failed copy/erase/score operations and sound mode select.
 */
void bhv_menu_button_move_in_out(struct Object *button, f32 size, s32 timer) { // by 160
    if (button->oMenuButtonTimer < (timer / 2)) {
        button->oParentRelativePosZ += size;
    } else {
        button->oParentRelativePosZ -= size;
    }
    bhv_menu_button_timer(button, timer, MENU_BUTTON_STATE_DEFAULT);
}

void bhv_menu_button_zoom(struct Object *button, f32 zoom, s32 timer) {
    button->oMenuButtonScale += zoom;
    bhv_menu_button_timer(button, timer, MENU_BUTTON_STATE_DEFAULT);
}

/**
 * Find a button of a specific type on the specified page.
 */
struct Object *get_button_of_type(s16 pageID, s16 type) {
    struct MenuPage *page = &sMenuPages[pageID];
    struct Object *buttonObj = NULL;

    for (s32 i = 0; i < MAX_BUTTONS_PER_PAGE; i++) {
        buttonObj = page->buttons[i];
        if (buttonObj == NULL) {
            break;
        }
        if (buttonObj->oMenuButtonType == type) {
            return buttonObj;
        }
    }

    return NULL;
}

#define MAIN_MENU_BUTTON_END_Z 17800
#define SUB_MENU_BUTTON_END_Z  -1860

/**
 * Menu Buttons Menu Initial Action
 * Aligns menu buttons so they can stay in their original
 * positions when you choose a button.
 */
void bhv_menu_button_init(void) {
    vec3f_copy(&o->oMenuButtonOrigPosVec, &o->oParentRelativePosVec);
}

/**
 * Menu Buttons Menu Loop Action
 * Handles the functions of the button states and
 * object scale for each button.
 */
void bhv_menu_button_loop(void) {
    switch (o->oMenuButtonState) {
        case MENU_BUTTON_STATE_DEFAULT: // Button state
            o->oMenuButtonOrigPosZ = o->oPosZ;
            break;

        case MENU_BUTTON_STATE_GROWING: // Switching from button to menu state
            bhv_menu_button_growing(o, (17800 - (19660 * sCurrentMenuLevel)), 16);
            // if (sCurrentMenuLevel == MENU_LAYER_MAIN) {
            //     // Grow from main menu, used by selecting files and menus.
            //     bhv_menu_button_growing(o, MAIN_MENU_BUTTON_END_Z, 16);
            // }
            // if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
            //     // Grow from submenu, used by selecting a file in the score menu.
            //     bhv_menu_button_growing(o, SUB_MENU_BUTTON_END_Z, 16); // Only used for score files
            // }
            sTextBaseAlpha = 0;
            sCursorClickingTimer = 4;
            break;

        case MENU_BUTTON_STATE_FULLSCREEN: // Menu state
            break;

        case MENU_BUTTON_STATE_SHRINKING: // Switching from menu to button state
            bhv_menu_button_shrinking(o, (17800 - (19660 * sCurrentMenuLevel)), 16);
            // if (sCurrentMenuLevel == MENU_LAYER_MAIN) {
            //     // Shrink back to main menu, used to return back while inside menus.
            //     bhv_menu_button_shrinking(o, MAIN_MENU_BUTTON_END_Z, 16);
            // }
            // if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
            //     // Shrink back to submenu, used to return back while inside a score save menu.
            //     bhv_menu_button_shrinking(o, SUB_MENU_BUTTON_END_Z, 16); // Only used for score files
            // }
            sTextBaseAlpha = 0;
            sCursorClickingTimer = 4;
            break;

        case MENU_BUTTON_STATE_ZOOM_IN_OUT:
            if (sCurrentMenuLevel == MENU_LAYER_MAIN) {
                bhv_menu_button_move_in_out(o, -20.0f, 8);
            }
            if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
                bhv_menu_button_move_in_out(o,  20.0f, 8);
            }
            sCursorClickingTimer = 4;
            break;

        case MENU_BUTTON_STATE_ZOOM_IN:
            // A small temporary increase in size.
            // Used while selecting a target copy/erase file or yes/no erase confirmation prompt.
            bhv_menu_button_zoom(o,  0.0022f, 10);
            sCursorClickingTimer = 4;
            break;

        case MENU_BUTTON_STATE_ZOOM_OUT:
            // A small temporary decrease in size.
            // Used after selecting a target copy/erase file or yes/no erase confirmation prompt to undo the zoom in.
            bhv_menu_button_zoom(o, -0.0022f, 10);
            sCursorClickingTimer = 4;
            break;
    }

    cur_obj_scale(o->oMenuButtonScale);
}

/**
 * Handles how to exit the score file menu using button states.
 */
void exit_score_file_to_score_menu(struct Object *scoreFileButton) {
    // Begin exit
    if (scoreFileButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN && sCursorClickingTimer == 2) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        scoreFileButton->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
    }
    // End exit
    if (scoreFileButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT) {
        sSelectedButton = sSelectedButton->parentObj;
        if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
            sCurrentMenuLevel = MENU_LAYER_MAIN;
        }
    }
}

void spawn_menu_buttons(s16 pageID, struct Object *parent) {
    struct MenuPage *page = &sMenuPages[pageID];
    const struct MenuButton *spawnDataPtr = page->buttonSpawnData;
    struct Object **buttonObjPtr = page->buttons;
    struct Object *buttonObj = NULL;
    f32 z = 0;
    f32 scaleX = 1.0f;
    f32 scaleY = 1.0f;
    s16 yaw = 0x0;

    if (parent->parentObj != NULL && obj_has_behavior(parent, bhvMenuButton)) {
        z = -100;
        scaleX = -(1.0f / MENU_SCALE);
        scaleY =  (1.0f / MENU_SCALE);
        yaw = -0x8000;
    }

    page->parentButton = parent;

    s16 id;
    ModelID16 model;
    for (s32 i = 0; i < MAX_BUTTONS_PER_PAGE; i++) {
        id = spawnDataPtr->id;
        if (id == MENU_BUTTON_NULL) {
            break;
        }
        model = spawnDataPtr->model;
        // The first four indices here are always the file buttons.
        if (i < NUM_SAVE_FILES && save_file_exists(i)) {
            switch (model) {
                case MODEL_MAIN_MENU_MARIO_NEW_BUTTON:      model = MODEL_MAIN_MENU_MARIO_SAVE_BUTTON;      break;
                case MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE: model = MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE; break;
            }
        }

        buttonObj = spawn_object_rel_with_rot(parent, model, bhvMenuButton, (spawnDataPtr->x * scaleX), (spawnDataPtr->y * scaleY), z, 0x0, yaw, 0x0);
        buttonObj->oMenuButtonScale = scaleY;
        buttonObj->oMenuButtonType = id;

        *buttonObjPtr++ = buttonObj;
        spawnDataPtr++;
    }
}

#define SCORE_TIMER 30
/**
 * In the score menu, checks if a button was clicked to play a sound, button state and other functions.
 */
void check_score_menu_clicked_buttons(struct Object *scoreButton) {
    struct Object *buttonObj = NULL;

    if (scoreButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        // Configure score menu button group
        for (s32 buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
            buttonObj = sMenuPages[MENU_PAGE_SCORE].buttons[buttonID];
            if (buttonObj == NULL) {
                break;
            }

            s32 buttonType = buttonObj->oMenuButtonType;

            if (check_clicked_button(buttonObj->oPosX, buttonObj->oPosY, CLICK_DEPTH_SUBMENU) && sMainMenuTimer > SCORE_TIMER) {
                // If menu button clicked, select it
                if (buttonType == MENU_BUTTON_RETURN
                 || buttonType == MENU_BUTTON_COPY
                 || buttonType == MENU_BUTTON_ERASE) {
                    play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                    queue_rumble_data(5, 80);
#endif
                    buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                    sSelectedButton = buttonObj;
                } else if (buttonType < NUM_SAVE_FILES) { // Check if a save file is clicked
                    if (sMainMenuTimer > SCORE_TIMER) {
                        // If clicked in a existing save file, select it too see it's score
                        if (save_file_exists(buttonType)) {
                            play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
                            queue_rumble_data(5, 80);
#endif
                            buttonObj->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
                            sSelectedButton = buttonObj;
                        } else {
                            // If clicked in a non-existing save file, play buzz sound
                            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                            queue_rumble_data(5, 80);
#endif
                            buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                            if (sMainMenuTimer > SCORE_TIMER) {
                                sFadeOutText = TRUE;
                                sMainMenuTimer = 0;
                            }
                        }
                    }
                }
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;
                break;
            }
        }
    }
}

#undef SCORE_TIMER

#define BUZZ_TIMER 20

/**
 * Copy Menu phase actions that handles what to do when a file button is clicked.
 */
void copy_action_file_button(struct Object *copyButton, s32 copyFileButtonID) {
    struct MenuPage *copyPage = &sMenuPages[MENU_PAGE_COPY];
    struct Object *copyFileButton = copyPage->buttons[copyFileButtonID];
    struct Object *mainFileButton = copyPage->buttons[copyFileButtonID];

    switch (copyButton->oMenuButtonActionPhase) {
        case COPY_PHASE_MAIN: // Copy Menu Main Phase
            if (sAllFilesExist) { // Don't enable copy if all save files exists
                return;
            }
            if (save_file_exists(copyFileButtonID)) {
                // If clicked in a existing save file, ask where it wants to copy
                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                copyFileButton->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
                sSelectedFileIndex = copyFileButtonID;
                copyButton->oMenuButtonActionPhase = COPY_PHASE_COPY_WHERE;
                sFadeOutText = TRUE;
                sMainMenuTimer = 0;
            } else {
                // If clicked in a non-existing save file, play buzz sound
                play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                copyFileButton->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                if (sMainMenuTimer > BUZZ_TIMER) {
                    sFadeOutText = TRUE;
                    sMainMenuTimer = 0;
                }
            }
            break;
        case COPY_PHASE_COPY_WHERE: // Copy Menu "COPY IT TO WHERE?" Phase (after a file is selected)
            copyFileButton->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
            if (!save_file_exists(copyFileButtonID)) {
                // If clicked in a non-existing save file, copy the file
                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                copyButton->oMenuButtonActionPhase = COPY_PHASE_COPY_COMPLETE;
                sFadeOutText = TRUE;
                sMainMenuTimer = 0;
                save_file_copy(sSelectedFileIndex, copyFileButtonID);
                copyFileButton->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE];
                mainFileButton->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE];
            } else {
                // If clicked in a existing save file, play buzz sound
                if (sSelectedFileIndex == copyFileButtonID) {
                    play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                    queue_rumble_data(5, 80);
#endif
                    copyPage->buttons[sSelectedFileIndex]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_OUT;
                    copyButton->oMenuButtonActionPhase = COPY_PHASE_MAIN;
                    sFadeOutText = TRUE;
                    return;
                }
                if (sMainMenuTimer > BUZZ_TIMER) {
                    sFadeOutText = TRUE;
                    sMainMenuTimer = 0;
                }
            }
            break;
    }
}

#define ACTION_TIMER 30

/**
 * In the copy menu, checks if a button was clicked to play a sound, button state and other functions.
 */
void check_copy_menu_clicked_buttons(struct Object *copyButton) {
    struct Object *buttonObj = NULL;

    if (copyButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        // Configure copy menu button group
        for (s32 buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
            buttonObj = sMenuPages[MENU_PAGE_COPY].buttons[buttonID];
            if (buttonObj == NULL) {
                break;
            }

            s32 buttonType = buttonObj->oMenuButtonType;

            if (check_clicked_button(buttonObj->oPosX, buttonObj->oPosY, CLICK_DEPTH_SUBMENU)) {
                // If menu button clicked, select it
                if (buttonType == MENU_BUTTON_RETURN
                 || buttonType == MENU_BUTTON_SCORE
                 || buttonType == MENU_BUTTON_ERASE) {
                    if (copyButton->oMenuButtonActionPhase == COPY_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                        queue_rumble_data(5, 80);
#endif
                        buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                        sSelectedButton = buttonObj;
                    }
                } else if (buttonType < NUM_SAVE_FILES) {
                    // Check if a file button is clicked to play a copy action
                    if (buttonObj->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT && sMainMenuTimer >= ACTION_TIMER) {
                        copy_action_file_button(copyButton, buttonID);
                    }
                }
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;
                break;
            }
        }

        // After copy is complete, return to main copy phase
        if (copyButton->oMenuButtonActionPhase == COPY_PHASE_COPY_COMPLETE && sMainMenuTimer > ACTION_TIMER) {
            copyButton->oMenuButtonActionPhase = COPY_PHASE_MAIN;
            sMenuPages[MENU_PAGE_COPY].buttons[sSelectedFileIndex]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_OUT;
        }
    }
}

/**
 * Erase Menu phase actions that handles what to do when a file button is clicked.
 */
void erase_action_file_button(struct Object *eraseButton, s32 eraseFileButtonID) {
    struct Object *eraseFileButton = sMenuPages[MENU_PAGE_ERASE].buttons[eraseFileButtonID];

    switch (eraseButton->oMenuButtonActionPhase) {
        case ERASE_PHASE_MAIN: // Erase Menu Main Phase
            if (save_file_exists(eraseFileButtonID)) {
                // If clicked in a existing save file, ask if it wants to delete it
                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                eraseFileButton->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
                sSelectedFileIndex = eraseFileButtonID;
                eraseButton->oMenuButtonActionPhase = ERASE_PHASE_PROMPT;
                sFadeOutText = TRUE;
                sMainMenuTimer = 0;
            } else {
                // If clicked in a non-existing save file, play buzz sound
                play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                eraseFileButton->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                if (sMainMenuTimer > BUZZ_TIMER) {
                    sFadeOutText = TRUE;
                    sMainMenuTimer = 0;
                }
            }
            break;
        case ERASE_PHASE_PROMPT: // Erase Menu "SURE? YES NO" Phase (after a file is selected)
            if (sSelectedFileIndex == eraseFileButtonID) {
                // If clicked in a existing save file, play click sound and zoom out button
                // Note: The prompt functions are actually called when the ERASE_MSG_PROMPT
                // message is displayed with print_erase_menu_prompt
                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                sMenuPages[MENU_PAGE_ERASE].buttons[sSelectedFileIndex]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_OUT;
                eraseButton->oMenuButtonActionPhase = ERASE_PHASE_MAIN;
                sFadeOutText = TRUE;
            }
            break;
    }
}

#undef BUZZ_TIMER

/**
 * In the erase menu, checks if a button was clicked to play a sound, button state and other functions.
 */
void check_erase_menu_clicked_buttons(struct Object *eraseButton) {
    struct Object *buttonObj = NULL;

    if (eraseButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        // Configure erase menu button group
        for (s32 buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
            buttonObj = sMenuPages[MENU_PAGE_ERASE].buttons[buttonID];
            if (buttonObj == NULL) {
                break;
            }

            s32 buttonType = buttonObj->oMenuButtonType;

            if (check_clicked_button(buttonObj->oPosX, buttonObj->oPosY, CLICK_DEPTH_SUBMENU)) {
                // If menu button clicked, select it
                if (buttonType == MENU_BUTTON_RETURN
                 || buttonType == MENU_BUTTON_SCORE
                 || buttonType == MENU_BUTTON_COPY) {
                    if (eraseButton->oMenuButtonActionPhase == ERASE_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                        queue_rumble_data(5, 80);
#endif
                        buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                        sSelectedButton = buttonObj;
                    }
                } else if (buttonType < NUM_SAVE_FILES) {
                    // Check if a file button is clicked to play an erase action
                    if (sMainMenuTimer >= ACTION_TIMER) {
                        erase_action_file_button(eraseButton, buttonID);
                    }
                }
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;
                break;
            }
        }

        // After erase is complete, return to main erase phase
        if (eraseButton->oMenuButtonActionPhase == ERASE_PHASE_MARIO_ERASED
         && sMainMenuTimer > ACTION_TIMER) {
            eraseButton->oMenuButtonActionPhase = ERASE_PHASE_MAIN;
            sMenuPages[MENU_PAGE_ERASE].buttons[sSelectedFileIndex]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_OUT;
        }
    }
}

#undef ACTION_TIMER

/**
 * In the sound mode menu, checks if a button was clicked to change sound mode & button state.
 */
void check_sound_mode_menu_clicked_buttons(struct Object *soundModeButton) {
    struct Object *buttonObj = NULL;

    if (soundModeButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        // Configure sound mode menu button group
        for (s32 buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
            buttonObj = sMenuPages[MENU_PAGE_OPTIONS].buttons[buttonID];
            if (buttonObj == NULL) {
                break;
            }

            s32 buttonType = buttonObj->oMenuButtonType;

            if (check_clicked_button(buttonObj->oPosX, buttonObj->oPosY, CLICK_DEPTH_SUBMENU)) {
                // If sound mode button clicked, select it and define sound mode
                // The check will always be true because of the group configured above (In JP & US)
                if (buttonType == MENU_BUTTON_STEREO
                 || buttonType == MENU_BUTTON_MONO
                 || buttonType == MENU_BUTTON_HEADSET) {
                    if (soundModeButton->oMenuButtonActionPhase == SOUND_MODE_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                        queue_rumble_data(5, 80);
#endif
                        buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
#if !MULTILANG
                        // Sound menu buttons don't return to Main Menu in EU
                        // because they don't have a case in bhv_menu_button_manager_loop
                        sSelectedButton = buttonObj;
#endif
                        sSoundMode = (buttonType - MENU_BUTTON_STEREO);
                        save_file_set_sound_mode(sSoundMode);
                    }
                }
#if MULTILANG
                // If language mode button clicked, select it and change language
                if (buttonType == MENU_BUTTON_LANGUAGE_ENGLISH
                 || buttonType == MENU_BUTTON_LANGUAGE_FRENCH
                 || buttonType == MENU_BUTTON_LANGUAGE_GERMAN) {
                    if (soundModeButton->oMenuButtonActionPhase == SOUND_MODE_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                        buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                        sLanguageMode = (buttonType - MENU_BUTTON_LANGUAGE_MIN);
                        eu_set_language(sLanguageMode);
                    }
                }
                // If neither of the buttons above are pressed, return to main menu
                if (buttonType == MENU_BUTTON_RETURN) {
                    play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                    buttonObj->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                    sSelectedButton = buttonObj;
                }
#endif
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;

                break;
            }
        }
    }
}

/**
 * Loads a save file selected after it goes into a full screen state
 * retuning sSelectedFileNum to a save value defined in fileNum.
 */
void load_main_menu_save_file(struct Object *fileButton, s32 fileNum) {
    if (fileButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        sSelectedFileNum = fileNum;
    }
}


void delete_menu_buttons(s16 pageID) {
    struct MenuPage *page = &sMenuPages[pageID];
    struct Object *buttonObj = NULL;
    s32 buttonID;

    for (buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
        buttonObj = page->buttons[buttonID];
        if (buttonObj == NULL) {
            break;
        }

        obj_mark_for_deletion(buttonObj);
    }
}

void delete_sub_menu_buttons(struct Object *pageButton) {
    struct Object *buttonObj = NULL;
    s32 buttonID;

    for (buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
        buttonObj = sMenuButtons[buttonID];
        if (buttonObj != NULL && buttonObj->parentObj == pageButton) {
            obj_mark_for_deletion(buttonObj);
        }
    }
}

/**
 * Initializes the page shrinking and plays a sound.
 * Returns true when done shrinking.
 */
s32 shrink_page(struct Object *prevButton) {
    // If the source button is in default state and the previous menu in full screen,
    // play zoom out sound and shrink previous menu
    if ((sSelectedButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT)
     && (prevButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN)) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
        prevButton->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
        sCurrentMenuLevel = MENU_LAYER_MAIN;
    }

    return (prevButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT);
}

/**
 * Returns from the previous menu back to the main menu using
 * the return button (or sound mode) as source button.
 */
void return_to_main_menu(void) {
    struct Object *prevSelectedButton = sSelectedButton->parentObj;
    s32 prevButtonType = prevSelectedButton->oMenuButtonType;
    s32 prevPage = sButtonTypeDestPages[prevButtonType];

    // If the previous button is in default state, return back to the main menu
    if (shrink_page(prevSelectedButton)) {
        sSelectedButton = NULL;
        // Hide buttons of corresponding button menu groups
        delete_menu_buttons(prevPage);
    }
}

// void open_submenu(struct Object *buttonObj) {
//     s32 selectedButtonType = sSelectedButton ? sSelectedButton->oMenuButtonType : MENU_BUTTON_NULL;

//     buttonObj->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
//     sSelectedButton = buttonObj;

//     spawn_menu_buttons(sButtonTypeDestPages[selectedButtonType], sSelectedButton);

//     if (selectedButtonType == MENU_BUTTON_OPTIONS) {
//         // Zoom in current selection
//         get_button_of_type(MENU_PAGE_OPTIONS, (MENU_BUTTON_STEREO + sSoundMode))->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
//     }
// }

void load_menu_from_submenu(struct Object *selectedButton) {
    struct Object *prevSelectedButton = selectedButton->parentObj;
    s32 prevButtonType = prevSelectedButton->oMenuButtonType;
    s32 nextButtonType = selectedButton->oMenuButtonType;
    s32 prevPage = sButtonTypeDestPages[prevButtonType];
    s32 nextPage = sButtonTypeDestPages[nextButtonType];

    // If the previous button is in default state
    if (shrink_page(prevSelectedButton)) {
        if (prevButtonType != nextButtonType) {
            delete_menu_buttons(prevPage);
        }
        // Play zoom in sound, select score menu and render its buttons
        selectedButton = get_button_of_type(MENU_PAGE_MAIN, nextButtonType);
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
        selectedButton->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
        spawn_menu_buttons(nextPage, selectedButton);
    }
}

/**
 * Menu Buttons Menu Manager Initial Action
 * Creates models of the buttons in the menu. For the Mario buttons it
 * checks if a save file exists to render an specific button model for it.
 * Unlike buttons on submenus, these are never hidden or recreated.
 */
void bhv_menu_button_manager_init(void) {
    spawn_menu_buttons(MENU_PAGE_MAIN, o);

    sTextBaseAlpha = 0;
}

// s32 is_in_submenu(struct Object *selectedButton) {
//     if (selectedButton != NULL) {
//         struct Object *parentObj = selectedButton->parentObj;
//         if (parentObj != NULL && obj_has_behavior(parentObj, bhvMenuButton)) {
//             return TRUE;
//         }
//     }

//     return FALSE;
// }

void clicked_menu_button(struct Object *buttonObj) {
    struct Object *parentObj = NULL;
    s32 selectedButtonType = MENU_BUTTON_NULL;
    s32 parentButtonType = MENU_BUTTON_NULL;
    // s32 isInSubMenu = FALSE;
    if (buttonObj != NULL) {
        selectedButtonType = buttonObj->oMenuButtonType;
        parentObj = buttonObj->parentObj;
        if (parentObj != NULL && obj_has_behavior(parentObj, bhvMenuButton)) {
            parentButtonType = parentObj->oMenuButtonType;
            // isInSubMenu = TRUE;
        }
    }

    if (selectedButtonType == MENU_BUTTON_NULL) {
        return;
    }

    s32 nextPage = sButtonTypeDestPages[selectedButtonType];

    if (parentButtonType != MENU_BUTTON_NULL) {
        // In sub menu
        if (shrink_page(parentObj)) {
            s32 prevPage = sButtonTypeDestPages[parentButtonType];
            delete_menu_buttons(prevPage);
            if (nextPage != MENU_PAGE_MAIN) {
                // Find next button
                sSelectedButton = get_button_of_type(MENU_PAGE_MAIN, selectedButtonType);
                play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
                sSelectedButton->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
                spawn_menu_buttons(nextPage, sSelectedButton);
            } else {
                sSelectedButton = NULL;
            }
        }
    } else {
        // Main menu
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        spawn_menu_buttons(nextPage, sSelectedButton);

        if (selectedButtonType == MENU_BUTTON_OPTIONS) {
            // Zoom in current selection
            get_button_of_type(MENU_PAGE_OPTIONS, (MENU_BUTTON_STEREO + sSoundMode))->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
        }
        switch (selectedButtonType) {
            case MENU_BUTTON_SCORE:
                check_score_menu_clicked_buttons(sSelectedButton);
                break;
            case MENU_BUTTON_COPY:
                check_copy_menu_clicked_buttons(sSelectedButton);
                break;
            case MENU_BUTTON_ERASE:
                check_erase_menu_clicked_buttons(sSelectedButton);
                break;
            case MENU_BUTTON_OPTIONS:
                check_sound_mode_menu_clicked_buttons(sSelectedButton);
                break;
        }
    }
}

/**
 * In the main menu, check if a button was clicked to play it's button growing state.
 * Also play a sound and/or render buttons depending of the button ID selected.
 */
void check_main_menu_clicked_buttons(void) {
    struct Object *buttonObj = NULL;
    s8 buttonID;

    // Configure Main Menu button group
    for (buttonID = 0; buttonID < MAX_BUTTONS_PER_PAGE; buttonID++) {
        buttonObj = sMenuPages[MENU_PAGE_MAIN].buttons[buttonID];
        if (buttonObj == NULL) {
            break;
        }

        if (check_clicked_button(buttonObj->oPosX, buttonObj->oPosY, CLICK_DEPTH_MAINMENU)) {
            // If menu button clicked, select it
            buttonObj->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
            sSelectedButton = buttonObj;
            break;
        }
    }

    s32 selectedButtonType = sSelectedButton ? sSelectedButton->oMenuButtonType : MENU_BUTTON_NULL;

    // Play sound of the save file clicked
    switch (selectedButtonType) {
        case MENU_BUTTON_FILE_A:
        case MENU_BUTTON_FILE_B:
        case MENU_BUTTON_FILE_C:
        case MENU_BUTTON_FILE_D:
            play_sound(SOUND_MENU_STAR_SOUND_OKEY_DOKEY, gGlobalSoundSource);
#if ENABLE_RUMBLE
            queue_rumble_data(60, 70);
            queue_rumble_decay(1);
#endif
            break;

        // Play sound of the button clicked and render buttons of that menu.
        case MENU_BUTTON_SCORE:
        case MENU_BUTTON_COPY:
        case MENU_BUTTON_ERASE:
        case MENU_BUTTON_OPTIONS:
            clicked_menu_button(sSelectedButton);
//             play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
// #if ENABLE_RUMBLE
//             queue_rumble_data(5, 80);
// #endif
//             spawn_menu_buttons(sButtonTypeDestPages[selectedButtonType], sSelectedButton);

//             if (selectedButtonType == MENU_BUTTON_OPTIONS) {
//                 // Zoom in current selection
//                 get_button_of_type(MENU_PAGE_OPTIONS, (MENU_BUTTON_STEREO + sSoundMode))->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
//             }
            break;
    }
}

/**
 * Menu Buttons Menu Manager Loop Action
 * Calls a menu function depending of the button chosen.
 * sSelectedButtonID is MENU_BUTTON_NULL when the file select
 * is loaded, and that checks what buttonID is clicked in the main menu.
 */
void bhv_menu_button_manager_loop(void) {
    struct Object *parentObj = NULL;
    s32 selectedButtonType = MENU_BUTTON_NULL;
    s32 parentButtonType = MENU_BUTTON_NULL;
    if (sSelectedButton != NULL) {
        selectedButtonType = sSelectedButton->oMenuButtonType;
        parentObj = sSelectedButton->parentObj;
        if (parentObj != NULL && obj_has_behavior(parentObj, bhvMenuButton)) {
            parentButtonType = parentObj->oMenuButtonType;
        }
    }

    switch (selectedButtonType) {
        case MENU_BUTTON_FILE_A:
        case MENU_BUTTON_FILE_B:
        case MENU_BUTTON_FILE_C:
        case MENU_BUTTON_FILE_D:
            switch (parentButtonType) {
                case MENU_PAGE_NULL:
                    load_main_menu_save_file(sSelectedButton, SAVE_INDEX_TO_NUM(selectedButtonType));
                    break;
                case MENU_BUTTON_SCORE:
                    exit_score_file_to_score_menu(sSelectedButton);
                    break;
            }
            break;

        case MENU_BUTTON_NULL:
            check_main_menu_clicked_buttons();
            break;

        case MENU_BUTTON_RETURN:
        case MENU_BUTTON_SCORE:
        case MENU_BUTTON_COPY:
        case MENU_BUTTON_ERASE:
        case MENU_BUTTON_OPTIONS:
            clicked_menu_button(sSelectedButton);
            break;
        // case MENU_BUTTON_RETURN:
        //     return_to_main_menu();
        //     break;

        // case MENU_BUTTON_SCORE:
        //     if (parentButtonType != MENU_BUTTON_NULL) {
        //         load_menu_from_submenu(sSelectedButton);
        //     } else {
        //         check_score_menu_clicked_buttons(sSelectedButton);
        //     }
        //     break;
        // case MENU_BUTTON_COPY:
        //     if (parentButtonType != MENU_BUTTON_NULL) {
        //         load_menu_from_submenu(sSelectedButton);
        //     } else {
        //         check_copy_menu_clicked_buttons(sSelectedButton);
        //     }
        //     break;
        // case MENU_BUTTON_ERASE:
        //     if (parentButtonType != MENU_BUTTON_NULL) {
        //         load_menu_from_submenu(sSelectedButton);
        //     } else {
        //         check_erase_menu_clicked_buttons(sSelectedButton);
        //     }
        //     break;
        // case MENU_BUTTON_OPTIONS:
        //     if (parentButtonType != MENU_BUTTON_NULL) {
        //         load_menu_from_submenu(sSelectedButton);
        //     } else {
        //         check_sound_mode_menu_clicked_buttons(sSelectedButton);
        //     }
        //     break;
        // STEREO, MONO and HEADSET buttons are undefined so they can be selected without
        // exiting the Options menu, as a result they added a return button
        case MENU_BUTTON_STEREO:
        case MENU_BUTTON_MONO:
        case MENU_BUTTON_HEADSET:
#if !MULTILANG
            return_to_main_menu();
#endif
            break;
    }

    sClickPos[0] = CLICK_POS_NULL;
    sClickPos[1] = CLICK_POS_NULL;
}

/**
 * Cursor function that handles button inputs.
 * If the cursor is clicked, sClickPos uses the same value as sCursorPos.
 */
void handle_cursor_button_input(void) {
    // s32 selectedButtonType = sSelectedButton ? sSelectedButton->oMenuButtonType : MENU_BUTTON_NULL;

    struct Object *parentObj = NULL;
    s32 selectedButtonType = MENU_BUTTON_NULL;
    s32 parentButtonType = MENU_BUTTON_NULL;
    if (sSelectedButton != NULL) {
        selectedButtonType = sSelectedButton->oMenuButtonType;
        parentObj = sSelectedButton->parentObj;
        if (parentObj != NULL && obj_has_behavior(parentObj, bhvMenuButton)) {
            parentButtonType = parentObj->oMenuButtonType;
        }
    }

    // If scoring a file, pressing A just changes the coin score mode.
    if (parentButtonType == MENU_BUTTON_SCORE
     && (selectedButtonType == MENU_BUTTON_FILE_A
      || selectedButtonType == MENU_BUTTON_FILE_B
      || selectedButtonType == MENU_BUTTON_FILE_C
      || selectedButtonType == MENU_BUTTON_FILE_D)) {
        if (gPlayer1Controller->buttonPressed & (B_BUTTON | START_BUTTON | Z_TRIG)) {
            sClickPos[0] = sCursorPos[0];
            sClickPos[1] = sCursorPos[1];
            sCursorClickingTimer = 1;
        } else if (gPlayer1Controller->buttonPressed & A_BUTTON) {
            sScoreFileCoinScoreMode = (1 - sScoreFileCoinScoreMode);
            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
        }
    } else { // If cursor is clicked
        if (gPlayer1Controller->buttonPressed & (A_BUTTON | B_BUTTON | START_BUTTON)) {
            sClickPos[0] = sCursorPos[0];
            sClickPos[1] = sCursorPos[1];
            sCursorClickingTimer = 1;
        }
    }
}

/**
 * Cursor function that handles analog stick input and button presses with a function near the end.
 */
void handle_controller_cursor_input(void) {
    s16 rawStickX = gPlayer1Controller->rawStickX;
    s16 rawStickY = gPlayer1Controller->rawStickY;

    // Handle deadzone
    if (rawStickY > -2 && rawStickY < 2) {
        rawStickY = 0;
    }
    if (rawStickX > -2 && rawStickX < 2) {
        rawStickX = 0;
    }

    // Move cursor
    sCursorPos[0] += rawStickX / 8;
    sCursorPos[1] += rawStickY / 8;

    // Stop cursor from going offscreen
    sCursorPos[0] = CLAMP(sCursorPos[0], -132, 132);
    sCursorPos[1] = CLAMP(sCursorPos[1],  -90,  90);

    if (sCursorClickingTimer == 0) {
        handle_cursor_button_input();
    }
}

/**
 * Prints the cursor (Mario Hand, different to the one in the Mario screen)
 * and loads it's controller inputs in handle_controller_cursor_input
 * to be usable on the file select.
 */
void print_menu_cursor(void) {
    handle_controller_cursor_input();
    create_dl_translation_matrix(
        MENU_MTX_PUSH,
        ((sCursorPos[0] + 160) -  5),
        ((sCursorPos[1] + 120) - 25),
        0.0f
    );
    Gfx *dlHead = gDisplayListHead;
    // Get the right graphic to use for the cursor.
    if (sCursorClickingTimer == 0) {
        // Idle
        gSPDisplayList(dlHead++, dl_menu_idle_hand);
    }
    if (sCursorClickingTimer != 0) {
        // Grabbing
        gSPDisplayList(dlHead++, dl_menu_grabbing_hand);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }
    if (sCursorClickingTimer != 0) {
        sCursorClickingTimer++; // This is a very strange way to implement a timer? It counts up and
                                // then resets to 0 instead of just counting down to 0.
        if (sCursorClickingTimer == 5) {
            sCursorClickingTimer = 0;
        }
    }
    gDisplayListHead = dlHead;
}

/**
 * Prints a hud string depending of the hud table list defined with text fade properties.
 */
void print_hud_lut_string_fade(s8 hudLUT, s16 x, s16 y, const unsigned char *text) {
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, (sTextBaseAlpha - sTextFadeAlpha));
    print_hud_lut_string(hudLUT, x, y, text);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

/**
 * Prints a generic white string with text fade properties.
 */
void print_generic_string_fade(s16 x, s16 y, const unsigned char *text) {
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, (sTextBaseAlpha - sTextFadeAlpha));
    print_generic_string(x, y, text);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

/**
 * Updates text fade at the top of a menu.
 */
s32 update_text_fade_out(void) {
    if (sFadeOutText) {
        sTextFadeAlpha += 50;
        if (sTextFadeAlpha == 250) {
            sFadeOutText = FALSE;

            return TRUE;
        }
    } else {
        if (sTextFadeAlpha > 0) {
            sTextFadeAlpha -= 50;
        }
    }

    return FALSE;
}

/**
 * Prints the amount of stars of a save file.
 * If a save doesn't exist, print "NEW" instead.
 */
void print_save_file_star_count(s8 fileIndex, s16 x, s16 y) {
    const unsigned char textNew[] = { TEXT_NEW };
    const unsigned char starIcon[] = { GLYPH_STAR, GLYPH_SPACE };
    const unsigned char xIcon[] = { GLYPH_MULTIPLY, GLYPH_SPACE };
    unsigned char starCountText[4];
    s8 offset = 0;

    if (save_file_exists(fileIndex)) {
        s16 starCount = save_file_get_total_star_count(
            fileIndex,
            COURSE_NUM_TO_INDEX(COURSE_MIN),
            COURSE_NUM_TO_INDEX(COURSE_MAX)
        );
        // Print star icon
        print_hud_lut_string(HUD_LUT_GLOBAL, x, y, starIcon);
        // If star count is less than 100, print x icon and move
        // the star count text one digit to the right.
        if (starCount < 100) {
            print_hud_lut_string(HUD_LUT_GLOBAL, (x + 16), y, xIcon);
            offset = 16;
        }
        // Print star count
        int_to_str(starCount, starCountText);
        print_hud_lut_string(HUD_LUT_GLOBAL, (x + offset + 16), y, starCountText);
    } else {
        // Print "new" text
        print_hud_lut_string(HUD_LUT_GLOBAL, x, y, LANGUAGE_ARRAY(textNew));
    }
}

#define SELECT_FILE_X 93
#define SELECT_FILE_Y 35

#define SCORE_X       52
#define COPY_X       117
#define ERASE_X      177

#define MAINMENU_SAVEFILE_X1   92
#define MAINMENU_SAVEFILE_Y1   78
#define MAINMENU_SAVEFILE_X2  209
#define MAINMENU_SAVEFILE_Y2  118

#define MAINMENU_MARIOTEXT_X1  MAINMENU_SAVEFILE_X1
#define MAINMENU_MARIOTEXT_Y1 (MAINMENU_SAVEFILE_Y1 - 13) // 65
#define MAINMENU_MARIOTEXT_X2 (MAINMENU_SAVEFILE_X2 -  2) // 207
#define MAINMENU_MARIOTEXT_Y2 (MAINMENU_SAVEFILE_Y2 - 13) // 105


/**
 * Prints main menu strings that shows on the yellow background menu screen.
 *
 * In EU this function acts like "print_save_file_strings" because
 * print_main_lang_strings is first called to render the strings for the 4 buttons.
 * Same rule applies for score, copy and erase strings.
 */
void print_main_menu_strings(void) {
    const unsigned char textSelectFile[] = { TEXT_SELECT_FILE };

    // Print "SELECT FILE" text
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_hud_lut_string(HUD_LUT_DIFF, SELECT_FILE_X, SELECT_FILE_Y, textSelectFile);

    // Print file star counts
    print_save_file_star_count(SAVE_FILE_A, MAINMENU_SAVEFILE_X1, MAINMENU_SAVEFILE_Y1);
    print_save_file_star_count(SAVE_FILE_B, MAINMENU_SAVEFILE_X2, MAINMENU_SAVEFILE_Y1);
    print_save_file_star_count(SAVE_FILE_C, MAINMENU_SAVEFILE_X1, MAINMENU_SAVEFILE_Y2);
    print_save_file_star_count(SAVE_FILE_D, MAINMENU_SAVEFILE_X2, MAINMENU_SAVEFILE_Y2);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    // Print file names
    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_menu_generic_string(MAINMENU_MARIOTEXT_X1, MAINMENU_MARIOTEXT_Y1, textMarioFiles[SAVE_FILE_A]);
    print_menu_generic_string(MAINMENU_MARIOTEXT_X2, MAINMENU_MARIOTEXT_Y1, textMarioFiles[SAVE_FILE_B]);
    print_menu_generic_string(MAINMENU_MARIOTEXT_X1, MAINMENU_MARIOTEXT_Y2, textMarioFiles[SAVE_FILE_C]);
    print_menu_generic_string(MAINMENU_MARIOTEXT_X2, MAINMENU_MARIOTEXT_Y2, textMarioFiles[SAVE_FILE_D]);
    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_end);

    // Print menu names
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

    print_generic_string(SCORE_X, 39, textScore);
    print_generic_string(COPY_X,  39, textCopy);
    print_generic_string(ERASE_X, 39, textErase);
    print_generic_string_centered(254, 39, textSoundModes[sSoundMode], 10.0f);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}


#define CHECK_FILE_X   95
#define CHECK_FILE_Y   35

#define NOSAVE_DATA_X  99
#define NOSAVE_DATA_Y 190

/**
 * Defines IDs for the top message of the score menu and displays it if the ID is called in messageID.
 */
void score_menu_display_message(s8 messageID) {
    const unsigned char textCheckFile[] = { TEXT_CHECK_FILE };

    switch (messageID) {
        case SCORE_MSG_CHECK_FILE:
            print_hud_lut_string_fade(HUD_LUT_DIFF, CHECK_FILE_X, CHECK_FILE_Y, LANGUAGE_ARRAY(textCheckFile));
            break;
        case SCORE_MSG_NOSAVE_DATA:
            print_generic_string_fade(NOSAVE_DATA_X, NOSAVE_DATA_Y, LANGUAGE_ARRAY(textNoSavedDataExists));
            break;
    }
}

#define RETURN_X      44
#define COPYFILE_X1  135
#define ERASEFILE_X1 231

#define FADEOUT_TIMER 20

#define SUBMENU_SAVEFILE_X1  (MAINMENU_SAVEFILE_X1 -  2) //  90
#define SUBMENU_SAVEFILE_Y1  (MAINMENU_SAVEFILE_Y1 -  2) //  76
#define SUBMENU_SAVEFILE_X2  (MAINMENU_SAVEFILE_X2 +  2) // 211
#define SUBMENU_SAVEFILE_Y2  (MAINMENU_SAVEFILE_Y2 +  1) // 119

// #define SUBMENU_MARIOTEXT_X1 (SUBMENU_SAVEFILE_X1  -  1) //  89
// #define SUBMENU_MARIOTEXT_Y1 (SUBMENU_SAVEFILE_Y1  - 14) //  62
// #define SUBMENU_MARIOTEXT_X2 (SUBMENU_SAVEFILE_X2  -  0) // 211
// #define SUBMENU_MARIOTEXT_Y2 (SUBMENU_SAVEFILE_Y2  - 14) // 105
#define SUBMENU_MARIOTEXT_X1 (MAINMENU_MARIOTEXT_X1 -  3) //  89
#define SUBMENU_MARIOTEXT_Y1 (MAINMENU_MARIOTEXT_Y1 -  3) //  62
#define SUBMENU_MARIOTEXT_X2 (MAINMENU_MARIOTEXT_X2 +  4) // 211
#define SUBMENU_MARIOTEXT_Y2 (MAINMENU_MARIOTEXT_Y2 +  0) // 105

void print_sub_menu_files(void) {
    // Print file star counts
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_save_file_star_count(SAVE_FILE_A, SUBMENU_SAVEFILE_X1, SUBMENU_SAVEFILE_Y1);
    print_save_file_star_count(SAVE_FILE_B, SUBMENU_SAVEFILE_X2, SUBMENU_SAVEFILE_Y1);
    print_save_file_star_count(SAVE_FILE_C, SUBMENU_SAVEFILE_X1, SUBMENU_SAVEFILE_Y2);
    print_save_file_star_count(SAVE_FILE_D, SUBMENU_SAVEFILE_X2, SUBMENU_SAVEFILE_Y2);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    // Print file names
    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_menu_generic_string(SUBMENU_MARIOTEXT_X1, SUBMENU_MARIOTEXT_Y1, textMarioFiles[SAVE_FILE_A]);
    print_menu_generic_string(SUBMENU_MARIOTEXT_X2, SUBMENU_MARIOTEXT_Y1, textMarioFiles[SAVE_FILE_B]);
    print_menu_generic_string(SUBMENU_MARIOTEXT_X1, SUBMENU_MARIOTEXT_Y2, textMarioFiles[SAVE_FILE_C]);
    print_menu_generic_string(SUBMENU_MARIOTEXT_X2, SUBMENU_MARIOTEXT_Y2, textMarioFiles[SAVE_FILE_D]);
    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_end);
}

/**
 * Prints score menu strings that shows on the green background menu screen.
 */
void print_score_menu_strings(void) {
    sScoreFileCoinScoreMode = 0;

    // Update and print the message at the top of the menu.
    if (sMainMenuTimer == FADEOUT_TIMER) {
        sFadeOutText = TRUE;
    }
    if (update_text_fade_out()) {
        sStatusMessageID = (sStatusMessageID != SCORE_MSG_CHECK_FILE) ? SCORE_MSG_CHECK_FILE : SCORE_MSG_NOSAVE_DATA;
    }
    // Print messageID called above
    score_menu_display_message(sStatusMessageID);

    print_sub_menu_files();

    // Print menu names
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

    print_generic_string(RETURN_X,     35, LANGUAGE_ARRAY(textReturn));
    print_generic_string(COPYFILE_X1,  35, LANGUAGE_ARRAY(textCopyFileButton));
    print_generic_string(ERASEFILE_X1, 35, LANGUAGE_ARRAY(textEraseFileButton));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#define NOFILE_COPY_X  119
#define COPY_FILE_X    104
#define COPYIT_WHERE_X 109
#define NOSAVE_DATA_X2 101
#define COPYCOMPLETE_X 110
#define SAVE_EXISTS_X1 110

/**
 * Defines IDs for the top message of the copy menu and displays it if the ID is called in messageID.
 */
void copy_menu_display_message(s8 messageID) {
    const unsigned char textNoFileToCopyFrom[] = { TEXT_NO_FILE_TO_COPY_FROM };
    const unsigned char textCopyFile[] = { TEXT_COPY_FILE };
    const unsigned char textCopyItToWhere[] = { TEXT_COPY_IT_TO_WHERE };
    const unsigned char textCopyCompleted[] = { TEXT_COPYING_COMPLETED };

    switch (messageID) {
        case COPY_MSG_MAIN_TEXT:
            if (sAllFilesExist) {
                print_generic_string_fade(NOFILE_COPY_X, 190, LANGUAGE_ARRAY(textNoFileToCopyFrom));
            } else {
                print_hud_lut_string_fade(HUD_LUT_DIFF, COPY_FILE_X, 35, LANGUAGE_ARRAY(textCopyFile));
            }
            break;
        case COPY_MSG_COPY_WHERE:
            print_generic_string_fade(COPYIT_WHERE_X, 190, LANGUAGE_ARRAY(textCopyItToWhere));
            break;
        case COPY_MSG_NOSAVE_EXISTS:
            print_generic_string_fade(NOSAVE_DATA_X2, 190, LANGUAGE_ARRAY(textNoSavedDataExists));
            break;
        case COPY_MSG_COPY_COMPLETE:
            print_generic_string_fade(COPYCOMPLETE_X, 190, LANGUAGE_ARRAY(textCopyCompleted));
            break;
        case COPY_MSG_SAVE_EXISTS:
            print_generic_string_fade(SAVE_EXISTS_X1, 190, LANGUAGE_ARRAY(textSavedDataExists));
            break;
    }
}

/**
 * Updates messageIDs of the copy menu depending of the copy phase value defined.
 */
void copy_menu_update_message(void) {
    struct Object *copyButton = get_button_of_type(MENU_PAGE_MAIN, MENU_BUTTON_COPY);

    switch (copyButton->oMenuButtonActionPhase) {
        case COPY_PHASE_MAIN:
            if (sMainMenuTimer == FADEOUT_TIMER) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out()) {
                sStatusMessageID = (sStatusMessageID != COPY_MSG_MAIN_TEXT) ? COPY_MSG_MAIN_TEXT : COPY_MSG_NOSAVE_EXISTS;
            }
            break;
        case COPY_PHASE_COPY_WHERE:
            if (sMainMenuTimer == FADEOUT_TIMER && sStatusMessageID == COPY_MSG_SAVE_EXISTS) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out()) {
                sStatusMessageID = (sStatusMessageID != COPY_MSG_COPY_WHERE) ? COPY_MSG_COPY_WHERE : COPY_MSG_SAVE_EXISTS;
            }
            break;
        case COPY_PHASE_COPY_COMPLETE:
            if (sMainMenuTimer == FADEOUT_TIMER) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out()) {
                sStatusMessageID = (sStatusMessageID != COPY_MSG_COPY_COMPLETE) ? COPY_MSG_COPY_COMPLETE : COPY_MSG_MAIN_TEXT;
            }
            break;
    }
}

#define VIEWSCORE_X1 128
#define ERASEFILE_X2 230

/**
 * Prints copy menu strings that shows on the blue background menu screen.
 */
void print_copy_menu_strings(void) {
    // Update and print the message at the top of the menu.
    copy_menu_update_message();

    // Print messageID called inside a copy_menu_update_message case
    copy_menu_display_message(sStatusMessageID);

    print_sub_menu_files();

    // Print menu names
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

    print_generic_string(RETURN_X,     35, LANGUAGE_ARRAY(textReturn));
    print_generic_string(VIEWSCORE_X1, 35, LANGUAGE_ARRAY(textViewScore));
    print_generic_string(ERASEFILE_X2, 35, LANGUAGE_ARRAY(textEraseFileButton));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#define MENU_ERASE_YES_MIN_X 140
#define MENU_ERASE_YES_MAX_X 169
#define MENU_ERASE_NO_MIN_X  189
#define MENU_ERASE_NO_MAX_X  218

#define MENU_ERASE_YES_NO_MIN_Y 191
#define MENU_ERASE_YES_NO_MAX_Y 210

/**
 * Prints the "YES NO" prompt and checks if one of the prompts are hovered to do it's functions.
 */
void print_erase_menu_prompt(s16 x, s16 y) {
    const unsigned char textYes[] = { TEXT_YES };
    const unsigned char textNo[]  = { TEXT_NO  };

    s16 colorFade = (gGlobalTimer << 12);

    s16 cursorX = sCursorPos[0] + (x + 70);
    s16 cursorY = sCursorPos[1] + 120;

    if (cursorX <  MENU_ERASE_YES_MAX_X
     && cursorX >= MENU_ERASE_YES_MIN_X
     && cursorY <  MENU_ERASE_YES_NO_MAX_Y
     && cursorY >= MENU_ERASE_YES_NO_MIN_Y) {
        // Fade "YES" string color but keep "NO" gray
        sYesColor = (sins(colorFade) * 50.0f) + 205;
        sNoColor  = 150;
        sEraseYesNoHoverState = MENU_ERASE_HOVER_YES;
    } else if (cursorX <  MENU_ERASE_NO_MAX_X
            && cursorX >= MENU_ERASE_NO_MIN_X
            && cursorY <  MENU_ERASE_YES_NO_MAX_Y
            && cursorY >= MENU_ERASE_YES_NO_MIN_Y) {
        // Fade "NO" string color but keep "YES" gray
        sYesColor = 150;
        sNoColor  = (sins(colorFade) * 50.0f) + 205;
        sEraseYesNoHoverState = MENU_ERASE_HOVER_NO;
    } else {
        // Don't fade both strings and keep them gray
        sYesColor = 150;
        sNoColor  = 150;
        sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
    }
    // If the cursor is clicked...
    if (sCursorClickingTimer == 2) {
        struct Object *eraseFileButton = sMenuPages[MENU_PAGE_ERASE].buttons[sSelectedFileIndex];
        struct Object *mainFileButton  = sMenuPages[MENU_PAGE_MAIN].buttons[sSelectedFileIndex];
        struct Object *mainEraseButton = get_button_of_type(MENU_PAGE_MAIN, MENU_BUTTON_ERASE);
        // ..and is hovering "YES", delete file
        if (sEraseYesNoHoverState == MENU_ERASE_HOVER_YES) {
            play_sound(SOUND_MARIO_WAAAOOOW, gGlobalSoundSource);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
            mainEraseButton->oMenuButtonActionPhase = ERASE_PHASE_MARIO_ERASED;
            sFadeOutText = TRUE;
            sMainMenuTimer = 0;
            save_file_erase(sSelectedFileIndex);
            eraseFileButton->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE];
            mainFileButton->header.gfx.sharedChild  = gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE];
            sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
            // ..and is hovering "NO", return back to main phase
        } else if (sEraseYesNoHoverState == MENU_ERASE_HOVER_NO) {
            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
            eraseFileButton->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_OUT;
            mainEraseButton->oMenuButtonActionPhase = ERASE_PHASE_MAIN;
            sFadeOutText = TRUE;
            sMainMenuTimer = 0;
            sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
        }
    }

    // Print "YES NO" strings
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, sYesColor, sYesColor, sYesColor, sTextBaseAlpha);
    print_generic_string((x + 56), y, LANGUAGE_ARRAY(textYes));
    gDPSetEnvColor(gDisplayListHead++,  sNoColor,  sNoColor,  sNoColor, sTextBaseAlpha);
    print_generic_string((x + 98), y, LANGUAGE_ARRAY(textNo));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

// MARIO_ERASED_VAR is the value there the letter "A" is, it works like this:
//   US and EU   ---    JP
// M a r i o   A --- マ リ オ Ａ
// 0 1 2 3 4 5 6 --- 0 1 2 3
#define ERASE_FILE_X      98
#define NOSAVE_DATA_X3   100
#define MARIO_ERASED_VAR   6
#define MARIO_ERASED_X   100
#define SAVE_EXISTS_X2   100

/**
 * Defines IDs for the top message of the erase menu and displays it if the ID is called in messageID.
 */
void erase_menu_display_message(s8 messageID) {
    const unsigned char textEraseFile[] = { TEXT_ERASE_FILE };
    const unsigned char textSure[] = { TEXT_SURE };
    const unsigned char textNoSavedDataExists[] = { TEXT_NO_SAVED_DATA_EXISTS };
    const unsigned char textSavedDataExists[] = { TEXT_SAVED_DATA_EXISTS };
    unsigned char textMarioAJustErased[] = { TEXT_FILE_MARIO_A_JUST_ERASED };

    switch (messageID) {
        case ERASE_MSG_MAIN_TEXT:
            print_hud_lut_string_fade(HUD_LUT_DIFF, ERASE_FILE_X, 35, LANGUAGE_ARRAY(textEraseFile));
            break;
        case ERASE_MSG_PROMPT:
            print_generic_string_fade(90, 190, LANGUAGE_ARRAY(textSure));
            print_erase_menu_prompt(90, 190); // YES NO, has functions for it too
            break;
        case ERASE_MSG_NOSAVE_EXISTS:
            print_generic_string_fade(NOSAVE_DATA_X3, 190, LANGUAGE_ARRAY(textNoSavedDataExists));
            break;
        case ERASE_MSG_MARIO_ERASED:
            LANGUAGE_ARRAY(textMarioAJustErased)[MARIO_ERASED_VAR] = sSelectedFileIndex + 10;
            print_generic_string_fade(MARIO_ERASED_X, 190, LANGUAGE_ARRAY(textMarioAJustErased));
            break;
        case ERASE_MSG_SAVE_EXISTS: // unused
            print_generic_string_fade(SAVE_EXISTS_X2, 190, LANGUAGE_ARRAY(textSavedDataExists));
            break;
    }
}

/**
 * Updates messageIDs of the erase menu depending of the erase phase value defined.
 */
void erase_menu_update_message(void) {
    struct Object *eraseButton = get_button_of_type(MENU_PAGE_MAIN, MENU_BUTTON_ERASE);

    switch (eraseButton->oMenuButtonActionPhase) {
        case ERASE_PHASE_MAIN:
            if (sMainMenuTimer == FADEOUT_TIMER && sStatusMessageID == ERASE_MSG_NOSAVE_EXISTS) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out()) {
                sStatusMessageID = (sStatusMessageID != ERASE_MSG_MAIN_TEXT) ? ERASE_MSG_MAIN_TEXT : ERASE_MSG_NOSAVE_EXISTS;
            }
            break;
        case ERASE_PHASE_PROMPT:
            if (update_text_fade_out()) {
                if (sStatusMessageID != ERASE_MSG_PROMPT) {
                    sStatusMessageID = ERASE_MSG_PROMPT;
                }
                sCursorPos[0] = 43;
                sCursorPos[1] = 80;
            }
            break;
        case ERASE_PHASE_MARIO_ERASED:
            if (sMainMenuTimer == FADEOUT_TIMER) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out()) {
                sStatusMessageID = (sStatusMessageID != ERASE_MSG_MARIO_ERASED) ? ERASE_MSG_MARIO_ERASED : ERASE_MSG_MAIN_TEXT;
            }
            break;
    }
}

#define VIEWSCORE_X2 127
#define COPYFILE_X2  233

/**
 * Prints erase menu strings that shows on the red background menu screen.
 */
void print_erase_menu_strings(void) {
    // Update and print the message at the top of the menu.
    erase_menu_update_message();

    // Print messageID called inside a erase_menu_update_message case
    erase_menu_display_message(sStatusMessageID);

    print_sub_menu_files();

    // Print menu names
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

    print_generic_string(RETURN_X,     35, textReturn);
    print_generic_string(VIEWSCORE_X2, 35, textViewScore);
    print_generic_string(COPYFILE_X2,  35, textCopyFileButton);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#if MULTILANG
    #define SOUND_HUD_X  96
    #define SOUND_HUD_Y 141
#else
    #define SOUND_HUD_X  47
    #define SOUND_HUD_Y  87
#endif

/**
 * Prints sound mode menu strings that shows on the purple background menu screen.
 *
 * In EU, this function acts like "print_option_mode_menu_strings" because of languages.
 */
void print_sound_mode_menu_strings(void) {
    const unsigned char textSoundSelect[] = { TEXT_SOUND_SELECT };
    s32 mode;
    s32 textX;

    // Print "SOUND SELECT" text
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

    print_hud_lut_string(HUD_LUT_DIFF, SOUND_HUD_X, 32, LANGUAGE_ARRAY(textSoundSelect));

#if MULTILANG
    print_hud_lut_string(HUD_LUT_DIFF, 47, 101, LANGUAGE_ARRAY(textLanguageSelect[0]));
#endif

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

    // Print sound mode names
    for (mode = 0, textX = 90; mode < 3; textX += 70, mode++) {
        if (mode == sSoundMode) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
        } else {
            gDPSetEnvColor(gDisplayListHead++,   0,   0,   0, sTextBaseAlpha);
        }
        print_generic_string_centered(textX, SOUND_HUD_Y, LANGUAGE_ARRAY(textSoundModes[mode]), 10.0f);
    }

#if MULTILANG
    // In EU, print language mode names
    for (mode = 0, textX = 90; mode < 3; textX += 70, mode++) {
        if (mode == LANGUAGE_FUNCTION) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
        } else {
            gDPSetEnvColor(gDisplayListHead++,   0,   0,   0, sTextBaseAlpha);
        }
        print_generic_string_centered(textX, 72, textLanguage[mode], 10.0f);
    }

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_generic_string(182, 29, LANGUAGE_ARRAY(textReturn));
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

/**
 * Prints castle secret stars collected in a score menu save file.
 */
void print_score_file_castle_secret_stars(s8 fileIndex, s16 x, s16 y) {
    unsigned char secretStarsText[20];
    const unsigned char textStarX[] = { TEXT_STAR_X };
    // Print "[star] x"
    print_menu_generic_string(x, y, textStarX);
    // Print number of castle secret stars
    int_to_str(
        save_file_get_total_star_count(
            fileIndex,
            COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES),
            COURSE_NUM_TO_INDEX(COURSE_MAX)
        ),
        secretStarsText
    );
    print_menu_generic_string((x + 16), y, secretStarsText);
}

#define HISCORE_COIN_ICON_X  18
#define HISCORE_COIN_TEXT_X  34
#define HISCORE_COIN_NAMES_X 60

/**
 * Prints course coins collected in a score menu save file.
 */
void print_score_file_course_coin_score(s8 fileIndex, s16 courseIndex, s16 x, s16 y) {
    unsigned char coinScoreText[20];
    u8 stars = save_file_get_star_flags(fileIndex, courseIndex);
    const unsigned char textCoinX[] = { TEXT_COIN_X };
    const unsigned char textStar[] = { TEXT_STAR };
    const unsigned char fileNames[][8] = {
        { TEXT_4DASHES       },
        { TEXT_SCORE_MARIO_A },
        { TEXT_SCORE_MARIO_B },
        { TEXT_SCORE_MARIO_C },
        { TEXT_SCORE_MARIO_D },
    };
    // MYSCORE
    if (sScoreFileCoinScoreMode == 0) {
        // Print "[coin] x"
        print_menu_generic_string((x + 25), y, textCoinX);
        // Print coin score
        int_to_str(save_file_get_course_coin_score(fileIndex, courseIndex), coinScoreText);
        print_menu_generic_string((x + 41), y, coinScoreText);
        // If collected, print 100 coin star
        if (stars & STAR_FLAG_ACT_100_COINS) {
            print_menu_generic_string((x + 70), y, textStar);
        }
    }
    // HISCORE
    else {
        // Print "[coin] x"
        print_menu_generic_string((x + HISCORE_COIN_ICON_X), y, textCoinX);
        // Print coin highscore
        int_to_str(((u16) save_file_get_max_coin_score(courseIndex) & BITMASK(16)), coinScoreText);
        print_menu_generic_string((x + HISCORE_COIN_TEXT_X), y, coinScoreText);
        // Print coin highscore file
        print_menu_generic_string((x + HISCORE_COIN_NAMES_X), y,
                         fileNames[(save_file_get_max_coin_score(courseIndex) >> 16) & BITMASK(16)]);
    }
}

/**
 * Prints stars collected in a score menu save file.
 */
void print_score_file_star_score(s8 fileIndex, s16 courseIndex, s16 x, s16 y) {
    s16 i = 0;
    unsigned char starScoreText[19];
    u8 stars = save_file_get_star_flags(fileIndex, courseIndex);
    s8 starCount = save_file_get_course_star_count(fileIndex, courseIndex);
    // Don't count 100 coin star
    if (stars & STAR_FLAG_ACT_100_COINS) {
        starCount--;
    }
    // Add 1 star character for every star collected
    for (i = 0; i < starCount; i++) {
        starScoreText[i] = DIALOG_CHAR_STAR_FILLED;
    }
    // Terminating byte
    starScoreText[i] = DIALOG_CHAR_TERMINATOR;
    print_menu_generic_string(x, y, starScoreText);
}

#define MARIO_X         25
#define FILE_LETTER_X   95
#define LEVEL_NUM_PAD    3
#define SECRET_STARS_PAD 6
#define LEVEL_NAME_X    23
#define LEVEL_NAME_Y    23
#define STAR_SCORE_X   171
#define MYSCORE_X      238
#define HISCORE_X      231
#define SCORE_Y         24

/**
 * Prints save file score strings that shows when a save file is chosen inside the score menu.
 */
void print_save_file_scores(s8 fileIndex) {
    s16 i;
    const unsigned char textMario[] = { TEXT_MARIO };
    const unsigned char textHiScore[] = { TEXT_HI_SCORE };
    const unsigned char textMyScore[] = { TEXT_MY_SCORE };
    unsigned char textFileLetter[] = { TEXT_ZERO };
    const void **levelNameTable = segmented_to_virtual(languageTable[gInGameLanguage][1]);

    textFileLetter[0] = (fileIndex + ASCII_TO_DIALOG('A')); // get letter of file selected

    // Print file name at top
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_hud_lut_string(HUD_LUT_DIFF,         MARIO_X, 15, textMario);
    print_hud_lut_string(HUD_LUT_GLOBAL, FILE_LETTER_X, 15, textFileLetter);

    // Print save file star count at top
    print_save_file_star_count(fileIndex, 124, 15);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    // Print course scores
    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

    //! TODO: clean up
    s16 y;
    for (i = 0; i < COURSE_STAGES_MAX; i++) {
        y = (LEVEL_NAME_Y + (12 * (i + 1)));
        print_menu_generic_string((LEVEL_NAME_X + ((i < 9) * LEVEL_NUM_PAD)), y, segmented_to_virtual(levelNameTable[i]));
        print_score_file_star_score(       fileIndex, i, STAR_SCORE_X, y);
        print_score_file_course_coin_score(fileIndex, i,          213, y);
    }

    y = (LEVEL_NAME_Y + (12 * (COURSE_STAGES_MAX + 1)));

    // Print castle secret stars text
    print_menu_generic_string((LEVEL_NAME_X + SECRET_STARS_PAD), y,
                              segmented_to_virtual(levelNameTable[25]));
    // Print castle secret stars score
    print_score_file_castle_secret_stars(fileIndex, STAR_SCORE_X, y);

    // Print current coin score mode
    if (sScoreFileCoinScoreMode == 0) {
        print_menu_generic_string(MYSCORE_X, SCORE_Y, LANGUAGE_ARRAY(textMyScore));
    } else {
        print_menu_generic_string(HISCORE_X, SCORE_Y, LANGUAGE_ARRAY(textHiScore));
    }

    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_end);
}

/**
 * Prints file select strings depending on the menu selected.
 * Also checks if all saves exists and defines text and main menu timers.
 */
void print_file_select_strings(void) {
    create_dl_ortho_matrix();

    struct Object *parentObj = NULL;
    s32 selectedButtonType = MENU_BUTTON_NULL;
    s32 parentButtonType = MENU_BUTTON_NULL;
    if (sSelectedButton != NULL) {
        selectedButtonType = sSelectedButton->oMenuButtonType;
        parentObj = sSelectedButton->parentObj;
        if (parentObj != NULL && obj_has_behavior(parentObj, bhvMenuButton)) {
            parentButtonType = parentObj->oMenuButtonType;
        }
    }

    switch (selectedButtonType) {
        case MENU_BUTTON_NULL:
            if (parentButtonType == MENU_BUTTON_NULL) {
                print_main_menu_strings();
            }
            break;
        case MENU_BUTTON_SCORE:
            if (parentButtonType == MENU_BUTTON_NULL) {
                print_score_menu_strings();
            }
            break;
        case MENU_BUTTON_FILE_A:
        case MENU_BUTTON_FILE_B:
        case MENU_BUTTON_FILE_C:
        case MENU_BUTTON_FILE_D:
            if (parentButtonType == MENU_BUTTON_SCORE) {
                print_save_file_scores(selectedButtonType);
            }
            break;
        case MENU_BUTTON_COPY:
            if (parentButtonType == MENU_BUTTON_NULL) {
                print_copy_menu_strings();
            }
            break;
        case MENU_BUTTON_ERASE:
            if (parentButtonType == MENU_BUTTON_NULL) {
                print_erase_menu_strings();
            }
            break;
        case MENU_BUTTON_OPTIONS:
            if (parentButtonType == MENU_BUTTON_NULL) {
                print_sound_mode_menu_strings();
            }
            break;
    }
    // If all 4 save file exists, define true to sAllFilesExist to prevent more copies in copy menu
    sAllFilesExist = (save_file_exists(SAVE_FILE_A)
                   && save_file_exists(SAVE_FILE_B)
                   && save_file_exists(SAVE_FILE_C)
                   && save_file_exists(SAVE_FILE_D));
    // Timers for menu alpha text and the main menu itself
    if (sTextBaseAlpha < 250) {
        sTextBaseAlpha += 10;
    }
    if (sMainMenuTimer < 1000) {
        sMainMenuTimer++;
    }
}

/**
 * Geo function that prints file select strings and the cursor.
 */
Gfx *geo_file_select_strings_and_menu_cursor(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        print_file_select_strings();
        print_menu_cursor();
    }

    return NULL;
}

static const Vec2s sFileButtonCursorPositions[] = {
    { -94,  46 },
    {  24,  46 },
    { -94,   5 },
    {  24,   5 },
};

/**
 * Initiates file select values after Mario Screen.
 * Relocates cursor position of the last save if the game goes back to the Mario Screen
 * either completing a course choosing "SAVE & QUIT" or having a game over.
 */
s32 lvl_init_menu_values_and_cursor_pos(UNUSED s32 arg, UNUSED s32 unused) {
    sSelectedButton = NULL;
    sCurrentMenuLevel = MENU_LAYER_MAIN;
    sTextBaseAlpha = 0;
    // Place the cursor over the save file that was being played.
    // gCurrSaveFileNum is 1 by default when the game boots, as such
    // the cursor will point on Mario A save file.
    sCursorPos[0] = sFileButtonCursorPositions[SAVE_NUM_TO_INDEX(gCurrSaveFileNum)][0];
    sCursorPos[1] = sFileButtonCursorPositions[SAVE_NUM_TO_INDEX(gCurrSaveFileNum)][1];
    sClickPos[0] = CLICK_POS_NULL;
    sClickPos[1] = CLICK_POS_NULL;
    sCursorClickingTimer = 0;
    sSelectedFileNum = 0;
    sSelectedFileIndex = MENU_BUTTON_NULL;
    sFadeOutText = FALSE;
    sStatusMessageID = 0;
    sTextFadeAlpha = 0;
    sMainMenuTimer = 0;
    sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
    sSoundMode = save_file_get_sound_mode();
    gCurrLevelNum = LEVEL_UNKNOWN_1;

    return 0;
}

/**
 * Updates file select menu button objects so they can be interacted.
 * When a save file is selected, it returns fileNum value
 * defined in load_main_menu_save_file.
 */
s32 lvl_update_obj_and_load_file_selected(UNUSED s32 arg, UNUSED s32 unused) {
    area_update_objects();

    return sSelectedFileNum;
}
