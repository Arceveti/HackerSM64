// lll_octagonal_rotating_mesh.inc.c

enum LLLOctagonalMeshInstructions {
    LLL_OCTMESH_CHANGE_DIR = 1, // 1
    LLL_OCTMESH_LINEAR_MOVE,    // 2
    LLL_OCTMESH_RESET,          // 3
    LLL_OCTMESH_WAIT_FOR_MARIO  // 4
};

struct LllOctagonalMeshAction {
    s16 instruction;
    s16 time;
    s16 moveAngle;
    s16 forwardVel;
};

// Path for big bullies platforms
static struct LllOctagonalMeshAction gLllOctagonalMeshAction0[] = {
    { .instruction = LLL_OCTMESH_LINEAR_MOVE,    .time =  30, .moveAngle =  0x4000, .forwardVel =   0 }, // 90 degrees
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time = 220, .moveAngle =     900, .forwardVel =  30 },
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time =  30, .moveAngle =       0, .forwardVel = -30 },
    { .instruction = LLL_OCTMESH_LINEAR_MOVE,    .time =  30, .moveAngle = -0x4000, .forwardVel =   0 }, // 90 degrees
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time = 220, .moveAngle =     900, .forwardVel =  30 },
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time =  30, .moveAngle =       0, .forwardVel = -30 },
    { .instruction = LLL_OCTMESH_RESET,          .time =   0, .moveAngle =       0, .forwardVel =   0 }
};

// Path for rolling log
static struct LllOctagonalMeshAction gLllOctagonalMeshAction1[] = {
    { .instruction = LLL_OCTMESH_WAIT_FOR_MARIO, .time =   0, .moveAngle =       0, .forwardVel =   0 },
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time = 475, .moveAngle =     900, .forwardVel =  30 },
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time =  30, .moveAngle =       0, .forwardVel = -30 },
    { .instruction = LLL_OCTMESH_LINEAR_MOVE,    .time =  30, .moveAngle =  0x8000, .forwardVel =   0 }, // 180 degrees
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time = 475, .moveAngle =     900, .forwardVel =  30 },
    { .instruction = LLL_OCTMESH_CHANGE_DIR,     .time =  30, .moveAngle =       0, .forwardVel = -30 },
    { .instruction = LLL_OCTMESH_RESET,          .time =   0, .moveAngle =       0, .forwardVel =   0 }
};

// picked by oBehParams2ndByte
static struct LllOctagonalMeshAction *gLllOctagonalMeshActionList[] = {
    gLllOctagonalMeshAction0,
    gLllOctagonalMeshAction1
};

s32 lll_octagonal_mesh_move(struct LllOctagonalMeshAction *actionTable, s32 actionOffset) {
    struct LllOctagonalMeshAction *action = &(actionTable[actionOffset]);
    switch (action->instruction) {
        case LLL_OCTMESH_WAIT_FOR_MARIO:
            o->oMoveAngleYaw = action->moveAngle;
            o->oForwardVel = action->forwardVel / 100.0f;
            if (cur_obj_is_mario_on_platform()) {
                actionOffset++;
                o->oTimer = 0;
            }
            break;

        case LLL_OCTMESH_LINEAR_MOVE:
            o->oMoveAngleYaw = action->moveAngle;
            o->oForwardVel = action->forwardVel / 100.0f;
            if (o->oTimer > action->time) {
                actionOffset++;
                o->oTimer = 0;
            }
            break;

        case LLL_OCTMESH_CHANGE_DIR:
            approach_f32_signed(&o->oForwardVel, action->moveAngle / 100.0f, action->forwardVel / 100.0f);
            if (o->oTimer > action->time) {
                actionOffset++;
                o->oTimer = 0;
            }
            break;

        case LLL_OCTMESH_RESET:
            o->oForwardVel = action->forwardVel;
            actionOffset = 0;
            break;
    }

    return actionOffset;
}

s32 lll_octagonal_mesh_find_y_offset(s32 *standTimer, f32 *posOffset, s32 standTimerInc, s32 moveDownAmount) {
    if (cur_obj_is_mario_on_platform()) {
        if (standTimer[0] < 0x4000) {
            standTimer[0] += standTimerInc;
        } else {
            standTimer[0] = 0x4000;
        }
    } else {
        if (standTimer[0] > 0) {
            standTimer[0] -= standTimerInc;
        } else {
            standTimer[0] = 0;
        }
    }

    posOffset[0] = sins(standTimer[0]) * moveDownAmount;
    return (standTimer[0] == 0 || standTimer[0] == 0x4000);
}

void bhv_lll_moving_octagonal_mesh_platform_loop(void) {
    if (o->oAction == 0) {
        o->oLllOctMeshActionOffset = 0;
        o->oAction++;
    } else {
        o->oLllOctMeshActionOffset =
            lll_octagonal_mesh_move(gLllOctagonalMeshActionList[o->oBehParams2ndByte], o->oLllOctMeshActionOffset);
    }

    print_debug_top_down_objectinfo("number %d\n", o->oLllOctMeshActionOffset);
    cur_obj_move_using_fvel_and_gravity();

    if (lll_octagonal_mesh_find_y_offset(&o->oLllOctMeshStandTimer, &o->oLllOctMeshYOffsetFromHome, 0x400, -80)) {
        o->oLllOctMeshWaveTimer += 0x800;
        o->oLllOctMeshWaveYOffset -= sins(o->oLllOctMeshWaveTimer) * 2;
    }

    o->oPosY = o->oLllOctMeshWaveYOffset + o->oHomeY + o->oLllOctMeshYOffsetFromHome;
}
