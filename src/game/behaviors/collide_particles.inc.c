// collide_particles.inc.c

struct TinyParticleMovementParams {
    s16 yaw;
    s16 vel;
};

static const struct TinyParticleMovementParams sTinyTriMovementParams[] = {
    { .yaw = 0xD000, .vel = 0x0000 },
    { .yaw = 0x3000, .vel = 0x0000 },
    { .yaw = 0xDE67, .vel = 0x2199 },
    { .yaw = 0x2199, .vel = 0x2199 },
    { .yaw = 0xDE67, .vel = 0xDE67 },
    { .yaw = 0x2199, .vel = 0xDE67 },
};

void bhv_punch_tiny_triangle_loop(void) {
    if (o->oTimer == 0) {
        s16 yaw = o->oMoveAngleYaw;
        o->oCollisionParticleScale = 1.28f;
        cur_obj_set_pos_relative(gMarioObject, 0.0f, 60.0f, 100.0f);
        o->oMoveAngleYaw = yaw;
    }

    cur_obj_move_using_fvel_and_gravity();
    o->oAnimState = 5;
    cur_obj_scale(o->oCollisionParticleScale);
    o->oCollisionParticleScale -= 0.2f;

    if ((gDebugInfo[DEBUG_PAGE_EFFECTINFO][0] + 6) < o->oTimer) {
        obj_mark_for_deletion(o);
    }
}

void bhv_punch_tiny_triangle_init(void) {
    s32 i;
    struct Object *triangle;

    for (i = 0; i < ARRAY_COUNT(sTinyTriMovementParams); i++) {
        triangle = spawn_object(o, MODEL_DIRT_ANIMATION, bhvPunchTinyTriangle);
        triangle->oMoveAngleYaw = gMarioObject->oMoveAngleYaw + sTinyTriMovementParams[i].yaw + 0x8000;
        triangle->oVelY       = sins(sTinyTriMovementParams[i].vel) * 25.0f;
        triangle->oForwardVel = coss(sTinyTriMovementParams[i].vel) * 25.0f;
    }
}

static const struct TinyParticleMovementParams sTinyStarMovementParams[] = {
    { .yaw = 0xE000, .vel = 0x0000 },
    { .yaw = 0x0000, .vel = 0x0000 },
    { .yaw = 0x2000, .vel = 0x0000 },
    { .yaw = 0xE99A, .vel = 0x1666 },
    { .yaw = 0x1666, .vel = 0x1666 },
    { .yaw = 0xE99A, .vel = 0xE99A },
    { .yaw = 0x1666, .vel = 0xE99A },
};

void bhv_wall_tiny_star_particle_loop(void) {
    if (o->oTimer == 0) {
        s16 yaw = o->oMoveAngleYaw;
        o->oCollisionParticleScale = 0.28f;
        cur_obj_set_pos_relative(gMarioObject, 0.0f, 30.0f, 110.0f);
        o->oMoveAngleYaw = yaw;
    }

    cur_obj_move_using_fvel_and_gravity();
    o->oAnimState = TINY_DIRT_PARTICLE_ANIM_STATE_YELLOW;
    cur_obj_scale(o->oCollisionParticleScale);
    o->oCollisionParticleScale -= 0.015f;
}

void bhv_tiny_star_particles_init(void) {
    s32 i;
    struct Object *particle;

    for (i = 0; i < ARRAY_COUNT(sTinyStarMovementParams); i++) {
        particle = spawn_object(o, MODEL_CARTOON_STAR, bhvWallTinyStarParticle);
        particle->oMoveAngleYaw = gMarioObject->oMoveAngleYaw + sTinyStarMovementParams[i].yaw + 0x8000;
        particle->oVelY       = sins(sTinyStarMovementParams[i].vel) * 25.0f;
        particle->oForwardVel = coss(sTinyStarMovementParams[i].vel) * 25.0f;
    }
}

void bhv_pound_tiny_star_particle_loop(void) {
    if (o->oTimer == 0) {
        o->oCollisionParticleScale = 0.28f;
        o->oForwardVel = 25.0f;
        o->oPosY -= 20.0f;
        o->oVelY = 14.0f;
    }

    cur_obj_move_using_fvel_and_gravity();
    o->oAnimState = TINY_DIRT_PARTICLE_ANIM_STATE_YELLOW;
    cur_obj_scale(o->oCollisionParticleScale);
    o->oCollisionParticleScale -= 0.015f;
}

void bhv_pound_tiny_star_particle_init(void) {
    s32 i;
    const s32 amt = 8;

    struct Object *particle;
    for (i = 0; i < amt; i++) {
        particle = spawn_object(o, MODEL_CARTOON_STAR, bhvPoundTinyStarParticle);
        particle->oMoveAngleYaw = ((i * 65536) / amt);
    }
}
