#ifndef MACROS_H
#define MACROS_H

#include <ultra64.h>

#include "platform_info.h"

#ifndef __sgi
#define GLOBAL_ASM(...)
#endif

#define member(T, m) ((T*)0)->m
#define sizeof_member(T, m) sizeof(member(T, m))
#define typeof_member(T, m) typeof(member(T, m))

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

#define BITS_PER_BYTE   8
#define BITS_PER_HEX    4
#define SIZEOF_BITS(t)  (sizeof(t) * BITS_PER_BYTE)
#define SIZEOF_HEX(t)   (SIZEOF_BITS(t) / BITS_PER_HEX)

#define GLUE(a, b) a ## b
#define GLUE2(a, b) GLUE(a, b)

#define TO_STRING(s)    #s
#define TO_STRING2(s)   TO_STRING(s)

#define STRLEN(s)           (sizeof(s) - 1)
#define STR_LAST_CHAR(s)    (s)[STRLEN(s) - 1]

// Includes the raw data of the file at 'path' into the rom, aligned by 'align' number of bytes, and creates a pointer to it with the given type and name.
#define INCBIN(type, name, path, align) \
    __asm__( \
        ".section \".rodata\", \"a\", @progbits\n" \
        ".balign "TO_STRING2(align)"\n" \
        ".global "TO_STRING2(name)"\n" \
        TO_STRING2(name)":\n" \
        ".incbin \""path"\"\n" \
        ".previous\n" \
    ); \
    extern type name[];

// Avoid compiler warnings for unused variables.
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

// Avoid undefined behaviour for non-returning functions.
#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

// Always inline a function.
#ifdef __GNUC__
#define ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ALWAYS_INLINE inline
#endif

// Never inline a function.
#ifdef __GNUC__
#define NEVER_INLINE __attribute__((noinline))
#else
#define NEVER_INLINE
#endif

// Fall through a switch case.
#ifdef __GNUC__
#define FALL_THROUGH __attribute__((fallthrough))
#else
#define FALL_THROUGH
#endif

// Use Og when compiling the function.
#ifdef __GNUC__
#define OPTIMIZE_OG __attribute__((optimize("Og")))
#else
#define OPTIMIZE_OG
#endif

// Use Os when compiling the function.
#ifdef __GNUC__
#define OPTIMIZE_OS __attribute__((optimize("Os")))
#else
#define OPTIMIZE_OS
#endif

// Use Ofast when compiling the function.
#ifdef __GNUC__
#define OPTIMIZE_OFAST __attribute__((optimize("Ofast")))
#else
#define OPTIMIZE_OFAST
#endif

// Ignore 4-byte alignment in structs.
#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

// Align to 4-byte boundary.
#ifdef __GNUC__
#define ALIGNED4 __attribute__((aligned(4)))
#else
#define ALIGNED4
#endif

// Align to 8-byte boundary (for DMA requirements).
#ifdef __GNUC__
#define ALIGNED8 __attribute__((aligned(8)))
#else
#define ALIGNED8
#endif

// Align to 16-byte boundary (for audio lib requirements).
#ifdef __GNUC__
#define ALIGNED16 __attribute__((aligned(16)))
#else
#define ALIGNED16
#endif

// Align to 32-byte boundary.
#ifdef __GNUC__
#define ALIGNED32 __attribute__((aligned(32)))
#else
#define ALIGNED32
#endif

// Align to 64-byte boundary.
#ifdef __GNUC__
#define ALIGNED64 __attribute__((aligned(64)))
#else
#define ALIGNED64
#endif

// Align to previous boundry.
#ifndef ALIGNFLOOR
#define ALIGNFLOOR(VAL_, ALIGNMENT_) ((VAL_) & ~(ALIGNMENT_ - 1))
#endif

// Align to next boundary.
#ifndef ALIGN
#define ALIGN(VAL_, ALIGNMENT_) (((VAL_) + ((ALIGNMENT_) - 1)) & ~((ALIGNMENT_) - 1))
#endif

// Round up to the next multiple.
#define ALIGN4(val)  ALIGN((val),  4)
#define ALIGN8(val)  ALIGN((val),  8)
#define ALIGN16(val) ALIGN((val), 16)
#define ALIGN32(val) ALIGN((val), 32)
#define ALIGN64(val) ALIGN((val), 64)

#ifndef NO_SEGMENTED_MEMORY
// Convert a physical address to virtual.
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr) | 0x80000000)

// Convert a virtual address to physical.
#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr) & 0x1FFFFFFF)

// Another way of converting virtual to physical.
#define VIRTUAL_TO_PHYSICAL2(addr)  ((u8*)(addr) - 0x80000000U)
#else // NO_SEGMENTED_MEMORY
// No conversion needed other than cast.
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr))
#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr))
#define VIRTUAL_TO_PHYSICAL2(addr)  ((void*)(addr))
#endif // NO_SEGMENTED_MEMORY


//! TODO: Move everything below here to assert.h?:

// Static (compile-time) assertions.
#ifdef __GNUC__
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define STATIC_ASSERT(cond, msg) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#endif

#include "game/asm.h"

#define FORCE_CRASH() EXCEPTION_RMISS() //! TODO: This should be EXCEPTION_TRAP() to save 8 bytes, but that freezes most emulators instead of doing a proper crash.

// Set where the program counter will be on the next crash.
#define SET_CRASH_PTR(ptr) \
do { \
    extern uintptr_t gSetCrashAddress; \
    gSetCrashAddress = (uintptr_t)&(ptr); \
} while (0)

// Cause a crash with the program counter at a specific location.
#define FORCE_CRASH_AT_PTR(ptr) \
do { \
    SET_CRASH_PTR(ptr); \
    FORCE_CRASH(); \
} while (0)

#endif // MACROS_H
