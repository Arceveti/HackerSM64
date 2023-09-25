#include <PR/ultratypes.h>

#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "level_update.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "types.h"
#include "sm64.h"
#include "behavior_data.h"

#include "config.h"

/**
 * Determine if Mario is standing on a platform object, meaning that he is
 * within 4 units of the floor. Set his referenced platform object accordingly.
 */
void update_mario_platform(void) {
    struct Surface* floor = gMarioState->floor;

    if (gMarioObject == NULL || floor == NULL) {
        return;
    }

    struct Object* floorObj = floor->object;

    if (
        floor != NULL &&
        floorObj != NULL &&
        absf(gMarioObject->oPosY - gMarioState->floorHeight) < 4.0f
    ) {
        gMarioObject->platform = floorObj;
    } else {
        gMarioObject->platform = NULL;
    }
}

#ifdef PLATFORM_DISPLACEMENT_2
static struct PlatformDisplacementInfo sMarioDisplacementInfo;
static Vec3f sMarioAmountDisplaced;

extern s32 gGlobalTimer;

/**
 * Apply one frame of platform displacement to Mario or an object using the given
 * platform.
 */
void apply_platform_displacement(struct PlatformDisplacementInfo* displaceInfo, Vec3f pos, s16* yaw, struct Object* platform) {
    Vec3f platformPos;
    Vec3f posDifference;
    Vec3f yawVec;
    Vec3f scaledPos;
    // Determine how much Mario turned on his own since last frame
    s16 yawDifference = *yaw - displaceInfo->prevYaw;

    // Avoid a crash if the platform unloaded its collision while stood on
    if (platform->header.gfx.throwMatrix == NULL) {
        return;
    }

    vec3f_copy(platformPos, (*platform->header.gfx.throwMatrix)[3]);

    // Determine how far Mario moved on his own since last frame
    vec3f_copy(posDifference, pos);
    vec3f_sub(posDifference, displaceInfo->prevPos);

    if ((platform == displaceInfo->prevPlatform) && (gGlobalTimer == displaceInfo->prevTimer + 1)) {
        // Upscale position
        vec3f_prod(scaledPos, displaceInfo->prevTransformedPos, platform->header.gfx.scale);

        // Transform from relative positions to world positions
        linear_mtxf_mul_vec3f(*platform->header.gfx.throwMatrix, pos, scaledPos);

        // Add on how much Mario moved in the previous frame
        vec3f_add(pos, posDifference);

        // Calculate new yaw
        linear_mtxf_mul_vec3f(*platform->header.gfx.throwMatrix, yawVec, displaceInfo->prevTransformedYawVec);
        *yaw = atan2s(yawVec[2], yawVec[0]) + yawDifference;
    } else {
        // First frame of standing on the platform, don't calculate a new position
        vec3f_sub(pos, platformPos);
    }

    // Apply velocity-based displacement for certain objects (like the TTC Treadmills)
    if (platform->oFlags & OBJ_FLAG_VELOCITY_PLATFORM) {
        vec3f_add(pos, &platform->oVelVec);
    }

    // Transform from world positions to relative positions for use next frame
    linear_mtxf_transpose_mul_vec3f(*platform->header.gfx.throwMatrix, scaledPos, pos);

    // Downscale position
    vec3f_quot(displaceInfo->prevTransformedPos, scaledPos, platform->header.gfx.scale);
    vec3f_add(pos, platformPos);

    // If the object is Mario, set inertia
    if (pos == gMarioState->pos) {
        vec3f_copy(sMarioAmountDisplaced, pos);
        vec3f_sub(sMarioAmountDisplaced, displaceInfo->prevPos);
        vec3f_sub(sMarioAmountDisplaced, posDifference);

        // Make sure inertia isn't set on the first frame otherwise the previous value isn't cleared
        if ((platform != displaceInfo->prevPlatform) || (gGlobalTimer != displaceInfo->prevTimer + 1)) {
            vec3_zero(sMarioAmountDisplaced);
        }
    }

    // Update info for next frame
    // Update position
    vec3f_copy(displaceInfo->prevPos, pos);

    // Set yaw info
    vec3f_set(yawVec, sins(*yaw), 0, coss(*yaw));
    linear_mtxf_transpose_mul_vec3f(*platform->header.gfx.throwMatrix, displaceInfo->prevTransformedYawVec, yawVec);
    displaceInfo->prevYaw = *yaw;

    // Update platform and timer
    displaceInfo->prevPlatform = platform;
    displaceInfo->prevTimer = gGlobalTimer;
}

// Doesn't change in the code, set this to FALSE if you don't want inertia
_Bool gDoInertia = TRUE;

static _Bool sShouldApplyInertia = FALSE;
static _Bool sInertiaFirstFrame = FALSE;

/**
 * Apply inertia based on Mario's last platform.
 */
static void apply_mario_inertia(void) {
#ifdef MARIO_INERTIA_UPWARD
    // On the first frame of leaving the ground, boost Mario's y velocity
    if (sInertiaFirstFrame) {
        if (sMarioAmountDisplaced[1] > 0.0f) {
            gMarioState->vel[1] += sMarioAmountDisplaced[1];
        }
    }
#endif

#ifdef MARIO_INERTIA_LATERAL
    // Apply sideways inertia
    gMarioState->pos[0] += sMarioAmountDisplaced[0];
    gMarioState->pos[2] += sMarioAmountDisplaced[2];

    // Drag
    sMarioAmountDisplaced[0] *= 0.97f;
    sMarioAmountDisplaced[2] *= 0.97f;
#endif

    // Stop applying inertia once Mario has landed, or when ground pounding
    if (!(gMarioState->action & ACT_FLAG_AIR) || (gMarioState->action == ACT_GROUND_POUND)) {
        sShouldApplyInertia = FALSE;
    }
}

/**
 * Apply platform displacement or inertia if required.
 */
void apply_mario_platform_displacement(void) {
    if (!(gTimeStopState & TIME_STOP_ACTIVE) && gMarioObject != NULL) {
        struct Object* platform = gMarioObject->platform;

        if (platform != NULL) {
            apply_platform_displacement(&sMarioDisplacementInfo, gMarioState->pos, &gMarioState->faceAngle[1], platform);
            sShouldApplyInertia = TRUE;
            sInertiaFirstFrame = TRUE;
        } else if (sShouldApplyInertia && gDoInertia) {
            apply_mario_inertia();
            sInertiaFirstFrame = FALSE;
        }
    }
}

#else

/**
 * Apply one frame of platform rotation to Mario or an object using the given
 * platform. If isMario is false, use gCurrentObject.
 */
void apply_platform_displacement(u32 isMario, struct Object* platform) {
    Vec3f pos;
    Vec3f platformPos;
    Vec3f currentObjectOffset;
    Vec3f relativeOffset;
    Vec3f newObjectOffset;
    Vec3s rotation;
    Mat4 displaceMatrix;

    vec3i_to_vec3s(rotation, &platform->oAngleVelVec);

    if (isMario) {
        vec3f_copy(pos, gMarioStates[0].pos);
    } else {
        vec3f_copy(pos, &o->oPosVec);
    }

    pos[0] += platform->oVelX;
    pos[2] += platform->oVelZ;

    if (rotation[0] != 0
     || rotation[1] != 0
     || rotation[2] != 0) {
        if (isMario) {
            gMarioStates[0].faceAngle[1] += rotation[1];
        }

        vec3f_copy(platformPos, &platform->oPosVec);

        vec3f_diff(currentObjectOffset, pos, platformPos);

        vec3_diff(rotation, &platform->oFaceAngleVec, &platform->oAngleVelVec);

        mtxf_rotate_zxy_and_translate(displaceMatrix, currentObjectOffset, rotation);
        linear_mtxf_transpose_mul_vec3f(displaceMatrix, relativeOffset, currentObjectOffset);

        vec3i_to_vec3s(rotation, &platform->oFaceAngleVec);

        mtxf_rotate_zxy_and_translate(displaceMatrix, currentObjectOffset, rotation);
        linear_mtxf_mul_vec3f(displaceMatrix, newObjectOffset, relativeOffset);

        vec3f_sum(pos, platformPos, newObjectOffset);
    }

    if (isMario) {
        vec3f_copy(gMarioStates[0].pos, pos);
    } else {
        vec3f_copy(&o->oPosVec, pos);
    }
}

/**
 * If Mario's platform is not null, apply platform displacement.
 */
void apply_mario_platform_displacement(void) {
    if (!(gTimeStopState & TIME_STOP_ACTIVE) && gMarioObject != NULL) {
        struct Object* platform = gMarioObject->platform;

        if (platform != NULL) {
            apply_platform_displacement(TRUE, platform);
        }
    }
}
#endif
