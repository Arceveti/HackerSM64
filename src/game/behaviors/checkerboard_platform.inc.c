// checkerboard_platform.inc.c

struct CheckerBoardPlatformInitPosition {
    s32 relPosZ;
    Vec3f scale;
    f32 radius;
};

struct CheckerBoardPlatformInitPosition sCheckerBoardPlatformInitPositions[] = {
    { .relPosZ = 145, .scale = { 0.7f, 1.5f, 0.7f }, .radius =  7.0f },
    { .relPosZ = 235, .scale = { 1.2f, 2.0f, 1.2f }, .radius = 11.6f }
};

void bhv_checkerboard_elevator_group_init(void) {
    s32 relativePosZ;
    s32 i;
    struct Object *platformObj;

    if (o->oBehParams2ndByte == 0) {
        o->oBehParams2ndByte = 65;
    }

    s32 relativePosY = o->oBehParams2ndByte * 10;
    s32 type = GET_BPARAM1(o->oBehParams);

    for (i = 0; i < ARRAY_COUNT(sCheckerBoardPlatformInitPositions); i++) {
        if (i == 0) {
            relativePosZ = -sCheckerBoardPlatformInitPositions[type].relPosZ;
        } else {
            relativePosZ =  sCheckerBoardPlatformInitPositions[type].relPosZ;
        }
        platformObj = spawn_object_relative(i, 0, (i * relativePosY), relativePosZ, o, MODEL_CHECKERBOARD_PLATFORM, bhvCheckerboardPlatformSub);
        platformObj->oCheckerBoardPlatformRadius = sCheckerBoardPlatformInitPositions[type].radius;
        vec3f_copy(platformObj->header.gfx.scale, sCheckerBoardPlatformInitPositions[type].scale);
    }
}

void checkerboard_plat_act_move_y(f32 vel, s32 time) {
    o->oMoveAnglePitch = 0x0;
    o->oAngleVelPitch = 0x0;
    o->oForwardVel = 0.0f;
    o->oVelY = vel;
    if (o->oTimer > time) {
        o->oAction++; // rotate
    }
}

void checkerboard_plat_act_rotate(s32 nextAction, s16 pitch) {
    o->oVelY = 0.0f;
    o->oAngleVelPitch = pitch;
    if ((o->oTimer + 1) == (0x8000 / abss(pitch))) {
        o->oAction = nextAction;
    }
    o->oCheckerBoardPlatformRotateAction = nextAction;
}

void bhv_checkerboard_platform_init(void) {
    o->oCheckerBoardPlatformHeight = o->parentObj->oBehParams2ndByte;
}

void bhv_checkerboard_platform_loop(void) {
    f32 radius = o->oCheckerBoardPlatformRadius;

    o->oCheckerBoardPlatformRotateAction = CHECKERBOARD_PLATFORM_ACT_MOVE_VERTICALLY;

    if (o->oDistanceToMario < 1000.0f) {
        cur_obj_play_sound_1(SOUND_ENV_ELEVATOR4);
    }

    switch (o->oAction) {
        case CHECKERBOARD_PLATFORM_ACT_MOVE_VERTICALLY:
            if (o->oBehParams2ndByte == CHECKERBOARD_PLATFORM_BP_MOVE_UP) {
                o->oAction = CHECKERBOARD_PLATFORM_ACT_MOVE_UP;
            } else {
                o->oAction = CHECKERBOARD_PLATFORM_ACT_MOVE_DOWN;
            }
            break;
        case CHECKERBOARD_PLATFORM_ACT_MOVE_UP:
            checkerboard_plat_act_move_y(10.0f, o->oCheckerBoardPlatformHeight);
            break;
        case CHECKERBOARD_PLATFORM_ACT_ROTATE_UP:
            checkerboard_plat_act_rotate(3, 512);
            break;
        case CHECKERBOARD_PLATFORM_ACT_MOVE_DOWN:
            checkerboard_plat_act_move_y(-10.0f, o->oCheckerBoardPlatformHeight);
            break;
        case CHECKERBOARD_PLATFORM_ACT_ROTATE_DOWN:
            checkerboard_plat_act_rotate(1, -512);
            break;
    }

    o->oMoveAnglePitch += absi(o->oAngleVelPitch);
    o->oFaceAnglePitch += absi(o->oAngleVelPitch);
    o->oFaceAngleYaw = o->oMoveAngleYaw;

    if (o->oMoveAnglePitch != 0x0) {
        f32 mul = ((o->oAngleVelPitch >= 0x0) ? 1.0f : -1.0f) * radius;
        o->oForwardVel = mul * sins(o->oMoveAnglePitch);
        o->oVelY       = mul * coss(o->oMoveAnglePitch);
    }

    if (o->oCheckerBoardPlatformRotateAction == CHECKERBOARD_PLATFORM_ACT_MOVE_UP) {
        o->oAngleVelPitch = 0x0;
        o->oFaceAnglePitch &= ~0x7FFF;
        cur_obj_move_using_fvel_and_gravity();
    } else {
        cur_obj_move_using_fvel_and_gravity();
    }

    load_object_collision_model();
}
