#pragma once


typedef union {
    u8 b1;
} Mem8;

typedef union {
    struct PACKED {
        u8 b1;
        u8 b2;
    };
    u16 h1;
} Mem16;

typedef union {
    struct PACKED {
        u8 b1;
        u8 b2;
        u8 b3;
        u8 b4;
    };
    struct PACKED {
        u16 h1;
        u16 h2;
    };
    u32 w1;
} Mem32;

typedef union {
    struct PACKED {
        u8 b1;
        u8 b2;
        u8 b3;
        u8 b4;
        u8 b5;
        u8 b6;
        u8 b7;
        u8 b8;
    };
    struct PACKED {
        u16 h1;
        u16 h2;
        u16 h3;
        u16 h4;
    };
    struct PACLED {
        u32 w1;
        u32 w2;
    };
    u64 d1;
} Mem64;

typedef union {
    struct PACKED {
        u8   : 4;
        u8 i : 4;
    };
} I4;

typedef union {
    u8 i;
} I8;

typedef union {
    struct PACKED {
        u8   : 4;
        u8 i : 3;
        u8 a : 1;
    };
    struct PACKED {
        u8    : 4;
        u8 ia : 4;
    };
} IA4;

typedef union {
    struct PACKED {
        u8 i : 4;
        u8 a : 4;
    };
    u8 ia;
} IA8;

typedef union {
    struct PACKED {
        u16 i : 8;
        u16 a : 8;
    };
    u16 ia;
} IA16;

typedef union {
    struct PACKED {
        u16 r : 5;
        u16 g : 5;
        u16 b : 5;
        u16 a : 1;
    };
    u16 rgba;
} RGBA16;

typedef union {
    struct PACKED {
        u32 r : 8;
        u32 g : 8;
        u32 b : 8;
        u32 a : 8;
    };
    u32 rgba;
} RGBA32;

typedef union {
    struct PACKED {
        u8 : 4;
        u8 ci : 4;
    };
} CI4;

typedef union {
    u8 ci;
} CI8;



typedef union {
    struct PACKED {
        u8 _0:1;
        u8 _1:1;
        u8 _2:1;
        u8 _3:1;
        u8 _4:1;
        u8 _5:1;
        u8 _6:1;
        u8 _7:1;
    } bit;
    struct PACKED {
        u8 _1;
    } byte;
} Byte;

typedef union {
    struct PACKED {
        Byte _1;
        Byte _2;
    } byte;
    struct PACKED {
        u16 _1;
    } halfWord;
} HalfWord;

typedef union {
    struct PACKED {
        Byte _1;
        Byte _2;
        Byte _3;
        Byte _4;
    } byte;
    struct PACKED {
        HalfWord _1;
        HalfWord _2;
    } halfWord;
    struct PACKED {
        u32 _1;
    } word;
} Word;

typedef union {
    struct PACKED {
        Byte _1;
        Byte _2;
        Byte _3;
        Byte _4;
        Byte _5;
        Byte _6;
        Byte _7;
        Byte _8;
    } byte;
    struct PACKED {
        HalfWord _1;
        HalfWord _2;
        HalfWord _3;
        HalfWord _4;
    } halfWord;
    struct PACKED {
        Word _1;
        Word _2;
    } word;
    struct PACKED {
        u64 _1;
    } doubleWord
} DoubleWord;

typedef union {
    struct PACKED {
        u8 _1;
    } b;
} Mem8;

typedef union {
    struct PACKED {
        u8  _1;
        u8  _2;
    } b;
    struct PACKED {
        u16 _1;
    } h;
} Mem16;

typedef union {
    struct PACKED {
        u8  _1;
        u8  _2;
        u8  _3;
        u8  _4;
    } b;
    struct PACKED {
        u16 _1;
        u16 _2;
    } h;
    struct PACKED {
        u32 _1;
    } w;
} Mem32;

typedef union {
    struct PACKED {
        u8  _1;
        u8  _2;
        u8  _3;
        u8  _4;
        u8  _5;
        u8  _6;
        u8  _7;
        u8  _8;
    } b;
    struct PACKED {
        u16 _1;
        u16 _2;
        u16 _3;
        u16 _4;
    } h;
    struct PACKED {
        u32 _1;
        u32 _2;
    } w;
    struct PACKED {
        u64 _1;
    } d;
} Mem64;
