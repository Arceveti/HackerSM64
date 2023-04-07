#include <ultra64.h>

#include "sm64.h"
#include "behavior_data.h"
#include "behavior_script.h"
#include "behavior_commands.h"
#include "game/area.h"
#include "game/behavior_actions.h"
#include "game/game_init.h"
#include "game/interaction.h"
#include "game/mario.h"
#include "game/memory.h"
#include "game/obj_behaviors_2.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "math_util.h"
#include "graph_node.h"
#include "surface_collision.h"
#include "game/puppylights.h"

// Macros for retrieving arguments from behavior scripts.

#define GET_B(a, n, s) (_SHIFTR((a), (32 - ((n) * (s))), (s)))

#define GET_B1(a) GET_B((a), 1,  8) // 0xFF000000
#define GET_B2(a) GET_B((a), 2,  8) // 0x00FF0000
#define GET_B3(a) GET_B((a), 3,  8) // 0x0000FF00
#define GET_B4(a) GET_B((a), 4,  8) // 0x000000FF
#define GET_H1(a) GET_B((a), 1, 16) // 0xFFFF0000
#define GET_H2(a) GET_B((a), 2, 16) // 0x0000FFFF

#define BHV_CMD_GET_1ST_U8(index)  (u8 )GET_B1(gCurBhvCommand[index])
#define BHV_CMD_GET_1ST_S8(index)  (s8 )GET_B1(gCurBhvCommand[index])

#define BHV_CMD_GET_2ND_U8(index)  (u8 )GET_B2(gCurBhvCommand[index])
#define BHV_CMD_GET_2ND_S8(index)  (s8 )GET_B2(gCurBhvCommand[index])

#define BHV_CMD_GET_3RD_U8(index)  (u8 )GET_B3(gCurBhvCommand[index])
#define BHV_CMD_GET_3RD_S8(index)  (s8 )GET_B3(gCurBhvCommand[index])

#define BHV_CMD_GET_4TH_U8(index)  (u8 )GET_B4(gCurBhvCommand[index])
#define BHV_CMD_GET_4TH_S8(index)  (s8 )GET_B4(gCurBhvCommand[index])

#define BHV_CMD_GET_1ST_U16(index) (u16)GET_H1(gCurBhvCommand[index])
#define BHV_CMD_GET_1ST_S16(index) (s16)GET_H1(gCurBhvCommand[index])

#define BHV_CMD_GET_2ND_U16(index) (u16)GET_H2(gCurBhvCommand[index])
#define BHV_CMD_GET_2ND_S16(index) (s16)GET_H2(gCurBhvCommand[index])

#define BHV_CMD_GET_U32(index)     (u32)(gCurBhvCommand[index])
#define BHV_CMD_GET_S32(index)     (s32)(gCurBhvCommand[index])

#define BHV_CMD_GET_VPTR(index)    (void*)(gCurBhvCommand[index])

#define BHV_CMD_GET_ADDR_OF_CMD(index) (uintptr_t)(&gCurBhvCommand[index])

// Unused function that directly jumps to a behavior command and resets the object's stack index.
UNUSED static void goto_behavior_unused(const BehaviorScript* bhvAddr) {
    gCurBhvCommand = segmented_to_virtual(bhvAddr);
    gCurrentObject->bhvStackIndex = 0;
}

// Update an object's graphical position and rotation to match its real position and rotation.
void obj_update_gfx_pos_and_angle(struct Object* obj) {
    vec3_copy_y_off(obj->header.gfx.pos, &obj->oPosVec, obj->oGraphYOffset);
    obj->header.gfx.angle[0] = (obj->oFaceAnglePitch & BITMASK(16));
    obj->header.gfx.angle[1] = (obj->oFaceAngleYaw   & BITMASK(16));
    obj->header.gfx.angle[2] = (obj->oFaceAngleRoll  & BITMASK(16));
}

#ifdef OBJ_OPACITY_BY_CAM_DIST
 #define OBJ_OPACITY_NEAR   128.0f
 #define OBJ_OPACITY_LENGTH 512.0f
void obj_set_opacity_from_cam_dist(struct Object* obj) {
    s32 opacityDist = ((-obj->header.gfx.cameraToObject[2] - OBJ_OPACITY_NEAR) * (256.0f / OBJ_OPACITY_LENGTH));
 #ifdef OBJECTS_REJ
    if (opacityDist > 0) {
        obj->header.gfx.ucode = GRAPH_NODE_UCODE_REJ;
    }
 #endif // OBJECTS_REJ
    obj->oOpacity = CLAMP(opacityDist, 0x00, 0xFF);
}
 #undef OBJ_OPACITY_NEAR
 #undef OBJ_OPACITY_LENGTH
#endif // OBJ_OPACITY_BY_CAM_DIST

// Push the address of a behavior command to the object's behavior stack.
static void cur_obj_bhv_stack_push(uintptr_t bhvAddr) {
    o->bhvStack[o->bhvStackIndex] = bhvAddr;
    o->bhvStackIndex++;
}

// Retrieve the last behavior command address from the object's behavior stack.
static uintptr_t cur_obj_bhv_stack_pop(void) {
    o->bhvStackIndex--;
    return o->bhvStack[o->bhvStackIndex];
}

// BHV_CMD_HIDE: Hides the current object.
// Usage: HIDE()
static s32 bhv_cmd_hide(void) {
    cur_obj_hide();

    gCurBhvCommand += SIZEOF_BHV_CMD(HIDE());
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_DISABLE_RENDERING: Disables rendering for the object.
// Usage: DISABLE_RENDERING()
static s32 bhv_cmd_disable_rendering(void) {
    gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;

    gCurBhvCommand += SIZEOF_BHV_CMD(DISABLE_RENDERING());
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_BILLBOARD: Billboards the current object, making it always face the camera.
// Usage: BILLBOARD()
static s32 bhv_cmd_billboard(void) {
    gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;

    gCurBhvCommand += SIZEOF_BHV_CMD(BILLBOARD());
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_MODEL: Sets the current model ID of the object.
// Usage: SET_MODEL(modelID)
static s32 bhv_cmd_set_model(void) {
    ModelID32 modelID = BHV_CMD_GET_2ND_S16(0);

    cur_obj_set_model(modelID);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_MODEL(modelID));;
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SPAWN_CHILD: Spawns a child object with the specified model and behavior.
// Usage: SPAWN_CHILD(modelID, behavior)
static s32 bhv_cmd_spawn_child(void) {
    ModelID32 model = BHV_CMD_GET_U32(1);
    const BehaviorScript* behavior = BHV_CMD_GET_VPTR(2);

    struct Object* child = spawn_object_at_origin(gCurrentObject, 0, model, behavior);
    obj_copy_pos_and_angle(child, gCurrentObject);

    gCurBhvCommand += SIZEOF_BHV_CMD(SPAWN_CHILD(model, behavior));;
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SPAWN_OBJ: Spawns a new object with the specified model and behavior.
// Usage: SPAWN_OBJ(modelID, behavior)
static s32 bhv_cmd_spawn_obj(void) {
    ModelID32 model = BHV_CMD_GET_U32(1);
    const BehaviorScript* behavior = BHV_CMD_GET_VPTR(2);

    struct Object* object = spawn_object_at_origin(gCurrentObject, 0, model, behavior);
    obj_copy_pos_and_angle(object, gCurrentObject);
    // TODO: Does this cmd need renaming? This line is the only difference between this and the above func.
    gCurrentObject->prevObj = object;

    gCurBhvCommand += SIZEOF_BHV_CMD(SPAWN_OBJ(MODEL_NONE, NULL));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SPAWN_CHILD_WITH_PARAM: Spawns a child object with the specified model and behavior, plus a behavior param.
// Usage: SPAWN_CHILD_WITH_PARAM(bhvParam, modelID, behavior)
static s32 bhv_cmd_spawn_child_with_param(void) {
    u32 bhvParam = BHV_CMD_GET_2ND_S16(0);
    ModelID32 modelID = BHV_CMD_GET_U32(1);
    const BehaviorScript* behavior = BHV_CMD_GET_VPTR(2);

    struct Object* child = spawn_object_at_origin(gCurrentObject, 0, modelID, behavior);
    obj_copy_pos_and_angle(child, gCurrentObject);
    child->oBehParams2ndByte = bhvParam;

    gCurBhvCommand += SIZEOF_BHV_CMD(SPAWN_CHILD_WITH_PARAM(OBJ_BP_NONE, MODEL_NONE, NULL));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_DEACTIVATE: Exits the behavior script and despawns the object.
// Usage: DEACTIVATE()
static s32 bhv_cmd_deactivate(void) {
    obj_mark_for_deletion(gCurrentObject);
    return BHV_PROC_BREAK;
}

// BHV_CMD_BREAK: Exits the behavior script.
// Usage: BREAK()
static s32 bhv_cmd_break(void) {
    return BHV_PROC_BREAK;
}

// BHV_CMD_CALL: Jumps to a new behavior command and stores the return address in the object's behavior stack.
// Usage: CALL(addr)
static s32 bhv_cmd_call(void) {
    const BehaviorScript* addr = segmented_to_virtual(BHV_CMD_GET_VPTR(1));

    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(SIZEOF_BHV_CMD(CALL(addr)))); // Store address of the next bhv command in the stack.
    
    gCurBhvCommand = addr; // Jump to the new address.

    return BHV_PROC_CONTINUE;
}

// BHV_CMD_RETURN: Jumps back to the behavior command stored in the object's behavior stack. Used after CALL.
// Usage: RETURN()
static s32 bhv_cmd_return(void) {
    gCurBhvCommand = (const BehaviorScript*)cur_obj_bhv_stack_pop(); // Retrieve command address and jump to it.
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_DELAY: Delays the behavior script for a certain number of frames.
// Usage: DELAY(num)
static s32 bhv_cmd_delay(void) {
    s16 num = BHV_CMD_GET_2ND_S16(0);

    if (gCurrentObject->bhvDelayTimer < (num - 1)) {
        gCurrentObject->bhvDelayTimer++; // Increment timer
    } else {
        gCurrentObject->bhvDelayTimer = 0;
        gCurBhvCommand += SIZEOF_BHV_CMD(DELAY(num)); // Delay ended, move to next bhv command (note: following commands will not execute until next frame)
    }

    return BHV_PROC_BREAK;
}

// BHV_CMD_DELAY_VAR: Delays the behavior script for the number of frames given by the value of the specified field.
// Usage: DELAY_VAR(field)
static s32 bhv_cmd_delay_var(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 num = cur_obj_get_int(field);

    if (gCurrentObject->bhvDelayTimer < (num - 1)) {
        gCurrentObject->bhvDelayTimer++; // Increment timer
    } else {
        gCurrentObject->bhvDelayTimer = 0;
        gCurBhvCommand += SIZEOF_BHV_CMD(DELAY_VAR(field)); // Delay ended, move to next bhv command
    }

    return BHV_PROC_BREAK;
}

// BHV_CMD_GOTO: Jumps to a new behavior script without saving anything.
// Usage: GOTO(addr)
static s32 bhv_cmd_goto(void) {
    gCurBhvCommand = segmented_to_virtual(BHV_CMD_GET_VPTR(1)); // Jump directly to address.
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_BEGIN_REPEAT: Marks the start of a loop that will repeat a certain number of times.
// Usage: BEGIN_REPEAT(count)
static s32 bhv_cmd_begin_repeat(void) {
    s32 count = BHV_CMD_GET_2ND_S16(0);

    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(1)); // Store address of the first command of the loop in the stack
    cur_obj_bhv_stack_push(count); // Store repeat count in the stack too

    gCurBhvCommand += SIZEOF_BHV_CMD(BEGIN_REPEAT(count));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_END_REPEAT: Marks the end of a repeating loop.
// Usage: END_REPEAT()
static s32 bhv_cmd_end_repeat(void) {
    u32 count = (cur_obj_bhv_stack_pop() - 1); // Retrieve loop count from the stack.

    if (count != 0) {
        gCurBhvCommand = (const BehaviorScript*)cur_obj_bhv_stack_pop(); // Jump back to the first command in the loop
        // Save address and count to the stack again
        cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(0));
        cur_obj_bhv_stack_push(count);
    } else { // Finished iterating over the loop
        cur_obj_bhv_stack_pop(); // Necessary to remove address from the stack
        gCurBhvCommand += SIZEOF_BHV_CMD(END_REPEAT());
    }

    // Don't execute following commands until next frame
    return BHV_PROC_BREAK;
}

// BHV_CMD_END_REPEAT_CONTINUE: Also marks the end of a repeating loop, but continues executing commands following the loop on the same frame.
// Usage: END_REPEAT_CONTINUE()
static s32 bhv_cmd_end_repeat_continue(void) {
    u32 count = (cur_obj_bhv_stack_pop() - 1);

    if (count != 0) {
        gCurBhvCommand = (const BehaviorScript*)cur_obj_bhv_stack_pop(); // Jump back to the first command in the loop
        // Save address and count to the stack again
        cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(0));
        cur_obj_bhv_stack_push(count);
    } else { // Finished iterating over the loop
        cur_obj_bhv_stack_pop(); // Necessary to remove address from the stack
        gCurBhvCommand += SIZEOF_BHV_CMD(END_REPEAT_CONTINUE());
    }

    // Start executing following commands immediately
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_BEGIN_LOOP: Marks the beginning of an infinite loop.
// Usage: BEGIN_LOOP()
static s32 bhv_cmd_begin_loop(void) {
    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(1)); // Store address of the first command of the loop in the stack

    gCurBhvCommand += SIZEOF_BHV_CMD(BEGIN_LOOP());
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_END_LOOP: Marks the end of an infinite loop.
// Usage: END_LOOP()
static s32 bhv_cmd_end_loop(void) {
    gCurBhvCommand = (const BehaviorScript*)cur_obj_bhv_stack_pop(); // Jump back to the first command in the loop
    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(0)); // Save address to the stack again

    return BHV_PROC_BREAK;
}

// BHV_CMD_CALL_NATIVE: Executes a native game function. Function must not take or return any values.
// Usage: CALL_NATIVE(func)
typedef void (*NativeBhvFunc)(void);
static s32 bhv_cmd_call_native(void) {
    NativeBhvFunc behaviorFunc = BHV_CMD_GET_VPTR(1);

    behaviorFunc();

    gCurBhvCommand += SIZEOF_BHV_CMD(CALL_NATIVE(NULL));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_FLOAT: Sets the specified field to a float.
// Usage: SET_FLOAT(field, value)
static s32 bhv_cmd_set_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_set_float(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_FLOAT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_SHORT: Sets the specified field to a short.
// Usage: SET_SHORT(field, value)
static s32 bhv_cmd_set_short(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s16 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_set_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_SHORT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_RANDOM_FLOAT: Sets the specified field to a random float in the given range.
// Usage: SET_RANDOM_FLOAT(field, min, range)
static s32 bhv_cmd_set_random_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 min = BHV_CMD_GET_2ND_S16(0);
    f32 range = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_float(field, ((range * random_float()) + min));

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_RANDOM_FLOAT(field, min, range));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_RANDOM_INT: Sets the specified field to a random integer in the given range.
// Usage: SET_RANDOM_INT(field, min, range)
static s32 bhv_cmd_set_random_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 min = BHV_CMD_GET_2ND_S16(0);
    s32 range = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_int(field, (s32)(range * random_float()) + min);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_RANDOM_INT(field, min, range));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_ADD_RANDOM_FLOAT: Adds a random float in the given range to the specified field.
// Usage: ADD_RANDOM_FLOAT(field, min, range)
static s32 bhv_cmd_add_random_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 min = BHV_CMD_GET_2ND_S16(0);
    f32 range = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_float(field, (cur_obj_get_float(field) + min + (range * random_float())));

    gCurBhvCommand += SIZEOF_BHV_CMD(ADD_RANDOM_FLOAT(field, min, range));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_ADD_FLOAT: Adds a float to the specified field.
// Usage: ADD_FLOAT(field, value)
static s32 bhv_cmd_add_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_add_float(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(ADD_FLOAT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_ADD_SHORT: Adds a short to the specified field.
// Usage: ADD_SHORT(field, value)
static s32 bhv_cmd_add_short(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s16 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_add_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(ADD_SHORT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_OR_SHORT: Performs a bitwise OR with the specified field and the given short.
// Usually used to set an object's flags.
// Usage: OR_SHORT(field, value)
static s32 bhv_cmd_or_short(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_2ND_S16(0);

    value &= BITMASK(16);
    cur_obj_or_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(OR_SHORT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_BIT_CLEAR: Performs a bit clear with the specified short. Unused.
// Usage: BIT_CLEAR(field, value)
static s32 bhv_cmd_bit_clear(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_2ND_S16(0);

    value = ((value & BITMASK(16)) ^ BITMASK(16));
    cur_obj_and_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(BIT_CLEAR(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_VPTR: Sets the specified field to a pointer.
// Usage: SET_VPTR(field, ptr)
static s32 bhv_cmd_set_vptr(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    void* ptr = BHV_CMD_GET_VPTR(1);

    cur_obj_set_vptr(field, ptr);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_VPTR(field, ptr));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_ANIMATE: Begins animation and sets the object's current animation index to the specified value.
// Usage: ANIMATE(animIndex)
static s32 bhv_cmd_animate(void) {
    s32 animIndex = BHV_CMD_GET_2ND_U8(0);
    struct Animation **animations = gCurrentObject->oAnimations;

    geo_obj_init_animation(&gCurrentObject->header.gfx, &animations[animIndex]);

    gCurBhvCommand += SIZEOF_BHV_CMD(ANIMATE(animIndex));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_DROP_TO_FLOOR: Finds the floor triangle directly under the object and moves the object down to it.
// Usage: DROP_TO_FLOOR()
static s32 bhv_cmd_drop_to_floor(void) {
    f32 floor = find_floor_height(gCurrentObject->oPosX, (gCurrentObject->oPosY + 200.0f), gCurrentObject->oPosZ);
    gCurrentObject->oPosY = floor;
    gCurrentObject->oMoveFlags |= OBJ_MOVE_ON_GROUND;

    gCurBhvCommand += SIZEOF_BHV_CMD(DROP_TO_FLOOR());
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_ADD_INT: Adds an integer to the specified field.
// Usage: ADD_INT(field, value)
static s32 bhv_cmd_add_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_S32(1);

    cur_obj_add_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(ADD_INT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_OR_INT: Performs a bitwise OR with the specified field and the given integer.
// Usage: OR_INT(field, value)
static s32 bhv_cmd_or_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_S32(1);

    cur_obj_or_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(OR_INT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_INT: Sets the specified field to an integer.
// Usage: SET_INT(field, value)
static s32 bhv_cmd_set_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_S32(1);

    cur_obj_set_int(field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_INT(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SUM_FLOAT: Sets the destination float field to the sum of the values of the given float fields.
// Usage: SUM_FLOAT(fieldDst, fieldSrc1, fieldSrc2)
static s32 bhv_cmd_sum_float(void) {
    u32 fieldDst = BHV_CMD_GET_2ND_U8(0);
    u32 fieldSrc1 = BHV_CMD_GET_3RD_U8(0);
    u32 fieldSrc2 = BHV_CMD_GET_4TH_U8(0);

    cur_obj_set_float(fieldDst, (cur_obj_get_float(fieldSrc1) + cur_obj_get_float(fieldSrc2)));

    gCurBhvCommand += SIZEOF_BHV_CMD(SUM_FLOAT(fieldDst, fieldSrc1, fieldSrc2));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SUM_INT: Sets the destination integer field to the sum of the values of the given integer fields. Unused.
// Usage: SUM_INT(fieldDst, fieldSrc1, fieldSrc2)
static s32 bhv_cmd_sum_int(void) {
    u32 fieldDst = BHV_CMD_GET_2ND_U8(0);
    u32 fieldSrc1 = BHV_CMD_GET_3RD_U8(0);
    u32 fieldSrc2 = BHV_CMD_GET_4TH_U8(0);

    cur_obj_set_int(fieldDst, (cur_obj_get_int(fieldSrc1) + cur_obj_get_int(fieldSrc2)));

    gCurBhvCommand += SIZEOF_BHV_CMD(SUM_INT(fieldDst, fieldSrc1, fieldSrc2));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_HITBOX: Sets the size of the object's cylindrical hitbox.
// Usage: SET_HITBOX(radius, height)
static s32 bhv_cmd_set_hitbox(void) {
    s16 radius = BHV_CMD_GET_1ST_S16(1);
    s16 height = BHV_CMD_GET_2ND_S16(1);

    gCurrentObject->hitboxRadius = radius;
    gCurrentObject->hitboxHeight = height;

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_HITBOX(radius, height));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_HURTBOX: Sets the size of the object's cylindrical hurtbox.
// Usage: SET_HURTBOX(radius, height)
static s32 bhv_cmd_set_hurtbox(void) {
    s16 radius = BHV_CMD_GET_1ST_S16(1);
    s16 height = BHV_CMD_GET_2ND_S16(1);

    gCurrentObject->hurtboxRadius = radius;
    gCurrentObject->hurtboxHeight = height;

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_HURTBOX(radius, height));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_HITBOX_WITH_OFFSET: Sets the size of the object's cylindrical hitbox, and applies a downwards offset.
// Usage: SET_HITBOX_WITH_OFFSET(radius, height, downOffset)
static s32 bhv_cmd_set_hitbox_with_offset(void) {
    s16 radius = BHV_CMD_GET_1ST_S16(1);
    s16 height = BHV_CMD_GET_2ND_S16(1);
    s16 downOffset = BHV_CMD_GET_1ST_S16(2);

    gCurrentObject->hitboxRadius = radius;
    gCurrentObject->hitboxHeight = height;
    gCurrentObject->hitboxDownOffset = downOffset;

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_HITBOX_WITH_OFFSET(radius, height, downOffset));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_BEGIN: Defines the start of the behavior script as well as the object list the object belongs to.
// Has some special behavior for certain objects.
// Usage: BEGIN(objList)
static s32 bhv_cmd_begin(void) {
    u8 objList = BHV_CMD_GET_2ND_U8(0);

    gCurBhvCommand += SIZEOF_BHV_CMD(BEGIN(objList));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_LOAD_COLLISION_DATA: Loads collision data for the object.
// Usage: LOAD_COLLISION_DATA(collisionData)
static s32 bhv_cmd_load_collision_data(void) {
    void* collisionData = segmented_to_virtual(BHV_CMD_GET_VPTR(1));

    gCurrentObject->collisionData = collisionData;

    gCurBhvCommand += SIZEOF_BHV_CMD(LOAD_COLLISION_DATA(collisionData));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_HOME: Sets the home position of the object to its current position.
// Usage: SET_HOME()
static s32 bhv_cmd_set_home(void) {
    vec3f_copy(&o->oHomeVec, &o->oPosVec);

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_HOME());
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SCALE: Sets the object's size to the specified percentage.
// Usage: SCALE(percent)
static s32 bhv_cmd_scale(void) {
    s16 percent = BHV_CMD_GET_2ND_S16(0);

    cur_obj_scale(percent / 100.0f);

    gCurBhvCommand += SIZEOF_BHV_CMD(SCALE(percent));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SET_OBJ_PHYSICS: Sets various parameters that the object uses for calculating physics.
// Usage: SET_OBJ_PHYSICS(wallHitboxRadius, gravity, bounciness, dragStrength, friction, buoyancy)
static s32 bhv_cmd_set_obj_physics(void) {
    s16 wallHitboxRadius = BHV_CMD_GET_1ST_S16(1);
    s16 gravity          = BHV_CMD_GET_2ND_S16(1);
    s16 bounciness       = BHV_CMD_GET_1ST_S16(2);
    s16 dragStrength     = BHV_CMD_GET_2ND_S16(2);
    s16 friction         = BHV_CMD_GET_1ST_S16(3);
    s16 buoyancy         = BHV_CMD_GET_2ND_S16(3);

    struct Object* obj = gCurrentObject;

    obj->oWallHitboxRadius = wallHitboxRadius;
    obj->oGravity          = gravity      / 100.0f;
    obj->oBounciness       = bounciness   / 100.0f;
    obj->oDragStrength     = dragStrength / 100.0f;
    obj->oFriction         = friction     / 100.0f;
    obj->oBuoyancy         = buoyancy     / 100.0f;

    gCurBhvCommand += SIZEOF_BHV_CMD(SET_OBJ_PHYSICS(wallHitboxRadius, gravity, bounciness, dragStrength, friction, buoyancy));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_PARENT_BIT_CLEAR: Performs a bit clear on the object's parent's field with the specified value.
// Used for clearing active particle flags fron Mario's object.
// Usage: PARENT_BIT_CLEAR(field, value)
static s32 bhv_cmd_parent_bit_clear(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_U32(1);

    value ^= 0xFFFFFFFF;
    obj_and_int(gCurrentObject->parentObj, field, value);

    gCurBhvCommand += SIZEOF_BHV_CMD(PARENT_BIT_CLEAR(field, value));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_SPAWN_WATER_DROPLET: Spawns a water droplet with the given parameters.
// Usage: SPAWN_WATER_DROPLET(dropletParams)
static s32 bhv_cmd_spawn_water_droplet(void) {
    struct WaterDropletParams *dropletParams = BHV_CMD_GET_VPTR(1);

    spawn_water_droplet(gCurrentObject, dropletParams);

    gCurBhvCommand += SIZEOF_BHV_CMD(SPAWN_WATER_DROPLET(dropletParams));
    return BHV_PROC_CONTINUE;
}

// BHV_CMD_ANIMATE_TEXTURE: Animates an object using texture animation. <field> is always set to oAnimState.
// Usage: ANIMATE_TEXTURE(field, rate)
static s32 bhv_cmd_animate_texture(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s16 rate = BHV_CMD_GET_2ND_S16(0);

    // Increase the field (oAnimState) by 1 every <rate> frames.
    if ((gGlobalTimer % rate) == 0) {
        cur_obj_add_int(field, 1);
    }

    gCurBhvCommand += SIZEOF_BHV_CMD(ANIMATE_TEXTURE(field, rate));
    return BHV_PROC_CONTINUE;
}

typedef s32 (*BhvCommandProc)(void);
static BhvCommandProc sBehaviorCmdTable[] = {
    [BHV_CMD_BEGIN                  ] = bhv_cmd_begin,
    [BHV_CMD_DELAY                  ] = bhv_cmd_delay,
    [BHV_CMD_CALL                   ] = bhv_cmd_call,
    [BHV_CMD_RETURN                 ] = bhv_cmd_return,
    [BHV_CMD_GOTO                   ] = bhv_cmd_goto,
    [BHV_CMD_BEGIN_REPEAT           ] = bhv_cmd_begin_repeat,
    [BHV_CMD_END_REPEAT             ] = bhv_cmd_end_repeat,
    [BHV_CMD_END_REPEAT_CONTINUE    ] = bhv_cmd_end_repeat_continue,
    [BHV_CMD_BEGIN_LOOP             ] = bhv_cmd_begin_loop,
    [BHV_CMD_END_LOOP               ] = bhv_cmd_end_loop,
    [BHV_CMD_BREAK                  ] = bhv_cmd_break,
    [BHV_CMD_CALL_NATIVE            ] = bhv_cmd_call_native,
    [BHV_CMD_ADD_FLOAT              ] = bhv_cmd_add_float,
    [BHV_CMD_SET_FLOAT              ] = bhv_cmd_set_float,
    [BHV_CMD_ADD_SHORT              ] = bhv_cmd_add_short,
    [BHV_CMD_SET_SHORT              ] = bhv_cmd_set_short,
    [BHV_CMD_OR_SHORT               ] = bhv_cmd_or_short,
    [BHV_CMD_BIT_CLEAR              ] = bhv_cmd_bit_clear,
    [BHV_CMD_SET_RANDOM_FLOAT       ] = bhv_cmd_set_random_float,
    [BHV_CMD_SET_RANDOM_INT         ] = bhv_cmd_set_random_int,
    [BHV_CMD_ADD_RANDOM_FLOAT       ] = bhv_cmd_add_random_float,
    [BHV_CMD_ADD_INT                ] = bhv_cmd_add_int,
    [BHV_CMD_SET_INT                ] = bhv_cmd_set_int,
    [BHV_CMD_OR_INT                 ] = bhv_cmd_or_int,
    [BHV_CMD_SET_MODEL              ] = bhv_cmd_set_model,
    [BHV_CMD_SPAWN_CHILD            ] = bhv_cmd_spawn_child,
    [BHV_CMD_DEACTIVATE             ] = bhv_cmd_deactivate,
    [BHV_CMD_DROP_TO_FLOOR          ] = bhv_cmd_drop_to_floor,
    [BHV_CMD_SUM_FLOAT              ] = bhv_cmd_sum_float,
    [BHV_CMD_SUM_INT                ] = bhv_cmd_sum_int,
    [BHV_CMD_BILLBOARD              ] = bhv_cmd_billboard,
    [BHV_CMD_HIDE                   ] = bhv_cmd_hide,
    [BHV_CMD_SET_HITBOX             ] = bhv_cmd_set_hitbox,
    [BHV_CMD_DELAY_VAR              ] = bhv_cmd_delay_var,
    [BHV_CMD_SET_VPTR               ] = bhv_cmd_set_vptr,
    [BHV_CMD_ANIMATE                ] = bhv_cmd_animate,
    [BHV_CMD_SPAWN_CHILD_WITH_PARAM ] = bhv_cmd_spawn_child_with_param,
    [BHV_CMD_LOAD_COLLISION_DATA    ] = bhv_cmd_load_collision_data,
    [BHV_CMD_SET_HITBOX_WITH_OFFSET ] = bhv_cmd_set_hitbox_with_offset,
    [BHV_CMD_SPAWN_OBJ              ] = bhv_cmd_spawn_obj,
    [BHV_CMD_SET_HOME               ] = bhv_cmd_set_home,
    [BHV_CMD_SET_HURTBOX            ] = bhv_cmd_set_hurtbox,
    [BHV_CMD_SET_OBJ_PHYSICS        ] = bhv_cmd_set_obj_physics,
    [BHV_CMD_SCALE                  ] = bhv_cmd_scale,
    [BHV_CMD_PARENT_BIT_CLEAR       ] = bhv_cmd_parent_bit_clear,
    [BHV_CMD_ANIMATE_TEXTURE        ] = bhv_cmd_animate_texture,
    [BHV_CMD_DISABLE_RENDERING      ] = bhv_cmd_disable_rendering,
    [BHV_CMD_SPAWN_WATER_DROPLET    ] = bhv_cmd_spawn_water_droplet,
};

// Execute the behavior script of the current object, process the object flags, and other miscellaneous code for updating objects.
void cur_obj_update(void) {
    struct Object* obj = o;
    u32 objFlags = obj->oFlags;
    f32 distanceFromMario;
    BhvCommandProc bhvCmdProc;
    s32 bhvProcResult;

    s32 inRoom = cur_obj_is_mario_in_room();

    if (inRoom == MARIO_OUTSIDE_ROOM && (objFlags & OBJ_FLAG_ONLY_PROCESS_INSIDE_ROOM)) {
        cur_obj_disable_rendering_in_room();
        return;
    }

    // Calculate the distance from the object to Mario.
    if (objFlags & OBJ_FLAG_COMPUTE_DIST_TO_MARIO) {
        obj->oDistanceToMario = dist_between_objects(obj, gMarioObject);
        distanceFromMario = obj->oDistanceToMario;
    } else {
        distanceFromMario = 0.0f;
    }

    // Calculate the angle from the object to Mario.
    if (objFlags & OBJ_FLAG_COMPUTE_ANGLE_TO_MARIO) {
        obj->oAngleToMario = obj_angle_to_object(obj, gMarioObject);
    }

    // If the object's action has changed, reset the action timer.
    if (obj->oAction != obj->oPrevAction) {
        obj->oTimer = 0;
        obj->oSubAction = 0;
        obj->oPrevAction = obj->oAction;
    }

    // Execute the behavior script.
    gCurBhvCommand = obj->curBhvCommand;

    do {
        bhvCmdProc = sBehaviorCmdTable[*gCurBhvCommand >> 24];
        bhvProcResult = bhvCmdProc();
    } while (bhvProcResult == BHV_PROC_CONTINUE);

    obj->curBhvCommand = gCurBhvCommand;

    // Increment the object's timer.
    if (obj->oTimer <= S32_MAX) {
        obj->oTimer++;
    }

    // If the object's action has changed, reset the action timer.
    if (obj->oAction != obj->oPrevAction) {
        obj->oTimer = 0;
        obj->oSubAction = 0;
        obj->oPrevAction = obj->oAction;
    }

    // Execute various code based on object flags.
    objFlags = obj->oFlags;

    if (objFlags & OBJ_FLAG_SET_FACE_ANGLE_TO_MOVE_ANGLE) {
        vec3i_copy(&obj->oFaceAngleVec, &obj->oMoveAngleVec);
    }

    if (objFlags & OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW) {
        obj->oFaceAngleYaw = obj->oMoveAngleYaw;
    }

    if (objFlags & OBJ_FLAG_MOVE_XZ_USING_FVEL) {
        cur_obj_move_xz_using_fvel_and_yaw();
    }

    if (objFlags & OBJ_FLAG_MOVE_Y_WITH_TERMINAL_VEL) {
        cur_obj_move_y_with_terminal_vel();
    }

    if (objFlags & OBJ_FLAG_TRANSFORM_RELATIVE_TO_PARENT) {
        obj_build_transform_relative_to_parent(obj);
    }

    if (objFlags & OBJ_FLAG_SET_THROW_MATRIX_FROM_TRANSFORM) {
        obj_set_throw_matrix_from_transform(obj);
    }

    if (objFlags & OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE) {
        obj_update_gfx_pos_and_angle(obj);
    }

#if SILHOUETTE
    obj->header.gfx.node.flags = COND_BIT(obj->header.gfx.node.flags, GRAPH_RENDER_SILHOUETTE,         (objFlags & OBJ_FLAG_SILHOUETTE        ));
    obj->header.gfx.node.flags = COND_BIT(obj->header.gfx.node.flags, GRAPH_RENDER_OCCLUDE_SILHOUETTE, (objFlags & OBJ_FLAG_OCCLUDE_SILHOUETTE));
#endif

#ifdef OBJECTS_REJ
    if ((objFlags & OBJ_FLAG_SILHOUETTE) || (objFlags & OBJ_FLAG_UCODE_SMALL)) {
        obj->header.gfx.ucode = GRAPH_NODE_UCODE_REJ;
    } else {
        obj->header.gfx.ucode = GRAPH_NODE_UCODE_DEFAULT;
    }
#endif

#ifdef OBJ_OPACITY_BY_CAM_DIST
    if (objFlags & OBJ_FLAG_OPACITY_FROM_CAMERA_DIST) {
        obj_set_opacity_from_cam_dist(obj);
    }
#endif

#ifdef PUPPYLIGHTS
    puppylights_object_emit(obj);
#endif

    // Handle visibility of object
    if (obj->oRoom != -1) {
        // If the object is in a room, only show it when Mario is in the room.
        if (
            (objFlags & OBJ_FLAG_ACTIVE_FROM_AFAR)
            || distanceFromMario < o->oDrawingDistance
        ) {
            if (inRoom == MARIO_OUTSIDE_ROOM) {
                cur_obj_disable_rendering_in_room();
            } else if (inRoom == MARIO_INSIDE_ROOM) {
                cur_obj_enable_rendering_in_room();
            }
            o->activeFlags &= ~ACTIVE_FLAG_FAR_AWAY;
        } else {
            o->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
            o->activeFlags |= ACTIVE_FLAG_FAR_AWAY;
        }
    } else if (
        obj->collisionData == NULL
        &&  (objFlags & OBJ_FLAG_COMPUTE_DIST_TO_MARIO)
        && !(objFlags & OBJ_FLAG_ACTIVE_FROM_AFAR)
    ) {
        // If the object has a render distance, check if it should be shown.
        if (distanceFromMario > obj->oDrawingDistance) {
            // Out of render distance, hide the object.
            cur_obj_disable_rendering();
            obj->activeFlags |= ACTIVE_FLAG_FAR_AWAY;
        } else if (obj->oHeldState == HELD_FREE) {
            // In render distance (and not being held), show the object.
            cur_obj_enable_rendering();
            obj->activeFlags &= ~ACTIVE_FLAG_FAR_AWAY;
        }
    }
}
