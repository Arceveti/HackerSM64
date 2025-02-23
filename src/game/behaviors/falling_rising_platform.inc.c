// falling_rising_platform.inc.c

void bhv_squishable_platform_loop(void) {
    o->header.gfx.scale[1] = ((sins(o->oBitfsPlatformTimer) + 1.0f) * 0.3f) + 0.4f;
    o->oBitfsPlatformTimer += 0x80;
}

void bhv_bitfs_sinking_platform_loop(void) {
    o->oPosY -= sins(o->oBitfsPlatformTimer) * 0.58f;
    o->oBitfsPlatformTimer += 0x100;
}

void bhv_bitfs_sinking_cage_pole_loop(void) {
    obj_copy_pos_and_angle(o, o->parentObj);
}

void bhv_bitfs_sinking_cage_platform_loop(void) {
    if (o->oBehParams2ndByte != SINKING_POLE_PLATFORM_BP_LOW) {
        if (o->oTimer == 0) {
            o->oPosY -= 300.0f;
        }
        o->oPosY += sins(o->oBitfsPlatformTimer) * 7.0f;
    } else {
        o->oPosY -= sins(o->oBitfsPlatformTimer) * 3.0f;
    }

    o->oBitfsPlatformTimer += 0x100;
}
