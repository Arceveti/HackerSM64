// koopa_shell_underwater.inc.c

struct ObjectHitbox sKoopaShellUnderwaterHitbox = {
    .interactType      = INTERACT_GRABBABLE,
    .downOffset        = 0,
    .damageOrCoinValue = 0,
    .health            = 1,
    .numLootCoins      = 0,
    .radius            = 80,
    .height            = 50,
    .hurtboxRadius     = 0,
    .hurtboxHeight     = 0,
};

void set_koopa_shell_underwater_hitbox(void) {
    obj_set_hitbox(o, &sKoopaShellUnderwaterHitbox);
}

void bhv_koopa_shell_underwater_loop(void) {
    switch (o->oHeldState) {
        case HELD_FREE:
            set_koopa_shell_underwater_hitbox();
            break;
        case HELD_HELD:
            cur_obj_unrender_set_action_and_anim(OBJ_ANIM_NONE, KOOPA_SHELL_UNDERWATER_ACT_DEFAULT);
            break;
        case HELD_THROWN:
        case HELD_DROPPED:
            spawn_mist_particles();
            obj_mark_for_deletion(o);
            break;
    }

    if (o->oInteractStatus & INT_STATUS_STOP_RIDING) {
        spawn_mist_particles();
        obj_mark_for_deletion(o);
    }

    o->oInteractStatus = INT_STATUS_NONE;
}
