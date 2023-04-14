#ifndef MAIN_H
#define MAIN_H

#include "config.h"

enum VIModes {
    MODE_NTSC,
    MODE_MPAL,
    MODE_PAL,
};

#define THREAD1_STACK 0x0100
#define THREAD2_STACK 0x0800
#define THREAD3_STACK 0x0200
#define THREAD4_STACK 0x2000
#define THREAD5_STACK 0x2000
#define THREAD6_STACK 0x0400

enum ThreadID {
    THREAD_0,
    THREAD_1_IDLE,
    THREAD_2_CRASH_SCREEN,
    THREAD_3_MAIN,
    THREAD_4_SOUND,
    THREAD_5_GAME_LOOP,
    THREAD_6_RUMBLE,
    THREAD_7_HVQM,
    THREAD_8_TIMEKEEPER,
    THREAD_9_DA_COUNTER,
#ifdef CRASH_SCREEN_CRASH_SCREEN
    THREAD_20_CRASH_SCREEN_CRASH_SCREEN,
#endif
    NUM_THREADS,
};

enum VblankHandlers {
    VBLANK_HANDLER_SOUND_INIT,
    VBLANK_HANDLER_GAME_INIT,
    VBLANK_HANDLER_HVQM,
    NUM_VBLANK_HANDLERS,
};

extern struct Config gConfig;

// extern OSThread gUnkThread;
extern OSThread gIdleThread;
extern OSThread gMainThread;
extern OSThread gGameLoopThread;
extern OSThread gSoundThread;
extern OSThread hvqmThread;

extern OSMesgQueue gPIMesgQueue;
extern OSMesgQueue gIntrMesgQueue;
extern OSMesgQueue gSPTaskMesgQueue;
extern OSMesg gDmaMesgBuf[1];
extern OSMesg gPIMesgBuf[32];
extern OSMesg gSIEventMesgBuf[1];
extern OSMesg gIntrMesgBuf[16];
extern OSMesg gUnknownMesgBuf[16];
extern OSIoMesg gDmaIoMesg;
extern OSMesg gMainReceivedMesg;
extern OSMesgQueue gDmaMesgQueue;
extern OSMesgQueue gSIEventMesgQueue;

extern struct VblankHandler* gVblankHandlers[NUM_VBLANK_HANDLERS];
extern struct SPTask* gActiveSPTask;
extern s8 sAudioEnabled;
extern u32 gNumVblanks;
extern s8 gResetTimer;
extern s8 gNmiResetBarsTimer;
extern s8 gDebugLevelSelect;
#ifdef VANILLA_DEBUG
extern s8 gShowDebugText;
#endif

void set_vblank_handler(s32 index, struct VblankHandler* handler, OSMesgQueue* queue, OSMesg* msg);
void dispatch_audio_sptask(struct SPTask* spTask);
void exec_display_list(struct SPTask* spTask);
void change_vi(OSViMode* mode, int width, int height);

#endif // MAIN_H
