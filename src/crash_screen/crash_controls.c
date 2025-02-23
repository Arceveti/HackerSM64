#include <ultra64.h>

#include "types.h"
#include "sm64.h"

#include "address_select.h"
#include "crash_draw.h"
#include "crash_main.h"
#include "crash_pages.h"
#include "crash_print.h"
#include "crash_settings.h"

#include "crash_controls.h"

#include "pages/page_stack.h"


struct CSSetting cs_settings_group_controls[] = {
    [CS_OPT_HEADER_CONTROLS             ] = { .type = CS_OPT_TYPE_HEADER,  .name = "CONTROLS",                       .valNames = &gValNames_bool,          .val = SECTION_EXPANDED_DEFAULT,  .defaultVal = SECTION_EXPANDED_DEFAULT,  .lowerBound = FALSE,                 .upperBound = TRUE,                       },
    [CS_OPT_CONTROLS_CURSOR_WAIT_FRAMES ] = { .type = CS_OPT_TYPE_SETTING, .name = "Hold direction wait frames",     .valNames = NULL,                     .val = 10,                        .defaultVal = 10,                        .lowerBound = 0,                     .upperBound = 1000,                       },
    [CS_OPT_CONTROLS_ANALOG_DEADZONE    ] = { .type = CS_OPT_TYPE_SETTING, .name = "Analog deadzone",                .valNames = NULL,                     .val = 60,                        .defaultVal = 60,                        .lowerBound = 0,                     .upperBound = 128,                        },
    [CS_OPT_END_CONTROLS                ] = { .type = CS_OPT_TYPE_END },
};


const enum ControlTypes cs_cont_list_default[] = {
    CONT_DESC_SWITCH_PAGE,
    CONT_DESC_SHOW_CONTROLS,
    CONT_DESC_CYCLE_DRAW,
    CONT_DESC_LIST_END,
};


_Bool gCSSwitchedPage = FALSE;
_Bool gCSDrawControls = FALSE;

CrashScreenDirections gCSDirectionFlags;

static OSTime sCSInputTimeY = 0;
static OSTime sCSInputTimeX = 0;

CSController gCSCompositeControllers[1];
CSController* const gCSCompositeController = &gCSCompositeControllers[0];


// Input string defines:
#define STR_A       "A"
#define STR_B       "B"
#define STR_Z       "Z"
#define STR_START   "START"
#define STR_UP      "UP"
#define STR_DOWN    "DOWN"
#define STR_LEFT    "LEFT"
#define STR_RIGHT   "RIGHT"
#define STR_L       "L"
#define STR_R       "R"

const ControlType gCSControlDescriptions[] = {
    [CONT_DESC_SWITCH_PAGE      ] = { .control = STR_L"/"STR_R,                             .description = "switch page",                       },
    [CONT_DESC_SHOW_CONTROLS    ] = { .control = STR_START,                                 .description = "show/hide page controls",           },
    [CONT_DESC_CYCLE_DRAW       ] = { .control = STR_Z,                                     .description = "hide crash screen",                 },
    [CONT_DESC_SCROLL_LIST      ] = { .control = STR_UP"/"STR_DOWN,                         .description = "scroll list",                       },
    [CONT_DESC_CURSOR           ] = { .control = STR_UP"/"STR_DOWN"/"STR_LEFT"/"STR_RIGHT,  .description = "move cursor",                       },
    [CONT_DESC_CURSOR_VERTICAL  ] = { .control = STR_UP"/"STR_DOWN,                         .description = "move cursor",                       },
    [CONT_DESC_CURSOR_HORIZONTAL] = { .control = STR_LEFT"/"STR_RIGHT,                      .description = "move cursor",                       },
    [CONT_DESC_JUMP_TO_ADDRESS  ] = { .control = STR_A,                                     .description = "jump to specific address",          },
    [CONT_DESC_TOGGLE_ASCII     ] = { .control = STR_B,                                     .description = "toggle bytes as hex or ascii",      },
#ifdef INCLUDE_DEBUG_MAP
    [CONT_DESC_TOGGLE_FUNCTIONS ] = { .control = STR_B,                                     .description = "toggle function names",             },
#endif
    [CONT_DESC_CYCLE_FLOATS_MODE] = { .control = STR_B,                                     .description = "toggle floats mode",                },
    [CONT_DESC_CHANGE_SETTING   ] = { .control = STR_A"/"STR_B"/"STR_LEFT"/"STR_RIGHT,      .description = "change selected setting",           },
    [CONT_DESC_RESET_SETTING    ] = { .control = STR_A"+"STR_B,                             .description = "reset selected setting to default", },
};


// Updates gCSDirectionFlags with directional inputs. Analog stick, D-pad, or C-buttons.
void cs_update_direction_input(void) {
    OSTime currTime = osGetTime();

    gCSDirectionFlags.pressed.up    = FALSE;
    gCSDirectionFlags.pressed.down  = FALSE;
    gCSDirectionFlags.pressed.left  = FALSE;
    gCSDirectionFlags.pressed.right = FALSE;

    s16 rawStickX  = gCSCompositeController->rawStickX;
    s16 rawStickY  = gCSCompositeController->rawStickY;
    u16 buttonDown = gCSCompositeController->buttonDown;

    s16 deadzone = cs_get_setting_val(CS_OPT_GROUP_CONTROLS, CS_OPT_CONTROLS_ANALOG_DEADZONE);

    _Bool up    = ((buttonDown & (U_CBUTTONS | U_JPAD)) || (rawStickY >  deadzone));
    _Bool down  = ((buttonDown & (D_CBUTTONS | D_JPAD)) || (rawStickY < -deadzone));
    _Bool left  = ((buttonDown & (L_CBUTTONS | L_JPAD)) || (rawStickX < -deadzone));
    _Bool right = ((buttonDown & (R_CBUTTONS | R_JPAD)) || (rawStickX >  deadzone));

    // How long to wait when holding a direction before it becomes continuous.
    const OSTime cursorWaitCycles = FRAMES_TO_CYCLES(cs_get_setting_val(CS_OPT_GROUP_CONTROLS, CS_OPT_CONTROLS_CURSOR_WAIT_FRAMES));

    if (up ^ down) {
        if (
            !(
                gCSDirectionFlags.held.up ||
                gCSDirectionFlags.held.down
            )
        ) { // Prev Y:
            // On press:
            sCSInputTimeY = currTime;
            gCSDirectionFlags.pressed.up   = up;
            gCSDirectionFlags.pressed.down = down;
        } else {
            // Held:
            OSTime diff = (currTime - sCSInputTimeY);
            if (diff > cursorWaitCycles) {
                gCSDirectionFlags.pressed.up   = up;
                gCSDirectionFlags.pressed.down = down;
            }
        }
    }

    if (left ^ right) {
        if (
            !(
                gCSDirectionFlags.held.left ||
                gCSDirectionFlags.held.right
            )
        ) { // Prev X:
            // On press:
            sCSInputTimeX = currTime;
            gCSDirectionFlags.pressed.left  = left;
            gCSDirectionFlags.pressed.right = right;
        } else {
            // Held:
            OSTime diff = (currTime - sCSInputTimeX);
            if (diff > cursorWaitCycles) {
                gCSDirectionFlags.pressed.left  = left;
                gCSDirectionFlags.pressed.right = right;
            }
        }
    }

    gCSDirectionFlags.held.up    = up;
    gCSDirectionFlags.held.down  = down;
    gCSDirectionFlags.held.left  = left;
    gCSDirectionFlags.held.right = right;
}

// Moves the viewport in a scrollable list to contain the selection cursor.
u32 cs_clamp_view_to_selection(u32 scrollIndex, u32 selectIndex, const u32 numRows, const u32 step) {
    const size_t size = ((numRows - 1) * step);

    // Selection is past the end of the viewport.
    if ((scrollIndex + size) < selectIndex) {
        scrollIndex = (selectIndex - size);
    }

    // Selection is before the beginning of the viewport.
    if (scrollIndex > selectIndex) {
        scrollIndex = selectIndex;
    }

    return ALIGNFLOOR(scrollIndex, step);
}

// Check for a page switch input (L or R).
_Bool cs_check_switch_page_input(void) {
    enum CSPages prevPage = gCSPageID;

    u16 buttonPressed = gCSCompositeController->buttonPressed;

    s8 change = 0;
    if (buttonPressed & L_TRIG) change = -1; // Previous Page.
    if (buttonPressed & R_TRIG) change = +1; // Next page.
    gCSPageID = WRAP(((s32)gCSPageID + change), FIRST_PAGE, (NUM_PAGES - 1));

    if (gCSPageID == prevPage) {
        return FALSE;
    }

    // Reset certain values when the page is changed.
    gCSDrawControls = FALSE;
    gCSSwitchedPage = TRUE;

    return TRUE;
}

// Global crash screen input function.
void cs_update_input(void) {
    handle_input_simple(&gActiveCSThreadInfo->mesg);

    bzero(&gCSCompositeControllers, sizeof(gCSCompositeControllers));

    for (int port = 0; port < ARRAY_COUNT(gControllers); port++) { //! TODO: < MAXCONTROLLERS when input PR is merged.
        struct Controller* controller = &gControllers[port];

        s16 rawStickX = controller->rawStickX;
        s16 rawStickY = controller->rawStickY;

        if (abss(gCSCompositeController->rawStickX) < abss(rawStickX)) {
            gCSCompositeController->rawStickX = rawStickX;
        }
        if (abss(gCSCompositeController->rawStickY) < abss(rawStickY)) {
            gCSCompositeController->rawStickY = rawStickY;
        }
        gCSCompositeController->buttonDown     |= controller->buttonDown;
        gCSCompositeController->buttonPressed  |= controller->buttonPressed;
        gCSCompositeController->buttonReleased |= controller->buttonReleased;
    }

    if (gCSCompositeController->buttonPressed & START_BUTTON) {
        gCSDrawControls ^= TRUE;
    }

    cs_update_direction_input();

    if (gCSDrawControls) {
        return;
    }

    if (gAddressSelectMenuOpen) {
        cs_address_select_input();
        return;
    }

    CSPage* page = gCSPages[gCSPageID];

    if (cs_check_switch_page_input()) {
        page = gCSPages[gCSPageID]; // gCSPageID may have changed in cs_check_switch_page_input.

        if ((page->initFunc != NULL) && !page->flags.initialized) {
            page->initFunc();
            page->flags.initialized = TRUE;
        }
    }

    // Run the page-specific input function.
    if ((page->inputFunc != NULL) && !page->flags.crashed) {
        page->inputFunc();
    }

    gCSSwitchedPage = FALSE;
}

// Controls popup box draw function.
//! TODO: Allow changing page-specific settings from here.
void cs_controls_box_draw(void) {
    cs_draw_dark_rect(
        (CRASH_SCREEN_X1 + (TEXT_WIDTH(1) / 2)), (CRASH_SCREEN_Y1 + (TEXT_HEIGHT(1) / 2)),
        (CRASH_SCREEN_W  -  TEXT_WIDTH(1)     ), (CRASH_SCREEN_H  -  TEXT_HEIGHT(1)     ),
        CS_DARKEN_SEVEN_EIGHTHS
    );
    CSPage* page = gCSPages[gCSPageID];

    // "[page name] PAGE CONTROLS"
    cs_print(TEXT_X(1), TEXT_Y(1), STR_COLOR_PREFIX"%s PAGE CONTROLS", COLOR_RGBA32_CRASH_PAGE_NAME, page->name);

    const enum ControlTypes* list = page->contList;

    if (list != NULL) {
        const ControlType* desc = NULL;

        u32 line = 3;

        while (*list != CONT_DESC_LIST_END) {
            desc = &gCSControlDescriptions[*list++];
            // [control]
            //   [description]
            cs_print(TEXT_X(2), TEXT_Y(line), "%s:\n "STR_COLOR_PREFIX"%s", desc->control, COLOR_RGBA32_CRASH_CONTROLS_DESCRIPTION, desc->description);
            line += 2;
        }
    }

    osWritebackDCacheAll();
}
