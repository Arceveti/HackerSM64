#ifndef AUDIO_EFFECTS_H
#define AUDIO_EFFECTS_H

#include <PR/ultratypes.h>

#include "internal.h"
#include "platform_info.h"

enum ADSRStates {
    ADSR_STATE_DISABLED,
    ADSR_STATE_INITIAL,
    ADSR_STATE_START_LOOP,
    ADSR_STATE_LOOP,
    ADSR_STATE_FADE,
    ADSR_STATE_HANG,
    ADSR_STATE_DECAY,
    ADSR_STATE_RELEASE,
    ADSR_STATE_SUSTAIN,
    ADSR_STATE_RESTART,
};

enum ADSRActions {
    ADSR_ACTION_RELEASE = BIT(4), // 0x10
    ADSR_ACTION_DECAY   = BIT(5), // 0x20
    ADSR_ACTION_HANG    = BIT(6), // 0x40
};

enum ADSRDelays {
    ADSR_HANG    = -1,
    ADSR_GOTO    = -2,
    ADSR_RESTART = -3,
    ADSR_DISABLE = -4,
};

enum VibratoModes {
    VIBMODE_NONE       = 0,
    VIBMODE_VIBRATO    = BIT(0),
    VIBMODE_PORTAMENTO = BIT(1),
};

// Envelopes are always stored as big endian, to match sequence files which are
// byte blobs and can embed envelopes. Hence this byteswapping macro.
#if IS_BIG_ENDIAN
#define BSWAP16(x) (x)
#else
#define BSWAP16(x) (((x) & 0xff) << 8 | (((x) >> 8) & 0xff))
#endif

void sequence_player_process_sound(struct SequencePlayer *seqPlayer);
void note_vibrato_update(struct Note *note);
void note_vibrato_init(struct Note *note);
void adsr_init(struct AdsrState *adsr, struct AdsrEnvelope *envelope, s16 *volOut);
#if defined(VERSION_EU) || defined(VERSION_SH)
f32 adsr_update(struct AdsrState *adsr);
#else
s32 adsr_update(struct Note *note);
#endif

#endif // AUDIO_EFFECTS_H
