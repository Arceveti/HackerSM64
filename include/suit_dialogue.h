#pragma once




typedef struct SuitDialogue {
    const char* txt;
    f32 speed;
    Color r;
    Color g;
    Color b;
    u32 timeout;
} SuitDialogue;
enum OnScreenDialogue {
    DLG_NONE,
    // Summit
    DLG_BOOTING,
    DLG_SCANNING,
    DLG_HELLO_FRIEND,
    DLG_HELLO_2,
    DLG_LAST_BOOT,
    DLG_WELCOME,
    DLG_LIGHTS_1,
    DLG_HOME_1,
    // Mountain
    DLG_START_RIGHT,
    DLG_READY_LEFT,
    DLG_GO_LEFT,
    DLG_READY_RIGHT,
    DLG_GO_RIGHT,
    DLG_READY_JUMP,
    DLG_GO_JUMP,
    DLG_JUMP_ICICLES,
    DLG_JUMP_THEN_LEFT,
    DLG_JUMP_THEN_RIGHT,
    DLG_KEEP_LEFT,
    DLG_KEEP_RIGHT,
    DLG_LEFT_NO_RIGHT,
    DLG_RIGHT_NO_LEFT,
    DLG_LEFT_AGAIN,
    DLG_RIGHT_AGAIN,
    DLG_JUMP_LEFT,
    DLG_JUMP_RIGHT,
    DLG_INTO_THE_CAVE,
    DLG_WRONG_WAY,
    DLG_GOOD,
    DLG_DONT_FALL,
    DLG_WATCH_OUT,
    // Lava
    DLG_WELCOME_HOME,
    DLG_HOTTER,
    DLG_SHIELD,
    DLG_UPHILL,
    DLG_APOLOGIZE_1,
    DLG_APOLOGIZE_2,
    DLG_ROLL_HACK,
    // Boss
    DLG_OFFSCREEN,
    DLG_OFFSCREEN_2,
    // Space
    DLG_GOODBYE_FRIEND,
    DLG_LIGHTS_2,
    DLG_HOME_2,
    DLG_YOUR_HOME,
    DLG_BLACK_HOLE,
    DLG_RAINBOW_ROAD,
};

extern void set_onscreen_dlg(int id);
