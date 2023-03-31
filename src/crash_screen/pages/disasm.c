#include <ultra64.h>
#include <stdarg.h>
#include <string.h>
#include "types.h"
#include "sm64.h"
#include "crash_screen/crash_screen.h"
#include "disasm.h"
#include "engine/colors.h"
#include "game/debug.h"
#include "game/game_input.h"

static _Bool sDisasmShowDestFunctionNames = TRUE;
static _Bool sDisasmShowDataAsBinary = FALSE;

_Bool gFillBranchBuffer = FALSE;

static const RGBA32 sBranchColors[] = {
    COLOR_RGBA32_ORANGE,
    COLOR_RGBA32_LIME,
    COLOR_RGBA32_CYAN,
    COLOR_RGBA32_MAGENTA,
    COLOR_RGBA32_YELLOW,
    COLOR_RGBA32_PINK,
    COLOR_RGBA32_LIGHT_GRAY,
    COLOR_RGBA32_LIGHT_BLUE,
};

#ifdef INCLUDE_DEBUG_MAP
static _Bool sContinueFillBranchBuffer = FALSE;

ALIGNED16 static struct BranchArrow sBranchArrows[DISASM_BRANCH_BUFFER_SIZE];
static u32 sNumBranchArrows = 0;

static uintptr_t sBranchBufferCurrAddr = 0x00000000;

void reset_branch_buffer(UNUSED const char *fname, uintptr_t funcAddr) {
    bzero(sBranchArrows, sizeof(sBranchArrows));
    sNumBranchArrows = 0;

    sBranchBufferCurrAddr = funcAddr;
}

//! TODO: Optimize this as much as possible
//! TODO: Version that works without INCLUDE_DEBUG_MAP (check for branches relative to viewport)
s32 crash_screen_fill_branch_buffer(const char *fname, uintptr_t funcAddr) {
    if (fname == NULL) {
        return FALSE;
    }

    s16 curBranchColorIndex;
    s32 curBranchX;

    if (sNumBranchArrows == 0) {
        // Start:
        curBranchColorIndex = 0;
        curBranchX = DISASM_BRANCH_ARROW_OFFSET;
    } else { //! TODO: Verify that this ordering is correct:
        // Continue:
        curBranchColorIndex = sBranchArrows[sNumBranchArrows - 1].colorIndex;
        curBranchX          = sBranchArrows[sNumBranchArrows - 1].xPos;
    }

    struct BranchArrow* currArrow = &sBranchArrows[sNumBranchArrows];

    OSTime startTime = osGetTime();
    while (TRUE) {
        // Too many entries searched.
        if (sBranchBufferCurrAddr > (funcAddr + DISASM_FUNCTION_SEARCH_MAX_OFFSET)) {
            return FALSE;
        }

        // Too many arrows for buffer.
        if (sNumBranchArrows >= DISASM_BRANCH_BUFFER_SIZE) {
            return FALSE;
        }

        // Left the function.
        uintptr_t checkAddr = sBranchBufferCurrAddr;
        if (fname != parse_map(&checkAddr)) {
            return FALSE;
        }

        // Get the offset for the current function;
        InsnData insn = (InsnData){ .raw = *(uintptr_t*)sBranchBufferCurrAddr };
        s16 branchOffset = get_branch_offset(insn);

        if (branchOffset != 0) { //! TODO: Verify ordering:
            curBranchX += DISASM_BRANCH_ARROW_SPACING;
            curBranchColorIndex = ((curBranchColorIndex + 1) % ARRAY_COUNT(sBranchColors));

            // Wrap around if extended past end of screen.
            if ((DISASM_BRANCH_ARROW_START_X + curBranchX) > CRASH_SCREEN_TEXT_X2) {
                curBranchX = DISASM_BRANCH_ARROW_OFFSET;
            }

            currArrow->startAddr    = sBranchBufferCurrAddr;
            currArrow->branchOffset = branchOffset;
            currArrow->colorIndex   = curBranchColorIndex;
            currArrow->xPos         = curBranchX;

            currArrow++;
            sNumBranchArrows++;
        }

        sBranchBufferCurrAddr += DISASM_STEP;

        // Took to long, so pause until next frame.
        if ((osGetTime() - startTime) > FRAMES_TO_CYCLES(1)) {
            return TRUE;
        }
    }

    return FALSE;
}
#endif

void draw_branch_arrow(s32 startLine, s32 endLine, s32 dist, RGBA32 color, s32 printLine) {
    // Check to see if arrow is fully away from the screen.
    if (
        ((startLine >= 0              ) || (endLine >= 0              )) &&
        ((startLine <  DISASM_NUM_ROWS) || (endLine <  DISASM_NUM_ROWS))
    ) {
        s32 arrowStartHeight = (TEXT_Y(printLine + startLine) + 3);
        s32 arrowEndHeight   = (TEXT_Y(printLine +   endLine) + 3);

        if (startLine < 0) {
            arrowStartHeight = (TEXT_Y(printLine) - 1);
        } else if (startLine >= DISASM_NUM_ROWS) {
            arrowStartHeight = (TEXT_Y(printLine + DISASM_NUM_ROWS) - 2);
        } else {
            crash_screen_draw_rect((DISASM_BRANCH_ARROW_START_X + 1), arrowStartHeight, dist, 1, color);
        }

        if (endLine < 0) {
            arrowEndHeight = (TEXT_Y(printLine) - 1);
        } else if (endLine >= DISASM_NUM_ROWS) {
            arrowEndHeight = (TEXT_Y(printLine + DISASM_NUM_ROWS) - 2);
        } else {
            u32 x = ((DISASM_BRANCH_ARROW_START_X + dist) - DISASM_BRANCH_ARROW_OFFSET);
            crash_screen_draw_rect((x + 0), (arrowEndHeight - 0), (DISASM_BRANCH_ARROW_OFFSET + 1), 1, color);
            // Arrow head
            crash_screen_draw_rect((x + 1), (arrowEndHeight - 1), 1, 3, color);
            crash_screen_draw_rect((x + 2), (arrowEndHeight - 2), 1, 5, color);
        }

        s32 height = abss(arrowEndHeight - arrowStartHeight);

        // Middle of arrow
        crash_screen_draw_rect((DISASM_BRANCH_ARROW_START_X + dist), MIN(arrowStartHeight, arrowEndHeight), 1, height, color);
    }
}

void disasm_draw_branch_arrows(s32 printLine) {
    // Draw branch arrows from the buffer.
    struct BranchArrow *currArrow = &sBranchArrows[0];

    for (u32 i = 0; i < sNumBranchArrows; i++) {
        s32 startLine = (((s32)currArrow->startAddr - (s32)gScrollAddress) / DISASM_STEP);
        s32 endLine = (startLine + currArrow->branchOffset + 1);

        draw_branch_arrow(startLine, endLine, currArrow->xPos, sBranchColors[currArrow->colorIndex], printLine);

        currArrow++;
    }

    osWritebackDCacheAll();
}

void print_as_binary(s32 charX, s32 charY, u32 data) { //! TODO: make this a formatting char, maybe \%b?
    s32 bitX = charX;

    for (u32 c = 0; c < 32; c++) {
        if ((c % 8) == 0) { // Space between each byte.
            bitX += TEXT_WIDTH(1);
        }

        crash_screen_draw_glyph(bitX, charY, (((data >> (32 - c)) % 2) ? '1' : '0'), COLOR_RGBA32_WHITE);

        bitX += TEXT_WIDTH(1);
    }
}

void disasm_draw_asm_entries(u32 line, uintptr_t selectedAddr, uintptr_t pc) {
    u32 charX = TEXT_X(0);
    u32 charY = TEXT_Y(line);

    gCrashScreenWordWrap = FALSE;

    for (u32 y = 0; y < DISASM_NUM_ROWS; y++) {
        uintptr_t addr = (gScrollAddress + (y * DISASM_STEP));
        InsnData insn;
        insn.raw = *(uintptr_t*)addr;

        charY = TEXT_Y(line + y);

        if (addr == pc) {
            crash_screen_draw_rect((charX - 1), (charY - 2), (CRASH_SCREEN_TEXT_W + 1), (TEXT_HEIGHT(1) + 1), COLOR_RGBA32_CRASH_PC);
        } else if (addr == selectedAddr) {
            crash_screen_draw_rect((charX - 1), (charY - 2), (CRASH_SCREEN_TEXT_W + 1), (TEXT_HEIGHT(1) + 1), COLOR_RGBA32_CRASH_SELECT);
        }

        if (is_in_code_segment(addr)) {
#ifndef INCLUDE_DEBUG_MAP
            s16 branchOffset = get_branch_offset(insn);
            if (branchOffset != 0) {
                s32 startLine = (((s32)addr - (s32)gScrollAddress) / DISASM_STEP);
                s32 endLine = (startLine + branchOffset + 1);
                draw_branch_arrow(startLine, endLine, DISASM_BRANCH_ARROW_OFFSET, COLOR_RGBA32_CRASH_FUNCTION_NAME_2, line);
            }
#endif
            const char *destFname = NULL;
            const char *insnAsStr = insn_disasm(insn, &destFname, sDisasmShowDestFunctionNames);
            crash_screen_print(charX, charY, "%s", insnAsStr);
            if (addr == pc) {
                char crashStr[] = "<-- CRASH";
                crash_screen_print((CRASH_SCREEN_TEXT_X2 - TEXT_WIDTH(strlen(crashStr))), charY, STR_COLOR_PREFIX"%s", COLOR_RGBA32_CRASH_AT, crashStr);
            }
#ifdef INCLUDE_DEBUG_MAP
            if (sDisasmShowDestFunctionNames && destFname != NULL) {
                crash_screen_print_scroll((charX + TEXT_WIDTH(7)), charY, (CRASH_SCREEN_NUM_CHARS_X - 7), STR_COLOR_PREFIX"%s", sDisasmColors[DISASM_COLOR_ADDRESS], destFname);
            }
#endif
        } else { // Outside of code segments:
            if (sDisasmShowDataAsBinary) {
                print_as_binary(charX, charY, insn.raw);
            } else {
                crash_screen_print(charX, charY, STR_HEX_WORD, insn.raw);
            }
        }
    }

    gCrashScreenWordWrap = TRUE;

    osWritebackDCacheAll();
}

//! TODO: automatically check page change:
// uintptr_t sCurrFuncAddr = 0x00000000;
// const char *sCurrFuncName = NULL;

void draw_disasm(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    const char *fname = NULL;
    uintptr_t alignedSelectedAddr = (gSelectedAddress & ~(DISASM_STEP - 1));

#ifdef INCLUDE_DEBUG_MAP
    uintptr_t funcAddr = alignedSelectedAddr;
    fname = parse_map(&funcAddr);

    //! TODO: Do this outside of the draw function:
    if (gCrashScreenSwitchedPage) {
        gFillBranchBuffer = TRUE;
    }

    if (gFillBranchBuffer) {
        gFillBranchBuffer = FALSE;
        reset_branch_buffer(fname, funcAddr);
        sContinueFillBranchBuffer = TRUE;
    }

    if (sContinueFillBranchBuffer) {
        sContinueFillBranchBuffer = crash_screen_fill_branch_buffer(fname, funcAddr);
        gCrashScreenUpdateFramebuffer = TRUE;
    }
#endif

    clamp_view_to_selection(DISASM_NUM_ROWS, DISASM_STEP);

    u32 line = 1;

    line += crash_screen_print(TEXT_X(0), TEXT_Y(line),
        (STR_COLOR_PREFIX"%s "STR_COLOR_PREFIX STR_HEX_WORD" in "STR_HEX_WORD"-"STR_HEX_WORD),
        COLOR_RGBA32_CRASH_PAGE_NAME, gCrashScreenPages[PAGE_DISASM].name,
        COLOR_RGBA32_WHITE, alignedSelectedAddr, gScrollAddress, (gScrollAddress + DISASM_SHOWN_SECTION)
    );
    crash_screen_draw_divider(DIVIDER_Y(line));

    if (fname == NULL) {
        line += crash_screen_print(TEXT_X(0), TEXT_Y(line), "NOT IN A FUNCTION");
    } else {
        line += crash_screen_print_scroll(TEXT_X(0), TEXT_Y(line),
            (CRASH_SCREEN_NUM_CHARS_X - 3), "IN:"STR_COLOR_PREFIX"%s",
            COLOR_RGBA32_CRASH_FUNCTION_NAME, fname
        );
    }

    osWritebackDCacheAll();

#ifdef INCLUDE_DEBUG_MAP
    disasm_draw_branch_arrows(line);
#endif

    disasm_draw_asm_entries(line, alignedSelectedAddr, tc->pc);

    crash_screen_draw_divider(DIVIDER_Y(line));

    u32 line2 = (line + DISASM_NUM_ROWS);

    crash_screen_draw_divider(DIVIDER_Y(line2));

    // Scroll bar
    crash_screen_draw_scroll_bar(DIVIDER_Y(line), DIVIDER_Y(line2), DISASM_SHOWN_SECTION, TOTAL_RAM_SIZE, (gScrollAddress - DISASM_SCROLL_MIN), 4, COLOR_RGBA32_LIGHT_GRAY);

    // Scroll bar crash position marker
    crash_screen_draw_scroll_bar(DIVIDER_Y(line), DIVIDER_Y(line2), DISASM_SHOWN_SECTION, TOTAL_RAM_SIZE, (tc->pc - DISASM_SCROLL_MIN), 1, COLOR_RGBA32_CRASH_AT);

    osWritebackDCacheAll();
}

const enum ControlTypes disasmPageControls[] = {
    CONT_DESC_SWITCH_PAGE,
    CONT_DESC_SHOW_CONTROLS,
    CONT_DESC_CYCLE_DRAW,
    CONT_DESC_CURSOR_VERTICAL,
    CONT_DESC_JUMP_TO_ADDRESS,
    CONT_DESC_TOGGLE_FUNCTIONS,
    CONT_DESC_LIST_END,
};

void crash_screen_input_disasm(void) {
#ifdef INCLUDE_DEBUG_MAP
    uintptr_t oldPos = gSelectedAddress;
#endif

    // gSelectedAddress = ALIGN(gSelectedAddress, DISASM_STEP);
    if ((gCrashScreenDirectionFlags.pressed.up)
     && ((gSelectedAddress - DISASM_STEP) >= RAM_START)) {
        // Scroll up.
        gSelectedAddress -= DISASM_STEP;
        gCrashScreenUpdateFramebuffer = TRUE;
    }

    if ((gCrashScreenDirectionFlags.pressed.down)
     && ((gSelectedAddress + DISASM_STEP) < RAM_END)) {
        // Scroll down.
        gSelectedAddress += DISASM_STEP;
        gCrashScreenUpdateFramebuffer = TRUE;
    }

    if (gPlayer1Controller->buttonPressed & A_BUTTON) { //! TODO: not if address select was just closed
        open_address_select(get_branch_target_from_addr(gSelectedAddress));
    }

    if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        toggle_display_var(&sDisasmShowDestFunctionNames);
        toggle_display_var(&sDisasmShowDataAsBinary);
    }

#ifdef INCLUDE_DEBUG_MAP
    if (!is_in_same_function(oldPos, gSelectedAddress)) {
        gFillBranchBuffer = TRUE;
    }
#endif
}
