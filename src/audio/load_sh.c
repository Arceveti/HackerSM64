#ifdef VERSION_SH
#include <ultra64.h>
#include <PR/os.h>

#include "data.h"
#include "external.h"
#include "heap.h"
#include "load.h"
#include "seqplayer.h"

struct SharedDma {
    /*0x0*/ u8 *buffer;       // target, points to pre-allocated buffer
    /*0x4*/ uintptr_t source; // device address
    /*0x8*/ u16 sizeUnused;   // set to bufSize, never read
    /*0xA*/ u16 bufSize;      // size of buffer
    /*0xC*/ u8 unused2;       // set to 0, never read
    /*0xD*/ u8 reuseIndex;    // position in sSampleDmaReuseQueue1/2, if ttl == 0
    /*0xE*/ u8 ttl;           // duration after which the DMA can be discarded
};                            // size = 0x10

void port_eu_init(void);
void patch_sound(struct AudioBankSound *sound, struct AudioBank *memBase, struct PatchStruct *patchInfo);
void *func_802f3f08(s32 poolIdx, s32 idx, s32 numChunks, s32 arg3, OSMesgQueue *retQueue);

struct Note *gNotes;

struct SequencePlayer gSequencePlayers[SEQUENCE_PLAYERS];
struct SequenceChannel gSequenceChannels[SEQUENCE_CHANNELS];
struct SequenceChannelLayer gSequenceLayers[SEQUENCE_LAYERS];

struct SequenceChannel gSequenceChannelNone;
struct AudioListItem gLayerFreeList;
struct NotePool gNoteFreeLists;

struct AudioBankSample *D_SH_8034EA88[0x80];
struct UnkStructSH8034EC88 D_SH_8034EC88[0x80];
s32 D_SH_8034F688; // index into D_SH_8034EA88
s32 D_SH_8034F68C; // index or size for D_SH_8034EC88

struct PendingDmaAudioBank {
    s8 inProgress;
    s8 timer;
    s8 medium;
    struct AudioBank *audioBank;
    uintptr_t devAddr;
    void *vAddr;
    u32 remaining;
    u32 transferSize;
    u32 encodedInfo;
    OSMesgQueue *retQueue;
    OSMesgQueue dmaRetQueue;
    OSMesg mesgs[1];
    OSIoMesg ioMesg;
};
struct PendingDmaAudioBank sPendingDmaAudioBanks[16];

OSMesgQueue gUnkQueue1;
OSMesg gUnkMesgBufs1[0x10];
OSMesgQueue gUnkQueue2;
OSMesg gUnkMesgBufs2[0x10];

OSMesgQueue gCurrAudioFrameDmaQueue;
OSMesg gCurrAudioFrameDmaMesgBufs[AUDIO_FRAME_DMA_QUEUE_SIZE];
OSIoMesg gCurrAudioFrameDmaIoMesgBufs[AUDIO_FRAME_DMA_QUEUE_SIZE];

OSMesgQueue gAudioDmaMesgQueue;
OSMesg gAudioDmaMesg;
OSIoMesg gAudioDmaIoMesg;

struct SharedDma *sSampleDmas;
u32 gSampleDmaNumListItems;
u32 sSampleDmaListSize1;

// Circular buffer of DMAs with ttl = 0. tail <= head, wrapping around mod 256.
u8 sSampleDmaReuseQueue1[256];
u8 sSampleDmaReuseQueue2[256];
u8 sSampleDmaReuseQueueTail1;
u8 sSampleDmaReuseQueueTail2;
u8 sSampleDmaReuseQueueHead1;
u8 sSampleDmaReuseQueueHead2;

ALSeqFile *gSeqFileHeader;
ALSeqFile *gAlCtlHeader;
ALSeqFile *gAlTbl;
u8 *gAlBankSets;
u16 gSequenceCount;

struct CtlEntry *gCtlEntries;

struct AudioBufferParametersEU gAudioBufferParameters;
u32 sDmaBufSize;
s32 gMaxAudioCmds;
s32 gMaxSimultaneousNotes;

s16 gTempoInternalToExternal;

s8 gSoundMode;

s8 gAudioUpdatesPerFrame;

extern u64 gAudioGlobalsStartMarker;
extern u64 gAudioGlobalsEndMarker;

extern u8 gSoundDataADSR[]; // ctl
extern u8 gSoundDataRaw[];  // tbl
extern u8 gMusicData[];     // sequences

ALSeqFile *get_audio_file_header(s32 poolIdx);

void *func_sh_802f3688(s32 bankId);
void *get_bank_or_seq_wrapper(s32 poolIdx, s32 id);
void func_sh_802f3d78(uintptr_t devAddr, void *vAddr, size_t nbytes, s32 arg3);
void func_sh_802f3c38(uintptr_t devAddr, void *vAddr, size_t nbytes, s32 medium);
s32 func_sh_802f3dd0(OSIoMesg *m, s32 pri, s32 direction, uintptr_t devAddr,
        void *dramAddr, s32 size, OSMesgQueue *retQueue, s32 medium, UNUSED const char *reason);
void func_sh_802f4a4c(s32 audioResetStatus);
void func_sh_802f4bd8(struct PendingDmaSample *arg0, s32 len);
void func_sh_802f4c5c(uintptr_t devAddr, void *vAddr, size_t nbytes, s32 arg3);
struct PendingDmaAudioBank *func_sh_802f4cb4(uintptr_t devAddr, void *vAddr, s32 size,
        s32 medium, s32 numChunks, OSMesgQueue *retQueue, s32 encodedInfo);
void func_sh_802f4dcc(s32 audioResetStatus);
void func_sh_802f4e50(struct PendingDmaAudioBank *audioBank, s32 audioResetStatus);
void func_sh_802f50ec(struct PendingDmaAudioBank *arg0, size_t len);
void func_sh_802f517c(uintptr_t devAddr, void *vAddr, size_t nbytes, s32 arg3);
void func_sh_802f5310(s32 bankId, struct AudioBank *mem, struct PatchStruct *patchInfo, s32 arg3);
s32 func_sh_802f573c(s32 audioResetStatus);
void *func_sh_802f3564(s32 seqId);

s32 canonicalize_index(s32 poolIdx, s32 idx);

void decrease_sample_dma_ttls() {
    u32 i;

    for (i = 0; i < sSampleDmaListSize1; i++) {
        struct SharedDma *temp = &sSampleDmas[i];
        if (temp->ttl != 0) {
            temp->ttl--;
            if (temp->ttl == 0) {
                temp->reuseIndex = sSampleDmaReuseQueueHead1;
                sSampleDmaReuseQueue1[sSampleDmaReuseQueueHead1++] = (u8) i;
            }
        }
    }

    for (i = sSampleDmaListSize1; i < gSampleDmaNumListItems; i++) {
        struct SharedDma *temp = &sSampleDmas[i];
        if (temp->ttl != 0) {
            temp->ttl--;
            if (temp->ttl == 0) {
                temp->reuseIndex = sSampleDmaReuseQueueHead2;
                sSampleDmaReuseQueue2[sSampleDmaReuseQueueHead2++] = (u8) i;
            }
        }
    }
}

extern char shindouDebugPrint62[]; // "SUPERDMA"
void *dma_sample_data(uintptr_t devAddr, u32 size, s32 arg2, u8 *dmaIndexRef, s32 medium) {
    struct SharedDma *dma;
    _Bool hasDma = FALSE;
    uintptr_t dmaDevAddr;
    u32 dmaIndex;
    u32 transfer;
    ssize_t bufferPos;
    u32 i;

    if (arg2 != 0 || *dmaIndexRef >= sSampleDmaListSize1) {
        for (i = sSampleDmaListSize1; i < gSampleDmaNumListItems; i++) {
            dma = &sSampleDmas[i];
            bufferPos = devAddr - dma->source;
            if (0 <= bufferPos && (size_t) bufferPos <= dma->bufSize - size) {
                // We already have a DMA request for this memory range.
                if (dma->ttl == 0 && sSampleDmaReuseQueueTail2 != sSampleDmaReuseQueueHead2) {
                    // Move the DMA out of the reuse queue, by swapping it with the
                    // tail, and then incrementing the tail.
                    if (dma->reuseIndex != sSampleDmaReuseQueueTail2) {
                        sSampleDmaReuseQueue2[dma->reuseIndex] =
                            sSampleDmaReuseQueue2[sSampleDmaReuseQueueTail2];
                        sSampleDmas[sSampleDmaReuseQueue2[sSampleDmaReuseQueueTail2]].reuseIndex =
                            dma->reuseIndex;
                    }
                    sSampleDmaReuseQueueTail2++;
                }
                dma->ttl = 60;
                *dmaIndexRef = (u8) i;
                return &dma->buffer[(devAddr - dma->source)];
            }
        }

        if (sSampleDmaReuseQueueTail2 != sSampleDmaReuseQueueHead2 && arg2 != 0) {
            // Allocate a DMA from reuse queue 2. This queue can be empty, since
            // TTL 60 is pretty large.
            dmaIndex = sSampleDmaReuseQueue2[sSampleDmaReuseQueueTail2];
            sSampleDmaReuseQueueTail2++;
            dma = sSampleDmas + dmaIndex;
            hasDma = TRUE;
        }
    } else {
        dma = sSampleDmas + *dmaIndexRef;
        bufferPos = devAddr - dma->source;
        if (0 <= bufferPos && (size_t) bufferPos <= dma->bufSize - size) {
            // We already have DMA for this memory range.
            if (dma->ttl == 0) {
                // Move the DMA out of the reuse queue, by swapping it with the
                // tail, and then incrementing the tail.
                if (dma->reuseIndex != sSampleDmaReuseQueueTail1) {
                    sSampleDmaReuseQueue1[dma->reuseIndex] =
                        sSampleDmaReuseQueue1[sSampleDmaReuseQueueTail1];
                    sSampleDmas[sSampleDmaReuseQueue1[sSampleDmaReuseQueueTail1]].reuseIndex =
                        dma->reuseIndex;
                }
                sSampleDmaReuseQueueTail1++;
            }
            dma->ttl = 2;
            return dma->buffer + (devAddr - dma->source);
        }
    }

    if (!hasDma) {
        // Allocate a DMA from reuse queue 1. This queue will hopefully never
        // be empty, since TTL 2 is so small.
        dmaIndex = sSampleDmaReuseQueue1[sSampleDmaReuseQueueTail1++];
        dma = sSampleDmas + dmaIndex;
        hasDma = TRUE;
    }

    transfer = dma->bufSize;
    dmaDevAddr = devAddr & ~0xF;
    dma->ttl = 2;
    dma->source = dmaDevAddr;
    dma->sizeUnused = transfer;
    func_sh_802f3dd0(&gCurrAudioFrameDmaIoMesgBufs[gCurrAudioFrameDmaCount++], OS_MESG_PRI_NORMAL, OS_READ,
         dmaDevAddr, dma->buffer, transfer, &gCurrAudioFrameDmaQueue, medium, shindouDebugPrint62);
    *dmaIndexRef = dmaIndex;
    return (devAddr - dmaDevAddr) + dma->buffer;
}

void init_sample_dma_buffers(UNUSED s32 arg0) {
    s32 i;

    sDmaBufSize = 0x2D0;
    sSampleDmas = sound_alloc_uninitialized(&gNotesAndBuffersPool,
            gMaxSimultaneousNotes * 4 * sizeof(struct SharedDma) * gAudioBufferParameters.presetUnk4);

    for (i = 0; i < gMaxSimultaneousNotes * 3 * gAudioBufferParameters.presetUnk4; i++) {
        sSampleDmas[gSampleDmaNumListItems].buffer = sound_alloc_uninitialized(&gNotesAndBuffersPool, sDmaBufSize);
        if (sSampleDmas[gSampleDmaNumListItems].buffer == NULL) {
            break;
        }
        sSampleDmas[gSampleDmaNumListItems].bufSize = sDmaBufSize;
        sSampleDmas[gSampleDmaNumListItems].source = 0;
        sSampleDmas[gSampleDmaNumListItems].sizeUnused = 0;
        sSampleDmas[gSampleDmaNumListItems].unused2 = 0;
        sSampleDmas[gSampleDmaNumListItems].ttl = 0;
        gSampleDmaNumListItems++;
    }

    for (i = 0; (u32) i < gSampleDmaNumListItems; i++) {
        sSampleDmaReuseQueue1[i] = (u8) i;
        sSampleDmas[i].reuseIndex = (u8) i;
    }

    for (i = gSampleDmaNumListItems; i < 0x100; i++) {
        sSampleDmaReuseQueue1[i] = 0;
    }

    sSampleDmaReuseQueueTail1 = 0;
    sSampleDmaReuseQueueHead1 = (u8) gSampleDmaNumListItems;
    sSampleDmaListSize1 = gSampleDmaNumListItems;

    sDmaBufSize = 0x2D0;
    for (i = 0; i < gMaxSimultaneousNotes; i++) {
        sSampleDmas[gSampleDmaNumListItems].buffer = sound_alloc_uninitialized(&gNotesAndBuffersPool, sDmaBufSize);
        if (sSampleDmas[gSampleDmaNumListItems].buffer == NULL) {
            break;
        }
        sSampleDmas[gSampleDmaNumListItems].bufSize = sDmaBufSize;
        sSampleDmas[gSampleDmaNumListItems].source = 0;
        sSampleDmas[gSampleDmaNumListItems].sizeUnused = 0;
        sSampleDmas[gSampleDmaNumListItems].unused2 = 0;
        sSampleDmas[gSampleDmaNumListItems].ttl = 0;
        gSampleDmaNumListItems++;
    }

    for (i = sSampleDmaListSize1; (u32) i < gSampleDmaNumListItems; i++) {
        sSampleDmaReuseQueue2[i - sSampleDmaListSize1] = (u8) i;
        sSampleDmas[i].reuseIndex = (u8)(i - sSampleDmaListSize1);
    }

    // This probably meant to touch the range size1..size2 as well... but it
    // doesn't matter, since these values are never read anyway.
    for (i = gSampleDmaNumListItems; i < 0x100; i++) {
        sSampleDmaReuseQueue2[i] = sSampleDmaListSize1;
    }

    sSampleDmaReuseQueueTail2 = 0;
    sSampleDmaReuseQueueHead2 = gSampleDmaNumListItems - sSampleDmaListSize1;
}

void patch_seq_file(ALSeqFile *seqFile, u8 *data, u16 arg2) {
    s32 i;

    seqFile->unk2 = arg2;
    seqFile->data = data;
    for (i = 0; i < seqFile->seqCount; i++) {
        if (seqFile->seqArray[i].len != 0 && seqFile->seqArray[i].medium == 2) {
            seqFile->seqArray[i].offset += (uintptr_t)data;
        }
    }
}

struct AudioBank *load_banks_immediate(s32 seqId, s32 *outDefaultBank) {
    u8 bank;
    s32 offset;
    s32 i;
    void *ret = NULL;

    offset = ((u16 *)gAlBankSets)[canonicalize_index(SOUND_POOL_SEQ, seqId)];
    bank = 0xFF;
    for (i = gAlBankSets[offset++]; i > 0; i--) {
        bank = gAlBankSets[offset++];
        ret = func_sh_802f3688(bank);
    }
    *outDefaultBank = bank;
    return ret;
}

void preload_sequence(u32 seqId, s32 preloadMask) {
    s32 temp;

    seqId = canonicalize_index(SOUND_POOL_SEQ, seqId);
    if (preloadMask & PRELOAD_BANKS) {
        load_banks_immediate(seqId, &temp);
    }
    if (preloadMask & PRELOAD_SEQUENCE) {
        func_sh_802f3564(seqId);
    }
}

s32 func_sh_802f2f38(struct AudioBankSample *sample, s32 bankId) {
    u8 *sp24;

    if (sample->isPatched && sample->medium != 0) {
        sp24 = func_sh_802f1d90(sample->size, bankId, sample->sampleAddr, sample->medium);
        if (sp24 == NULL) {
            return -1;
        }
        if (sample->medium == 1) {
            func_sh_802f3d78((uintptr_t) sample->sampleAddr, sp24, sample->size, gAlTbl->unk2);
        } else {
            func_sh_802f3c38((uintptr_t) sample->sampleAddr, sp24, sample->size, sample->medium);
        }
        sample->medium = 0;
        sample->sampleAddr = sp24;
    }
    return 0;
}

s32 func_sh_802f3024(s32 bankId, s32 instId, s32 drumId) {
    struct Instrument *instr;
    struct Drum *drum;

    if (instId < 0x7F) {
        instr = get_instrument_inner(bankId, instId);
        if (instr == NULL) {
            return -1;
        }
        if (instr->normalRangeLo != 0) {
            func_sh_802f2f38(instr->lowNotesSound.sample, bankId);
        }
        func_sh_802f2f38(instr->normalNotesSound.sample, bankId);
        if (instr->normalRangeHi != 0x7F) {
            func_sh_802f2f38(instr->highNotesSound.sample, bankId);
        }
    } else if (instId == 0x7F) {
        drum = get_drum(bankId, drumId);
        if (drum == NULL) {
            return -1;
        }
        func_sh_802f2f38(drum->sound.sample, bankId);
    }
    return 0;
}

void func_sh_802f30f4(s32 arg0, s32 numChunks, s32 arg2, OSMesgQueue *retQueue) {
    if (func_802f3f08(SOUND_POOL_UNK, canonicalize_index(SOUND_POOL_UNK, arg0), numChunks, arg2, retQueue) == 0) {
        osSendMesg(retQueue, 0, 0);
    }
}

void func_sh_802f3158(s32 seqId, s32 numChunks, s32 arg2, OSMesgQueue *retQueue) {
    s32 val = ((u16 *) gAlBankSets)[canonicalize_index(SOUND_POOL_SEQ, seqId)];
    s32 v = gAlBankSets[val++];

    while (v > 0) {
        func_802f3f08(SOUND_POOL_BANK, canonicalize_index(SOUND_POOL_BANK, gAlBankSets[val++]), numChunks, arg2, retQueue);
        v--;
    }
}

u8 *func_sh_802f3220(u32 seqId, u32 *a1) {
    s32 val = ((u16 *) gAlBankSets)[canonicalize_index(SOUND_POOL_SEQ, seqId)];
    *a1 = gAlBankSets[val++];
    if (*a1 == 0) {
        return NULL;
    }
    return &gAlBankSets[val];
}

void func_sh_802f3288(s32 idx) {
    s32 bankId;
    s32 s2;

    idx = ((u16*)gAlBankSets)[canonicalize_index(SOUND_POOL_SEQ, idx)];
    s2 = gAlBankSets[idx++];
    while (s2 > 0) {
        s2--;
        bankId = canonicalize_index(SOUND_POOL_BANK, gAlBankSets[idx++]);

        if (unk_pool1_lookup(SOUND_POOL_BANK, bankId) == NULL) {
            func_sh_802f3368(bankId);
            if (gBankLoadStatus[bankId] != SOUND_LOAD_STATUS_5) {
                gBankLoadStatus[bankId] = SOUND_LOAD_STATUS_NOT_LOADED;
            }

            continue;
        }
    }
}

void func_sh_802f3368(s32 bankId) {
    struct SoundMultiPool *pool = &gBankLoadedPool;
    struct TemporaryPool *temporary = &pool->temporary;
    struct PersistentPool *persistent;
    u32 i;

    if (temporary->entries[0].id == bankId) {
        temporary->entries[0].id = -1;
    } else if (temporary->entries[1].id == bankId) {
        temporary->entries[1].id = -1;
    }

    persistent = &pool->persistent;
    for (i = 0; i < persistent->numEntries; i++) {
        if (persistent->entries[i].id == bankId) {
            persistent->entries[i].id = -1;
        }
    }

    discard_bank(bankId);
}

void load_sequence_internal(s32 player, s32 seqId, s32 loadAsync);
void load_sequence(u32 player, u32 seqId, s32 loadAsync) {
    load_sequence_internal(player, seqId, loadAsync);
}

void load_sequence_internal(s32 player, s32 seqId, UNUSED s32 loadAsync) {
    struct SequencePlayer *seqPlayer;
    u8 *sequenceData;
    u32 s0;
    s32 count;
    u8 bank;

    seqPlayer = &gSequencePlayers[player];

    seqId = canonicalize_index(SOUND_POOL_SEQ, seqId);
    sequence_player_disable(seqPlayer);

    s0 = ((u16 *) gAlBankSets)[seqId];
    count = gAlBankSets[s0++];
    bank = 0xff;

    while (count > 0) {
        bank = gAlBankSets[s0++];
        func_sh_802f3688(bank);
        count--;
    }

    sequenceData = func_sh_802f3564(seqId);
    init_sequence_player(player);
    seqPlayer->seqId = seqId;
    seqPlayer->defaultBank[0] = bank;
    seqPlayer->enabled = TRUE;
    seqPlayer->seqData = sequenceData;
    seqPlayer->scriptState.pc = sequenceData;
    seqPlayer->scriptState.depth = 0;
    seqPlayer->delay = 0;
    seqPlayer->finished = FALSE;
}

void *func_sh_802f3564(s32 seqId) {
    s32 seqId2 = canonicalize_index(SOUND_POOL_SEQ, seqId);
    s32 temp;
    return func_sh_802f3764(SOUND_POOL_SEQ, seqId2, &temp);
}

extern u8 gUnkLoadStatus[MAX_NUM_SOUNDBANKS];

void *func_sh_802f3598(s32 idx, s32 *medium) {
    void *ret;
    ALSeqFile *f;
    s32 temp;
    s32 sp28;

    f = get_audio_file_header(SOUND_POOL_UNK);
    idx = canonicalize_index(SOUND_POOL_UNK, idx);
    ret = get_bank_or_seq_wrapper(SOUND_POOL_UNK, idx);
    if (ret != NULL) {
        if (gUnkLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
            gUnkLoadStatus[idx] = SOUND_LOAD_STATUS_COMPLETE;
        }

        *medium = 0;
        return ret;
    }

    temp = f->seqArray[idx].magic;
    if (temp == 4) {
        *medium = f->seqArray[idx].medium;
        return f->seqArray[idx].offset;
    } else {
        ret = func_sh_802f3764(SOUND_POOL_UNK, idx, &sp28);
        if (ret != 0) {
            *medium = 0;
            return ret;
        }

        *medium = f->seqArray[idx].medium;
    }
    return f->seqArray[idx].offset;

}

void *func_sh_802f3688(s32 bankId) {
    void *ret;
    s32 bankId1;
    s32 bankId2;
    s32 sp38;
    struct PatchStruct patchInfo;

    bankId = canonicalize_index(SOUND_POOL_BANK, bankId);
    bankId1 = gCtlEntries[bankId].bankId1;
    bankId2 = gCtlEntries[bankId].bankId2;

    patchInfo.bankId1 = bankId1;
    patchInfo.bankId2 = bankId2;

    if (patchInfo.bankId1 != 0xFF) {
        patchInfo.baseAddr1 = func_sh_802f3598(patchInfo.bankId1, &patchInfo.medium1);
    } else {
        patchInfo.baseAddr1 = NULL;
    }

    if (bankId2 != 0xFF) {
        patchInfo.baseAddr2 = func_sh_802f3598(bankId2, &patchInfo.medium2);
    } else {
        patchInfo.baseAddr2 = NULL;
    }

    ret = func_sh_802f3764(SOUND_POOL_BANK, bankId, &sp38);

    if (ret != NULL && sp38 == 1) {
        func_sh_802f5310(bankId, ret, &patchInfo, 0);
    }

    return ret;
}

void *func_sh_802f3764(s32 poolIdx, s32 idx, s32 *arg2) {
    s8 loadStatus;

    void *vAddr = get_bank_or_seq_wrapper(poolIdx, idx);
    if (vAddr != NULL) {
        *arg2 = 0;
        loadStatus = SOUND_LOAD_STATUS_COMPLETE;
    } else {
        ALSeqFile *f = get_audio_file_header(poolIdx);
        s32 size = f->seqArray[idx].len;
        size = ALIGN16(size);
        s32 medium = f->seqArray[idx].medium;
        s32 sp18 = f->seqArray[idx].magic;
        u8 *devAddr = f->seqArray[idx].offset;

        switch (sp18) {
            case 0:
                vAddr = unk_pool1_alloc(poolIdx, idx, size);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;
            case 1:
                vAddr = alloc_bank_or_seq(poolIdx, size, 1, idx);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;
            case 2:
                vAddr = alloc_bank_or_seq(poolIdx, size, 0, idx);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;

            case 3:
            case 4:
                vAddr = alloc_bank_or_seq(poolIdx, size, 2, idx);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;
        }

        *arg2 = 1;
        if (medium == 1) {
            func_sh_802f3d78((uintptr_t) devAddr, vAddr, size, f->unk2);
        } else {
            func_sh_802f3c38((uintptr_t) devAddr, vAddr, size, medium);
        }

        switch (sp18) {
            case 0:
                loadStatus = SOUND_LOAD_STATUS_5;
                break;

            default:
                loadStatus = SOUND_LOAD_STATUS_COMPLETE;
                break;
        }
    }

    switch (poolIdx) {
        case SOUND_POOL_SEQ:
            if (gSeqLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
                gSeqLoadStatus[idx] = loadStatus;
            }
            break;

        case SOUND_POOL_BANK:
            if (gBankLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
                gBankLoadStatus[idx] = loadStatus;
            }
            break;

        case SOUND_POOL_UNK:
            if (gUnkLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
                gUnkLoadStatus[idx] = loadStatus;
            }
            break;
    }

    return vAddr;
}

s32 canonicalize_index(s32 poolIdx, s32 idx) {
    ALSeqFile *f = get_audio_file_header(poolIdx);
    if (f->seqArray[idx].len == 0) {
        idx = (s32) (uintptr_t) f->seqArray[idx].offset;
    }
    return idx;
}

void *get_bank_or_seq_wrapper(s32 poolIdx, s32 id) {
    void *ret = unk_pool1_lookup(poolIdx, id);
    if (ret != NULL) {
        return ret;
    }
    ret = get_bank_or_seq(poolIdx, 2, id);
    if (ret != 0) {
        return ret;
    }
    return NULL;
}

ALSeqFile *get_audio_file_header(s32 poolIdx) {
    ALSeqFile *ret;
    switch (poolIdx) {
        default:
            ret = NULL;
            break;
        case SOUND_POOL_SEQ:
            ret = gSeqFileHeader;
            break;
        case SOUND_POOL_BANK:
            ret = gAlCtlHeader;
            break;
        case SOUND_POOL_UNK:
            ret = gAlTbl;
            break;
    }
    return ret;
}

void patch_audio_bank(s32 bankId, struct AudioBank *mem, struct PatchStruct *patchInfo) {
    struct Instrument *instrument;
    struct Instrument **itInstrs;
    struct Instrument **end;
    s32 i;
    void *patched;
    struct Drum *drum;

#define BASE_OFFSET(x, base) (void *)((uintptr_t) (x) + (uintptr_t) (base))
#define PATCH(x, base) (patched = BASE_OFFSET((x), (base)))

    s32 numDrums = gCtlEntries[bankId].numDrums;
    s32 numInstruments = gCtlEntries[bankId].numInstruments;
    struct Drum **drums = mem->drums;
    if (drums != NULL && numDrums != 0) {
        mem->drums = PATCH(drums, mem);
        for (i = 0; i < numDrums; i++) {
            patched = mem->drums[i];
            if (patched != NULL) {
                drum = PATCH(patched, mem);
                mem->drums[i] = drum;
                if (!drum->loaded) {
                    patch_sound(&drum->sound, mem, patchInfo);
                    patched = drum->envelope;
                    drum->envelope = BASE_OFFSET(patched, mem);
                    drum->loaded = TRUE;
                }
            }
        }
    }

    if (numInstruments > 0) {
        itInstrs = mem->instruments;
        end = numInstruments + itInstrs;

        do {
            if (*itInstrs != NULL) {
                *itInstrs = BASE_OFFSET(*itInstrs, mem);
                instrument = *itInstrs;

                if (!instrument->loaded) {
                    if (instrument->normalRangeLo != 0) {
                        patch_sound(&instrument->lowNotesSound, mem, patchInfo);
                    }
                    patch_sound(&instrument->normalNotesSound, mem, patchInfo);
                    if (instrument->normalRangeHi != 0x7F) {
                        patch_sound(&instrument->highNotesSound, mem, patchInfo);
                    }
                    patched = instrument->envelope;

                    instrument->envelope = BASE_OFFSET(patched, mem);
                    instrument->loaded = TRUE;
                }
            }
            itInstrs++;
        } while (itInstrs != end);
    }
    gCtlEntries[bankId].drums       = mem->drums;
    gCtlEntries[bankId].instruments = mem->instruments;
#undef PATCH
#undef BASE_OFFSET
}

extern char shindouDebugPrint81[]; // "FastCopy"
extern char shindouDebugPrint82[]; // "FastCopy"
void func_sh_802f3c38(uintptr_t devAddr, void *vAddr, size_t nbytes, s32 medium) {
    nbytes = ALIGN16(nbytes);
    osInvalDCache(vAddr, nbytes);

    while (TRUE) {
        if (gAudioLoadLockSH != 0) {
            continue;
        }

        if (nbytes >= 0x400U) {
            func_sh_802f3dd0(&gAudioDmaIoMesg, 1, 0, devAddr, vAddr, 0x400, &gAudioDmaMesgQueue, medium, shindouDebugPrint81);
            osRecvMesg(&gAudioDmaMesgQueue, NULL, 1);
            nbytes -= 0x400;
            devAddr += 0x400;
            vAddr = (u8*)vAddr + 0x400;
            continue;
        }
        break;
    }

    if (nbytes != 0) {
        func_sh_802f3dd0(&gAudioDmaIoMesg, 1, 0, devAddr, vAddr, nbytes, &gAudioDmaMesgQueue, medium, shindouDebugPrint82);
        osRecvMesg(&gAudioDmaMesgQueue, NULL, 1);
    }
}

void func_sh_802f3d78(UNUSED uintptr_t devAddr, void *vAddr, size_t nbytes, UNUSED s32 arg3) {
    osInvalDCache(vAddr, nbytes);
}

s32 func_sh_802f3dd0(OSIoMesg *m, s32 pri, s32 direction, uintptr_t devAddr, void *dramAddr, s32 size, OSMesgQueue *retQueue, s32 medium, UNUSED const char *reason) {
    OSPiHandle *handle;
    if (gAudioLoadLockSH >= 0x11U) {
        return -1;
    }
    switch (medium) {
        case 2:
            handle = osCartRomInit();
            break;
        default:
            return 0;
    }
    if ((size & 0xf) != 0) {
        size = ALIGN16(size);
    }
    m->hdr.pri = pri;
    m->hdr.retQueue = retQueue;
    m->dramAddr = dramAddr;
    m->devAddr = devAddr;
    m->size = size;
    handle->transferInfo.cmdType = 2;
    osEPiStartDma(handle, m, direction);
    return 0;
}

UNUSED void *func_sh_802f3ee8(s32 poolIdx, s32 idx) {
    s32 temp;
    return func_sh_802f3764(poolIdx, idx, &temp);
}

void *func_802f3f08(s32 poolIdx, s32 idx, s32 numChunks, s32 arg3, OSMesgQueue *retQueue) {
    s32 loadStatus;

    switch (poolIdx) {
        case SOUND_POOL_SEQ: // sequence
            if (gSeqLoadStatus[idx] == SOUND_LOAD_STATUS_IN_PROGRESS) {
                return 0;
            }
            break;
        case SOUND_POOL_BANK: // bank
            if (gBankLoadStatus[idx] == SOUND_LOAD_STATUS_IN_PROGRESS) {
                return 0;
            }
            break;
        case SOUND_POOL_UNK: // unknown
            if (gUnkLoadStatus[idx] == SOUND_LOAD_STATUS_IN_PROGRESS) {
                return 0;
            }
            break;

    }
    void *vAddr = get_bank_or_seq_wrapper(poolIdx, idx);
    if (vAddr != NULL) {
        loadStatus = SOUND_LOAD_STATUS_COMPLETE;
        osSendMesg(retQueue, (OSMesg) (arg3 << 24), 0);
    } else {
        ALSeqFile *f = get_audio_file_header(poolIdx);
        s32 size = f->seqArray[idx].len;
        size = ALIGN16(size);
        s32 medium = f->seqArray[idx].medium;
        s32 sp18 = f->seqArray[idx].magic;
        uintptr_t devAddr = (uintptr_t) f->seqArray[idx].offset;
        loadStatus = SOUND_LOAD_STATUS_COMPLETE;

        switch (sp18) {
            case 0:
                vAddr = unk_pool1_alloc(poolIdx, idx, size);
                if (vAddr == NULL) {
                    return vAddr;
                }
                loadStatus = SOUND_LOAD_STATUS_5;
                break;
            case 1:
                vAddr = alloc_bank_or_seq(poolIdx, size, 1, idx);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;
            case 2:
                vAddr = alloc_bank_or_seq(poolIdx, size, 0, idx);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;

            case 4:
            case 3:
                vAddr = alloc_bank_or_seq(poolIdx, size, 2, idx);
                if (vAddr == NULL) {
                    return vAddr;
                }
                break;
        }

        func_sh_802f4cb4(devAddr, vAddr, size, medium, numChunks, retQueue, ((arg3 << 0x18) | (poolIdx << 0x10) | (idx << 8) | loadStatus));
        loadStatus = SOUND_LOAD_STATUS_IN_PROGRESS;
    }

    switch (poolIdx) {
        case SOUND_POOL_SEQ: // sequence
            if (gSeqLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
                gSeqLoadStatus[idx] = loadStatus;
            }
            break;

        case SOUND_POOL_BANK: // bank
            if (gBankLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
                gBankLoadStatus[idx] = loadStatus;
            }
            break;

        case SOUND_POOL_UNK: // unknown
            if (gUnkLoadStatus[idx] != SOUND_LOAD_STATUS_5) {
                gUnkLoadStatus[idx] = loadStatus;
            }
            break;
    }

    return vAddr;
}

void func_sh_802f41e4(s32 audioResetStatus) {
    func_sh_802f4a4c(audioResetStatus);
    func_sh_802f573c(audioResetStatus);
    func_sh_802f4dcc(audioResetStatus);
}

u8 gShindouSoundBanksHeader[] = {
#include "sound/ctl_header.inc.c"
};

u8 gBankSetsData[] = {
#include "sound/bank_sets.inc.c"
};

u8 gShindouSampleBanksHeader[] = {
#include "sound/tbl_header.inc.c"
};

u8 gShindouSequencesHeader[] = {
#include "sound/sequences_header.inc.c"
};

// (void) must be omitted from parameters
void audio_init() {
    s32 i, j, k;
    s32 lim;
    u64 *ptr64;
    void *data;
    s32 seqCount;

    gAudioLoadLockSH = 0;

    for (i = 0; i < gAudioHeapSize / 8; i++) {
        ((u64 *) gAudioHeap)[i] = 0;
    }

#ifdef TARGET_N64
    // It seems boot.s doesn't clear the .bss area for audio, so do it here.
    lim = ((uintptr_t) &gAudioGlobalsEndMarker - (uintptr_t) &gAudioGlobalsStartMarker) / 8;
    ptr64 = &gAudioGlobalsStartMarker;
    for (k = lim; k >= 0; k--) {
        *ptr64++ = 0;
    }
#endif

    D_EU_802298D0 = 16.713f;
    gRefreshRate = 60;
    port_eu_init();

#ifdef TARGET_N64
    eu_stubbed_printf_3(
        "Clear Workarea %x -%x size %x \n",
        (uintptr_t) &gAudioGlobalsStartMarker,
        (uintptr_t) &gAudioGlobalsEndMarker,
        (uintptr_t) &gAudioGlobalsEndMarker - (uintptr_t) &gAudioGlobalsStartMarker
    );
#endif

    eu_stubbed_printf_1("AudioHeap is %x\n", gAudioHeapSize);

    for (i = 0; i < NUMAIBUFFERS; i++) {
        gAiBufferLengths[i] = 0xA0;
    }

    gAudioFrameCount = 0;
    gAudioTaskIndex = 0;
    gCurrAiBufferIndex = 0;
    gSoundMode = SOUND_MODE_STEREO;
    gAudioTask = NULL;
    gAudioTasks[0].task.t.data_size = 0;
    gAudioTasks[1].task.t.data_size = 0;
    osCreateMesgQueue(&gAudioDmaMesgQueue, &gAudioDmaMesg, 1);
    osCreateMesgQueue(&gCurrAudioFrameDmaQueue, gCurrAudioFrameDmaMesgBufs,
                      ARRAY_COUNT(gCurrAudioFrameDmaMesgBufs));
    osCreateMesgQueue(&gUnkQueue1, gUnkMesgBufs1, 0x10);
    osCreateMesgQueue(&gUnkQueue2, gUnkMesgBufs2, 0x10);
    gCurrAudioFrameDmaCount = 0;
    gSampleDmaNumListItems = 0;

    sound_init_main_pools(gAudioInitPoolSize);

    for (i = 0; i < NUMAIBUFFERS; i++) {
        gAiBuffers[i] = sound_alloc_uninitialized(&gAudioInitPool, AIBUFFER_LEN);

        for (j = 0; j < (s32) (AIBUFFER_LEN / sizeof(s16)); j++) {
            gAiBuffers[i][j] = 0;
        }
    }

    gAudioResetPresetIdToLoad = 0;
    gAudioResetStatus = 1;
    audio_shut_down_and_reset_step();

    // Not sure about these prints
    eu_stubbed_printf_1("Heap reset.Synth Change %x \n", 0);
    eu_stubbed_printf_3("Heap %x %x %x\n", 0, 0, 0);
    eu_stubbed_printf_0("Main Heap Initialize.\n");

    // Load headers for sounds and sequences
    gSeqFileHeader = (ALSeqFile *) gShindouSequencesHeader;
    gAlCtlHeader   = (ALSeqFile *) gShindouSoundBanksHeader;
    gAlTbl         = (ALSeqFile *) gShindouSampleBanksHeader;
    gAlBankSets = gBankSetsData;
    gSequenceCount = (s16) gSeqFileHeader->seqCount;
    patch_seq_file(gSeqFileHeader, gMusicData,     D_SH_80315EF4);
    patch_seq_file(gAlCtlHeader,   gSoundDataADSR, D_SH_80315EF8);
    patch_seq_file(gAlTbl,         gSoundDataRaw,  D_SH_80315EFC);
    seqCount = gAlCtlHeader->seqCount;
    gCtlEntries = sound_alloc_uninitialized(&gAudioInitPool, seqCount * sizeof(struct CtlEntry));
    for (i = 0; i < seqCount; i++) {
        gCtlEntries[i].bankId1        = (u8) ((gAlCtlHeader->seqArray[i].ctl.as_s16.bankAndFf              >> 8) & 0xff);
        gCtlEntries[i].bankId2        = (u8) ((gAlCtlHeader->seqArray[i].ctl.as_s16.bankAndFf                  ) & 0xff);
        gCtlEntries[i].numInstruments = (u8) ((gAlCtlHeader->seqArray[i].ctl.as_s16.numInstrumentsAndDrums >> 8) & 0xff);
        gCtlEntries[i].numDrums       = (u8) ((gAlCtlHeader->seqArray[i].ctl.as_s16.numInstrumentsAndDrums     ) & 0xff);
    }
    data = sound_alloc_uninitialized(&gAudioInitPool, D_SH_80315EF0);
    if (data == NULL) {
        D_SH_80315EF0 = 0;
    }
    sound_alloc_pool_init(&gUnkPool1.pool, data, D_SH_80315EF0);
    init_sequence_players();
}

s32 func_sh_802f47c8(s32 bankId, u8 idx, s8 *io) {
    struct AudioBankSample *sample = func_sh_802f4978(bankId, idx);
    struct PendingDmaSample *temp;
    if (sample == NULL) {
        *io = 0;
        return -1;
    }
    if (sample->medium == 0) {
        *io = 2;
        return 0;
    }
    temp = &D_SH_80343D00.arr[D_SH_80343D00.someIndex];
    if (temp->state == 3) {
        temp->state = 0;
    }
    temp->sample = *sample;
    temp->io = io;
    temp->vAddr = func_sh_802f1d40(sample->size, bankId, sample->sampleAddr, sample->medium);
    if (temp->vAddr == NULL) {
        if (sample->medium == 1 || sample->codec == CODEC_SKIP) {
            *io = 0;
            return -1;
        } else {
            *io = 3;
            return -1;
        }
    }
    temp->state = 1;
    temp->remaining = ALIGN16(sample->size);
    temp->resultSampleAddr = (u8 *) temp->vAddr;
    temp->devAddr = (uintptr_t) sample->sampleAddr;
    temp->medium = sample->medium;
    temp->bankId = bankId;
    temp->idx = idx;
    D_SH_80343D00.someIndex ^= 1;
    return 0;
}

struct AudioBankSample *func_sh_802f4978(s32 bankId, s32 idx) {
    struct Drum *drum;
    struct Instrument *inst;
    struct AudioBankSample *ret;

    if (idx < 128) {
        inst = get_instrument_inner(bankId, idx);
        if (inst == 0) {
            return NULL;
        }
        ret = inst->normalNotesSound.sample;
    } else {
        drum = get_drum(bankId, idx - 128);
        if (drum == 0) {
            return NULL;
        }
        ret = drum->sound.sample;
    }
    return ret;
}

void func_sh_802f49e4(struct PendingDmaSample *arg0) {
    struct AudioBankSample *sample = func_sh_802f4978(arg0->bankId, arg0->idx);
    if (sample != NULL) {
        arg0->sample = *sample;
        sample->sampleAddr = arg0->resultSampleAddr;
        sample->medium = 0;
    }
}

void func_sh_802f4a4c(s32 audioResetStatus) {
    ALSeqFile *alTbl;
    struct PendingDmaSample *entry;

    s32 i;

    alTbl = gAlTbl;

    for (i = 0; i < ARRAY_COUNT(D_SH_80343D00.arr); i++) {
        entry = &D_SH_80343D00.arr[i];
        switch (entry->state) {
            case 2:
                osRecvMesg(&entry->queue, NULL, 1);
                if (audioResetStatus != 0) {
                    entry->state = 3;
                    break;
                }
                // fallthrough
                FALL_THROUGH;

            case 1:
                entry->state = 2;
                if (entry->remaining == 0) {
                    func_sh_802f49e4(entry);
                    entry->state = 3;
                    *entry->io = 1;
                } else if (entry->remaining < 0x1000) {
                    if (entry->medium == 1) {
                        func_sh_802f4c5c(entry->devAddr, entry->vAddr, entry->remaining, alTbl->unk2);
                    } else {
                        func_sh_802f4bd8(entry, entry->remaining);
                    }
                    entry->remaining = 0;
                } else {
                    if (entry->medium == 1) {
                        func_sh_802f4c5c(entry->devAddr, entry->vAddr, 0x1000, alTbl->unk2);
                    } else {
                        func_sh_802f4bd8(entry, 0x1000);
                    }
                    entry->remaining = (s32) (entry->remaining - 0x1000);
                    entry->vAddr = (u8 *) entry->vAddr + 0x1000;
                    entry->devAddr = entry->devAddr + 0x1000;
                }
                break;
        }
    }
}

extern char shindouDebugPrint102[]; // "SLOWCOPY"
void func_sh_802f4bd8(struct PendingDmaSample *arg0, s32 len) { // len must be signed
    osInvalDCache(arg0->vAddr, len);
    osCreateMesgQueue(&arg0->queue, arg0->mesgs, 1);
    func_sh_802f3dd0(&arg0->ioMesg, 0, 0, arg0->devAddr, arg0->vAddr, len, &arg0->queue, arg0->medium, shindouDebugPrint102);
}

void func_sh_802f4c5c(UNUSED uintptr_t devAddr, void *vAddr, size_t nbytes, UNUSED s32 arg3) {
    osInvalDCache(vAddr, nbytes);
}

struct PendingDmaAudioBank *func_sh_802f4cb4(uintptr_t devAddr, void *vAddr, s32 size, s32 medium, s32 numChunks, OSMesgQueue *retQueue, s32 encodedInfo) {
    struct PendingDmaAudioBank *item;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sPendingDmaAudioBanks); i++) {
        if (sPendingDmaAudioBanks[i].inProgress == 0) {
            item = &sPendingDmaAudioBanks[i];
            break;
        }
    }
    if (i == ARRAY_COUNT(sPendingDmaAudioBanks)) {
        return NULL;
    }

    item->inProgress = 1;
    item->devAddr = devAddr;
    item->audioBank = vAddr;
    item->vAddr = vAddr;
    item->remaining = size;
    if (numChunks == 0) {
        item->transferSize = 0x1000;
    } else {
        item->transferSize = ((size / numChunks) + 0xFF) & ~0xFF;
        if (item->transferSize < 0x100) {
            item->transferSize = 0x100;
        }
    }
    item->retQueue = retQueue;
    item->timer = 3;
    item->medium = medium;
    item->encodedInfo = encodedInfo;
    osCreateMesgQueue(&item->dmaRetQueue, item->mesgs, 1);
    return item;
}

void func_sh_802f4dcc(s32 audioResetStatus) {
    s32 i;

    if (gAudioLoadLockSH != 1) {
        for (i = 0; i < ARRAY_COUNT(sPendingDmaAudioBanks); i++) {
            if (sPendingDmaAudioBanks[i].inProgress == 1) {
                func_sh_802f4e50(&sPendingDmaAudioBanks[i], audioResetStatus);
            }
        }
    }
}

void func_sh_802f4e50(struct PendingDmaAudioBank *audioBank, s32 audioResetStatus) {
    ALSeqFile *alSeqFile;
    u32 *encodedInfo;
    OSMesg mesg;
    u32 temp;
    u32 bankId;
    s32 bankId1;
    s32 bankId2;
    struct PatchStruct patchStruct;
    alSeqFile = gAlTbl;
    if (audioBank->timer >= 2) {
        audioBank->timer--;
        return;
    }
    if (audioBank->timer == 1) {
        audioBank->timer = 0;
    } else {
        if (audioResetStatus != 0) {
            osRecvMesg(&audioBank->dmaRetQueue, NULL, OS_MESG_BLOCK);
            audioBank->inProgress = 0;
            return;
        }
        if (osRecvMesg(&audioBank->dmaRetQueue, NULL, OS_MESG_NOBLOCK) == -1) {
            return;
        }
    }

    encodedInfo = &audioBank->encodedInfo;
    if (audioBank->remaining == 0) {
        mesg = (OSMesg) audioBank->encodedInfo;
        temp = *encodedInfo;
        bankId = ((temp >> 8) & BITMASK(8));
        switch ((u8) (temp >> 0x10)) {
            case 0:
                if (gSeqLoadStatus[bankId] != SOUND_LOAD_STATUS_5) {
                    gSeqLoadStatus[bankId] = (u8) (temp & BITMASK(8));
                }
                break;
            case 2:
                if (gUnkLoadStatus[bankId] != SOUND_LOAD_STATUS_5) {
                    gUnkLoadStatus[bankId] = (u8) (temp & BITMASK(8));
                }
                break;
            case 1:
                if (gBankLoadStatus[bankId] != SOUND_LOAD_STATUS_5) {
                    gBankLoadStatus[bankId] = (u8) (temp & BITMASK(8));
                }
                bankId1 = gCtlEntries[bankId].bankId1;
                bankId2 = gCtlEntries[bankId].bankId2;
                patchStruct.bankId1 = bankId1;
                patchStruct.bankId2 = bankId2;
                if (bankId1 != BITMASK(8)) {
                    patchStruct.baseAddr1 = func_sh_802f3598(bankId1, &patchStruct.medium1);
                } else {
                    patchStruct.baseAddr1 = NULL;
                }
                if (bankId2 != BITMASK(8)) {
                    patchStruct.baseAddr2 = func_sh_802f3598(bankId2, &patchStruct.medium2);
                } else {
                    patchStruct.baseAddr2 = NULL;
                }

                func_sh_802f5310(bankId, audioBank->audioBank, &patchStruct, 1);
                break;
        }
        mesg = (OSMesg) audioBank->encodedInfo;
        audioBank->inProgress = 0;
        osSendMesg(audioBank->retQueue, mesg, OS_MESG_NOBLOCK);
    } else if (audioBank->remaining < audioBank->transferSize) {
        if (audioBank->medium == 1) {
            func_sh_802f517c(audioBank->devAddr, audioBank->vAddr, audioBank->remaining, alSeqFile->unk2);
        } else {
            func_sh_802f50ec(audioBank, audioBank->remaining);
        }

        audioBank->remaining = 0;
    } else {
        if (audioBank->medium == 1) {
            func_sh_802f517c(audioBank->devAddr, audioBank->vAddr, audioBank->transferSize, alSeqFile->unk2);
        } else {
            func_sh_802f50ec(audioBank, audioBank->transferSize);
        }

        audioBank->remaining -= audioBank->transferSize;
        audioBank->devAddr   += audioBank->transferSize;
        audioBank->vAddr = ((u8 *) audioBank->vAddr) + audioBank->transferSize;
    }
}

extern char shindouDebugPrint110[]; // "BGCOPY"
void func_sh_802f50ec(struct PendingDmaAudioBank *arg0, size_t len) {
    len += 0xf;
    len &= ~0xf;
    osInvalDCache(arg0->vAddr, len);
    osCreateMesgQueue(&arg0->dmaRetQueue, arg0->mesgs, 1);
    func_sh_802f3dd0(&arg0->ioMesg, 0, 0, arg0->devAddr, arg0->vAddr, len, &arg0->dmaRetQueue, arg0->medium, shindouDebugPrint110);
}

void func_sh_802f517c(UNUSED uintptr_t devAddr, void *vAddr, size_t nbytes, UNUSED s32 arg3) {
    osInvalDCache(vAddr, nbytes);
}

void patch_sound(struct AudioBankSound *sound, struct AudioBank *memBase, struct PatchStruct *patchInfo) {
    struct AudioBankSample *sample;
    void *patched;

#define PATCH(x, base) (patched = (void *)((uintptr_t) (x) + (uintptr_t) base))

    if ((uintptr_t) sound->sample <= 0x80000000) {
        sample = sound->sample = PATCH(sound->sample, memBase);
        if (sample->size != 0 && !sample->isPatched) {
            sample->loop = PATCH(sample->loop, memBase);
            sample->book = PATCH(sample->book, memBase);
            switch (sample->medium) {
                case 0:
                    sample->sampleAddr = PATCH(sample->sampleAddr, patchInfo->baseAddr1);
                    sample->medium = patchInfo->medium1;
                    break;
                case 1:
                    sample->sampleAddr = PATCH(sample->sampleAddr, patchInfo->baseAddr2);
                    sample->medium = patchInfo->medium2;
                    break;

                case 2:
                case 3:
                    break;
            }
            sample->isPatched = TRUE;
            if (sample->bit1 && sample->medium != 0) {
                D_SH_8034EA88[D_SH_8034F688++] = sample;
            }
        }
    }
#undef PATCH
}

void func_sh_802f5310(s32 bankId, struct AudioBank *mem, struct PatchStruct *patchInfo, s32 arg3) {
    u8 *addr = NULL;
    s32 sp4C;
    struct AudioBankSample *temp_s0;
    s32 i;
    uintptr_t count;
    s32 temp;

    sp4C = 0;
    if (D_SH_8034F68C != 0) {
        sp4C = 1;
    }
    D_SH_8034F688 = 0;
    patch_audio_bank(bankId, mem, patchInfo);

    count = 0;
    for (i = 0; i < D_SH_8034F688; i++) {
        count += ALIGN16(D_SH_8034EA88[i]->size);
    }

    for (i = 0; i < D_SH_8034F688; i++) {
        if (D_SH_8034F68C != 0x78) {
            temp_s0 = D_SH_8034EA88[i];
            switch (arg3) {
                case 0:
                    temp = temp_s0->medium = patchInfo->medium1;
                    if (temp != 0) {
                        addr = func_sh_802f1d90(temp_s0->size, patchInfo->bankId1, temp_s0->sampleAddr, temp_s0->medium);
                    } else {
                        temp = temp_s0->medium = patchInfo->medium2;
                        if (temp != 0) {
                            addr = func_sh_802f1d90(temp_s0->size, patchInfo->bankId2, temp_s0->sampleAddr, temp_s0->medium);
                        }
                    }
                    break;

                case 1:
                    temp = temp_s0->medium = patchInfo->medium1;
                    if (temp != 0) {
                        addr = func_sh_802f1d40(temp_s0->size, patchInfo->bankId1, temp_s0->sampleAddr, temp_s0->medium);
                    } else {
                        temp = temp_s0->medium = patchInfo->medium2;
                        if (temp != 0) {
                            addr = func_sh_802f1d40(temp_s0->size, patchInfo->bankId2, temp_s0->sampleAddr, temp_s0->medium);
                        }
                    }
                    break;
            }
            if (addr != NULL) {
                switch (arg3) {
                    case 0:
                        if (temp_s0->medium == 1) {
                            func_sh_802f3d78((uintptr_t) temp_s0->sampleAddr, addr, temp_s0->size, gAlTbl->unk2);
                            temp_s0->sampleAddr = addr;
                            temp_s0->medium = 0;
                        } else {
                            func_sh_802f3c38((uintptr_t) temp_s0->sampleAddr, addr, temp_s0->size, temp_s0->medium);
                            temp_s0->sampleAddr = addr;
                            temp_s0->medium = 0;
                        }
                        break;

                    case 1:
                        D_SH_8034EC88[D_SH_8034F68C].sample = temp_s0;
                        D_SH_8034EC88[D_SH_8034F68C].ramAddr = addr;
                        D_SH_8034EC88[D_SH_8034F68C].encodedInfo = ((D_SH_8034F68C << 24) | 0xffffff);
                        D_SH_8034EC88[D_SH_8034F68C].isFree = FALSE;
                        D_SH_8034EC88[D_SH_8034F68C].endAndMediumIdentification = temp_s0->sampleAddr + temp_s0->size + temp_s0->medium;
                        D_SH_8034F68C++;
                        break;
                }
            }
            continue;
        }
        break;
    }

    D_SH_8034F688 = 0;
    if (D_SH_8034F68C != 0 && sp4C == 0) {
        temp_s0 = D_SH_8034EC88[D_SH_8034F68C - 1].sample;
        temp = (temp_s0->size >> 12) + 1;
        count = (uintptr_t) temp_s0->sampleAddr;
        func_sh_802f4cb4(
            count,
            D_SH_8034EC88[D_SH_8034F68C - 1].ramAddr,
            temp_s0->size,
            temp_s0->medium,
            temp,
            &gUnkQueue2,
            D_SH_8034EC88[D_SH_8034F68C - 1].encodedInfo);
    }
}

s32 func_sh_802f573c(s32 audioResetStatus) {
    struct AudioBankSample *sample;
    u32 idx;
    u8 *sampleAddr;
    u32 size;
    s32 unk;
    u8 *added;

    if (D_SH_8034F68C > 0) {
        if (audioResetStatus != 0) {
            osRecvMesg(&gUnkQueue2, (OSMesg *) &idx, OS_MESG_NOBLOCK);
            D_SH_8034F68C = 0;
            return FALSE;
        }
        if (osRecvMesg(&gUnkQueue2, (OSMesg *) &idx, OS_MESG_NOBLOCK) == -1) {
            return FALSE;
        }
        idx >>= 0x18;
        if (!D_SH_8034EC88[idx].isFree) {
            sample = D_SH_8034EC88[idx].sample;
            added = (sample->sampleAddr + sample->size + sample->medium);
            if (added == D_SH_8034EC88[idx].endAndMediumIdentification) {
                sample->sampleAddr = D_SH_8034EC88[idx].ramAddr;
                sample->medium = 0;
            }
            D_SH_8034EC88[idx].isFree = TRUE;
        }

        while (D_SH_8034F68C > 0) {
            if (D_SH_8034EC88[D_SH_8034F68C - 1].isFree) {
                D_SH_8034F68C--;
                continue;
            }
            sample = D_SH_8034EC88[D_SH_8034F68C - 1].sample;
            sampleAddr = sample->sampleAddr;
            size = sample->size;
            unk = (size >> 0xC) + 1;
            added = ((sampleAddr + size) + sample->medium);
            if (added != D_SH_8034EC88[D_SH_8034F68C - 1].endAndMediumIdentification) {
                D_SH_8034EC88[D_SH_8034F68C - 1].isFree = TRUE;
                D_SH_8034F68C--;
                continue;
            }
            size = sample->size;
            func_sh_802f4cb4((uintptr_t) sampleAddr, D_SH_8034EC88[D_SH_8034F68C - 1].ramAddr, size, sample->medium,
                             unk, &gUnkQueue2, D_SH_8034EC88[D_SH_8034F68C - 1].encodedInfo);
            break;
        }
    }
    return TRUE;
}

s32 func_sh_802f5900(struct AudioBankSample *sample, s32 numLoaded, struct AudioBankSample *arg2[]) {
    s32 i;

    for (i = 0; i < numLoaded; i++) {
        if (sample->sampleAddr == arg2[i]->sampleAddr) {
            break;
        }
    }
    if (i == numLoaded) {
        arg2[numLoaded++] = sample;
    }
    return numLoaded;
}

s32 func_sh_802f5948(s32 bankId, struct AudioBankSample *list[]) {
    s32 i;
    struct Drum *drum;
    struct Instrument *inst;
    s32 numLoaded;
    s32 numDrums;
    s32 numInstruments;

    numLoaded = 0;
    numDrums = gCtlEntries[bankId].numDrums;
    numInstruments = gCtlEntries[bankId].numInstruments;

    for (i = 0; i < numDrums; i++) {
        drum = get_drum(bankId, i);
        if (drum == NULL) {
            continue;
        }
        numLoaded = func_sh_802f5900(drum->sound.sample, numLoaded, list);
    }
    for (i = 0; i < numInstruments; i++) {
        inst = get_instrument_inner(bankId, i);
        if (inst == NULL) {
            continue;

        }
        if (inst->normalRangeLo != 0) {
            numLoaded = func_sh_802f5900(inst->lowNotesSound.sample, numLoaded, list);
        }
        if (inst->normalRangeHi != 127) {
            numLoaded = func_sh_802f5900(inst->highNotesSound.sample, numLoaded, list);
        }
        numLoaded = func_sh_802f5900(inst->normalNotesSound.sample, numLoaded, list);
    }
    return numLoaded;
}
#endif
