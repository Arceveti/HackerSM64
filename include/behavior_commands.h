#ifndef BEHAVIOR_COMMANDS_H
#define BEHAVIOR_COMMANDS_H

#include "types.h"

enum BehaviorCommands {
    /*0x00*/ BHV_CMD_BEGIN,
    /*0x01*/ BHV_CMD_DELAY,
    /*0x02*/ BHV_CMD_CALL,
    /*0x03*/ BHV_CMD_RETURN,
    /*0x04*/ BHV_CMD_GOTO,
    /*0x05*/ BHV_CMD_BEGIN_REPEAT,
    /*0x06*/ BHV_CMD_END_REPEAT,
    /*0x07*/ BHV_CMD_END_REPEAT_CONTINUE,
    /*0x08*/ BHV_CMD_BEGIN_LOOP,
    /*0x09*/ BHV_CMD_END_LOOP,
    /*0x0A*/ BHV_CMD_BREAK,
    /*0x0B*/ BHV_CMD_CALL_NATIVE,
    /*0x0C*/ BHV_CMD_ADD_FLOAT,
    /*0x0D*/ BHV_CMD_SET_FLOAT,
    /*0x0E*/ BHV_CMD_ADD_SHORT,
    /*0x0F*/ BHV_CMD_SET_SHORT,
    /*0x10*/ BHV_CMD_OR_SHORT,
    /*0x11*/ BHV_CMD_BIT_CLEAR,
    /*0x12*/ BHV_CMD_SET_RANDOM_FLOAT,
    /*0x13*/ BHV_CMD_SET_RANDOM_INT,
    /*0x14*/ BHV_CMD_ADD_RANDOM_FLOAT,
    /*0x15*/ BHV_CMD_ADD_INT,
    /*0x16*/ BHV_CMD_SET_INT,
    /*0x17*/ BHV_CMD_OR_INT,
    /*0x18*/ BHV_CMD_SET_MODEL,
    /*0x19*/ BHV_CMD_SPAWN_CHILD,
    /*0x1A*/ BHV_CMD_DEACTIVATE,
    /*0x1B*/ BHV_CMD_DROP_TO_FLOOR,
    /*0x1C*/ BHV_CMD_SUM_FLOAT,
    /*0x2D*/ BHV_CMD_SUM_INT,
    /*0x2E*/ BHV_CMD_BILLBOARD,
    /*0x2F*/ BHV_CMD_HIDE,
    /*0x20*/ BHV_CMD_SET_HITBOX,
    /*0x21*/ BHV_CMD_DELAY_VAR,
    /*0x22*/ BHV_CMD_SET_VPTR,
    /*0x23*/ BHV_CMD_ANIMATE,
    /*0x24*/ BHV_CMD_SPAWN_CHILD_WITH_PARAM,
    /*0x25*/ BHV_CMD_LOAD_COLLISION_DATA,
    /*0x26*/ BHV_CMD_SET_HITBOX_WITH_OFFSET,
    /*0x27*/ BHV_CMD_SPAWN_OBJ,
    /*0x28*/ BHV_CMD_SET_HOME,
    /*0x29*/ BHV_CMD_SET_HURTBOX,
    /*0x2A*/ BHV_CMD_SET_OBJ_PHYSICS,
    /*0x2B*/ BHV_CMD_SCALE,
    /*0x2C*/ BHV_CMD_PARENT_BIT_CLEAR,
    /*0x2D*/ BHV_CMD_ANIMATE_TEXTURE,
    /*0x2E*/ BHV_CMD_DISABLE_RENDERING,
    /*0x2F*/ BHV_CMD_SPAWN_WATER_DROPLET,
};

#define B(a, n, s) (_SHIFTL((a), (32 - ((n) * (s))), (s)))

#define B1(a) B((a), 1,  8) // 0xFF000000
#define B2(a) B((a), 2,  8) // 0x00FF0000
#define B3(a) B((a), 3,  8) // 0x0000FF00
#define B4(a) B((a), 4,  8) // 0x000000FF
#define H1(a) B((a), 1, 16) // 0xFFFF0000
#define H2(a) B((a), 2, 16) // 0x0000FFFF

#define BC_B(a)             (B1(a) | B2(0) | B3(0) | B4(0)) // 0xAA000000
#define BC_BB(a, b)         (B1(a) | B2(b) | B3(0) | B4(0)) // 0xAABB0000
#define BC_BBB(a, b, c)     (B1(a) | B2(b) | B3(c) | B4(0)) // 0xAABBCC00
#define BC_BBBB(a, b, c, d) (B1(a) | B2(b) | B3(c) | B4(d)) // 0xAABBCCDD

#define BC_BBH(a, b, c)     (B1(a) | B2(b) |         H2(c)) // 0xAABBCCCC
#define BC_B0H(a, b)        (B1(a) | B2(0) |         H2(b)) // 0xAA00BBBB

#define BC_H(a)             (        H1(a) |         H2(0)) // 0xAAAA0000
#define BC_HH(a, b)         (        H1(a) |         H2(b)) // 0xAAAABBBB

#define BC_W(a)             ((uintptr_t)(u32)(a))
#define BC_PTR(a)           ((uintptr_t)(a))

#define SIZEOF_BHV_CMD(_cmd) sizeof((u8[]){_cmd})

// Goto destination
#define GOTO_DEST(behavior, identifier) \
    BC_B0H(BHV_CMD_GOTO_DEST, identifier), \
    BC_PTR(behavior)

// Goto destination
#define DESTINATION(identifier) \
    BC_B0H(BHV_CMD_DEST, identifier)

// Defines the start of the behavior script as well as the object list the object belongs to.
// Has some special behavior for certain objects.
#define BEGIN(objList) \
    BC_BB(BHV_CMD_BEGIN, objList)

// Delays the behavior script for a certain number of frames.
#define DELAY(num) \
    BC_B0H(BHV_CMD_DELAY, num)

// Jumps to a new behavior command and stores the return address in the object's stack.
#define CALL(addr) \
    BC_B(BHV_CMD_CALL), \
    BC_PTR(addr)

// Jumps back to the behavior command stored in the object's stack.
#define RETURN() \
    BC_B(BHV_CMD_RETURN)

// Jumps to a new behavior script without saving anything.
#define GOTO(addr) \
    BC_B(BHV_CMD_GOTO), \
    BC_PTR(addr)

// Marks the start of a loop that will repeat a certain number of times.
#define BEGIN_REPEAT(count) \
    BC_B0H(BHV_CMD_BEGIN_REPEAT, count)

// Marks the end of a repeating loop.
#define END_REPEAT() \
    BC_B(BHV_CMD_END_REPEAT)

// Also marks the end of a repeating loop, but continues executing commands following the loop on the same frame.
#define END_REPEAT_CONTINUE() \
    BC_B(BHV_CMD_END_REPEAT_CONTINUE)

// Marks the beginning of an infinite loop.
#define BEGIN_LOOP() \
    BC_B(BHV_CMD_BEGIN_LOOP)

// Marks the end of an infinite loop.
#define END_LOOP() \
    BC_B(BHV_CMD_END_LOOP)

// Exits the behavior script.
// Often used to end behavior scripts that do not contain an infinite loop.
#define BREAK() \
    BC_B(BHV_CMD_BREAK)

// Executes a native game function.
#define CALL_NATIVE(func) \
    BC_B(BHV_CMD_CALL_NATIVE), \
    BC_PTR(func)

// Adds a float to the specified field.
#define ADD_FLOAT(field, value) \
    BC_BBH(BHV_CMD_ADD_FLOAT, field, value)

// Sets the specified field to a float.
#define SET_FLOAT(field, value) \
    BC_BBH(BHV_CMD_SET_FLOAT, field, value)

// Shortcut:
#define SET_DRAWING_DISTANCE(dist) \
    SET_FLOAT(oDrawingDistance, (dist))

// Shortcut:
#define SET_COLLISION_DISTANCE(dist) \
    SET_FLOAT(oCollisionDistance, (dist))

// Shortcut:
#define SET_POS(x, y, z) \
    SET_FLOAT(oPosX, (x)), \
    SET_FLOAT(oPosY, (y)), \
    SET_FLOAT(oPosZ, (z))

// Adds a short to the specified field.
#define ADD_SHORT(field, value) \
    BC_BBH(BHV_CMD_ADD_SHORT, field, value)

// Sets the specified field to a short.
#define SET_SHORT(field, value) \
    BC_BBH(BHV_CMD_SET_SHORT, field, value)

// Performs a bitwise OR with the specified field and the given short.
// Usually used to set an object's flags.
#define OR_SHORT(field, value) \
    BC_BBH(BHV_CMD_OR_SHORT, field, value)

// Performs a bit clear with the specified short. Unused in favor of the 32-bit version.
#define BIT_CLEAR(field, value) \
    BC_BBH(BHV_CMD_BIT_CLEAR, field, value)

// Sets the specified field to a random float in the given range.
#define SET_RANDOM_FLOAT(field, min, range) \
    BC_BBH(BHV_CMD_SET_RANDOM_FLOAT, field, min), \
    BC_H(range)

// Sets the specified field to a random integer in the given range.
#define SET_RANDOM_INT(field, min, range) \
    BC_BBH(BHV_CMD_SET_RANDOM_INT, field, min), \
    BC_H(range)

// Adds a random float in the given range to the specified field.
#define ADD_RANDOM_FLOAT(field, min, range) \
    BC_BBH(BHV_CMD_ADD_RANDOM_FLOAT, field, min), \
    BC_H(range)

// Adds an integer to the specified field.
#define ADD_INT(field, value) \
    BC_BB(BHV_CMD_ADD_INT, field), \
    BC_W(value)

// Shortcut:
#define ADD_ANIM_STATE(n) \
    ADD_INT(oAnimState, (n))

// Sets the specified field to an integer.
#define SET_INT(field, value) \
    BC_BB(BHV_CMD_SET_INT, field), \
    BC_W(value)

// Shortcut:
#define SET_ANIM_STATE(n) \
    SET_INT(oAnimState, (n))

// Shortcut:
#define SET_ROOM(room) \
    SET_INT(oRoom, (room))

// Shortcut:
#define BECOME_TANGIBLE() \
    SET_INT(oIntangibleTimer, 0)

// Shortcut:
#define SET_INTERACT_TYPE(type) \
    SET_INT(oInteractType, (type))

// Shortcut:
#define SET_INTERACT_SUBTYPE(type) \
    SET_INT(oInteractionSubtype, (type))

// Shortcut:
#define SET_FACE_ANGLE(pitch, yaw, roll) \
    SET_INT(oFaceAnglePitch, (pitch)), \
    SET_INT(oFaceAngleYaw,   (yaw  )), \
    SET_INT(oFaceAngleRoll,  (roll ))

// Shortcut:
#define SET_MOVE_ANGLE(pitch, yaw, roll) \
    SET_INT(oMoveAnglePitch, (pitch)), \
    SET_INT(oMoveAngleYaw,   (yaw  )), \
    SET_INT(oMoveAngleRoll,  (roll ))

// Performs a bitwise OR with the specified field and the given integer.
// Usually used to set an object's flags.
#define OR_INT(field, value) \
    BC_BB(BHV_CMD_OR_INT, field), \
    BC_W(value)

// Shortcut:
#define OR_FLAGS(flags) \
    OR_INT(oFlags, (flags))

// HackerSM64 backwards compatibility:
#define OR_LONG(field, value) OR_INT(field, value)

// Sets the current model ID of the object.
#define SET_MODEL(modelID) \
    BC_B0H(BHV_CMD_SET_MODEL, modelID)

// Spawns a child object with the specified model and behavior.
#define SPAWN_CHILD(modelID, behavior) \
    BC_B(BHV_CMD_SPAWN_CHILD), \
    BC_W(modelID), \
    BC_PTR(behavior)

// Exits the behavior script and despawns the object.
// Often used to end behavior scripts that do not contain an infinite loop.
#define DEACTIVATE() \
    BC_B(BHV_CMD_DEACTIVATE)

// Finds the floor triangle directly under the object and moves the object down to it.
#define DROP_TO_FLOOR() \
    BC_B(BHV_CMD_DROP_TO_FLOOR)

// Sets the destination float field to the sum of the values of the given float fields.
#define SUM_FLOAT(fieldDst, fieldSrc1, fieldSrc2) \
    BC_BBBB(BHV_CMD_SUM_FLOAT, fieldDst, fieldSrc1, fieldSrc2)

// Sets the destination integer field to the sum of the values of the given integer fields. Unused.
#define SUM_INT(fieldDst, fieldSrc1, fieldSrc2) \
    BC_BBBB(BHV_CMD_SUM_INT, fieldDst, fieldSrc1, fieldSrc2)

// Billboards the current object, making it always face the camera.
#define BILLBOARD() \
    BC_B(BHV_CMD_BILLBOARD)

// Hides the current object.
#define HIDE() \
    BC_B(BHV_CMD_HIDE)

// Sets the size of the object's cylindrical hitbox.
#define SET_HITBOX(radius, height) \
    BC_B(BHV_CMD_SET_HITBOX), \
    BC_HH(radius, height)

// Delays the behavior script for the number of frames given by the value of the specified field.
#define DELAY_VAR(field) \
    BC_BB(BHV_CMD_DELAY_VAR, field)

// Sets the specified field to a pointer.
#define SET_VPTR(field, anims) \
    BC_BB(BHV_CMD_SET_VPTR, field), \
    BC_PTR(anims)

// Loads the animations for the object. <field> is always set to oAnimations.
#define LOAD_ANIMATIONS(field, anims) \
    SET_VPTR(field, anims)

// Shortcut:
#define SET_ANIMATIONS(anims) \
    LOAD_ANIMATIONS(oAnimations, (anims))

// Begins animation and sets the object's current animation index to the specified value.
#define ANIMATE(animIndex) \
    BC_BB(BHV_CMD_ANIMATE, animIndex)

// Spawns a child object with the specified model and behavior, plus a behavior param.
#define SPAWN_CHILD_WITH_PARAM(bhvParam, modelID, behavior) \
    BC_B0H(BHV_CMD_SPAWN_CHILD_WITH_PARAM, bhvParam), \
    BC_W(modelID), \
    BC_PTR(behavior)

// Loads collision data for the object.
#define LOAD_COLLISION_DATA(collisionData) \
    BC_B(BHV_CMD_LOAD_COLLISION_DATA), \
    BC_PTR(collisionData)

// Sets the size of the object's cylindrical hitbox, and applies a downwards offset.
#define SET_HITBOX_WITH_OFFSET(radius, height, downOffset) \
    BC_B(BHV_CMD_SET_HITBOX_WITH_OFFSET), \
    BC_HH(radius, height), \
    BC_H(downOffset)

// Spawns a new object with the specified model and behavior.
#define SPAWN_OBJ(modelID, behavior) \
    BC_B(BHV_CMD_SPAWN_OBJ), \
    BC_W(modelID), \
    BC_PTR(behavior)

// Sets the home position of the object to its current position.
#define SET_HOME() \
    BC_B(BHV_CMD_SET_HOME)

// Sets the size of the object's cylindrical hurtbox.
#define SET_HURTBOX(radius, height) \
    BC_B(BHV_CMD_SET_HURTBOX), \
    BC_HH(radius, height)

// Sets various parameters that the object uses for calculating physics.
#define SET_OBJ_PHYSICS(wallHitboxRadius, gravity, bounciness, dragStrength, friction, buoyancy) \
    BC_B(BHV_CMD_SET_OBJ_PHYSICS), \
    BC_HH(wallHitboxRadius, gravity), \
    BC_HH(bounciness, dragStrength), \
    BC_HH(friction, buoyancy)

// Sets the object's size to the specified percentage.
#define SCALE(percent) \
    BC_B0H(BHV_CMD_SCALE, percent)

// Performs a bit clear on the object's parent's field with the specified value.
// Used for clearing active particle flags fron Mario's object.
#define PARENT_BIT_CLEAR(field, flags) \
    BC_BB(BHV_CMD_PARENT_BIT_CLEAR, field), \
    BC_W(flags)

// Animates an object using texture animation. <field> is always set to oAnimState.
#define ANIMATE_TEXTURE(field, rate) \
    BC_BBH(BHV_CMD_ANIMATE_TEXTURE, field, rate)

// Disables rendering for the object.
#define DISABLE_RENDERING() \
    BC_B(BHV_CMD_DISABLE_RENDERING)

// Spawns a water droplet with the given parameters.
#define SPAWN_WATER_DROPLET(dropletParams) \
    BC_B(BHV_CMD_SPAWN_WATER_DROPLET), \
    BC_PTR(dropletParams)

#endif // BEHAVIOR_COMMANDS_H
