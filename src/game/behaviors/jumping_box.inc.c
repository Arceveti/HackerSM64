// jumping_box.inc.c

struct ObjectHitbox sJumpingBoxHitbox = {
    .interactType      = INTERACT_GRABBABLE,
    .downOffset        = 20,
    .damageOrCoinValue = 0,
    .health            = 1,
    .numLootCoins      = 5,
    .radius            = 150,
    .height            = 250,
    .hurtboxRadius     = 150,
    .hurtboxHeight     = 250,
};

void jumping_box_act_0(void) {
    if (o->oSubAction == 0) {
        if (o->oJumpingBoxRandomTimer-- < 0) {
            o->oSubAction++;
        }
        if (o->oTimer > o->oJumpingBoxUnusedTimerMin) {
            o->oVelY = random_float() * 5.0f + 15.0f;
            o->oSubAction++;
        }
    } else if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
        o->oSubAction = 0;
        o->oJumpingBoxRandomTimer = random_float() * 60.0f + 30.0f;
    }
}

void jumping_box_act_1(void) {
    if (o->oMoveFlags & (OBJ_MOVE_HIT_WALL | OBJ_MOVE_MASK_IN_WATER | OBJ_MOVE_LANDED)) {
        obj_mark_for_deletion(o);
        spawn_mist_particles();
    }
}

ObjActionFunc sJumpingBoxActions[] = {
    jumping_box_act_0,
    jumping_box_act_1
};

void jumping_box_free_update(void) {
    cur_obj_scale(0.5f);
    obj_set_hitbox(o, &sJumpingBoxHitbox);
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
    cur_obj_call_action_function(sJumpingBoxActions);
}

void bhv_jumping_box_loop(void) {
    switch (o->oHeldState) {
        case HELD_FREE:
            jumping_box_free_update();
            break;

        case HELD_HELD:
            obj_copy_pos(o, gMarioObject);
            cur_obj_unrender_set_action_and_anim(OBJ_ANIM_NONE, JUMPING_BOX_ACT_IDLE);
            break;

        case HELD_THROWN:
            cur_obj_get_thrown_or_placed(40.0f, 20.0f, 1);
            break;

        case HELD_DROPPED:
            cur_obj_get_dropped();
            o->oAction = 1;
            break;
    }

    if (o->oInteractStatus & INT_STATUS_STOP_RIDING) {
        create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
        obj_explode_and_spawn_coins(46.0f, COIN_TYPE_YELLOW);
    }

    o->oInteractStatus = INT_STATUS_NONE;
}
