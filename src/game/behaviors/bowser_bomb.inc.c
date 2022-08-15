// bowser_bomb.inc.c

void bhv_bowser_bomb_loop(void) {
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        o->oInteractStatus &= ~INT_STATUS_INTERACTED;
        spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
        obj_mark_for_deletion(o);
    }

    if (o->oInteractStatus & INT_STATUS_HIT_MINE) {
        spawn_object(o, MODEL_BOWSER_FLAMES, bhvBowserBombExplosion);
        create_sound_spawner(SOUND_GENERAL_BOWSER_BOMB_EXPLOSION);
        set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY, o->oPosZ);
        obj_mark_for_deletion(o);
    }

    set_object_visibility(o, 7000);
}

void bhv_bowser_bomb_explosion_loop(void) {
    cur_obj_scale((((f32) o->oTimer / 14.0f) * 9.0f) + 1.0f);

    if (((o->oTimer & (4 - 1)) == 0) && (o->oTimer < 20)) {
        struct Object *mineSmoke = spawn_object(o, MODEL_BOWSER_SMOKE, bhvBowserBombSmoke);
        mineSmoke->oPosX += random_float() * 600.0f - 400.0f;
        mineSmoke->oPosZ += random_float() * 600.0f - 400.0f;
        mineSmoke->oVelY += random_float() * 10.0f;
    }

    if ((o->oTimer & (2 - 1)) == 0) {
        o->oAnimState++;
    }

    if (o->oTimer == 28) {
        obj_mark_for_deletion(o);
    }
}

void bhv_bowser_bomb_smoke_loop(void) {
    cur_obj_scale((((f32) o->oTimer / 14.0f) * 9.0f) + 1.0f);

    if ((o->oTimer & (2 - 1)) == 0) {
        o->oAnimState++;
    }

    o->oOpacity -= 10;

    if (o->oOpacity < 10) {
        o->oOpacity = 0;
    }

    o->oPosY += o->oVelY;

    if (o->oTimer == 28) {
        obj_mark_for_deletion(o);
    }
}
