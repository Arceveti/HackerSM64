// door.inc.c

struct DoorAction {
    u32 flag;
    ObjAction32 action;
};

static struct DoorAction sDoorActions[] = {
    { INT_STATUS_WARP_DOOR_PULLED, DOOR_ACT_WARP_PULLED },
    { INT_STATUS_WARP_DOOR_PUSHED, DOOR_ACT_WARP_PUSHED },
    { INT_STATUS_DOOR_PULLED,      DOOR_ACT_PULLED      },
    { INT_STATUS_DOOR_PUSHED,      DOOR_ACT_PUSHED      },
    { -1,                          DOOR_ACT_CLOSED      },
};

static s32 sDoorOpenSounds[] = { SOUND_GENERAL_OPEN_WOOD_DOOR, SOUND_GENERAL_OPEN_IRON_DOOR };

static s32 sDoorCloseSounds[] = { SOUND_GENERAL_CLOSE_WOOD_DOOR, SOUND_GENERAL_CLOSE_IRON_DOOR };

void door_animation_and_reset(s32 animIndex) {
    cur_obj_init_animation_with_sound(animIndex);
    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = DOOR_ACT_CLOSED;
    }
}

void set_door_camera_event(void) {
    if (segmented_to_virtual(bhvDoor) == o->behavior) {
        gPlayerCameraState->cameraEvent = CAM_EVENT_DOOR;
    } else {
        gPlayerCameraState->cameraEvent = CAM_EVENT_DOOR_WARP;
    }
    gPlayerCameraState->usedObj = o;
}

void play_door_open_noise(void) {
    s32 isMetalDoor = cur_obj_has_model(MODEL_HMC_METAL_DOOR);
    if (o->oTimer == 0) {
        cur_obj_play_sound_2(sDoorOpenSounds[isMetalDoor]);
        gTimeStopState |= TIME_STOP_MARIO_OPENED_DOOR;
    }
    if (o->oTimer == 70) {
        cur_obj_play_sound_2(sDoorCloseSounds[isMetalDoor]);
    }
}

void play_warp_door_open_noise(void) {
    s32 isMetalDoor = cur_obj_has_model(MODEL_HMC_METAL_DOOR);
    if (o->oTimer == 30) {
        cur_obj_play_sound_2(sDoorCloseSounds[isMetalDoor]);
    }
}

void bhv_door_loop(void) {
    s32 index = 0;

    while (sDoorActions[index].flag != 0xFFFFFFFF) {
        if (cur_obj_clear_interact_status_flag(sDoorActions[index].flag)) {
            set_door_camera_event();
            cur_obj_change_action(sDoorActions[index].action);
        }
        index++;
    }

    switch (o->oAction) {
        case DOOR_ACT_CLOSED:
            cur_obj_init_animation_with_sound(DOOR_ANIM_CLOSED);
            load_object_collision_model();
            break;
        case DOOR_ACT_PULLED:
            door_animation_and_reset(DOOR_ANIM_PULLED);
            play_door_open_noise();
            break;
        case DOOR_ACT_PUSHED:
            door_animation_and_reset(DOOR_ANIM_PUSHED);
            play_door_open_noise();
            break;
        case DOOR_ACT_WARP_PULLED:
            door_animation_and_reset(DOOR_ANIM_WARP_PULLED);
            play_warp_door_open_noise();
            break;
        case DOOR_ACT_WARP_PUSHED:
            door_animation_and_reset(DOOR_ANIM_WARP_PUSHED);
            play_warp_door_open_noise();
            break;
    }

    bhv_door_rendering_loop();
}

void bhv_door_init(void) {
    const f32 checkDist = 200.0f;

    f32 x = o->oPosX;
    f32 y = o->oPosY;
    f32 z = o->oPosZ;

    o->oDoorSelfRoom = get_room_at_pos(x, y, z);

    x = o->oPosX + (sins(o->oMoveAngleYaw) *  checkDist);
    z = o->oPosZ + (coss(o->oMoveAngleYaw) *  checkDist);

    o->oDoorForwardRoom = get_room_at_pos(x, y, z);

    x = o->oPosX + (sins(o->oMoveAngleYaw) * -checkDist);
    z = o->oPosZ + (coss(o->oMoveAngleYaw) * -checkDist);

    o->oDoorBackwardRoom = get_room_at_pos(x, y, z);

    if (
        // Ensure the room number is in bounds.
        o->oDoorSelfRoom > 0 && o->oDoorSelfRoom < ARRAY_COUNT(gDoorAdjacentRooms)
        // Only set gDoorAdjacentRooms for transition rooms.
        && o->oDoorSelfRoom    != o->oDoorForwardRoom
        && o->oDoorSelfRoom    != o->oDoorBackwardRoom
        && o->oDoorForwardRoom != o->oDoorBackwardRoom
    ) {
        gDoorAdjacentRooms[o->oDoorSelfRoom].forwardRoom  = o->oDoorForwardRoom;
        gDoorAdjacentRooms[o->oDoorSelfRoom].backwardRoom = o->oDoorBackwardRoom;
    }
}

void bhv_door_rendering_loop(void) {
    struct TransitionRoomData* transitionRoom = &gDoorAdjacentRooms[gMarioCurrentRoom];

    o->oDoorIsRendering = (
        gMarioCurrentRoom            == 0                    || // Mario is in the "global" room.
        gMarioCurrentRoom            == o->oDoorSelfRoom     || // Mario is in the same room as the door.
        gMarioCurrentRoom            == o->oDoorForwardRoom  || // Mario is in the door's  forward room.
        gMarioCurrentRoom            == o->oDoorBackwardRoom || // Mario is in the door's backward room.
        transitionRoom->forwardRoom  == o->oDoorForwardRoom  || // The transition room's  forward room is in the same room as this door's  forward room.
        transitionRoom->forwardRoom  == o->oDoorBackwardRoom || // The transition room's  forward room is in the same room as this door's backward room.
        transitionRoom->backwardRoom == o->oDoorForwardRoom  || // The transition room's backward room is in the same room as this door's  forward room.
        transitionRoom->backwardRoom == o->oDoorBackwardRoom    // The transition room's backward room is in the same room as this door's backward room.
    );

    COND_BIT(o->oDoorIsRendering, o->header.gfx.node.flags, GRAPH_RENDER_ACTIVE);
}



void bhv_shield_init(void) {

}
void bhv_shield_loop(void) {
    struct MarioState* m = &gMarioStates[0];
    vec3_copy_y_off(&o->oPosVec, m->pos, 80.0f);
    o->oFaceAnglePitch += 0x800 * random_float();
    o->oFaceAngleYaw   += 0x800 * random_float();
    o->oFaceAngleRoll  += 0x800 * random_float();
    // o->oFaceAngleYaw = (s16)random_u16();
}

void bhv_robot_init(void) {

}
void bhv_robot_loop(void) {

}
void bhv_scan_init(void) {

}
void bhv_scan_loop(void) {
    o->oFaceAngleYaw += 0x200;
}
void bhv_dlg_trigger_init(void) {

}
#include "suit_dialogue.h"
extern u32 gInitialTextIndex;
void bhv_dlg_trigger_loop(void) {
    if (gMarioStates[0].health <= 0xFF) {
        return;
    }
    u8 size = BPARAM1;
    u8 dlgId = BPARAM2;
    if (o->oDistanceToMario < 100.0f * (f32)size) {
        gInitialTextIndex = 0xFFFF;
        set_onscreen_dlg(dlgId);
        if (o->oTimer > 1 && (dlgId == DLG_BOOTING || dlgId == DLG_UPHILL/* || dlgId == DLG_BLACK_HOLE*/)) {
            obj_mark_for_deletion(o);
        }
    }
}

void bhv_background_init(void) {

}
// extern struct Camera* gCamera;
void bhv_background_loop(void) {
    // o->oPosX = gLakituState.pos[0];
    // o->oPosY = gLakituState.pos[1];
    // o->oPosZ = gLakituState.pos[2];
}

void bhv_rainbow_path_init(void) {

}
extern s16 gRainbowSlidePathPitch;
extern s16 gRainbowSlidePathRoll;
void bhv_rainbow_path_loop(void) {
    // obj_
    if (o->oTimer > 60) {
        mark_obj_for_deletion(o);
    }
    
    // o->oFaceAnglePitch = approach_angle(o->oFaceAnglePitch, -gMarioState->movePitch, 0x100);
    // o->oFaceAngleRoll = approach_angle(o->oFaceAngleRoll, gMarioState->marioBodyState->torsoAngle[2], 0x100);
}
extern f32 gBlackHoleDistortion;

u32 enteredBlackHoleAmount = 0;
void bhv_black_hole_loop(void) {
    o->oFaceAngleYaw = random_u16();
    if (o->oDistanceToMario < 8192.0f) {
        enteredBlackHoleAmount++;
        struct MarioState* m = &gMarioStates[0];
        if (m->action != ACT_DEBUG_FREE_MOVE) {
            // f32 modifier = (o->oDistanceToMario / 8192.0f);
            gBlackHoleDistortion = (o->oDistanceToMario / 8192.0f);
            s16 marioAngleToObj = mario_obj_angle_to_object(m, o);
            // f32 speed = 32.0f * (1.0f - modifier);
            // m->pos[0] += speed * sins(marioAngleToObj);
            // m->pos[2] += speed * coss(marioAngleToObj);
            m->faceAngle[1] = approach_angle(m->faceAngle[1], marioAngleToObj, 0x200);
        } else {
            gBlackHoleDistortion = 1.0f;
        }
    } else {
        if (enteredBlackHoleAmount >= 300) { 
            level_trigger_warp(gMarioState, WARP_OP_CREDITS_END);
        }
    }
}
