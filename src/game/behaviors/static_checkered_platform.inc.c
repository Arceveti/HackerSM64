// static_checkered_platform.inc.c

void bhv_static_checkered_platform_loop(void) {
    if (gDebugInfo[DEBUG_PAGE_ENEMYINFO][0] == 1) {
        obj_set_angle(o, 0, 0, 0);
        vec3_zero(&o->oAngleVelVec);
    }

    if (gDebugInfo[DEBUG_PAGE_ENEMYINFO][0] == 2) {
        o->oFaceAnglePitch = (gDebugInfo[DEBUG_PAGE_ENEMYINFO][1] << 12);
        o->oFaceAngleYaw   = (gDebugInfo[DEBUG_PAGE_ENEMYINFO][2] << 12);
        o->oFaceAngleRoll  = (gDebugInfo[DEBUG_PAGE_ENEMYINFO][3] << 12);
    }

    o->oAngleVelPitch = gDebugInfo[DEBUG_PAGE_ENEMYINFO][4];
    o->oAngleVelYaw   = gDebugInfo[DEBUG_PAGE_ENEMYINFO][5];
    o->oAngleVelRoll  = gDebugInfo[DEBUG_PAGE_ENEMYINFO][6];

    if (gDebugInfo[DEBUG_PAGE_ENEMYINFO][0] == 3) {
        vec3_add(&o->oFaceAngleVec, &o->oAngleVelVec);
    }
}
