// orange_number.inc.c

void bhv_orange_number_init(void) {
    o->oAnimState = o->oBehParams2ndByte;
    o->oVelY = 26.0f;
    o->oHomeY = o->oPosY;
}

void bhv_orange_number_loop(void) {
#ifdef DIALOG_INDICATOR
    if (o->oAnimState <= ORANGE_NUMBER_9) {
#endif
        o->oPosY += o->oVelY;
        o->oVelY -= 2.0f;

        if (o->oVelY < -21.0f) {
            o->oVelY = 14.0f;
        }

        s32 offsetX = (o->oOrangeNumberOffset * sins(gCamera->nextYaw + 0x4000));
        s32 offsetZ = (o->oOrangeNumberOffset * coss(gCamera->nextYaw + 0x4000));

        o->oPosX = (o->oHomeX + offsetX);
        o->oPosZ = (o->oHomeZ + offsetZ);

        if (o->oTimer == 35) {
            struct Object *sparkleObj = spawn_object(o, MODEL_SPARKLES, bhvCoinSparklesSpawner);
            sparkleObj->oPosY -= 30.0f;
            obj_mark_for_deletion(o);
        }
#ifdef DIALOG_INDICATOR
    } else if (o->oTimer >= 1 || gMarioState->action == ACT_READING_SIGN) {
        obj_mark_for_deletion(o);
    }
#endif
}
