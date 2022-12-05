// whirlpool.inc.c

static struct ObjectHitbox sWhirlpoolHitbox = {
    /* interactType:      */ INTERACT_WHIRLPOOL,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 200,
    /* height:            */ 500,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

void bhv_whirlpool_init(void) {
    o->oWhirlpoolInitFacePitch = o->oFaceAnglePitch;
    o->oWhirlpoolInitFaceRoll = o->oFaceAngleRoll;
    o->oFaceAnglePitch = 0;
    o->oFaceAngleRoll = 0;
}

void whirpool_orient_graph(void) {
    f32 cosPitch = coss(o->oFaceAnglePitch);
    f32 sinPitch = sins(o->oFaceAnglePitch);
    f32 cosRoll = coss(o->oFaceAngleRoll);
    f32 sinRoll = sins(o->oFaceAngleRoll);
    f32 normalX = sinRoll * cosPitch;
    f32 normalY = cosPitch * cosRoll;
    f32 normalZ = sinPitch;
    obj_orient_graph(o, normalX, normalY, normalZ);
}

void bhv_whirlpool_loop(void) {
    if (o->oDistanceToMario < 5000.0f) {
        cur_obj_unhide();

        // not sure if actually an array
        gEnvFxBubbleConfig->particleCount = 60;
        vec3_copy_y_off(gEnvFxBubbleConfig->src, &o->oPosVec, 800.0f);
        vec3f_to_vec3s(gEnvFxBubbleConfig->dest, &o->oPosVec);
        gEnvFxBubbleConfig->angle[0] = o->oWhirlpoolInitFacePitch;
        gEnvFxBubbleConfig->angle[1] = o->oWhirlpoolInitFaceRoll;

        whirpool_orient_graph();

        o->oFaceAngleYaw += 8000;
    } else {
        cur_obj_hide();
        gEnvFxBubbleConfig->particleCount = 0;
    }

    cur_obj_play_sound_1(SOUND_ENV_WATER);

    obj_set_hitbox(o, &sWhirlpoolHitbox);
}

void bhv_jet_stream_loop(void) {
    if (o->oDistanceToMario < 5000.0f) {
        gEnvFxBubbleConfig->particleCount = 60;
        vec3f_to_vec3s(gEnvFxBubbleConfig->src, &o->oPosVec);
    } else {
        gEnvFxBubbleConfig->particleCount = 0;
    }

    cur_obj_play_sound_1(SOUND_ENV_WATER);
}
