// bullet_bill.inc.c

void bhv_white_puff_smoke_init(void) {
    cur_obj_scale((random_float() * 2) + 2.0f);
}

void bhv_bullet_bill_init(void) {
    o->oBulletBillInitialMoveYaw = o->oMoveAngleYaw;
}

void bullet_bill_act_reset(void) {
    cur_obj_become_tangible();
    o->oForwardVel = 0.0f;
    o->oMoveAngleYaw = o->oBulletBillInitialMoveYaw;
    o->oFaceAnglePitch = 0;
    o->oFaceAngleRoll = 0;
    o->oMoveFlags = OBJ_MOVE_NONE;
    cur_obj_set_pos_to_home();
    o->oAction = BULLET_BILL_ACT_IDLE;
}

void bullet_bill_act_idle(void) {
    if (o->oDistanceToMario >  400.0f
     && o->oDistanceToMario < 1500.0f
     && abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x2000) {
        o->oAction = BULLET_BILL_ACT_CHASING_MARIO;
    }
}

void bullet_bill_act_chasing_mario(void) {
    if (o->oTimer < 40) {
        o->oForwardVel = 3.0f;
    } else if (o->oTimer < 50) {
        o->oForwardVel = ((o->oTimer % 2) != 0) ? 3.0f : -3.0f;
    } else {
        if (o->oTimer > 70) {
            cur_obj_update_floor_and_walls();
        }

        spawn_object(o, MODEL_SMOKE, bhvWhitePuffSmoke);
        o->oForwardVel = 30.0f;

        if (o->oDistanceToMario > 300.0f) {
            cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x100);
        }

        if (o->oTimer == 50) {
            cur_obj_play_sound_2(SOUND_OBJ_POUNDING_CANNON);
            cur_obj_shake_screen(SHAKE_POS_SMALL);
        }

        if (o->oTimer > 150 || (o->oMoveFlags & OBJ_MOVE_HIT_WALL)) {
            o->oAction = BULLET_BILL_ACT_HIT_WALL;
            spawn_mist_particles();
        }
    }
}

void bullet_bill_act_hit_wall(void) {
    o->oAction = BULLET_BILL_ACT_RESET;
}

void bullet_bill_act_hit_mario(void) {
    if (o->oTimer == 0) {
        o->oForwardVel = -30.0f;
        cur_obj_become_intangible();
    }

    o->oFaceAnglePitch += 0x1000;
    o->oFaceAngleRoll += 0x1000;
    o->oPosY += 20.0f;

    if (o->oTimer > 90) {
        o->oAction = BULLET_BILL_ACT_RESET;
    }
}

ObjActionFunc sBulletBillActions[] = {
    bullet_bill_act_reset,
    bullet_bill_act_idle,
    bullet_bill_act_chasing_mario,
    bullet_bill_act_hit_wall,
    bullet_bill_act_hit_mario,
};

void bhv_bullet_bill_loop(void) {
    cur_obj_call_action_function(sBulletBillActions);
    if (cur_obj_check_interacted()) {
        o->oAction = BULLET_BILL_ACT_HIT_MARIO;
    }
}
