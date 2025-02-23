// elevator.inc.c

static s16 sElevatorHeights[][2] = {
    {   -51,    0 }, // HMC Black Hole
    {  -461,    0 }, // HMC Emergency Exit
    {  -512,    0 }, // HMC Navigating
    { -2611,    0 }, // HMC Lake
    { -2360,    0 }, // BBH Mesh
    {   214,    0 }, // Rainbow Ride
    {   -50, 1945 }, // Roomless?
};

void elevator_starting_shake(void) {
    cur_obj_play_sound_2(SOUND_GENERAL_QUIET_POUND1);
    cur_obj_shake_screen(SHAKE_POS_SMALL);
}

void elevator_act_idle(void) {
    o->oVelY = 0;
    if (o->oElevatorType == ELEVATOR_TYPE_ABOVE_HOME) {
        if (gMarioObject->platform == o) {
            if (o->oPosY > o->oElevatorMidY) {
                o->oAction = ELEVATOR_ACT_MOVING_DOWN;
            } else {
                o->oAction = ELEVATOR_ACT_MOVING_UP;
            }
        }
    } else if (gMarioObject->oPosY > o->oElevatorMidY || o->oElevatorType == 1) {
        o->oPosY = o->oElevatorMaxY;
        if (gMarioObject->platform == o) {
            o->oAction = ELEVATOR_ACT_MOVING_DOWN;
        }
    } else {
        o->oPosY = o->oElevatorMinY;
        if (gMarioObject->platform == o) {
            o->oAction = ELEVATOR_ACT_MOVING_UP;
        }
    }
}

void elevator_act_moving_up(void) {
    cur_obj_play_sound_1(SOUND_ENV_ELEVATOR1);
    if (o->oTimer == 0 && cur_obj_is_mario_on_platform()) {
        elevator_starting_shake();
    }
    approach_f32_signed(&o->oVelY, 10.0f, 2.0f);
    o->oPosY += o->oVelY;
    if (o->oPosY > o->oElevatorMaxY) {
        o->oPosY = o->oElevatorMaxY;
        if (o->oElevatorType == ELEVATOR_TYPE_ABOVE_HOME
         || o->oElevatorType == ELEVATOR_TYPE_RR) {
            o->oAction = ELEVATOR_ACT_LANDED;
        } else if (gMarioObject->oPosY < o->oElevatorMidY) {
            o->oAction = ELEVATOR_ACT_MOVING_DOWN;
        } else {
            o->oAction = ELEVATOR_ACT_LANDED;
        }
    }
}

void elevator_act_moving_down(void) {
    cur_obj_play_sound_1(SOUND_ENV_ELEVATOR1);
    if (o->oTimer == 0 && cur_obj_is_mario_on_platform()) {
        elevator_starting_shake();
    }
    approach_f32_signed(&o->oVelY, -10.0f, -2.0f);
    o->oPosY += o->oVelY;
    if (o->oPosY < o->oElevatorMinY) {
        o->oPosY = o->oElevatorMinY;
        if (o->oElevatorType == ELEVATOR_TYPE_RR) {
            o->oAction = ELEVATOR_ACT_LANDED_RR;
        } else if (o->oElevatorType == ELEVATOR_TYPE_ABOVE_HOME) {
            o->oAction = ELEVATOR_ACT_LANDED;
        } else if (gMarioObject->oPosY > o->oElevatorMidY) {
            o->oAction = ELEVATOR_ACT_MOVING_UP;
        } else {
            o->oAction = ELEVATOR_ACT_LANDED;
        }
    }
}

void elevator_act_landed_rr(void) {
    o->oVelY = 0;
    if (o->oTimer == 0) {
        cur_obj_shake_screen(SHAKE_POS_SMALL);
        cur_obj_play_sound_2(SOUND_GENERAL_ELEVATOR_LAND);
    }
    if (!mario_is_in_air_action() && !cur_obj_is_mario_on_platform()) {
        o->oAction = ELEVATOR_ACT_MOVING_UP;
    }
}

void elevator_act_landed(void) {
    o->oVelY = 0;
    if (o->oTimer == 0) {
        cur_obj_shake_screen(SHAKE_POS_SMALL);
        cur_obj_play_sound_2(SOUND_GENERAL_ELEVATOR_LAND);
    }
    if (!mario_is_in_air_action() && !cur_obj_is_mario_on_platform()) {
        o->oAction = ELEVATOR_ACT_IDLE;
    }
}

void bhv_elevator_init(void) {
    s32 index = o->oBehParams2ndByte;
    if (sElevatorHeights[index][1] == 0) {
        o->oElevatorMinY = sElevatorHeights[index][0];
        o->oElevatorMaxY = o->oHomeY;
        o->oElevatorMidY = ((o->oElevatorMinY + o->oElevatorMaxY) / 2);
        o->oElevatorType = cur_obj_has_behavior(bhvRrElevatorPlatform);
    } else {
        o->oElevatorMinY = sElevatorHeights[index][0];
        o->oElevatorMaxY = sElevatorHeights[index][1];
        o->oElevatorMidY = ((o->oElevatorMinY + o->oElevatorMaxY) / 2);
        o->oElevatorType = ELEVATOR_TYPE_ABOVE_HOME;
    }
}

ObjActionFunc sElevatorActions[] = {
    elevator_act_idle,
    elevator_act_moving_up,
    elevator_act_moving_down,
    elevator_act_landed,
    elevator_act_landed_rr,
};

void bhv_elevator_loop(void) {
    cur_obj_call_action_function(sElevatorActions);
}
