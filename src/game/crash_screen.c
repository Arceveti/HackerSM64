#include <ultra64.h>
#include <PR/os_internal_error.h>
#include <stdarg.h>
#include <string.h>
#include "buffers/framebuffers.h"
#include "types.h"
#include "puppyprint.h"
#include "audio/external.h"
#include "farcall.h"
#include "game_init.h"
#include "main.h"
#include "debug.h"
#include "rumble_init.h"
#include "engine/colors.h"

#include "sm64.h"

#include "printf.h"

enum crashPages {
    PAGE_CONTEXT,
#ifdef PUPPYPRINT_DEBUG
    PAGE_LOG,
#endif
    PAGE_STACKTRACE,
    PAGE_DISASM,
    PAGE_ASSERTS,
    PAGE_CONTROLS,
    PAGE_COUNT,
    PAGES_MAX = 255,
};

enum AnalogFlags {
    ANALOG_FLAGS_NONE = 0x0,
    ANALOG_FLAG_LEFT  = BIT(0),
    ANALOG_FLAG_RIGHT = BIT(1),
    ANALOG_FLAG_UP    = BIT(2),
    ANALOG_FLAG_DOWN  = BIT(3),
};

// A height of 7 pixels for each character * 26 rows of characters + 1 row unused.
ALIGNED8 u32 gCrashScreenFont[7 * 26 + 1] = {
    #include "textures/crash_screen/crash_screen_font.custom.ia1.inc.c"
};

#define STACK_SIZE 256 // (s32)(0x800 / sizeof(u64))

struct FunctionInStack {
    u32 addr;
    char *name;
};

struct FunctionInStack sAllFunctionStack[STACK_SIZE];
struct FunctionInStack sKnownFunctionStack[STACK_SIZE];
static s32 sNumKnownFunctions = 0;
static s32 sNumShownFunctions = STACK_SIZE;

static s8 sAnalogFlags = ANALOG_FLAGS_NONE;
static s8 sDrawCrashScreen = TRUE;
static s8 sDrawFrameBuffer = TRUE;
static s8 sStackTraceShowNames = TRUE;
static s8 sStackTraceSkipUnknowns = FALSE;
static u32 sProgramPosition = 0;
static s32 sStackTraceIndex = 0;

u8 sCrashPage = PAGE_CONTEXT;
u8 sUpdateBuffer = TRUE;


char *gCauseDesc[18] = {
    "Interrupt",
    "TLB modification",
    "TLB exception on load or inst.",
    "TLB exception on store",
    "Address error on load or inst.",
    "Address error on store",
    "Bus error on inst.",
    "Bus error on data",
    "Failed Assert: See Assert Page",
    "Breakpoint exception",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap exception",
    "Virtual coherency on inst.",
    "Floating point exception",
    "Watchpoint exception",
    "Virtual coherency on data",
};

char *gFpcsrDesc[6] = {
    "Unimplemented operation",
    "Invalid operation",
    "Division by zero",
    "Overflow",
    "Underflow",
    "Inexact operation",
};

char *gRegNames[] = {
    "AT", "V0", "V1", "A0", "A1", "A2", "A3",
    "T0", "T1", "T2", "T3", "T4", "T5", "T6", "T7",
    "S0", "S1", "S2", "S3", "S4", "S5", "S6", "S7",
    "T8", "T9",             "GP", "SP", "S8", "RA",
};


extern u64 osClockRate;
extern far char *parse_map(u32 pc);
extern far void map_data_init(void);
extern far char *find_function_in_stack(u32 *sp);

struct {
    OSThread thread;
    u64 stack[0x800 / sizeof(u64)];
    OSMesgQueue mesgQueue;
    OSMesg mesg;
} gCrashScreen;

#ifdef CRASH_SCREEN_CRASH_SCREEN
struct {
    OSThread thread;
    u64 stack[0x800 / sizeof(u64)];
    OSMesgQueue mesgQueue;
    OSMesg mesg;
} gCrashScreen2;
#endif // CRASH_SCREEN_CRASH_SCREEN

extern u16 sRenderedFramebuffer;
extern u16 sRenderingFramebuffer;
u16 sScreenshotFrameBuffer;

void crash_screen_draw_rect(s32 x, s32 y, s32 w, s32 h, RGBA16 color, s32 isTransparent) {
    s32 i, j;

    RGBA16 *ptr = gFramebuffers[sRenderingFramebuffer] + (SCREEN_WIDTH * y) + x;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (isTransparent) {
                *ptr = (((*ptr & color) >> 2) | 0x1);
            } else {
                *ptr = color;
            }
            ptr++;
        }

        ptr += SCREEN_WIDTH - w;
    }
}

void crash_screen_draw_glyph(s32 x, s32 y, s32 glyph, RGBA16 color) {
    u32 bit;
    u32 rowMask;
    s32 i, j;

    u32 *data = &gCrashScreenFont[glyph / 5 * 7];

    RGBA16 *ptr = gFramebuffers[sRenderingFramebuffer] + (SCREEN_WIDTH * y) + x;

    for (i = 0; i < 7; i++) {
        bit = (0x80000000U >> ((glyph % 5) * 6));
        rowMask = *data++;

        for (j = 0; j < 6; j++) {
            if (bit & rowMask) {
                *ptr = color;
            }
            ptr++;
            bit >>= 1;
        }

        ptr += SCREEN_WIDTH - 6;
    }
}

static char *write_to_buf(char *buffer, const char *data, size_t size) {
    return (char *) memcpy(buffer, data, size) + size;
}

u32 index_to_hex(u32 glyph) {
    u32 ret = 0;
    if (glyph >= '0' && glyph <= '9') {
        ret = (glyph - '0');
    } else if (glyph >= 'A' && glyph <= 'F') {
        ret = (glyph - 'A') + 10;
    } else if (glyph >= 'a' && glyph <= 'f') {
        ret = (glyph - 'a') + 10;
    }
    return (ret & 0xF);
}

void crash_screen_print(s32 x, s32 y, const char *fmt, ...) {
    u32 glyph;
    char buf[0x100];
    bzero(&buf, sizeof(buf));

    va_list args;
    va_start(args, fmt);

    s32 size = _Printf(write_to_buf, buf, fmt, args);

    RGBA16 color = COLOR_RGBA16_WHITE;

    if (size > 0) {
        char *ptr = buf;

        while (*ptr) {
            glyph = (*ptr & 0x7f);

            if (glyph == '@') {
                ptr++;
                if (!*ptr) {
                    break;
                }
                s32 i, j;
                Color component = 0;
                ColorRGBA rgba = { 0, 0, 0, 0 };
                color = 0;
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 2; j++) {
                        if (!*ptr) {
                            break;
                        }
                        glyph = (*ptr & 0x7f);
                        component |= (index_to_hex(glyph) << ((1 - j) * 4));
                        ptr++;
                    }
                    rgba[i] = component;
                    component = 0;
                }

                color = GPACK_RGBA5551(rgba[0], rgba[1], rgba[2], rgba[3]);
            } else {
                if (glyph != 0xff) {
                    crash_screen_draw_glyph(x, y, glyph, color);
                }

                ptr++;
                x += 6;
            }
        }
    }

    va_end(args);
}

void crash_screen_sleep(s32 ms) {
    u64 cycles = ms * 1000LL * osClockRate / 1000000ULL;
    osSetTime(0);
    while (osGetTime() < cycles) { }
}

void crash_screen_print_float_reg(s32 x, s32 y, s32 regNum, void *addr) {
    u32 bits = *(u32 *) addr;
    s32 exponent = ((bits & 0x7f800000U) >> 0x17) - 0x7F;

    if ((exponent >= -0x7E && exponent <= 0x7F) || bits == 0x0) {
        f32 val = *(f32 *) addr;
        if (val >= 0) {
            crash_screen_print(x, y, "@3FC07FFFF%02d: @FFFFFFFF%.3e",  regNum, *(f32 *) addr);
        } else {
            crash_screen_print(x, y, "@3FC07FFFF%02d:@FFFFFFFF%.3e",  regNum, *(f32 *) addr);
        }
    } else {
        crash_screen_print(x, y, "@3FC07FFFF%02d:@FFFFFFFF%08XD", regNum, *(u32 *) addr);
    }
}

void crash_screen_print_fpcsr(u32 fpcsr) {
    s32 i;
    u32 bit = BIT(17);

    crash_screen_print(30, 155, "@3FC07FFFFPCSR:@FFFFFFFF%08X", fpcsr);
    for (i = 0; i < 6; i++) {
        if (fpcsr & bit) {
            crash_screen_print(132, 155, "@FF3F00FF(%s)", gFpcsrDesc[i]);
            return;
        }
        bit >>= 1;
    }
}

void crash_screen_print_registers(__OSThreadContext *tc) {
    s32 regNum = 0;
    u64 *reg = &tc->at;

    crash_screen_print(30, 40, "@3FC07FFFPC:@FFFFFFFF%08X    @3FC07FFFSR:@FFFFFFFF%08X    @3FC07FFFVA:@FFFFFFFF%08X", (u32) tc->pc, (u32) tc->sr, (u32) tc->badvaddr);

    for (s32 y = 0; y < 10; y++) {
        for (s32 x = 0; x < 3; x++) {
            crash_screen_print((30 + (x * 90)), (50 + (y * 10)), "@3FC07FFF%s:@FFFFFFFF%08X", gRegNames[regNum], (u32) *(reg + regNum));
            regNum++;

            if ((reg + regNum) > &tc->ra) {
                return;
            }
        }
    }
}

void crash_screen_print_float_registers(__OSThreadContext *tc) {
    s32 regNum = 0;
    __OSfp *osfp = &tc->fp0;

    crash_screen_print_fpcsr(tc->fpcsr);

    for (s32 y = 0; y < 6; y++) {
        for (s32 x = 0; x < 3; x++) {
            crash_screen_print_float_reg((30 + (x * 90)), (170 + (y * 10)), regNum, &osfp->f.f_even);

            osfp++;
            regNum += 2;

            if (regNum > 30) {
                return;
            }
        }
    }
}

void draw_crash_context(OSThread *thread, s32 cause) {
    __OSThreadContext *tc = &thread->context;
    crash_screen_print(30, 20, "@7F7FFFFFTHREAD:%d", thread->id);
    crash_screen_print(90, 20, "@FF3F00FF%s", gCauseDesc[cause]);

    osWritebackDCacheAll();

    if ((u32)parse_map != MAP_PARSER_ADDRESS) {
        char *fname = parse_map(tc->pc);
        crash_screen_print(30, 30, "@FF7F7FFFCRASH AT:");
        if (fname == NULL) {
            crash_screen_print(90, 30, "&@7F7F7FFFUNKNOWN");
        } else {
            crash_screen_print(90, 30, "@FFFF7FFF%s", fname);
        }
    }

    crash_screen_print_registers(tc);

    osWritebackDCacheAll();

    crash_screen_print_float_registers(tc);
}

#ifdef PUPPYPRINT_DEBUG
void draw_crash_log(void) {
    s32 i;
    osWritebackDCacheAll();
 #define LINE_HEIGHT (25 + ((LOG_BUFFER_SIZE - 1) * 10))
    for (i = 0; i < LOG_BUFFER_SIZE; i++) {
        crash_screen_print(30, (LINE_HEIGHT - (i * 10)), consoleLogTable[i]);
    }
 #undef LINE_HEIGHT
}
#endif

#define STACK_TRACE_NUM_ROWS 18

// prints any function pointers it finds in the stack format:
// SP address: function name
void draw_stacktrace(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    u32 temp_sp = (tc->sp + 0x14);
    s32 currIndex;
    u32 faddr;
    char *fname;
    struct FunctionInStack *functionList = (sStackTraceSkipUnknowns ? sKnownFunctionStack : sAllFunctionStack);
    struct FunctionInStack *function = NULL;

    crash_screen_print(30, 20, "STACK TRACE FROM %08X:", temp_sp);
    crash_screen_print(30, 30, "@FF7F7FFFCURRFUNC:");
    if ((u32) parse_map == MAP_PARSER_ADDRESS) {
        crash_screen_print(84, 30, "NONE");
    } else {
        crash_screen_print(84, 30, "@FFFF7FFF%s", parse_map(tc->pc));
    }

    osWritebackDCacheAll();

    // Print
    for (s32 j = 0; j < STACK_TRACE_NUM_ROWS; j++) {
        s32 y = (40 + (j * 10));

        if ((u32) find_function_in_stack == MAP_PARSER_ADDRESS) {
            crash_screen_print(30, y, "STACK TRACE DISABLED");
            break;
        }

        currIndex = sStackTraceIndex + j;

        if (currIndex >= sNumShownFunctions) {
            break;
        }

        function = &functionList[currIndex];

        faddr = function->addr;
        fname = function->name;

        crash_screen_print(30, y, "%08X:", faddr);

        if (!sStackTraceSkipUnknowns && ((fname == NULL) || ((*(u32*)faddr & 0x80000000) == 0))) {
            // Print unknown function
            crash_screen_print(84, y, "@C0C0C0FF%08X", *(u32*)faddr);
        } else {
            // Print known function
            if (sStackTraceShowNames) {
                crash_screen_print(84, y, "@FFFFC0FF%s", fname);
            } else {
                crash_screen_print(84, y, "@FFFFC0FF%08X", *(u32*)faddr);
            }
        }
    }

    osWritebackDCacheAll();

    // Scroll bar
    const s32 totalHeight = 180;
    s32 height;
    if (sNumShownFunctions <= STACK_TRACE_NUM_ROWS) {
        height = totalHeight;
    } else {
        height = MAX(10, (totalHeight / (sNumShownFunctions - STACK_TRACE_NUM_ROWS)));
    }
    const s32 moveHeight = totalHeight - height;
    const s32 scaledPos = (sStackTraceIndex * moveHeight / (sNumShownFunctions - STACK_TRACE_NUM_ROWS));

    if ((scaledPos >= 0) && (scaledPos <= moveHeight)) {
        crash_screen_draw_rect(294, (38 + scaledPos), 1, height, COLOR_RGBA16_LIGHT_GRAY, FALSE);
    }

    crash_screen_draw_rect(25,  28, 270, 1, COLOR_RGBA16_LIGHT_GRAY, FALSE);
    crash_screen_draw_rect(25,  38, 270, 1, COLOR_RGBA16_LIGHT_GRAY, FALSE);
    crash_screen_draw_rect(25, 218, 270, 1, COLOR_RGBA16_LIGHT_GRAY, FALSE);
    crash_screen_print( 30, 220, "@C0C0C0FFup/down:scroll    toggle: a:names b:unknowns");

    osWritebackDCacheAll();
}

extern char *insn_disasm(u32 insn, u32 isPC);
void draw_disasm(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    // u32 insn = *(u32*)tc->pc;

    if (sProgramPosition == 0) {
        sProgramPosition = (tc->pc - 36);
    }

    crash_screen_print(30, 20, "DISASM %08X", sProgramPosition);

    osWritebackDCacheAll();

    for (int i = 0; i < 19; i++) {
        u32 addr = (sProgramPosition + (i * 4));
        u32 toDisasm = *(u32*)(addr);

        crash_screen_print(30, (30 + (i * 10)), "%s", insn_disasm(toDisasm, (addr == tc->pc)));
    }

    // Scroll bar
    u32 ramPos = (sProgramPosition - 0x80000000);
    const s32 ramSize = 0x800000;
    if (ramPos <= ramSize) {
        const s32 totalHeight = 190;
        const s32 height = 10;
        const s32 moveHeight = totalHeight - height;
        const s32 scaledPos = (ramPos * moveHeight / ramSize);
        if ((scaledPos > 0) && (scaledPos < moveHeight)) {
            crash_screen_draw_rect(294, (28 + scaledPos), 1, height, COLOR_RGBA16_LIGHT_GRAY, FALSE);
        }
    }

    crash_screen_draw_rect(25,  28, 270, 1, COLOR_RGBA16_LIGHT_GRAY, FALSE);
    crash_screen_draw_rect(25, 218, 270, 1, COLOR_RGBA16_LIGHT_GRAY, FALSE);
    crash_screen_print(30, 220, "@C0C0C0FFup/down:scroll");

    osWritebackDCacheAll();
}

void draw_assert(UNUSED OSThread *thread) {
    crash_screen_print(30, 20, "ASSERT PAGE");

    if (__n64Assert_Filename != NULL) {
        crash_screen_print(30, 30, "FILE: %s LINE %d", __n64Assert_Filename, __n64Assert_LineNum);
        crash_screen_print(30, 50, "MESSAGE:");
        crash_screen_print(30, 65, " %s", __n64Assert_Message);
    } else {
        crash_screen_print(30, 30, "no failed assert to report.");
    }

    osWritebackDCacheAll();
}

void draw_controls(UNUSED OSThread *thread) {
    s32 y = 15;
    crash_screen_print(30, (y += 10), "CRASH SCREEN CONTROLS");
    crash_screen_print(40, (y += 20), "START:");
    crash_screen_print(40, (y += 10), "@C0C0C0FFtoggle framebuffer background");
    crash_screen_print(40, (y += 20), "Z:");
    crash_screen_print(40, (y += 10), "@C0C0C0FFtoggle framebuffer only view");
    crash_screen_print(40, (y += 20), "ANALOG STICK, D-PAD, OR C BUTTONS:");
    crash_screen_print(50, (y += 20), "LEFT/RIGHT:");
    crash_screen_print(50, (y += 10), "@C0C0C0FFswitch page");
    crash_screen_print(50, (y += 20), "UP/DOWN:");
    crash_screen_print(50, (y += 10), "@C0C0C0FFscroll page");

    osWritebackDCacheAll();
}

#define FRAMEBUFFER_SIZE (SCREEN_SIZE * sizeof(RGBA16))

void crash_screen_take_screenshot(void) {
    if (gIsConsole) {
        // Save a screenshot of the game to a framebuffer that's not sRenderedFramebuffer or sRenderingFramebuffer
        sScreenshotFrameBuffer = ((sRenderingFramebuffer + 1) % 3);
        memcpy(gFramebuffers[sScreenshotFrameBuffer], gFramebuffers[sRenderingFramebuffer], FRAMEBUFFER_SIZE);
    } else {
        sScreenshotFrameBuffer = sRenderedFramebuffer;
        sRenderedFramebuffer = ((sScreenshotFrameBuffer + 1) % 3);
        sRenderingFramebuffer = ((sRenderedFramebuffer + 1) % 3);
        memcpy(gFramebuffers[sRenderingFramebuffer], gFramebuffers[sScreenshotFrameBuffer], FRAMEBUFFER_SIZE);
    }
}

void reset_crash_screen_framebuffer(void) {
    if (sDrawFrameBuffer) {
        memcpy(gFramebuffers[sRenderingFramebuffer], gFramebuffers[sScreenshotFrameBuffer], FRAMEBUFFER_SIZE);
    } else {
        crash_screen_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_RGBA16_BLACK, FALSE);
    }
}

void update_crash_screen_framebuffer(void) {
    memcpy(gFramebuffers[sRenderedFramebuffer], gFramebuffers[sRenderingFramebuffer], FRAMEBUFFER_SIZE);
}

void update_crash_screen_input(void) {
    if (gPlayer1Controller->buttonPressed & Z_TRIG) {
        sDrawCrashScreen ^= TRUE;
        sUpdateBuffer = TRUE;
    }

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        sDrawFrameBuffer ^= TRUE;
        sUpdateBuffer = TRUE;
    }

    if (!sDrawCrashScreen && !sDrawFrameBuffer) {
        sDrawCrashScreen = TRUE;
    }

    if (sDrawCrashScreen) {
        if (gPlayer1Controller->buttonPressed & (R_CBUTTONS | R_JPAD)) {
            sCrashPage++;
            sUpdateBuffer = TRUE;
        }
        if (gPlayer1Controller->buttonPressed & (L_CBUTTONS | L_JPAD)) {
            sCrashPage--;
            sUpdateBuffer = TRUE;
        }

        if (gPlayer1Controller->rawStickX > 60 && !(sAnalogFlags & ANALOG_FLAG_RIGHT)) {
            sCrashPage++;
            sUpdateBuffer = TRUE;
            sAnalogFlags |= ANALOG_FLAG_RIGHT;
        } else if (gPlayer1Controller->rawStickX < 10) {
            sAnalogFlags &= ~ANALOG_FLAG_RIGHT;
        }

        if (gPlayer1Controller->rawStickX < -60 && !(sAnalogFlags & ANALOG_FLAG_LEFT)) {
            sCrashPage--;
            sUpdateBuffer = TRUE;
            sAnalogFlags |= ANALOG_FLAG_LEFT;
        } else if (gPlayer1Controller->rawStickX > -10) {
            sAnalogFlags &= ~ANALOG_FLAG_LEFT;
        }

        s32 scrollDown = ((gPlayer1Controller->buttonDown & (D_CBUTTONS | D_JPAD))
                       || (gPlayer1Controller->rawStickY < -60));
        s32 scrollUp   = ((gPlayer1Controller->buttonDown & (U_CBUTTONS | U_JPAD))
                       || (gPlayer1Controller->rawStickY > 60));

        // Page-specific inputs.
        switch (sCrashPage) {
            case PAGE_STACKTRACE:
                if (gPlayer1Controller->buttonPressed & A_BUTTON) {
                    sStackTraceShowNames ^= TRUE;
                    sUpdateBuffer = TRUE;
                }

                if (gPlayer1Controller->buttonPressed & B_BUTTON) {
                    sStackTraceSkipUnknowns ^= TRUE;
                    sNumShownFunctions = (sStackTraceSkipUnknowns ? sNumKnownFunctions : STACK_SIZE);
                    sStackTraceIndex = 0;
                    sUpdateBuffer = TRUE;
                }

                if (scrollUp) {
                    if (sStackTraceIndex > 0) {
                        sStackTraceIndex--;
                    }
                    sUpdateBuffer = TRUE;
                }
                if (scrollDown) {
                    if (sStackTraceIndex < (sNumShownFunctions - STACK_TRACE_NUM_ROWS)) {
                        sStackTraceIndex++;
                    }
                    sUpdateBuffer = TRUE;
                }
                break;
            case PAGE_DISASM:
                if (scrollUp) {
                    sProgramPosition -= 4;
                    sUpdateBuffer = TRUE;
                }
                if (scrollDown) {
                    sProgramPosition += 4;
                    sUpdateBuffer = TRUE;
                }
                break;
        }

        if ((sCrashPage >= PAGE_COUNT) && (sCrashPage != PAGES_MAX)) {
            sCrashPage = PAGE_CONTEXT;
        }
        if (sCrashPage == PAGES_MAX) {
            sCrashPage = (PAGE_COUNT - 1);
        }
    }
}

void draw_crash_screen(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    u8 prevPage = sCrashPage;

    s32 cause = ((tc->cause >> 2) & 0x1F);
    if (cause == (EXC_WATCH >> 2)) {
        cause = 16;
    }
    if (cause == (EXC_VCED >> 2)) {
        cause = 17;
    }

    update_crash_screen_input();

    if (sUpdateBuffer) {
        if (sCrashPage != prevPage) {
            sProgramPosition = 0;
            sStackTraceIndex = 0;
        }

        reset_crash_screen_framebuffer();

        if (sDrawCrashScreen) {
            if (sDrawFrameBuffer) {
                crash_screen_draw_rect(25, 8, 270, 222, COLOR_RGBA16_CRASH_BACKGROUND, TRUE);
            }
            crash_screen_print( 30, 10, "@C0C0C0FFHackerSM64 v%s", HACKERSM64_VERSION);
            crash_screen_print(234, 10, "@C0C0C0FF<Page:%02d>", sCrashPage);
            crash_screen_draw_rect(25, 18, 270, 1, COLOR_RGBA16_LIGHT_GRAY, FALSE);
            switch (sCrashPage) {
                case PAGE_CONTEXT:    draw_crash_context(thread, cause); break;
#ifdef PUPPYPRINT_DEBUG
                case PAGE_LOG: 		  draw_crash_log(); break;
#endif
                case PAGE_STACKTRACE: draw_stacktrace(thread); break;
                case PAGE_DISASM:     draw_disasm(thread); break;
                case PAGE_ASSERTS:    draw_assert(thread); break;
                case PAGE_CONTROLS:   draw_controls(thread); break;
            }
        }

        update_crash_screen_framebuffer();

        osWritebackDCacheAll();
        osViBlack(FALSE);
        osViSwapBuffer(gFramebuffers[sRenderedFramebuffer]);
        sUpdateBuffer = FALSE;
    }
}

OSThread *get_crashed_thread(void) {
    OSThread *thread = __osGetCurrFaultedThread();

    while (thread->priority != -1) {
        if (thread->priority > OS_PRIORITY_IDLE
         && thread->priority <= OS_PRIORITY_APPMAX
         && ((thread->flags & (BIT(0) | BIT(1))) != 0)) {
            return thread;
        }
        thread = thread->tlnext;
    }
    return NULL;
}

void fill_function_stack_trace(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    u32 temp_sp = (tc->sp + 0x14);
    struct FunctionInStack *function = NULL;
    char *fname;

    if ((u32) find_function_in_stack == MAP_PARSER_ADDRESS) {
        return;
    }

    // Fill the stack buffer.
    for (s32 i = 0; i < STACK_SIZE; i++) {
        fname = find_function_in_stack(&temp_sp);

        function = &sAllFunctionStack[i];
        function->addr = temp_sp;
        function->name = fname;

        if (!((fname == NULL) || ((*(u32*)temp_sp & 0x80000000) == 0))) {
            function = &sKnownFunctionStack[sNumKnownFunctions++];
            function->addr = temp_sp;
            function->name = fname;
        }
    }
}

#ifdef FUNNY_CRASH_SOUND
extern void audio_signal_game_loop_tick(void);
extern void stop_sounds_in_continuous_banks(void);
extern struct SequenceQueueItem sBackgroundMusicQueue[6];
#endif
extern void read_controller_inputs(s32 threadID);

#ifdef CRASH_SCREEN_CRASH_SCREEN
extern u8 _crash_screen_crash_screenSegmentRomStart[];
extern u8 _crash_screen_crash_screenSegmentRomEnd[];
extern void dma_read(u8 *dest, u8 *srcStart, u8 *srcEnd);

#define SRC_IMG_SIZE (SCREEN_SIZE / 2)

void draw_crashed_image_i4(void) {
    u8 srcColor;
    Color color;
    RGBA16 *fb_u16 = gFramebuffers[sRenderingFramebuffer];

    u8 *segStart = _crash_screen_crash_screenSegmentRomStart;
    u8 *segEnd = _crash_screen_crash_screenSegmentRomEnd;
    u32 size = (u32) (segEnd - segStart);
    u8 *fb_u8 = (u8*) ((u32) fb_u16 + (SCREEN_SIZE * sizeof(RGBA16*)) - size);

    // Make sure the source image is the correct size.
    if (size != SRC_IMG_SIZE) {
        return;
    }

    // DMA the data directly onto the framebuffer.
    dma_read(fb_u8, segStart, segEnd);

    // Copy and convert the image data from the framebuffer to itself.
    for (s32 i = 0; i < SRC_IMG_SIZE; i++) {
        srcColor = *fb_u8++;

        color = (srcColor & 0xF0);
        *fb_u16++ = ((color <<  8) | (color << 3) | (color >> 2) | 0x1); // GPACK_RGBA5551

        color = (srcColor & 0x0F);
        *fb_u16++ = ((color << 12) | (color << 7) | (color << 2) | 0x1); // GPACK_RGBA5551
    }
}

void thread20_crash_screen_2(UNUSED void *arg) {
    OSMesg mesg;
    OSThread *thread = NULL;

    osSetEventMesg(OS_EVENT_CPU_BREAK, &gCrashScreen2.mesgQueue, (OSMesg) 1);
    osSetEventMesg(OS_EVENT_FAULT,     &gCrashScreen2.mesgQueue, (OSMesg) 2);

    while (TRUE) {
        if (thread == NULL) {
            osRecvMesg(&gCrashScreen2.mesgQueue, &mesg, OS_MESG_BLOCK);
            thread = get_crashed_thread();
            if (thread) {
#ifdef FUNNY_CRASH_SOUND
                gCrashScreen2.thread.priority = 15;
                stop_sounds_in_continuous_banks();
                stop_background_music(sBackgroundMusicQueue[0].seqId);
                audio_signal_game_loop_tick();
                crash_screen_sleep(200);
                play_sound(SOUND_MARIO_MAMA_MIA, gGlobalSoundSource);
                audio_signal_game_loop_tick();
                crash_screen_sleep(200);
#endif
                draw_crashed_image_i4();

                memcpy(gFramebuffers[sRenderedFramebuffer], gFramebuffers[sRenderingFramebuffer], FRAMEBUFFER_SIZE);

                osWritebackDCacheAll();
                osViBlack(FALSE);
                osViSwapBuffer(gFramebuffers[sRenderedFramebuffer]);
            }
        }
    }
}
#endif

void thread2_crash_screen(UNUSED void *arg) {
    OSMesg mesg;
    OSThread *thread = NULL;

    osSetEventMesg(OS_EVENT_CPU_BREAK, &gCrashScreen.mesgQueue, (OSMesg) 1);
    osSetEventMesg(OS_EVENT_FAULT,     &gCrashScreen.mesgQueue, (OSMesg) 2);

    while (TRUE) {
        if (thread == NULL) {
            osRecvMesg(&gCrashScreen.mesgQueue, &mesg, OS_MESG_BLOCK);
            crash_screen_take_screenshot();
            thread = get_crashed_thread();
            if (thread) {
                if ((u32) map_data_init != MAP_PARSER_ADDRESS) {
                    map_data_init();
                }
                fill_function_stack_trace(thread);
#ifdef FUNNY_CRASH_SOUND
                gCrashScreen.thread.priority = 15;
                stop_sounds_in_continuous_banks();
                stop_background_music(sBackgroundMusicQueue[0].seqId);
                audio_signal_game_loop_tick();
                crash_screen_sleep(200);
                play_sound(SOUND_MARIO_WAAAOOOW, gGlobalSoundSource);
                audio_signal_game_loop_tick();
                crash_screen_sleep(200);
#endif
#ifdef CRASH_SCREEN_CRASH_SCREEN
                osCreateMesgQueue(&gCrashScreen2.mesgQueue, &gCrashScreen2.mesg, 1);
                osCreateThread(&gCrashScreen2.thread, THREAD_20_CRASH_SCREEN_CRASH_SCREEN, thread20_crash_screen_2, NULL,
                            (u8 *) gCrashScreen2.stack + sizeof(gCrashScreen2.stack),
                            OS_PRIORITY_APPMAX);
                osStartThread(&gCrashScreen2.thread);
#endif
            }
        } else {
            if (gControllerBits) {
#if ENABLE_RUMBLE
                block_until_rumble_pak_free();
#endif
                osContStartReadData(&gSIEventMesgQueue);
            }
            read_controller_inputs(THREAD_2_CRASH_SCREEN);
            draw_crash_screen(thread);
        }
    }
}

void crash_screen_init(void) {
    osCreateMesgQueue(&gCrashScreen.mesgQueue, &gCrashScreen.mesg, 1);
    osCreateThread(&gCrashScreen.thread, THREAD_2_CRASH_SCREEN, thread2_crash_screen, NULL,
                   (u8 *) gCrashScreen.stack + sizeof(gCrashScreen.stack),
                   OS_PRIORITY_APPMAX);
    osStartThread(&gCrashScreen.thread);
}
