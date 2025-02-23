#include <PR/ultratypes.h>
#include <stdio.h>

#include "debug_utils.h"
#include "dynlist_proc.h"
#include "gd_macros.h"
#include "gd_main.h"
#include "gd_math.h"
#include "gd_types.h"
#include "macros.h"
#include "objects.h"
#include "old_menu.h"
#include "renderer.h"
#include "shape_helper.h"
#include "draw_objects.h"

/**
 * @file draw_objects.c
 * This file contains the functions and helpers for rendering the various
 * GdObj primitives to the screen.
 */

// forward declarations
void func_80179B64(struct ObjGroup *);
void update_shaders(struct ObjShape *, struct GdVec3f *);
void draw_shape_faces(struct ObjShape *);
void register_light(struct ObjLight *);

// types
/**
 * Modes for drawscene()
 */
enum SceneType {
    RENDER_SCENE = 26, ///< render the primitives to screen
    FIND_PICKS   = 27, ///< only check position of primitives relative to cursor click
};

// data
static struct GdColour sClrWhite        =   { 1.0f, 1.0f, 1.0f };   // @ 801A8070
static struct GdColour sClrRed          =   { 1.0f, 0.0f, 0.0f };   // @ 801A807C
static struct GdColour sClrGreen        =   { 0.0f, 1.0f, 0.0f };   // @ 801A8088
static struct GdColour sClrBlue         =   { 0.0f, 0.0f, 1.0f };   // @ 801A8094
static struct GdColour sClrErrDarkBlue  =   { 0.0f, 0.0f, 6.0f };   // @ 801A80A0
static struct GdColour sClrPink         =   { 1.0f, 0.0f, 1.0f };   // @ 801A80AC
static struct GdColour sClrBlack        =   { 0.0f, 0.0f, 0.0f };   // @ 801A80B8
static struct GdColour sClrGrey         =   { 0.6f, 0.6f, 0.6f };   // @ 801A80C4
static struct GdColour sClrDarkGrey     =   { 0.4f, 0.4f, 0.4f };   // @ 801A80D0
static struct GdColour sClrYellow       =   { 1.0f, 1.0f, 0.0f };   // @ 801A80DC
static struct GdColour sLightColours[1] = { { 1.0f, 1.0f, 0.0f } }; // @ 801A80E8
static struct GdColour *sSelectedColour = &sClrRed;                 // @ 801A80F4
struct ObjCamera *gViewUpdateCamera = NULL;                         // @ 801A80F8
// @ 801A8100
struct GdColour *sColourPalette[5] = { // @ 801A8104
    &sClrWhite, &sClrYellow, &sClrRed, &sClrBlack, &sClrBlack
};
struct GdColour *sWhiteBlack[2] = {
    //@ 801A8118
    &sClrWhite,
    &sClrBlack,
};
static s32 sLightDlCounter = 1; // @ 801A81A0

// bss
struct ObjGroup *gGdLightGroup; // @ 801B9BB8; is this the main light group? only light group?

static enum SceneType sSceneProcessType; // @ 801B9C00
static s32 sUseSelectedColor;            // @ 801B9C04
static s16 sPickBuffer[100];             ///< buffer of objects near click
static s32 sPickDataTemp;                ///< now, only data is the object number of a selected joint
static f32 sPickObjDistance;             ///< distance between object position and cursor click location
static struct GdObj *sPickedObject;      ///< object selected with cursor
/// Various counters and pointers set in update_view() and used in various `draw_XXX` functions
static struct {
    u8 filler1[4];        // @ 801B9CE0
    struct ObjView *view; // @ 801B9CE4
    s32 unreadCounter;    // @ 801B9CE8
    s32 mtlDlNum;         // @ 801B9CEC; name is a big guess
    s32 shapesDrawn;      // @ 801B9CF0
    s32 unused;           // @ 801B9CF4
} sUpdateViewState;
static struct ObjLight *sPhongLight;          // material light? phong light?
static struct GdVec3f sPhongLightPosition;    //@ 801B9D00; guess; light source unit position for light
                                              // flagged 0x20 (sPhongLight)
static struct GdVec3f sLightPositionOffset;   // @ 801B9D10
static struct GdVec3f sLightPositionCache[8]; // @ 801B9D20; unit positions
static s32 sNumActiveLights;                  // @ 801B9D80; maybe?
static struct GdVec3f sGrabCords;             ///< x, y grabbable point near cursor

/**
 * Set the ambient light color and turn on G_CULL_BACK.
 */
void setup_lights(void) {
    set_light_num(NUMLIGHTS_2);
    gd_setproperty(GD_PROP_AMB_COLOUR, 0.5f, 0.5f, 0.5f);
    gd_setproperty(GD_PROP_CULLING, 1.0f, 0.0f, 0.0f); // set G_CULL_BACK
}

/**
 * @note Not called
 */
UNUSED void Unknown801781DC(struct ObjZone *zone) {
    struct GdVec3f lightPos; // 3c
    struct ObjUnk200000 *unk;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    struct ObjLight *light;
    struct ListNode *link = zone->unk30->firstMember; // s0 (24)
    struct GdObj *obj;                                 // 20

    while (link != NULL) {
        obj = link->obj;
        light = (struct ObjLight *) gGdLightGroup->firstMember->obj;
        lightPos.x = light->position.x;
        lightPos.y = light->position.y;
        lightPos.z = light->position.z;
        unk = (struct ObjUnk200000 *) obj;
        sp34 = gd_dot_vec3f(&unk->unk34->normal, &unk->unk30->pos);
        sp30 = gd_dot_vec3f(&unk->unk34->normal, &lightPos);
        lightPos.x -= unk->unk34->normal.x * (sp30 - sp34);
        lightPos.y -= unk->unk34->normal.y * (sp30 - sp34);
        lightPos.z -= unk->unk34->normal.z * (sp30 - sp34);
        unk->unk30->pos.x = lightPos.x;
        unk->unk30->pos.y = lightPos.y;
        unk->unk30->pos.z = lightPos.z;
        sp2C = ABS((sp30 - sp34));
        if (sp2C > 600.0f) {
            sp2C = 600.0f;
        }
        sp2C = 1.0f - sp2C / 600.0f;
        unk->unk30->normal.x = sp2C * light->colour.r;
        unk->unk30->normal.y = sp2C * light->colour.g;
        unk->unk30->normal.z = sp2C * light->colour.b;
        link = link->next;
    }
}

/* 226C6C -> 226FDC */
void draw_shape(struct ObjShape *shape, s32 flag, f32 c, f32 d, f32 e, // "sweep" indices 0-2 x, y, z
                f32 f, f32 g, f32 h, // translate shape + store offset (unused)
                f32 i, f32 j, f32 k, // translate shape
                f32 l, f32 m, f32 n, // rotate x, y, z
                s32 colorIdx, Mat4f *rotMtx) {
    struct GdVec3f offset;

    restart_timer("drawshape");
    sUpdateViewState.shapesDrawn++;

    if (shape == NULL) {
        return;
    }

    offset.x = offset.y = offset.z = 0.0f;
    if (flag & SHAPE_FLAG_TRANSLATE) {
        gd_dl_load_trans_matrix(f, g, h);
        offset.x += f;
        offset.y += g;
        offset.z += h;
    }

    if ((flag & SHAPE_FLAG_10) && rotMtx != NULL) {
        gd_dl_load_matrix(rotMtx);
        offset.x += (*rotMtx)[3][0];
        offset.y += (*rotMtx)[3][1];
        offset.z += (*rotMtx)[3][2];
    }

    if (flag & SHAPE_FLAG_ROTATE) {
        if (m != 0.0f) gd_dl_rotate(m, GD_Y_AXIS);
        if (l != 0.0f) gd_dl_rotate(l, GD_X_AXIS);
        if (n != 0.0f) gd_dl_rotate(n, GD_Z_AXIS);
    }

    if (colorIdx != 0) {
        sUseSelectedColor = TRUE;
        sSelectedColour = gd_get_colour(colorIdx);
        if (sSelectedColour != NULL) {
            gd_dl_material_lighting(-1, sSelectedColour, GD_MTL_LIGHTS);
        } else {
            fatal_print("Draw_shape(): Bad colour");
        }
    } else {
        sUseSelectedColor = FALSE;
        sSelectedColour = NULL;
    }

    if (sNumActiveLights != 0 && shape->mtlGroup != NULL) {
        if (rotMtx != NULL) {
            offset.x = (*rotMtx)[3][0];
            offset.y = (*rotMtx)[3][1];
            offset.z = (*rotMtx)[3][2];
        } else {
            offset.x = offset.y = offset.z = 0.0f;
        }
        update_shaders(shape, &offset);
    }

    if (flag & SHAPE_FLAG_MUL_TRANS_MTX) {
        gd_dl_mul_trans_matrix(i, j, k);
    }

    if (flag & SHAPE_FLAG_SCALE) {
        gd_dl_scale(c, d, e);
    }

    draw_shape_faces(shape);
    sUseSelectedColor = FALSE;
    split_timer("drawshape");
}

void draw_shape_2d(struct ObjShape *shape, s32 flag, UNUSED f32 c, UNUSED f32 d, UNUSED f32 e, f32 f,
                   f32 g, f32 h, UNUSED f32 i, UNUSED f32 j, UNUSED f32 k, UNUSED f32 l, UNUSED f32 m,
                   UNUSED f32 n, UNUSED s32 color, UNUSED s32 p) {
    struct GdVec3f sp1C;

    restart_timer("drawshape2d");
    sUpdateViewState.shapesDrawn++;

    if (shape == NULL) {
        return;
    }

    if (flag & SHAPE_FLAG_TRANSLATE) {
        sp1C.x = f;
        sp1C.y = g;
        sp1C.z = h;
        if (gViewUpdateCamera != NULL) {
            gd_rotate_and_translate_vec3f(&sp1C, &gViewUpdateCamera->unkE8);
        }
        gd_dl_load_trans_matrix(sp1C.x, sp1C.y, sp1C.z);
    }
    draw_shape_faces(shape);
    split_timer("drawshape2d");
}

void draw_light(struct ObjLight *light) {
    struct GdVec3f sp94;
    Mat4f sp54;
    UNUSED Mat4f *uMatPtr;
    UNUSED f32 uMultiplier;
    struct ObjShape *shape;

    if (sSceneProcessType == FIND_PICKS) {
        return;
    }

    sLightColours[0].r = light->colour.r;
    sLightColours[0].g = light->colour.g;
    sLightColours[0].b = light->colour.b;

    if (light->flags & LIGHT_UNK02) {
        gd_set_identity_mat4(&sp54);
        sp94.x = -light->direction.x;
        sp94.y = -light->direction.y;
        sp94.z = -light->direction.z;
        gd_create_origin_lookat(&sp54, &sp94, 0.0f);
        uMultiplier = light->unk38 / 45.0f;
        shape = gSpotShape;
        uMatPtr = &sp54;
    } else {
        uMultiplier = 1.0f;
        shape = light->unk9C;
        uMatPtr = NULL;
        if (++sLightDlCounter >= 17) {
            sLightDlCounter = 1;
        }
        shape->unk50 = sLightDlCounter;
    }

    draw_shape_2d(shape, 2, 1.0f, 1.0f, 1.0f, light->position.x, light->position.y, light->position.z,
                  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1, 0);
}

void draw_material(struct ObjMaterial *mtl) {
    s32 mtlType = mtl->type; // 24

    if (mtlType == GD_MTL_SHINE_DL) {
        if (sPhongLight != NULL && sPhongLight->diffuseFac > 0.0f) {
            if (gViewUpdateCamera != NULL) {
                gd_dl_hilite(mtl->gddlNumber, gViewUpdateCamera, &sPhongLight->position,
                              &sLightPositionOffset, &sPhongLightPosition, &sPhongLight->colour);
            } else {
                fatal_printf("draw_material() no active camera for phong");
            }
        } else {
            mtlType = GD_MTL_BREAK;
        }
    }
    if (!sUseSelectedColor) {
        gd_dl_material_lighting(mtl->gddlNumber, &mtl->Kd, mtlType);
    } else {
        gd_dl_material_lighting(mtl->gddlNumber, sSelectedColour, GD_MTL_LIGHTS);
    }
}

/**
 * Create a `GdDisplayList` and store its number in the input `ObjMaterial`
 * if this material doesn't have one
 */
void create_mtl_gddl_if_empty(struct ObjMaterial *mtl) {
    if (mtl->gddlNumber == 0) {
        mtl->gddlNumber = create_mtl_gddl(mtl->type);
    }
}

/**
 * A function for checking if an `ObjFace` has bad vertices. These could be either
 * unconverted vertex data, or old vertex structures (like `BetaVtx`)
 * @note Not called
 */
UNUSED void check_face_bad_vtx(struct ObjFace *face) {
    s32 i;
    struct ObjVertex *vtx;

    for (i = 0; i < face->vtxCount; i++) {
        vtx = face->vertices[i];
        // These seem to be checks against bad conversions, or an outdated vertex structure..?
        if ((uintptr_t) vtx == 39) {
            gd_printf("bad1\n");
            return;
        }
        if ((uintptr_t) vtx->gbiVerts == 0x3F800000) { // 1.0f
            fatal_printf("bad2 %x,%d,%d,%d\n", (u32) (uintptr_t) vtx, vtx->scaleFactor, vtx->id, vtx->header.type);
        }
    }
}

/**
 * @brief Convert a numeric index into pointer to a struct GdColour
 *
 * A simple switch case to convert from index @p idx to a pointer to the
 * three f32 GdColour structure. Goddard stored the index in a structure,
 * and uses this function to get the colour RGB values if needed.
 * -1 uses the environment colour.
 * A possible enhancement for this is to ennumerate all colours, and then
 * use those enumerations and/or enum type where ever a colour is requested
 *
 * @param idx Index of colour
 * @return Pointer to a GdColour struct
 */
struct GdColour *gd_get_colour(s32 idx) {
    switch (idx) {
        case COLOUR_BLACK:
            return &sClrBlack;
        case COLOUR_WHITE:
            return &sClrWhite;
        case COLOUR_RED:
            return &sClrRed;
        case COLOUR_GREEN:
            return &sClrGreen;
        case COLOUR_BLUE:
            return &sClrBlue;
        case COLOUR_GRAY:
            return &sClrGrey;
        case COLOUR_DARK_GRAY:
            return &sClrDarkGrey;
        case COLOUR_DARK_BLUE:
            return &sClrErrDarkBlue;
        case COLOUR_BLACK2:
            return &sClrBlack;
        case COLOUR_YELLOW:
            return &sClrYellow;
        case COLOUR_PINK:
            return &sClrPink;
        case -1:
            return &sLightColours[0];
        default:
            return NULL;
    }
}

/**
 * Uncalled function that would render a triangle
 * @note Not called
 */
UNUSED void Unknown80178ECC(f32 v0X, f32 v0Y, f32 v0Z, f32 v1X, f32 v1Y, f32 v1Z) {
    f32 difY = v1Y - v0Y;
    f32 difX = v1X - v0X;
    f32 difZ = v1Z - v0Z;

    gd_dl_make_triangle(v0X, v0Y, v0Z, v1X, v1Y, v1Z, v0X + difY * 0.1f, v0Y + difX * 0.1f, v0Z + difZ * 0.1f);
}

/**
 * Rendering function for `ObjFace` structures. It has a fair amount
 * of stub code
 */
void draw_face(struct ObjFace *face) {
    struct ObjVertex *vtx; // 3c
    f32 z;                 // 38
    f32 y;                 // 34
    f32 x;                 // 30
    s32 i;             // 20; also used to store mtl's gddl number
    Vtx *gbiVtx;       // 18

    imin("draw_face");
    if (!sUseSelectedColor && face->mtlId >= 0 && face->mtl != NULL) { // -1 == colored face
        i = face->mtl->gddlNumber;

        if (i != 0 && i != sUpdateViewState.mtlDlNum) {
            gd_dl_flush_vertices();
            branch_to_gddl(i);
            sUpdateViewState.mtlDlNum = i;
        }
    }

    check_tri_display(face->vtxCount);

    if (!gGdUseVtxNormal) {
        set_Vtx_norm_buf(&face->normal);
    }

    for (i = 0; i < face->vtxCount; i++) {
        vtx = face->vertices[i];
        x = vtx->pos.x;
        y = vtx->pos.y;
        z = vtx->pos.z;
        if (gGdUseVtxNormal) {
            set_Vtx_norm_buf(&vtx->normal);
        }

        gbiVtx = gd_dl_make_vertex(x, y, z, vtx->alpha);

        if (gbiVtx != NULL) {
            vtx->gbiVerts = make_vtx_link(vtx->gbiVerts, gbiVtx);
        }
    }
    func_8019FEF0();
    imout();
}

/**
 * Render a filled rectangle from (`ulx`, `uly`) to (`lrx`, `lry`).
 *
 * @param color `GdColour` index
 * @param ulx,uly upper left point
 * @param lrx,lry lower right point
 */
void draw_rect_fill(s32 color, f32 ulx, f32 uly, f32 lrx, f32 lry) {
    gd_dl_set_fill(gd_get_colour(color));
    gd_draw_rect(ulx, uly, lrx, lry);
}

/**
 * Render a stroked rectangle (aka border box) from (`ulx`, `uly`) to (`lrx`, `lry`).
 *
 * @param color `GdColour` index
 * @param ulx,uly upper left point
 * @param lrx,lry lower right point
 */
void draw_rect_stroke(s32 color, f32 ulx, f32 uly, f32 lrx, f32 lry) {
    gd_dl_set_fill(gd_get_colour(color));
    gd_draw_border_rect(ulx, uly, lrx, lry);
}

/**
 * Uncalled function that calls other orphan stub functions.
 * @note Not called
 */
UNUSED void Unknown801792F0(struct GdObj *obj) {
    char objId[32];
    struct GdVec3f objPos;

    format_object_id(objId, obj);
    set_cur_dynobj(obj);
    d_get_world_pos(&objPos);
    func_801A4438(objPos.x, objPos.y, objPos.z);
    stub_draw_label_text(objId);
}

/**
 * Draws a label
 */
void draw_label(struct ObjLabel *label) {
    struct GdVec3f position;
    char strbuf[0x100];
    union ObjVarVal varval;
    valptrproc_t valfn = label->valfn;
    struct ObjValPtr *valptr = label->valptr;

    if (valptr != NULL) {
        if (valptr->flag == 0x40000) {
            // position is offset from object
            set_cur_dynobj(valptr->obj);
            d_get_world_pos(&position);
        } else {
            // position is absolute
            position.x = position.y = position.z = 0.0f;
        }

        switch (valptr->datatype) {
            case OBJ_VALUE_FLOAT:
                get_objvalue(&varval, OBJ_VALUE_FLOAT, valptr->obj, valptr->offset);
                if (valfn != NULL) {
                    valfn(&varval, varval);
                }
                sprintf(strbuf, label->fmtstr, varval.f);
                break;
            case OBJ_VALUE_INT:
                get_objvalue(&varval, OBJ_VALUE_INT, valptr->obj, valptr->offset);
                if (valfn != NULL) {
                    valfn(&varval, varval);
                }
                sprintf(strbuf, label->fmtstr, varval.i);
                break;
            default:
                if (label->fmtstr != NULL) {
                    gd_strcpy(strbuf, label->fmtstr);
                } else {
                    gd_strcpy(strbuf, "NONAME");
                }
                break;
        }
    } else {
        position.x = position.y = position.z = 0.0f;
        if (label->fmtstr != NULL) {
            gd_strcpy(strbuf, label->fmtstr);
        } else {
            gd_strcpy(strbuf, "NONAME");
        }
    }
    position.x += label->position.x;
    position.y += label->position.y;
    position.z += label->position.z;
    func_801A4438(position.x, position.y, position.z);
    stub_draw_label_text(strbuf);
}

/* 227DF8 -> 227F3C; orig name: Proc80179628 */
void draw_net(struct ObjNet *self) {
    struct ObjNet *net = self;
    s32 netColor;

    if (sSceneProcessType == FIND_PICKS) {
        return;
    }

    if (net->header.drawFlags & OBJ_HIGHLIGHTED) {
        netColor = COLOUR_YELLOW;
    } else {
        netColor = net->colourNum;
    }

    if (net->shapePtr != NULL) {
        draw_shape(net->shapePtr, SHAPE_FLAG_10, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, netColor, &net->mat168);
    }

    if (net->unk1C8 != NULL) {
        draw_group(net->unk1C8);
    }
}

/**
 * Draws a gadget
 */
void draw_gadget(struct ObjGadget *gdgt) {
    s32 colour = COLOUR_BLACK;

    if (gdgt->colourNum != COLOUR_BLACK) {
        colour = gdgt->colourNum;
    }

    draw_rect_fill(colour,
        gdgt->worldPos.x,
        gdgt->worldPos.y,
        gdgt->worldPos.x + gdgt->sliderPos * gdgt->size.x,
        gdgt->worldPos.y + gdgt->size.y);

    if (gdgt->header.drawFlags & OBJ_HIGHLIGHTED) {
        draw_rect_stroke(COLOUR_YELLOW,
            gdgt->worldPos.x,
            gdgt->worldPos.y,
            gdgt->worldPos.x + gdgt->sliderPos * gdgt->size.x,
            gdgt->worldPos.y + gdgt->size.y);
    }
    gdgt->header.drawFlags &= ~OBJ_HIGHLIGHTED;
}

/* 22803C -> 22829C */
void draw_camera(struct ObjCamera *cam) {
    struct GdVec3f pos;

    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    if (cam->obj != NULL) {
        set_cur_dynobj(cam->obj);
        d_get_world_pos(&pos);
        pos.x += cam->lookAt.x;
        pos.y += cam->lookAt.y;
        pos.z += cam->lookAt.z;
    } else {
        pos.x = cam->lookAt.x;
        pos.y = cam->lookAt.y;
        pos.z = cam->lookAt.z;
    }

    if (ABS(cam->worldPos.x - pos.x) + ABS(cam->worldPos.z - pos.z) == 0.0f) {
        gd_printf("Draw_Camera(): Zero view distance\n");
        return;
    }
    gd_dl_lookat(cam, cam->worldPos.x, cam->worldPos.y, cam->worldPos.z, pos.x, pos.y, pos.z, cam->roll);
}

/**
 * Forms uncalled recursive loop with func_80179B64().
 * This function seems to turn off the otherwise unused `OBJ_DRAW_UNK01` flag
 * for the GdObj.drawFlags
 * @note Not called
 */
UNUSED void Unknown80179ACC(struct GdObj *obj) {
    switch (obj->type) {
        case OBJ_TYPE_NETS:
            if (((struct ObjNet *) obj)->unk1C8 != NULL) {
                func_80179B64(((struct ObjNet *) obj)->unk1C8);
            }
            break;
        default:
            break;
    }
    obj->drawFlags &= ~OBJ_DRAW_UNK01;
}

/**
 * Forms uncalled recursive loop with Unknown80179ACC()
 * @note Not called
 */
UNUSED void func_80179B64(struct ObjGroup *group) {
    apply_to_obj_types_in_group((OBJ_TYPE_LABELS | OBJ_TYPE_GADGETS | OBJ_TYPE_CAMERAS | OBJ_TYPE_NETS
                                    | OBJ_TYPE_JOINTS | OBJ_TYPE_BONES),
                                (applyproc_t) Unknown80179ACC, group);
}

/* 22836C -> 228498 */
void world_pos_to_screen_coords(struct GdVec3f *pos, struct ObjCamera *cam, struct ObjView *view) {
    gd_rotate_and_translate_vec3f(pos, &cam->unkE8);
    if (pos->z > -256.0f) {
        return;
    }

    pos->x *= 256.0f / -pos->z;
    pos->y *= 256.0f /  pos->z;
    pos->x += view->lowerRight.x / 2.0f;
    pos->y += view->lowerRight.y / 2.0f;
}

/**
 * Check if the current cursor position is near enough to @p input to
 * grab that `GdObj`. The range is +/- 20 units for being close to a
 * grab point.
 *
 * If the object can be grabbed, its information is stored in a buffer by
 * `store_in_pickbuf()`.
 *
 * @param input `GdObj` to check position of
 * @return void
 */
void check_grabbable_click(struct GdObj *input) {
    struct GdVec3f objPos;
    struct GdObj *obj;
    Mat4f *mtx;

    if (gViewUpdateCamera == NULL) {
        return;
    }
    obj = input;
    if (!(obj->drawFlags & OBJ_IS_GRABBABLE)) {
        return;
    }

    set_cur_dynobj(obj);
    mtx = d_get_rot_mtx_ptr();
    objPos.x = (*mtx)[3][0];
    objPos.y = (*mtx)[3][1];
    objPos.z = (*mtx)[3][2];
    world_pos_to_screen_coords(&objPos, gViewUpdateCamera, sUpdateViewState.view);
    if (ABS(gGdCtrl.csrX - objPos.x) < 20.0f
     && ABS(gGdCtrl.csrY - objPos.y) < 20.0f) {
        // store (size, Obj Type, Obj Index) in s16 pick buffer array
        store_in_pickbuf(2);
        store_in_pickbuf(obj->type);
        store_in_pickbuf(obj->index);
        sGrabCords.x = objPos.x;
        sGrabCords.y = objPos.y;
    }
}

/**
 * The main function for rendering the components of an `ObjView`. It called
 * both for drawing the various `GdObj` primatives as well as when checking
 * the location of a cursor click.
 * @note This has to be called from update_view() due to that function setting
 *       state variables on which this function relies
 *
 * @param process determines if this is rendering the scene
 *                or just checking click location
 * @param interactables components of `ObjView`
 * @param lightgrp lights of `ObjView
 */
void drawscene(enum SceneType process, struct ObjGroup *interactables, struct ObjGroup *lightgrp) {
    restart_timer("drawscene");
    imin("draw_scene()");
    sUpdateViewState.unreadCounter = 0;
    restart_timer("draw1");
    set_gd_mtx_parameters(G_MTX_PROJECTION | G_MTX_MUL | G_MTX_PUSH);
    if (sUpdateViewState.view->projectionType == 1) {
        gd_create_perspective_matrix(sUpdateViewState.view->clipping.z,
                      sUpdateViewState.view->lowerRight.x / sUpdateViewState.view->lowerRight.y,
                      sUpdateViewState.view->clipping.x, sUpdateViewState.view->clipping.y);
    } else {
        gd_create_ortho_matrix(
            -sUpdateViewState.view->lowerRight.x / 2.0f, sUpdateViewState.view->lowerRight.x / 2.0f,
            -sUpdateViewState.view->lowerRight.y / 2.0f, sUpdateViewState.view->lowerRight.y / 2.0f,
            sUpdateViewState.view->clipping.x, sUpdateViewState.view->clipping.y);
    }

    if (lightgrp != NULL) {
        set_gd_mtx_parameters(G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);
        apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS | OBJ_TYPE_PARTICLES,
                                    (applyproc_t) apply_obj_draw_fn, lightgrp);
        set_gd_mtx_parameters(G_MTX_PROJECTION | G_MTX_MUL | G_MTX_PUSH);
    }

    if (gViewUpdateCamera != NULL) {
        draw_camera(gViewUpdateCamera);
    } else {
        gd_dl_mul_trans_matrix(0.0f, 0.0f, -1000.0f);
    }

    setup_lights();
    set_gd_mtx_parameters(G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);
    gd_dl_push_matrix();
    sSceneProcessType = process;

    if ((sNumActiveLights = sUpdateViewState.view->flags & VIEW_LIGHT)) {
        sUpdateViewState.view->flags &= ~VIEW_LIGHT;
    }

    sNumActiveLights = 1;
    apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS, (applyproc_t) register_light, gGdLightGroup);
    split_timer("draw1");
    restart_timer("drawobj");
    imin("process_group");
    if (sSceneProcessType == FIND_PICKS) {
        apply_to_obj_types_in_group(OBJ_TYPE_ALL, (applyproc_t) check_grabbable_click, interactables);
    } else {
        apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS | OBJ_TYPE_GADGETS | OBJ_TYPE_NETS
                                        | OBJ_TYPE_PARTICLES,
                                    (applyproc_t) apply_obj_draw_fn, interactables);
    }
    imout();
    split_timer("drawobj");
    gd_setproperty(GD_PROP_LIGHTING, 0.0f, 0.0f, 0.0f);
    apply_to_obj_types_in_group(OBJ_TYPE_LABELS, (applyproc_t) apply_obj_draw_fn, interactables);
    gd_setproperty(GD_PROP_LIGHTING, 1.0f, 0.0f, 0.0f);
    gd_dl_pop_matrix();
    imout();
    split_timer("drawscene");
}

/**
 * A drawing function that does nothing. This function is used for
 * `GdObj`s that don't need to be rendered
 */
void draw_nothing(UNUSED struct GdObj *nop) {
}

/**
 * Render the `faceGroup` of an `ObjShape`. This is called from
 * draw_shape() and draw_shape_2d(), or when creating the shape
 * `GdDisplayList` when calling create_shape_gddl()
 */
void draw_shape_faces(struct ObjShape *shape) {
    sUpdateViewState.mtlDlNum = 0;
    sUpdateViewState.unreadCounter = 0;
    set_render_alpha(shape->alpha);
    if (shape->dlNums[gGdFrameBufNum] != 0) {
        draw_indexed_dl(shape->dlNums[gGdFrameBufNum], shape->unk50);
    } else if (shape->faceGroup != NULL) {
        func_801A0038();
        draw_group(shape->faceGroup);
        gd_dl_flush_vertices();
    }
}

/**
 * Rendering function for `ObjParticle`.
 */
void draw_particle(struct GdObj *obj) {
    struct ObjParticle *ptc = (struct ObjParticle *) obj;
    struct GdColour *white;
    struct GdColour *black;
    f32 brightness;

    if (ptc->timeout > 0) {
        white = sColourPalette[0];
        black = sWhiteBlack[1];
        brightness = ptc->timeout / 10.0f;
        sLightColours[0].r = (white->r - black->r) * brightness + black->r;
        sLightColours[0].g = (white->g - black->g) * brightness + black->g;
        sLightColours[0].b = (white->b - black->b) * brightness + black->b;
    } else {
        sLightColours[0].r = 0.0f;
        sLightColours[0].g = 0.0f;
        sLightColours[0].b = 0.0f;
    }

    if (ptc->timeout > 0) {
        ptc->shapePtr->unk50 = ptc->timeout;
        draw_shape_2d(ptc->shapePtr, 2, 1.0f, 1.0f, 1.0f, ptc->pos.x, ptc->pos.y, ptc->pos.z, 0.0f,
                      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1, 0);
    }
    if (ptc->unk60 == 3) {
        if (ptc->subParticlesGrp != NULL) {
            draw_group(ptc->subParticlesGrp);
        }
    }
}

/**
 * Rendering fucntion for `ObjBone`.
 *
 * @note This function returns before doing any work. It seems
 *       that Goddard moved away from using bones in the final code
 */
void draw_bone(struct GdObj *obj) {
    struct ObjBone *bone = (struct ObjBone *) obj;
    s32 colour;
    struct GdVec3f scale; // guess

    return;

    //! dead code
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = bone->length / 50.0f;

    if (bone->header.drawFlags & OBJ_HIGHLIGHTED) {
        bone->header.drawFlags &= ~OBJ_HIGHLIGHTED;
        colour = COLOUR_YELLOW;
    } else {
        colour = bone->colourNum;
    }

    if (sSceneProcessType != FIND_PICKS) {
        draw_shape(bone->shapePtr, (SHAPE_FLAG_SCALE | SHAPE_FLAG_TRANSLATE | SHAPE_FLAG_ROTATE | SHAPE_FLAG_10),
                   scale.x, scale.y, scale.z,
                   bone->worldPos.x, bone->worldPos.y, bone->worldPos.z,
                   0.0f, 0.0f, 0.0f,
                   bone->rotation.x, bone->rotation.y, bone->rotation.z,
                   colour,
                   &bone->mat70);
    }
}

/**
 * Rendering function for `ObjJoint`.
 */
void draw_joint(struct GdObj *obj) {
    struct ObjJoint *joint = (struct ObjJoint *) obj;
    s32 colour;
    struct ObjShape *boneShape = joint->shapePtr;

    if (boneShape == NULL) {
        return;
    }

    if (joint->header.drawFlags & OBJ_HIGHLIGHTED) {
        colour = COLOUR_YELLOW;
    } else {
        colour = joint->colourNum;
    }

    draw_shape(boneShape, SHAPE_FLAG_10, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
               colour, &joint->mat128);
}

/**
 * Call `apply_obj_draw_fn()` to all `GdObj` in input `ObjGroup`
 *
 * @param grp `ObjGroup` of objects to draw
 * @return void
 */
void draw_group(struct ObjGroup *grp) {
    if (grp == NULL) {
        fatal_print("Draw_Group: Bad group definition!");
    }

    apply_to_obj_types_in_group(OBJ_TYPE_ALL, (applyproc_t) apply_obj_draw_fn, grp);
}

/**
 * Rendering function for `ObjPlane`.
 */
void draw_plane(struct GdObj *obj) {
    struct ObjPlane *plane = (struct ObjPlane *) obj;

    if (obj->drawFlags & OBJ_HIGHLIGHTED) {
        obj->drawFlags &= ~OBJ_HIGHLIGHTED;
    } else {
        sUseSelectedColor = FALSE;
    }
    draw_face(plane->face);
}

/**
 * Apply `GdObj.objDrawFn` to the input `GdObj` if that object is draw-able.
 *
 * @param obj `GdObj` to draw
 * @return void
 */
void apply_obj_draw_fn(struct GdObj *obj) {
    if (obj == NULL) {
        fatal_print("Bad object!");
    }
    if (obj->drawFlags & OBJ_INVISIBLE) {
        return;
    }

    obj->objDrawFn(obj);
}

/**
 * Count input `ObjLight` as an active light, if it wasn't already counted.
 */
void register_light(struct ObjLight *light) {
    set_light_id(light->id);
    gd_setproperty(GD_PROP_LIGHTING, 2.0f, 0.0f, 0.0f);
    if (light->flags & LIGHT_NEW_UNCOUNTED) {
        sNumActiveLights++;
    }
    light->flags &= ~LIGHT_NEW_UNCOUNTED;
}

/* 229180 -> 229564 */
void update_lighting(struct ObjLight *light) {
    f32 diffuseFac; // diffuse factor?
    f32 facMul;
    f32 diffuseThreshold;

    light->colour.r = light->diffuse.r * light->diffuseFac;
    light->colour.g = light->diffuse.g * light->diffuseFac;
    light->colour.b = light->diffuse.b * light->diffuseFac;
    sLightPositionCache[light->id].x = light->position.x - sLightPositionOffset.x;
    sLightPositionCache[light->id].y = light->position.y - sLightPositionOffset.y;
    sLightPositionCache[light->id].z = light->position.z - sLightPositionOffset.z;
    gd_normalize_vec3f(&sLightPositionCache[light->id]);
    if (light->flags & LIGHT_UNK20) {
        sPhongLightPosition.x = sLightPositionCache[light->id].x;
        sPhongLightPosition.y = sLightPositionCache[light->id].y;
        sPhongLightPosition.z = sLightPositionCache[light->id].z;
        sPhongLight = light;
    }
    diffuseFac = light->diffuseFac;
    if (light->flags & LIGHT_UNK02) {
        facMul = -gd_dot_vec3f(&sLightPositionCache[light->id], &light->direction);
        diffuseThreshold = 1.0f - light->unk38 / 90.0f;
        if (facMul > diffuseThreshold) {
            facMul = (facMul - diffuseThreshold) * (1.0f / (1.0f - diffuseThreshold));
            if (facMul > 1.0f) {
                facMul = 1.0f;
            } else if (facMul < 0.0f) {
                facMul = 0.0f;
            }
        } else {
            facMul = 0.0f;
        }
        diffuseFac *= facMul;
    }
    set_light_id(light->id);
    gd_setproperty(GD_PROP_DIFUSE_COLOUR,
                   light->diffuse.r * diffuseFac,
                   light->diffuse.g * diffuseFac,
                   light->diffuse.b * diffuseFac);
    gd_setproperty(GD_PROP_LIGHT_DIR,
                   sLightPositionCache[light->id].x,
                   sLightPositionCache[light->id].y,
                   sLightPositionCache[light->id].z);
    gd_setproperty(GD_PROP_LIGHTING, 2.0f, 0.0f, 0.0f);
}

/* 229568 -> 229658; orig name: func_8017AD98 */
void update_shaders(struct ObjShape *shape, struct GdVec3f *offset) {
    restart_timer("updateshaders");
    stash_current_gddl();
    sLightPositionOffset.x = offset->x;
    sLightPositionOffset.y = offset->y;
    sLightPositionOffset.z = offset->z;
    sPhongLight = NULL;
    if (gGdLightGroup != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS, (applyproc_t) update_lighting, gGdLightGroup);
    }
    if (shape->mtlGroup != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_MATERIALS, (applyproc_t) apply_obj_draw_fn,
                                    shape->mtlGroup);
    }
    pop_gddl_stash();
    split_timer("updateshaders");
}

/**
 * Create `GdDisplayList`s for any `ObjMaterial`s in `shape` that don't already
 * have a GdDL. Doesn't do anything if `shape`'s `mtlGroup` is NULL
 *
 * @param shape Input `ObjShape` to create material GdDLs for
 * @return void
 */
void create_shape_mtl_gddls(struct ObjShape *shape) {
    if (shape->mtlGroup != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_MATERIALS, (applyproc_t) create_mtl_gddl_if_empty,
                                    shape->mtlGroup);
    }
}

/**
 * Start a new `GdDisplayList` struct and store its reference index
 * in the input `ObjShape`.
 *
 * @param s `ObjShape` to create GdDL for
 * @return Either `-1` if the DL couldn't be created,
 *         or the created DL's reference index
 * @bug Nothing is returned if the DL is created
 * @note Contains string literals that suggest a removed `printf` call
 */
void create_shape_gddl(struct ObjShape *s) {
    struct ObjShape *shape = s; // 24
    s32 shapedl;                // 20
    UNUSED s32 enddl;           // 1C

    create_shape_mtl_gddls(shape);
    shapedl = gd_startdisplist(7);
    if (shapedl == 0) {
        return;
    }

    setup_lights();
    sUseSelectedColor = FALSE;
    if (shape->unk3C == 0) {
        draw_shape_faces(shape);
    }
    enddl = gd_enddlsplist_parent();
    shape->dlNums[0] = shapedl;
    shape->dlNums[1] = shapedl;

    if (shape->name[0] != '\0') {
        printf("Generated '%s' (%d) display list ok.(%d)\n", shape->name, shapedl, enddl);
    } else {
        printf("Generated 'UNKNOWN' (%d) display list ok.(%d)\n", shapedl, enddl);
    }
}

/**
 * Create `GdDisplayList` structs for all `ObjShapes` in `grp` by calling
 * `create_shape_gddl()`.
 *
 * @param grp `ObjGroup` containing `ObjShape` to create GdDLs for
 * @return void
 * @note Contains string literals that suggest a removed `printf` call
 */
void create_gddl_for_shapes(struct ObjGroup *grp) {
    UNUSED s32 shapedls =
        apply_to_obj_types_in_group(OBJ_TYPE_SHAPES, (applyproc_t) create_shape_gddl, grp);
    printf("made %d display lists\n", shapedls);
}

/**
 * Map material id's to `ObjMaterial` pointers for an `ObjGroup` of `ObjFace` structs.
 * This is the final function used in dynlist processing (see `chk_shapegen()`)
 *
 * @param[in,out] faces `ObjGroup` of `ObjFace` structs to map over
 * @param[in]     mtls  `ObjGroup` of `ObjMaterial` structs to map ids to pointers
 * @return void
 */
void map_face_materials(struct ObjGroup *faces, struct ObjGroup *mtls) {
    struct ObjFace *face;
    struct ListNode *linkFaces;
    struct GdObj *temp;
    struct ListNode *linkMtls;
    struct ObjMaterial *mtl;

    linkFaces = faces->firstMember;
    while (linkFaces != NULL) {
        temp = linkFaces->obj;
        face = (struct ObjFace *) temp;
        linkMtls = mtls->firstMember;
        while (linkMtls != NULL) {
            mtl = (struct ObjMaterial *) linkMtls->obj;
            if (mtl->id == face->mtlId) {
                break;
            }
            linkMtls = linkMtls->next;
        }

        if (linkMtls != NULL) {
            face->mtl = mtl;
        }

        linkFaces = linkFaces->next;
    }
}

/**
 * @brief Calculate the normal for @p vtx by averaging the normals of all
 *  `ObjFaces` in @p facegrp
 *
 * Calculate the normal for the input `ObjVetex` @p vtx based on the
 * `ObjFace` structures in @p facegrp of which that vertex is a part.
 *
 * @param vtx     `ObjVertex` to update normal
 * @param facegrp `ObjGroup` of `ObjFace` structures that use @p vtx
 * @return void
 */
static void calc_vtx_normal(struct ObjVertex *vtx, struct ObjGroup *facegrp) {
    s32 i;
    s32 faceCount;
    struct ListNode *node;
    struct ObjFace *curFace;

    vtx->normal.x = vtx->normal.y = vtx->normal.z = 0.0f;

    faceCount = 0;
    node = facegrp->firstMember;
    while (node != NULL) {
        curFace = (struct ObjFace *) node->obj;
        for (i = 0; i < curFace->vtxCount; i++) {
            if (curFace->vertices[i] == vtx) {
                vtx->normal.x += curFace->normal.x;
                vtx->normal.y += curFace->normal.y;
                vtx->normal.z += curFace->normal.z;
                faceCount++;
            }
        }
        node = node->next;
    }
    if (faceCount != 0) {
        vtx->normal.x /= faceCount;
        vtx->normal.y /= faceCount;
        vtx->normal.z /= faceCount;
    }
}

/**
 * @brief Convert vertex indices in an `ObjFace` into pointers and computes the
 * face's normal
 *
 * Using the group of `ObjVertex` or `ObjParticle` structures in @p verts,
 * convert indices in @p face into pointers. The indices are indices
 * into the list contained in @p vertexGrp group
 *
 * @param face  `ObjFace` to find vertices for
 * @param vertexGrp  `ObjGroup` to index in for `ObjVertex` or `ObjPaticle` structures
 * @return void
 */
static void find_thisface_verts(struct ObjFace *face, struct ObjGroup *vertexGrp) {
    s32 i;
    u32 currIndex;
    struct ListNode *node;

    for (i = 0; i < face->vtxCount; i++) {
        // find the vertex or particle whose index in vertexGrp equals face->vertices[i]
        node = vertexGrp->firstMember;
        currIndex = 0;
        while (node != NULL) {
            if (node->obj->type == OBJ_TYPE_VERTICES || node->obj->type == OBJ_TYPE_PARTICLES) {
                if (currIndex++ == (u32) (uintptr_t) face->vertices[i]) {
                    break;
                }
            }
            node = node->next;
        }
        if (node == NULL) {
            fatal_printf("find_thisface_verts(): Vertex not found");
        }

        // set the vertex to point to the resolved `ObjVertex`
        face->vertices[i] = (struct ObjVertex *) node->obj;
    }
    calc_face_normal(face);
}

/**
 * @brief Convert vertex ID numbers for an `ObjGroup` of `ObjFace`s into pointers
 *        to `ObjVertex` structures
 *
 * This function takes an `ObjGroup` of `ObjFace` structures whose `vertices` field
 * has indices and not pointers. These indices are transformed into pointers of
 * `ObjVertex` or `ObjParticle` structures from the @p vtxgrp `ObjGroup`.
 *
 * @param facegrp `ObjGroup` of `ObjFaces` to map vertex indices to pointers
 * @param vtxgrp  `ObjGroup` of `ObjVertices`/`ObjParticles` to be mapped against
 * @return void
 * @note It seems that this function was replaced by `chk_shapegen()`, which performs
 *       a very similar task...
 */
void map_vertices(struct ObjGroup *facegrp, struct ObjGroup *vtxgrp) {
    struct ListNode *faceNode;
    struct ObjFace *curFace;
    struct ListNode *vtxNode;
    struct ObjVertex *vtx;

    imin("map_vertices");

    // resolve vertex indices to actual vertices
    faceNode = facegrp->firstMember;
    while (faceNode != NULL) {
        curFace = (struct ObjFace *) faceNode->obj;
        find_thisface_verts(curFace, vtxgrp);
        faceNode = faceNode->next;
    }

    // compute normals of vertices in vtxgrp
    vtxNode = vtxgrp->firstMember;
    while (vtxNode != NULL) {
        vtx = (struct ObjVertex *) vtxNode->obj;
        calc_vtx_normal(vtx, facegrp);
        vtxNode = vtxNode->next;
    }

    imout();
}

/**
 * Unselect a grabbable objects
 *
 * @param obj `GdObj` to unselect
 * @return void
 * @note Not Called
 */
UNUSED void unpick_obj(struct GdObj *obj) {
    struct GdObj *why = obj;
    if (why->drawFlags & OBJ_IS_GRABBABLE) {
        why->drawFlags &= ~(OBJ_PICKED | OBJ_HIGHLIGHTED);
    }
}

/**
 * @brief Find the closest object to the cursor on an A-button press
 *
 * This function is applied to all objects in an `ObjView.components` group
 * to find the object closest to the cursor when there's an A press
 *
 * @param input `GdObj` to check
 * @return void
 */
void find_closest_pickable_obj(struct GdObj *input) {
    struct GdObj *obj = input;
    f32 distance;

    if (obj->drawFlags & OBJ_IS_GRABBABLE) {
        if (obj->index == sPickDataTemp) {
            if (gViewUpdateCamera != NULL) {
                distance = d_calc_world_dist_btwn(&gViewUpdateCamera->header, obj);
            } else {
                distance = 0.0f;
            }

            if (distance < sPickObjDistance) {
                sPickObjDistance = distance;
                sPickedObject = obj;
            }
        }
    }
}

/**
 * @brief Set the global view camera if not already set.
 *
 * This function is used to find the first `ObjCamera` when running `update_view()`.
 *
 * @param cam `ObjCamera` to set to the update camera, if possible
 * @return void
 */
void set_view_update_camera(struct ObjCamera *cam) {
    if (gViewUpdateCamera != NULL) {
        return;
    }

    gViewUpdateCamera = cam;
}

/**
 * @brief The main per-frame function for handling a view
 *
 * This function handles updating and rendering a given `ObjView` structure.
 * It also handles the A button input for grabbing an area of an `ObjShape`
 * that is contained in the `ObjView.components` group
 *
 * @param view The `ObjView` to update
 * @return void
 */
void update_view(struct ObjView *view) {
    s32 i;
    s32 pickOffset;
    s32 pickDataSize;
    s32 j;
    s32 pickDataIdx;
    s32 pickedObjType;
    char objTypeAbbr[0x100];

    sUpdateViewState.shapesDrawn = 0;
    sUpdateViewState.unused = 0;

    if (!(view->flags & VIEW_UPDATE)) {
        view->flags &= ~VIEW_WAS_UPDATED;
        return;
    }

    imin("UpdateView()");
    if (view->proc != NULL) {
        view->proc(view);
    }

    if (!(view->flags & VIEW_WAS_UPDATED)) {
        view->flags |= VIEW_WAS_UPDATED;
    }

    gViewUpdateCamera = NULL;
    if (view->components != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_CAMERAS, (applyproc_t) set_view_update_camera,
                                    view->components);
        view->activeCam = gViewUpdateCamera;

        if (view->activeCam != NULL) {
            gViewUpdateCamera->view = view;
        }
    }

    if (view->flags & VIEW_MOVEMENT) {
        split_timer("dlgen");
        restart_timer("dynamics");
        proc_view_movement(view);
        split_timer("dynamics");
        restart_timer("dlgen");
        gViewUpdateCamera = view->activeCam;
    }

    if (!(view->flags & VIEW_DRAW)) {
        imout();
        return;
    }

    sUpdateViewState.view = view;
    set_active_view(view);
    view->gdDlNum = gd_startdisplist(8);
    start_view_dl(sUpdateViewState.view);
    gd_shading(9);

    if (view->flags & (VIEW_UNK_2000 | VIEW_UNK_4000)) {
        gd_set_one_cycle();
    }

    if (view->components != NULL) {
        if (gGdCtrl.dragging) {
            if (gd_getproperty(3, 0) && gGdCtrl.startedDragging) { //! gd_getproperty here is always TRUE
                init_pick_buf(sPickBuffer, ARRAY_COUNT(sPickBuffer));
                drawscene(FIND_PICKS, sUpdateViewState.view->components, NULL);
                pickOffset = get_cur_pickbuf_offset(sPickBuffer);
                sPickDataTemp = 0;
                sPickedObject = NULL;
                sPickObjDistance = 10000000.0f;

                if (pickOffset < 0) {
                    fatal_printf("UpdateView(): Pick buffer too small");
                } else if (pickOffset > 0) {
                    pickDataIdx = 0;
                    for (i = 0; i < pickOffset; i++) {
                        pickDataSize = sPickBuffer[pickDataIdx++];
                        if (pickDataSize != 0) {
                            switch ((pickedObjType = sPickBuffer[pickDataIdx++])) {
                                case OBJ_TYPE_JOINTS:
                                    gd_strcpy(objTypeAbbr, "J");
                                    break;
                                case OBJ_TYPE_NETS:
                                    gd_strcpy(objTypeAbbr, "N");
                                    break;
                                case OBJ_TYPE_PARTICLES:
                                    gd_strcpy(objTypeAbbr, "P");
                                    break;
                                default:
                                    gd_strcpy(objTypeAbbr, "?");
                                    break;
                            }
                        }

                        if (pickDataSize >= 2) {
                            for (j = 0; j < pickDataSize - 1; j++) {
                                sPickDataTemp = sPickBuffer[pickDataIdx++];
                                apply_to_obj_types_in_group(pickedObjType,
                                                            (applyproc_t) find_closest_pickable_obj,
                                                            sUpdateViewState.view->components);
                            }
                        }
                    }
                }

                if (sPickedObject != NULL) {
                    sPickedObject->drawFlags |= (OBJ_PICKED | OBJ_HIGHLIGHTED);
                    sUpdateViewState.view->pickedObj = sPickedObject;
                    gGdCtrl.dragStartX = gGdCtrl.csrX = sGrabCords.x;
                    gGdCtrl.dragStartY = gGdCtrl.csrY = sGrabCords.y;
                }
            }

            find_and_drag_picked_object(sUpdateViewState.view->components);
        } else { // check for any previously picked objects, and turn off?
            if (sUpdateViewState.view->pickedObj != NULL) {
                sUpdateViewState.view->pickedObj->drawFlags &= ~(OBJ_PICKED | OBJ_HIGHLIGHTED);
                sUpdateViewState.view->pickedObj = NULL;
            }
        }

        drawscene(RENDER_SCENE, sUpdateViewState.view->components, sUpdateViewState.view->lights);
    }

    border_active_view();
    gd_enddlsplist_parent();
    imout();
}
