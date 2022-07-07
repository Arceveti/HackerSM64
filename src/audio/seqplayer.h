#ifndef AUDIO_SEQPLAYER_H
#define AUDIO_SEQPLAYER_H

#include <PR/ultratypes.h>

#include "internal.h"
#include "playback.h"

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode &  0x80)
#define PORTAMENTO_MODE(x)       ((x).mode & ~0x80)
enum PortamentoModes {
    PORTAMENTO_MODE_0,
    PORTAMENTO_MODE_1,
    PORTAMENTO_MODE_2,
    PORTAMENTO_MODE_3,
    PORTAMENTO_MODE_4,
    PORTAMENTO_MODE_5
};

enum SeqChannelCommands {
    chan_end = 0xff,
    chan_delay1 = 0xfe,
    chan_delay = 0xfd,
#if !defined(VERSION_EU) && !defined(VERSION_SH)
    chan_hang = 0xf3,
#endif
#ifdef VERSION_SH
    chan_unk_0xb0_threshold = 0xb0,
#else
    chan_unk_0xc0_threshold = 0xc0,
#endif
#if defined(VERSION_EU) || defined(VERSION_SH)
    chan_unk_0xea_stop = 0xea,
#endif
    chan_call = 0xfc,
    chan_loop = 0xf8, // loop start, N iterations (or 256 if N = 0)
    chan_loopend = 0xf7,
    chan_break = 0xf6, // break loop, if combined with jump
    chan_jump = 0xfb,
    chan_beqz = 0xfa,
    chan_bltz = 0xf9,
    chan_bgez = 0xf5,
#if defined(VERSION_EU) || defined(VERSION_SH)
    chan_jump_rel = 0xf4,
    chan_beqz_rel = 0xf3,
    chan_bltz_rel = 0xf2,
    chan_reservenotes = 0xf1,
    chan_unreservenotes = 0xf0,
#else
    chan_reservenotes = 0xf2,
    chan_unreservenotes = 0xf1,
#endif
    chan_setdyntable = 0xc2,
    chan_dynsetdyntable = 0xc5,
#if defined(VERSION_EU) || defined(VERSION_SH)
    chan_setbankandinstr = 0xeb,
#endif
    chan_setinstr = 0xc1, // "set program"
    chan_largenotesoff = 0xc3,
    chan_largenoteson = 0xc4,
    chan_setvol = 0xdf,
    chan_setvolscale = 0xe0,
    chan_freqscale = 0xde, // pitch bend using raw frequency multiplier N/2^15 (N is u16)
    chan_pitchbend = 0xd3, // pitch bend by <= 1 octave in either direction (-127..127)
#ifdef VERSION_SH
    chan_unk_0xee_set_freqscale = 0xee,
#endif
    chan_setpan = 0xdd,
    chan_setpanmix = 0xdc, // set proportion of pan to come from channel (0..128)
    chan_transpose = 0xdb, // set transposition in semitones
    chan_setenvelope = 0xda,
    chan_setdecayrelease = 0xd9,
    chan_setvibratoextent = 0xd8,
    chan_setvibratorate = 0xd7,
    chan_setvibratoextentlinear = 0xe2,
    chan_setvibratoratelinear = 0xe1,
    chan_setvibratodelay = 0xe3,
#if defined(VERSION_JP) || defined(VERSION_US)
    chan_setupdatesperframe_unimplemented = 0xd6,
#endif
    chan_setreverb = 0xd4,
    chan_setbank = 0xc6, // switch bank within set
    chan_writeseq = 0xc7, // write to sequence data (!)
    chan_subtract = 0xc8,
    chan_bitand = 0xc9,
    chan_setval = 0xcc,
#ifdef VERSION_SH
    chan_unk_0xcd = 0xcd,
#endif
    chan_setmutebhv = 0xca,
    chan_readseq = 0xcb,
#ifdef VERSION_SH
    chan_unk_0xce_set_unkC8 = 0xce, // needs asm
    chan_unk_0xcf_read_unkC8 = 0xcf, // needs asm
#endif
    chan_stereoheadseteffects = 0xd0,
    chan_setnoteallocationpolicy = 0xd1,
    chan_setsustain = 0xd2,
    chan_dyncall = 0xe4,
#if defined(VERSION_EU) || defined(VERSION_SH)
    chan_setreverbindex = 0xe5, // needs asm
    chan_setbookoffset = 0xe6, // needs asm
    chan_unk_0xe7_set = 0xe7, // needs asm
    chan_unk_0xe8_set = 0xe8, // needs asm
    chan_unk_0xec_reset = 0xec, // needs asm
    chan_setnotepriority = 0xe9,
#endif
#ifdef VERSION_SH
    chan_setsynthesisvolume = 0xed, // needs asm
    chan_unk_0xef_read = 0xef, // needs asm
    chan_unk_0xb0_set_filter = 0xb0, // needs asm
    chan_unk_0xb1_null_filter = 0xb1, // needs asm
    chan_unk_0xb3_fill_filter = 0xb3, // needs asm
    chan_unk_0xb2_set_unkC8 = 0xb2, // needs asm
    chan_unk_0xb4_set_dyntable = 0xb4, // needs asm
    chan_unk_0xb5_dyn_set_unkC8 = 0xb5, // needs asm
    chan_unk_0xb6_read_dyntable = 0xb6, // needs asm
// cmd & 0xf8
    chan_unk_0x80_read_finished = 0x80, // needs asm
    chan_unk_0x88_set_scriptstate_pc = 0x88, // needs asm
    chan_unk_0x90_layer_free = 0x90, // needs asm
    chan_unk_0x98_dyn_set_scriptstate_pc = 0x98, // needs asm
// cmd & 0xf0
    chan_unk_0x00_set_delay = 0x00, // needs asm
    chan_unk_0x10_set_soundscriptio = 0x10, // needs asm
#else
    chan_testlayerfinished = 0x00,
#endif
    chan_iowriteval = 0x70, // write data back to audio lib
#ifdef VERSION_SH
    chan_ioreadval = 0x60, // read data from audio lib
#else
    chan_ioreadval = 0x80, // read data from audio lib
#endif
    chan_ioreadvalsub = 0x50, // subtract with read data from audio lib
#ifndef VERSION_SH
 #ifdef VERSION_EU
    chan_delayshort = 0x60, // sh: 0x00
 #endif
    chan_setlayer = 0x90,
    chan_freelayer = 0xa0,
    chan_dynsetlayer = 0xb0,
 #ifndef VERSION_EU
    chan_setnotepriority = 0x60, // arg bust be >= 2
 #endif
#endif
#ifdef VERSION_SH
    chan_startchannel = 0x20,
#else
    chan_startchannel = 0x10,
#endif
#ifndef VERSION_SH
    chan_disablechannel = 0x20,
#endif
    chan_iowriteval2 = 0x30, // write data back to audio lib for another channel
    chan_ioreadval2 = 0x40, // read data from audio lib from another channel
};

void seq_channel_layer_disable(struct SequenceChannelLayer *seqPlayer);
void sequence_channel_disable(struct SequenceChannel *seqPlayer);
void sequence_player_disable(struct SequencePlayer* seqPlayer);
void audio_list_push_back(struct AudioListItem *list, struct AudioListItem *item);
void *audio_list_pop_back(struct AudioListItem *list);
void process_sequences(s32 iterationsRemaining);
void init_sequence_player(u32 player);
void init_sequence_players(void);

#endif // AUDIO_SEQPLAYER_H
