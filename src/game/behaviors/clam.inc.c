// clam.inc.c

struct ObjectHitbox sClamShellHitbox = {
    .interactType      = INTERACT_CLAM_OR_BUBBA,
    .downOffset        = 0,
    .damageOrCoinValue = 2,
    .health            = 99,
    .numLootCoins      = 0,
    .radius            = 150,
    .height            = 80,
    .hurtboxRadius     = 150,
    .hurtboxHeight     = 80,
};

void clam_act_closing(void) {
    if (cur_obj_init_anim_check_frame(CLAM_ANIM_CLOSING, 25)) {
        cur_obj_play_sound_2(SOUND_GENERAL_CLAM_SHELL_CLOSE);
        spawn_mist_from_global();
        cur_obj_become_tangible();

        o->oClamShakeTimer = 10;
        o->oTimer = CLAM_ACT_CLOSING;
    } else if (o->oTimer > 150 && o->oDistanceToMario < 500.0f) {
        cur_obj_play_sound_2(SOUND_GENERAL_CLAM_SHELL_OPEN);
        o->oAction = CLAM_ACT_OPENING;
    } else if (o->oClamShakeTimer != 0) {
        o->oClamShakeTimer--;
        cur_obj_shake_y(3.0f);
    }
}

void clam_act_opening(void) {
    s16 i;
    s16 bubblesX, bubblesZ;

    if (o->oTimer > 150) {
        o->oAction = CLAM_ACT_CLOSING;
    } else if (obj_is_rendering_enabled() && cur_obj_init_anim_check_frame(CLAM_ANIM_OPENING, 8)) {
        for (i = -DEGREES(45); i < DEGREES(45); i += 0x0555) { // ~7.5 degrees each frame
            bubblesX = (s16)(100.0f * sins(i));
            bubblesZ = (s16)(100.0f * coss(i));

            spawn_object_relative(OBJ_BP_NONE, bubblesX, 30, bubblesZ, o, MODEL_BUBBLE, bhvBubbleMaybe);
        }
    } else if (cur_obj_check_anim_frame(30)) {
        cur_obj_become_intangible();
    }
}

void bhv_clam_loop(void) {
    o->header.gfx.scale[1] = 1.5f;

    switch (o->oAction) {
        case CLAM_ACT_CLOSING:
            clam_act_closing();
            break;
        case CLAM_ACT_OPENING:
            clam_act_opening();
            break;
    }

    obj_check_attacks(&sClamShellHitbox, o->oAction);
}
