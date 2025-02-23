//! Copt inlining for US/JP. Here be dragons
// This version is basically identical to EU

#include <ultra64.h>
#include <macros.h>

#include "audio/heap.h"
#include "audio/data.h"
#include "audio/load.h"
#include "audio/seqplayer.h"
#include "audio/external.h"
#include "audio/effects.h"
#include "engine/math_util.h"

#define COPT 0
#if COPT
#define M64_READ_U8(state, dst) \
    dst = m64_read_u8(state);
#else // !COPT
#define M64_READ_U8(state, dst) \
{                               \
    u8 * _ptr_pc;               \
    u8  _pc;                    \
    _ptr_pc = (*state).pc;      \
    ((*state).pc)++;            \
    _pc = *_ptr_pc;             \
    dst = _pc;                  \
}
#endif // !COPT

#if COPT
#define M64_READ_S16(state, dst) \
    dst = m64_read_s16(state);
#else // !COPT
#define M64_READ_S16(state, dst)    \
{                                   \
    s16 _ret;                       \
    _ret = (*(*state).pc << 8);     \
    ((*state).pc)++;                \
    _ret = (*(*state).pc | _ret);   \
    ((*state).pc)++;                \
    dst = _ret;                     \
}
#endif // !COPT
#if COPT
#define M64_READ_COMPRESSED_U16(state, dst) \
    dst = m64_read_compressed_u16(state);
#else // !COPT
#define M64_READ_COMPRESSED_U16(state, dst) \
{                                           \
    u16 ret = *(state->pc++);               \
    if (ret & 0x80) {                       \
        ret = ((ret << 8) & 0x7f00);        \
        ret = (*(state->pc++) | ret);       \
    }                                       \
    dst = ret;                              \
}
#endif // !COPT

#if COPT
#define GET_INSTRUMENT(seqChannel, instId, _instOut, _adsr, dst, l) \
    dst = get_instrument(seqChannel, instId, _instOut, _adsr);
#else // !COPT
#define GET_INSTRUMENT(seqChannel, instId, _instOut, _adsr, dst, l) \
{ \
struct AdsrSettings *adsr = _adsr; \
struct Instrument **instOut = _instOut;\
    u8 _instId = instId; \
    struct Instrument *inst; \
    /* copt inlines instId here  */ \
    if (instId >= gCtlEntries[(*seqChannel).bankId].numInstruments) { \
        _instId = gCtlEntries[(*seqChannel).bankId].numInstruments; \
        if (_instId == 0) { \
            dst = 0; \
            goto ret ## l; \
        } \
        _instId--; \
    } \
    inst = gCtlEntries[(*seqChannel).bankId].instruments[_instId]; \
    if (inst == NULL) { \
        while (_instId != 0xff) { \
            inst = gCtlEntries[(*seqChannel).bankId].instruments[_instId]; \
            if (inst != NULL) { \
                goto gi ## l; \
            } \
            _instId--; \
        } \
        gi ## l:; \
    } \
    if (((uintptr_t) gBankLoadedPool.persistent.pool.start <= (uintptr_t) inst \
         && (uintptr_t) inst <= (uintptr_t)(gBankLoadedPool.persistent.pool.start \
                                          + gBankLoadedPool.persistent.pool.size)) \
        || ((uintptr_t) gBankLoadedPool.temporary.pool.start <= (uintptr_t) inst \
            && (uintptr_t) inst <= (uintptr_t)(gBankLoadedPool.temporary.pool.start \
                                             + gBankLoadedPool.temporary.pool.size))) { \
        (*adsr).envelope = (*inst).envelope; \
        (*adsr).releaseRate = (*inst).releaseRate; \
        *instOut = inst; \
        _instId++; \
        goto ret ## l; \
    } \
    gAudioErrorFlags = _instId + 0x20000; \
    *instOut = NULL; \
    ret ## l: ; \
}
#endif // !COPT

void seq_channel_drum(u8 *cmdSemitone, struct SequenceChannelLayer *layer, struct SequenceChannel *seqChannel) {
    struct Drum *drum;

    *cmdSemitone += (*seqChannel).transposition + (*layer).transposition;
    if (*cmdSemitone >= gCtlEntries[seqChannel->bankId].numDrums) {
        *cmdSemitone  = gCtlEntries[seqChannel->bankId].numDrums;
        if (*cmdSemitone == 0) {
            layer->stopSomething = TRUE;
            return;
        }

        (*cmdSemitone)--;
    }

    drum = gCtlEntries[seqChannel->bankId].drums[*cmdSemitone];
    if (drum == NULL) {
        layer->stopSomething = TRUE;
    } else {
        layer->adsr.envelope = drum->envelope;
        layer->adsr.releaseRate = drum->releaseRate;
        layer->pan = (f32)(drum->pan) / 128.0f;
        layer->sound = &drum->sound;
        layer->freqScale = layer->sound->tuning;
    }
}

void seq_channel_layer_process_script(struct SequenceChannelLayer *layer) {
    struct M64ScriptState *state;
    struct Portamento *portamento;
    struct AudioBankSound *sound;
    struct Instrument *instrument;
    s32 temp_a0_5;
    u8 cmd;                             // a0 sp3E, EU s2
    u8 cmdSemitone;                     // sp3D, t0
    u16 sp3A = 0;                       // t2, a0, a1
    f32 tuning;                         // f0
    s32 vel = 0;                        // sp30, t3
    s32 usedSemitone;                   // a1
    f32 freqScale = 0.0f;               // sp28, f0
    f32 sp24 = 0.0f;
    f32 temp_f12;
    f32 temp_f2;

//! Copt: manually inline these functions in the scope of this routine
#ifdef __sgi
#pragma inline routine(m64_read_u8)
#pragma inline routine(m64_read_compressed_u16)
#pragma inline routine(m64_read_s16)
#pragma inline routine(get_instrument)
#endif

    _Bool sameSound = TRUE;
    if (!(*layer).enabled) {
        return;
    }

    if ((*layer).delay > 1) {
        (*layer).delay--;
        if (!layer->stopSomething && layer->delay <= layer->duration) {
            seq_channel_layer_note_decay(layer);
            layer->stopSomething = TRUE;
        }
        return;
    }

    if (!layer->continuousNotes) {
        seq_channel_layer_note_decay(layer);
    }

    if (PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_1 ||
        PORTAMENTO_MODE(layer->portamento) == PORTAMENTO_MODE_2) {
        layer->portamento.mode = 0;
    }

    struct SequenceChannel *seqChannel = (*layer).seqChannel;
    struct SequencePlayer  *seqPlayer = (*seqChannel).seqPlayer;
    for (;;) {
        state = &layer->scriptState;
        //M64_READ_U8(state, cmd);
        {
            u8 *_ptr_pc;
            _ptr_pc = (*state).pc++;
            cmd = *_ptr_pc;
        }

        if (cmd <= 0xc0) {
            break;
        }

        switch (cmd) {
            case layer_end: // layer_end; function return or end of script
                if (state->depth == 0) {
                    // N.B. this function call is *not* inlined even though it's
                    // within the same file, unlike in the rest of this function.
                    seq_channel_layer_disable(layer);
                    return;
                }
                state->depth--, state->pc = state->stack[state->depth];
                break;

            case layer_call: // layer_call
                M64_READ_S16(state, sp3A);
                state->depth++, state->stack[state->depth - 1] = state->pc;
                state->pc = seqPlayer->seqData + sp3A;
                break;

            case layer_loop: // layer_loop; loop start, N iterations (or 256 if N = 0)
                M64_READ_U8(state, state->remLoopIters[state->depth]);
                state->depth++, state->stack[state->depth - 1] = state->pc;
                break;

            case layer_loopend: // layer_loopend
                if (--state->remLoopIters[state->depth - 1] != 0) {
                    state->pc = state->stack[state->depth - 1];
                } else {
                    state->depth--;
                }
                break;

            case layer_jump: // layer_jump
                M64_READ_S16(state, sp3A);
                state->pc = seqPlayer->seqData + sp3A;
                break;

            case layer_setshortnotevelocity: // layer_setshortnotevelocity
            case layer_setpan: // layer_setpan
                temp_a0_5 = *(state->pc++);
                if (cmd == layer_setshortnotevelocity) {
                    layer->velocitySquare = (f32)sqr(temp_a0_5);
                } else {
                    layer->pan = (f32) temp_a0_5 / 128.0f;
                }
                break;

            case layer_transpose: // layer_transpose; set transposition in semitones
            case layer_setshortnoteduration: // layer_setshortnoteduration
                temp_a0_5 = *(state->pc++);
                if (cmd == layer_setshortnoteduration) {
                    layer->noteDuration = temp_a0_5;
                } else {
                    layer->transposition = temp_a0_5;
                }
                break;

            case layer_somethingon: // layer_somethingon
            case layer_somethingoff: // layer_somethingoff
                layer->continuousNotes = (cmd == layer_somethingon);
                seq_channel_layer_note_decay(layer);
                break;

            case layer_setshortnotedefaultplaypercentage: // layer_setshortnotedefaultplaypercentage
                M64_READ_COMPRESSED_U16(state, sp3A);
                layer->shortNoteDefaultPlayPercentage = sp3A;
                break;

            case layer_setinstr: // layer_setinstr
                M64_READ_U8(state, cmdSemitone);

                if (cmdSemitone < 127) {
                    GET_INSTRUMENT(seqChannel, cmdSemitone, &(*layer).instrument, &(*layer).adsr, cmdSemitone, 1);
                }
                break;

            case layer_portamento: // layer_portamento
                M64_READ_U8(state, (*layer).portamento.mode);
                M64_READ_U8(state, cmdSemitone);

                cmdSemitone = cmdSemitone + (*seqChannel).transposition;
                cmdSemitone += (*layer).transposition;
                cmdSemitone += (*seqPlayer).transposition;

                if (cmdSemitone >= 0x80) {
                    cmdSemitone = 0;
                }
                layer->portamentoTargetNote = cmdSemitone;

                // If special, the next param is u8 instead of var
                if (PORTAMENTO_IS_SPECIAL((*layer).portamento)) {
                    layer->portamentoTime = *((state)->pc++);
                    break;
                }

                M64_READ_COMPRESSED_U16(state, sp3A);
                layer->portamentoTime = sp3A;
                break;

            case layer_disableportamento: // layer_disableportamento
                layer->portamento.mode = 0;
                break;

            default:
                switch (cmd & 0xf0) {
                    case layer_setshortnotevelocityfromtable: // layer_setshortnotevelocityfromtable
                        sp3A = seqPlayer->shortNoteVelocityTable[cmd & 0x0f];
                        (*layer).velocitySquare = (f32)sqr(sp3A);
                        break;
                    case layer_setshortnotedurationfromtable: // layer_setshortnotedurationfromtable
                        (*layer).noteDuration = seqPlayer->shortNoteDurationTable[cmd & 0x0f];
                        break;
                }
        }
    }

    if (cmd == layer_delay) { // layer_delay
        M64_READ_COMPRESSED_U16(state, layer->delay);
        layer->stopSomething = TRUE;
    } else {
        layer->stopSomething = FALSE;

        if (seqChannel->largeNotes) {
            switch (cmd & 0xc0) {
                case layer_note0: // layer_note0 (play percentage, velocity, duration)
                    M64_READ_COMPRESSED_U16(state, sp3A);
                    vel = *((*state).pc++);
                    layer->noteDuration = *((*state).pc++);
                    layer->playPercentage = sp3A;
                    break;

                case layer_note1: // layer_note1 (play percentage, velocity)
                    M64_READ_COMPRESSED_U16(state, sp3A);
                    vel = *((*state).pc++);
                    layer->noteDuration = 0;
                    layer->playPercentage = sp3A;
                    break;

                case layer_note2: // layer_note2 (velocity, duration; uses last play percentage)
                    sp3A = layer->playPercentage;
                    vel = *((*state).pc++);
                    layer->noteDuration = *((*state).pc++);
                    break;
            }
            cmdSemitone = cmd - (cmd & 0xc0);
            layer->velocitySquare = vel * vel;
        } else {
            switch (cmd & 0xc0) {
                case layer_notetype0: // play note, type 0 (play percentage)
                    M64_READ_COMPRESSED_U16(state, sp3A);
                    layer->playPercentage = sp3A;
                    break;

                case layer_notetype1: // play note, type 1 (uses default play percentage)
                    sp3A = layer->shortNoteDefaultPlayPercentage;
                    break;

                case layer_notetype2: // play note, type 2 (uses last play percentage)
                    sp3A = layer->playPercentage;
                    break;
            }

            cmdSemitone = cmd - (cmd & 0xc0);
        }

        layer->delay = sp3A;
        layer->duration = layer->noteDuration * sp3A / 256;
        if ((seqPlayer->muted && (seqChannel->muteBehavior & MUTE_BEHAVIOR_STOP_NOTES))
            || seqChannel->stopSomething2
            || !seqChannel->hasInstrument
        ) {
            layer->stopSomething = TRUE;
        } else {
            if (seqChannel->instOrWave == 0) { // drum
                seq_channel_drum(&cmdSemitone, layer, seqChannel);
            } else { // instrument
                cmdSemitone += (*seqPlayer).transposition + (*seqChannel).transposition + (*layer).transposition;
                if (cmdSemitone >= 0x80) {
                    layer->stopSomething = TRUE;
                } else {
                    instrument = layer->instrument;
                    if (instrument == NULL) {
                        instrument = seqChannel->instrument;
                    }

                    if (layer->portamento.mode != 0) {
                        usedSemitone = (layer->portamentoTargetNote < cmdSemitone) ? cmdSemitone : layer->portamentoTargetNote;

                        if (instrument != NULL) {
                            sound = ((u8) usedSemitone < instrument->normalRangeLo) ? &instrument->lowNotesSound
                                  : (((u8) usedSemitone <= instrument->normalRangeHi) ?
                                        &instrument->normalNotesSound : &instrument->highNotesSound);

                            sameSound = (sound == (*layer).sound);
                            layer->sound = sound;
                            tuning = (*sound).tuning;
                        } else {
                            layer->sound = NULL;
                            tuning = 1.0f;
                        }

                        temp_f2 = gNoteFrequencies[cmdSemitone] * tuning;
                        temp_f12 = gNoteFrequencies[layer->portamentoTargetNote] * tuning;

                        portamento = &layer->portamento;
                        switch (PORTAMENTO_MODE(layer->portamento)) {
                            case PORTAMENTO_MODE_1:
                            case PORTAMENTO_MODE_3:
                            case PORTAMENTO_MODE_5:
                                sp24 = temp_f2;
                                freqScale = temp_f12;
                                break;

                            case PORTAMENTO_MODE_2:
                            case PORTAMENTO_MODE_4:
                                freqScale = temp_f2;
                                sp24 = temp_f12;
                                break;
                        }
                        portamento->extent = sp24 / freqScale - 1.0f;
                        if (PORTAMENTO_IS_SPECIAL((*layer).portamento)) {
                            portamento->speed = 32512.0f * (f32)(*seqPlayer).tempo
                                                / ((f32)(*layer).delay * (f32) gTempoInternalToExternal
                                                   * (*layer).portamentoTime);
                        } else {
                            portamento->speed = 127.0f / (*layer).portamentoTime;
                        }
                        portamento->cur = 0.0f;
                        layer->freqScale = freqScale;
                        if (PORTAMENTO_MODE((*layer).portamento) == PORTAMENTO_MODE_5) {
                            layer->portamentoTargetNote = cmdSemitone;
                        }
                    } else if (instrument != NULL) {
                        sound = (cmdSemitone < instrument->normalRangeLo) ?
                                         &instrument->lowNotesSound : ((cmdSemitone <= instrument->normalRangeHi) ?
                                         &instrument->normalNotesSound : &instrument->highNotesSound);

                        sameSound = (sound == (*layer).sound);
                        layer->sound = sound;
                        layer->freqScale = gNoteFrequencies[cmdSemitone] * (*sound).tuning;
                    } else {
                        layer->sound = NULL;
                        layer->freqScale = gNoteFrequencies[cmdSemitone];
                    }
                }
            }
            layer->delayUnused = layer->delay;
        }
    }

    if (layer->stopSomething) {
        if (layer->note != NULL || layer->continuousNotes) {
            seq_channel_layer_note_decay(layer);
        }
        return;
    }

    cmdSemitone = FALSE;
    if (!layer->continuousNotes) {
        cmdSemitone = TRUE;
    } else if (layer->note == NULL || layer->status == SOUND_LOAD_STATUS_NOT_LOADED) {
        cmdSemitone = TRUE;
    } else if (!sameSound) {
        seq_channel_layer_note_decay(layer);
        cmdSemitone = TRUE;
    } else if (layer->sound == NULL) {
        init_synthetic_wave(layer->note, layer);
    }

    if (cmdSemitone) {
        (*layer).note = alloc_note(layer);
    }

    if (layer->note != NULL && layer->note->parentLayer == layer) {
        note_vibrato_init(layer->note);
    }
}
