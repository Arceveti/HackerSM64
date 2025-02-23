#include <ultra64.h>

#include <string.h>
#include <stdarg.h>

#include "types.h"
#include "sm64.h"

#include "crash_screen/crash_controls.h"
#include "crash_screen/crash_draw.h"
#include "crash_screen/crash_main.h"
#include "crash_screen/crash_pages.h"
#include "crash_screen/crash_print.h"
#include "crash_screen/crash_settings.h"

#include "page_log.h"

#include "game/assert.h"
#include "game/debug.h"
#include "game/puppyprint.h"


struct CSSetting cs_settings_group_page_log[] = {
    [CS_OPT_HEADER_PAGE_LOG     ] = { .type = CS_OPT_TYPE_HEADER,  .name = "LOG",                            .valNames = &gValNames_bool,          .val = SECTION_EXPANDED_DEFAULT,  .defaultVal = SECTION_EXPANDED_DEFAULT,  .lowerBound = FALSE,                 .upperBound = TRUE,                       },
    [CS_OPT_LOG_INDEX_NUMBERS   ] = { .type = CS_OPT_TYPE_SETTING, .name = "Show index numbers",             .valNames = &gValNames_bool,          .val = TRUE,                      .defaultVal = TRUE,                      .lowerBound = FALSE,                 .upperBound = TRUE,                       },
    [CS_OPT_END_LOG             ] = { .type = CS_OPT_TYPE_END },
};


const enum ControlTypes cs_cont_list_log[] = {
    CONT_DESC_SWITCH_PAGE,
    CONT_DESC_SHOW_CONTROLS,
    CONT_DESC_CYCLE_DRAW,
    CONT_DESC_SCROLL_LIST,
    CONT_DESC_LIST_END,
};


static u32 sLogSelectedIndex = 0;
static u32 sLogViewportIndex = 0;
static u32 sLogNumShownRows = LOG_BUFFER_SIZE;
static u32 sLogTotalRows    = LOG_BUFFER_SIZE;

void log_init(void) {
    sLogSelectedIndex = 0;
    sLogViewportIndex = 0;

    sLogNumShownRows = LOG_BUFFER_SIZE;
    sLogTotalRows    = LOG_BUFFER_SIZE;
}

// Draws the red background for the assert section.
static void draw_assert_highlight(u32 line) {
    //! Prints the assert message early, but with 0 alpha (skips framebuffer writes).
    //   This is a hacky way to get the amount of lines the wrapped assert text will be.
    //   This can't be done after the normal print because it would show up in front of the text.
    cs_print(TEXT_X(0), TEXT_Y(line),
        STR_COLOR_PREFIX"MESSAGE:%s",
        COLOR_RGBA32_NONE, __n64Assert_Message
    );
    cs_draw_rect(CRASH_SCREEN_X1, (DIVIDER_Y(line) + 1), CRASH_SCREEN_W, TEXT_HEIGHT(4 + gCSNumLinesPrinted), RGBA32_SET_ALPHA(COLOR_RGBA32_RED, 0x7F));
}

u32 print_assert_section(u32 line) {
    u32 charX = 0;

    draw_assert_highlight(line);

    // "ASSERT:"
    cs_print(TEXT_X(0), TEXT_Y(line), STR_COLOR_PREFIX"ASSERT:", COLOR_RGBA32_CRASH_HEADER);
    line++;
    cs_draw_divider(DIVIDER_Y(line));

    size_t lineStrStart = (CRASH_SCREEN_NUM_CHARS_X - STRLEN("LINE:0000"));
    // "FILE: [file name]"
    charX += cs_print(TEXT_X(0), TEXT_Y(line),
        STR_COLOR_PREFIX"FILE:",
        COLOR_RGBA32_CRASH_HEADER
    );
    charX += cs_print_scroll(TEXT_X(charX), TEXT_Y(line), (lineStrStart - charX),
        STR_COLOR_PREFIX"%s",
        COLOR_RGBA32_CRASH_FILE_NAME, __n64Assert_Filename
    );
    // "LINE:[line number]"
    cs_print(TEXT_X(lineStrStart), TEXT_Y(line),
        STR_COLOR_PREFIX"LINE:"STR_COLOR_PREFIX"%d",
        COLOR_RGBA32_CRASH_HEADER,
        COLOR_RGBA32_CRASH_FILE_NAME, __n64Assert_LineNum
    );
    line++;

    // "COND:[condition]"
    if (__n64Assert_Condition != NULL) {
        charX = 0;
        charX += cs_print(TEXT_X(charX), TEXT_Y(line),
            STR_COLOR_PREFIX"COND:", COLOR_RGBA32_CRASH_HEADER
        );
        charX += cs_print_scroll(TEXT_X(charX), TEXT_Y(line),
            (CRASH_SCREEN_NUM_CHARS_X - charX),
            STR_COLOR_PREFIX"%s",
            COLOR_RGBA32_CRASH_AT, __n64Assert_Condition
        );
        line++;
    }

    // "MESSAGE:[message]"
    cs_print(TEXT_X(0), TEXT_Y(line),
        STR_COLOR_PREFIX"MESSAGE:"STR_COLOR_PREFIX"%s",
        COLOR_RGBA32_CRASH_HEADER,
        gCSDefaultPrintColor, __n64Assert_Message
    );
    line += gCSNumLinesPrinted;

    osWritebackDCacheAll();

    return line;
}

#ifdef PUPPYPRINT_DEBUG
void draw_log_section(u32 line, u32 numLines) {
    const _Bool showIndexNumbers = cs_get_setting_val(CS_OPT_GROUP_PAGE_LOG, CS_OPT_LOG_INDEX_NUMBERS);

    // "PUPPYPRINT LOG:"
    cs_print(TEXT_X(0), TEXT_Y(line), STR_COLOR_PREFIX"PUPPYPRINT LOG:", COLOR_RGBA32_CRASH_HEADER);
    line++;

    // Print entries:
    for (u32 y = 0; y < numLines; y++) {
        u32 printIndex = (sLogViewportIndex + y);
        char* entry = consoleLogTable[(LOG_BUFFER_SIZE - 1) - printIndex];

        if (entry[0] == CHAR_NULL) {
            break;
        }

        u32 charY = TEXT_Y(line + y);

        if (printIndex == sLogSelectedIndex) {
            cs_draw_row_selection_box(charY);
        }

        if (showIndexNumbers) {
            cs_print(TEXT_X(0), charY, "%i:\t%s", ((gConsoleLogLastIndex - 1) - printIndex), entry);
        } else {
            cs_print(TEXT_X(0), charY, "%s", entry);
        }
    }

    cs_draw_divider(DIVIDER_Y(line));

    if (sLogTotalRows > sLogNumShownRows) {
        cs_draw_scroll_bar((DIVIDER_Y(line) + 1), DIVIDER_Y(CRASH_SCREEN_NUM_CHARS_Y), sLogNumShownRows, sLogTotalRows, sLogViewportIndex, COLOR_RGBA32_CRASH_DIVIDER, TRUE);
        cs_draw_divider(DIVIDER_Y(CRASH_SCREEN_NUM_CHARS_Y));
    }

    osWritebackDCacheAll();
}
#endif

void log_draw(void) {
    u32 line = 2;

    gCSWordWrap = TRUE;

    if (__n64Assert_Message != NULL) {
        line = print_assert_section(line);
#ifdef PUPPYPRINT_DEBUG
        line++;
        cs_draw_divider(DIVIDER_Y(line));
#else
    } else {
        cs_print(TEXT_X(0), TEXT_Y(line), "No log or assert data.");
#endif
    }

#ifdef PUPPYPRINT_DEBUG
    sLogNumShownRows = ((CRASH_SCREEN_NUM_CHARS_Y - line) - 1);
    sLogTotalRows = MIN(gConsoleLogLastIndex, LOG_BUFFER_SIZE);

    draw_log_section(line, sLogNumShownRows);
#endif

    gCSWordWrap = FALSE;

    osWritebackDCacheAll();
}

void log_input(void) {
    if (gCSDirectionFlags.pressed.up) {
        // Scroll up.
        if (sLogSelectedIndex > 0) {
            sLogSelectedIndex--;
        }
    }
    if (gCSDirectionFlags.pressed.down) {
        // Scroll down.
        if (sLogSelectedIndex < (sLogTotalRows - 1)) {
            sLogSelectedIndex++;
        }
    }

    if (sLogTotalRows > sLogNumShownRows) {
        sLogViewportIndex = cs_clamp_view_to_selection(sLogViewportIndex, sLogSelectedIndex, sLogNumShownRows, 1);
    }
}


struct CSPage gCSPage_log = {
    .name         = "LOG",
    .initFunc     = log_init,
    .drawFunc     = log_draw,
    .inputFunc    = log_input,
    .contList     = cs_cont_list_log,
    .settingsList = cs_settings_group_page_log,
    .flags = {
        .initialized = FALSE,
        .crashed     = FALSE,
        .printName   = TRUE,
    },
};
