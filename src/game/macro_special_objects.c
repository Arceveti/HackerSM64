#include <PR/ultratypes.h>

#include "sm64.h"
#include "object_helpers.h"
#include "macro_special_objects.h"
#include "object_list_processor.h"

#include "behavior_data.h"

#include "macro_presets.h"

#include "special_presets.h"

/*
 * Converts the rotation value supplied by macro objects into one
 * that can be used by in-game objects.
 */
s32 convert_rotation(s16 inRotation) {
    u16 rotation = ((u16)(inRotation & 0xFF) << 8);
    switch (rotation) { //! TODO: What's the purpose of this?
        case 0x3F00: rotation = 0x4000; break;
        case 0x7F00: rotation = 0x8000; break;
        case 0xBF00: rotation = 0xC000; break;
        case 0xFF00: rotation = 0x0000; break;
    }
    return (s16) rotation;
}

/*
 * Spawns an object at an absolute location with rotation around the y-axis and
 * parameters filling up the upper 2 bytes of newObj->oBehParams.
 * The object will not spawn if 'behavior' is NULL.
 */
void spawn_macro_abs_yrot_2params(ModelID32 model, const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 ry, s16 params) {
    if (behavior != NULL) {
        struct Object *newObj =
            spawn_object_abs_with_rot(&gMacroObjectDefaultParent, 0, model, behavior, x, y, z, 0, convert_rotation(ry), 0);
        newObj->oBehParams = ((u32) params << 16);
    }
}

/*
 * Spawns an object at an absolute location with rotation around the y-axis and
 * a single parameter filling up the upper byte of newObj->oBehParams.
 * The object will not spawn if 'behavior' is NULL.
 */
void spawn_macro_abs_yrot_param1(ModelID32 model, const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 ry, s16 param) {
    if (behavior != NULL) {
        struct Object *newObj =
            spawn_object_abs_with_rot(&gMacroObjectDefaultParent, 0, model, behavior, x, y, z, 0, convert_rotation(ry), 0);
        newObj->oBehParams = ((u32) param << 24);
    }
}

/*
 * Spawns an object at an absolute location with currently 3 unknown variables that get converted to
 * floats. Oddly enough, this function doesn't care if 'behavior' is NULL or not.
 */
void spawn_macro_abs_special(ModelID32 model, const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 unkA, s16 unkB, s16 unkC) {
    struct Object *newObj =
        spawn_object_abs_with_rot(&gMacroObjectDefaultParent, 0, model, behavior, x, y, z, 0, 0, 0);

    // Are all three of these values unused?
    newObj->oMacroUnk108 = (f32) unkA;
    newObj->oMacroUnk10C = (f32) unkB;
    newObj->oMacroUnk110 = (f32) unkC;
}

void spawn_macro_objects(s32 areaIndex, MacroObject *macroObjList) {
    s32 presetID;
    struct Object *newObj;
    struct MacroPreset preset;
    MacroObject yaw;
    MacroObject x, y, z;
    MacroObject behParams;
    u8 bparam2, bparam3;

    gMacroObjectDefaultParent.header.gfx.areaIndex = areaIndex;
    gMacroObjectDefaultParent.header.gfx.activeAreaIndex = areaIndex;

    while (*macroObjList != -1) { // An encountered value of -1 means the list has ended.
        presetID = (*macroObjList & BITMASK(9)) - 31; // Preset identifier for MacroObjectPresets array.

        if (presetID < 0) {
            break;
        }

        // Set macro object properties from the list
        yaw       = (((*macroObjList++ >> 9) & BITMASK(7)) << 1);
        x         = *macroObjList++;
        y         = *macroObjList++;
        z         = *macroObjList++;
        behParams = *macroObjList++; // The higher bits are bparam3, the lower bits are bparam2.

        // Get the preset values from the MacroObjectPresets list.
        preset = MacroObjectPresets[presetID];

        // If the preset has a defined param, use it instead.
        if (preset.param != 0) {
            bparam2 = (preset.param & BITMASK(8));
        } else {
            bparam2 = (behParams & BITMASK(8));
        }

        bparam3 = ((behParams >> 16) & BITMASK(8));

        // If the object has been killed, prevent it from respawning.
        if (bparam3 != RESPAWN_INFO_DONT_RESPAWN) {
            // Spawn the new macro object.
            newObj = spawn_object_abs_with_rot(
                         &gMacroObjectDefaultParent,    // Parent object
                         0,                             // Unused
                         preset.model,                  // Model ID
                         preset.behavior,               // Behavior address
                         x,                             // X-position
                         y,                             // Y-position
                         z,                             // Z-position
                         0x0,                           // X-rotation
                         convert_rotation(yaw),         // Y-rotation
                         0x0                            // Z-rotation
                     );

            newObj->oBehParams = ((bparam2 << 16) | (bparam3 << 8));
            newObj->oBehParams2ndByte = bparam2;
            newObj->respawnInfoType = RESPAWN_INFO_TYPE_MACRO_OBJECT;
            newObj->respawnInfo = macroObjList - 1;
            newObj->parentObj = newObj;
        }
    }
}

void spawn_special_objects(s32 areaIndex, TerrainData **specialObjList) {
    s32 i;
    s32 offset;
    Vec3s pos;
    TerrainData extraParams[4];
    ModelID16 model;
    u8 type;
    u8 presetID;
    u8 defaultParam;
    const BehaviorScript *behavior;

    s32 numOfSpecialObjects = *(*specialObjList)++;

    gMacroObjectDefaultParent.header.gfx.areaIndex = areaIndex;
    gMacroObjectDefaultParent.header.gfx.activeAreaIndex = areaIndex;

    for (i = 0; i < numOfSpecialObjects; i++) {
        presetID = *(*specialObjList)++;
        pos[0]   = *(*specialObjList)++;
        pos[1]   = *(*specialObjList)++;
        pos[2]   = *(*specialObjList)++;

        offset = 0;
        while (SpecialObjectPresets[offset].preset_id != presetID) {
            offset++;
        }

        model        = SpecialObjectPresets[offset].model;
        behavior     = SpecialObjectPresets[offset].behavior;
        type         = SpecialObjectPresets[offset].type;
        defaultParam = SpecialObjectPresets[offset].defParam;

        switch (type) {
            case SPTYPE_NO_YROT_OR_PARAMS:
                spawn_macro_abs_yrot_2params(model, behavior, pos[0], pos[1], pos[2], 0, 0);
                break;
            case SPTYPE_YROT_NO_PARAMS:
                extraParams[0] = *(*specialObjList)++; // Y-rotation
                spawn_macro_abs_yrot_2params(model, behavior, pos[0], pos[1], pos[2], extraParams[0], 0);
                break;
            case SPTYPE_PARAMS_AND_YROT:
                extraParams[0] = *(*specialObjList)++; // Y-rotation
                extraParams[1] = *(*specialObjList)++; // Params
                spawn_macro_abs_yrot_2params(model, behavior, pos[0], pos[1], pos[2], extraParams[0], extraParams[1]);
                break;
            case SPTYPE_UNKNOWN:
                extraParams[0] = *(*specialObjList)++; // Unknown, gets put into obj->oMacroUnk108 as a float
                extraParams[1] = *(*specialObjList)++; // Unknown, gets put into obj->oMacroUnk10C as a float
                extraParams[2] = *(*specialObjList)++; // Unknown, gets put into obj->oMacroUnk110 as a float
                spawn_macro_abs_special(model, behavior, pos[0], pos[1], pos[2], extraParams[0], extraParams[1], extraParams[2]);
                break;
            case SPTYPE_DEF_PARAM_AND_YROT:
                extraParams[0] = *(*specialObjList)++; // Y-rotation
                spawn_macro_abs_yrot_param1(model, behavior, pos[0], pos[1], pos[2], extraParams[0], defaultParam);
                break;
            default:
                break;
        }
    }
}

#ifdef NO_SEGMENTED_MEMORY
u32 get_special_objects_size(MacroObject *data) {
    MacroObject *startPos = data;
    s32 i;
    u8 presetID;
    s32 offset;

    s32 numOfSpecialObjects = *data++;

    for (i = 0; i < numOfSpecialObjects; i++) {
        presetID = (u8) *data++;
        data += 3;
        offset = 0;

        while (SpecialObjectPresets[offset].preset_id != presetID) {
            offset++;
        }

        switch (SpecialObjectPresets[offset].type) {
            case SPTYPE_NO_YROT_OR_PARAMS:
                break;
            case SPTYPE_YROT_NO_PARAMS:
                data++;
                break;
            case SPTYPE_PARAMS_AND_YROT:
                data += 2;
                break;
            case SPTYPE_UNKNOWN:
                data += 3;
                break;
            case SPTYPE_DEF_PARAM_AND_YROT:
                data++;
                break;
            default:
                break;
        }
    }

    return (data - startPos);
}
#endif
