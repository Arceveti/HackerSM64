// jrb_ship.inc.c

struct ObjectHitbox sSkullSlidingBoxHitbox = {
    .interactType      = INTERACT_DAMAGE,
    .downOffset        = 0,
    .damageOrCoinValue = 1,
    .health            = 1,
    .numLootCoins      = 0,
    .radius            = 130,
    .height            = 100,
    .hurtboxRadius     = 0,
    .hurtboxHeight     = 0,
};

void bhv_jrb_floating_ship_loop(void) {
    s16 initialPitch = o->oFaceAnglePitch;
    s16 initialRoll = o->oFaceAngleRoll;

    cur_obj_set_pos_to_home_with_debug();

    o->oShipPart3LoopPitch += 0x100;

    o->oFaceAnglePitch = sins(o->oShipPart3LoopPitch) * 0x400;
    o->oFaceAngleRoll = sins(o->oShipPart3LoopRoll) * 0x400;
    o->oAngleVelPitch = o->oFaceAnglePitch - initialPitch;
    o->oAngleVelRoll = o->oFaceAngleRoll - initialRoll;

    if (gMarioObject->oPosY > 1000.0f) {
        cur_obj_play_sound_1(SOUND_ENV_BOAT_ROCKING1);
    }
}

void bhv_jrb_sliding_box_loop(void) {
    Vec3s shipRotation;
    struct Object *shipObj;

    if (o->oJrbSlidingBoxShipObj == NULL) {
        shipObj = cur_obj_nearest_object_with_behavior(bhvJrbFloatingShipCollision);

        if (shipObj != NULL) {
            o->oJrbSlidingBoxShipObj = shipObj;
            vec3f_diff(&o->oParentRelativePosVec, &o->oPosVec, &shipObj->oPosVec);
        }
    } else {
        shipObj = o->oJrbSlidingBoxShipObj;
        vec3i_to_vec3s(shipRotation, &shipObj->oFaceAngleVec);
        vec3f_local_pos_to_world_pos(&o->oPosVec, &o->oParentRelativePosVec, &shipObj->oPosVec, shipRotation);
        o->oFaceAnglePitch = shipObj->oFaceAnglePitch;
    }

    o->oJrbSlidingBoxAdditiveZ = sins(o->oJrbSlidingBoxAngle) * 20.0f;
    o->oJrbSlidingBoxAngle += 0x100;
    o->oParentRelativePosZ += o->oJrbSlidingBoxAdditiveZ;

    if (gMarioObject->oPosY > 1000.0f && absf(o->oJrbSlidingBoxAdditiveZ) > 3.0f) {
        cur_obj_play_sound_1(SOUND_AIR_ROUGH_SLIDE);
    }

    obj_set_hitbox(o, &sSkullSlidingBoxHitbox);

    if (!(o->oJrbSlidingBoxAngle & 0x7FFF)) {
        cur_obj_become_tangible();
    }

    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        o->oInteractStatus = INT_STATUS_NONE;
        cur_obj_become_intangible();
    }
}
