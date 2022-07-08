#include <PR/ultratypes.h>

#include "debug_utils.h"
#include "draw_objects.h"
#include "dynlist_proc.h"
#include "gd_macros.h"
#include "gd_main.h"
#include "gd_math.h"
#include "gd_types.h"
#include "joints.h"
#include "macros.h"
#include "objects.h"
#include "renderer.h"
#include "sfx.h"
#include "skin.h"
#include "skin_movement.h"

// data
static struct ObjBone *gGdTempBone = NULL; // @ 801A82D4

static Mat4f *D_801BA964;
static struct GdVec3f D_801BA968;
static s32 sJointCount;                   // @ 801BA974
static s32 sJointNotF1Count;              // @ 801BA978
static s32 sBoneCount;                    // @ 801BA97C
static s32 sJointArrLen;                  // @ 801BA980
static struct ObjJoint *sJointArr[10];    // @ 801BA988
static struct GdVec3f sJointArrVecs[10];  // @ 801BA9B0
static s32 sJointArr2Len;                 // @ 801BAA28
static struct ObjJoint *sJointArr2[10];   // @ 801BAA30
static struct GdVec3f sJointArr2Vecs[10]; // @ 801BAA58
static struct GdVec3f D_801BAAD0;
static struct GdVec3f D_801BAAE0;

// forward declarations
void set_joint_vecs(struct ObjJoint *j, f32 x, f32 y, f32 z);

/**
 * @brief 
 * 
 * @param self 
 * @return * Controls 
 */
void grabbable_joint_update_func(struct ObjJoint *self) {
    Mat4f *attObjMtx;
    struct GdVec3f offset;  // difference between current position and initial position
    struct ListNode *att;
    struct GdObj *attobj;

    // The joint acts somewhat like a spring in that the further it is moved
    // from its original position, the more resistance it has to moving further

    offset.x = self->mat128[3][0] - self->initPos.x;
    offset.y = self->mat128[3][1] - self->initPos.y;
    offset.z = self->mat128[3][2] - self->initPos.z;

    if (self->header.drawFlags & OBJ_PICKED) {
        self->velocity.x = offset.x * -0.25f;
        self->velocity.y = offset.y * -0.25f;
        self->velocity.z = offset.z * -0.25f;

        self->flags |= JOINT_FLAG_2000;
    } else {
        if (!gGdCtrl.trgR) { // R trigger is released
            // Set velocity so that the joint approaches its initial position
            self->velocity.x -= offset.x * 0.5f;
            self->velocity.y -= offset.y * 0.5f;
            self->velocity.z -= offset.z * 0.5f;

            // Decay the velocity
            self->velocity.x *= 0.8f;
            self->velocity.y *= 0.8f;
            self->velocity.z *= 0.8f;

            // If the joint's velocity has decayed enough and it is very close
            // to its original position, stop its movement altogether
            if (ABS(self->velocity.x) + ABS(self->velocity.y) + ABS(self->velocity.z) < 1.0f) {
                if (ABS(offset.x) + ABS(offset.y) + ABS(offset.z) < 1.0f) {
                    self->velocity.x = self->velocity.y = self->velocity.z = 0.0f;
                    self->mat128[3][0] -= offset.x;
                    self->mat128[3][1] -= offset.y;
                    self->mat128[3][2] -= offset.z;
                }
            }

            if (self->flags & JOINT_FLAG_2000) {
                gd_play_sfx(GD_SFX_LET_GO_FACE);
            }

            self->flags &= ~JOINT_FLAG_2000;
        } else {
            // freeze position of joint
            self->velocity.x = self->velocity.y = self->velocity.z = 0.0f;
        }
    }

    // update position
    self->mat128[3][0] += self->velocity.x;
    self->mat128[3][1] += self->velocity.y;
    self->mat128[3][2] += self->velocity.z;

    if (self->header.drawFlags & OBJ_PICKED) {
        gGdCtrl.csrX -= (gGdCtrl.csrX - gGdCtrl.dragStartX) * 0.2f;
        gGdCtrl.csrY -= (gGdCtrl.csrY - gGdCtrl.dragStartY) * 0.2f;
    }

    // update position of attached objects
    offset.x = self->mat128[3][0] - self->initPos.x;
    offset.y = self->mat128[3][1] - self->initPos.y;
    offset.z = self->mat128[3][2] - self->initPos.z;
    for (att = self->attachedObjsGrp->firstMember; att != NULL; att = att->next) {
        attobj = att->obj;
        set_cur_dynobj(attobj);
        attObjMtx = d_get_matrix_ptr();
        gd_add_vec3f_to_mat4f_offset(attObjMtx, &offset);
    }
}

/**
 * Update function for Mario's eye joints, which makes them follow the cursor
 */
void eye_joint_update_func(struct ObjJoint *self) {
    Mat4f *mtx;
    struct GdVec3f sp50;
    struct GdVec3f sp44;
    struct ListNode *att;
    struct GdObj *attobj;

    if (sCurrentMoveCamera == NULL) {
        return;
    }

    if (self->rootAnimator != NULL && self->rootAnimator->state != 7) {
        return;
    }

    set_cur_dynobj((struct GdObj *)self);
    mtx = d_get_rot_mtx_ptr();
    sp44.x = (*mtx)[3][0];
    sp44.y = (*mtx)[3][1];
    sp44.z = (*mtx)[3][2];
    world_pos_to_screen_coords(&sp44, sCurrentMoveCamera, sCurrentMoveView);

    sp50.x = gGdCtrl.csrX - sp44.x;
    sp50.y = -(gGdCtrl.csrY - sp44.y);
    sp50.z = 0.0f;

    sp50.x *= 2.0f;
    sp50.y *= 2.0f;
    sp50.z *= 2.0f;
    if (gd_vec3f_magnitude(&sp50) > 30.0f) {
        gd_normalize_vec3f(&sp50);
        sp50.x *= 30.0f;
        sp50.y *= 30.0f;
        sp50.z *= 30.0f;
    }

    for (att = self->attachedObjsGrp->firstMember; att != NULL; att = att->next) {
        attobj = att->obj;
        set_cur_dynobj(attobj);
        mtx = d_get_rot_mtx_ptr();
        gd_add_vec3f_to_mat4f_offset(mtx, &sp50);
    }
}

/* 23D62C -> 23D748; not called */
UNUSED void func_8018EE5C(struct ObjJoint *j1, struct ObjJoint *j2, struct ObjJoint *j3) {
    struct GdVec3f vec;
    struct ObjJoint *curj;

    if (j3 == NULL) {
        return;
    }

    vec.z = j3->worldPos.x;
    vec.y = j3->worldPos.y;
    vec.x = j3->worldPos.z;

    curj = j1;
    while (curj != NULL) {
        set_joint_vecs(curj, curj->worldPos.x + vec.z, curj->worldPos.y + vec.y, curj->worldPos.z + vec.x);
        if (curj == j2) {
            break;
        }
        curj = curj->prevjoint;
    }
}

/* 23D748 -> 23D818; orig name: func_8018EF78 */
void set_joint_vecs(struct ObjJoint *j, f32 x, f32 y, f32 z) {
    j->worldPos.x = x;
    j->worldPos.y = y;
    j->worldPos.z = z;

    j->unk30.x = x;
    j->unk30.y = y;
    j->unk30.z = z;

    j->unk3C.x = x;
    j->unk3C.y = y;
    j->unk3C.z = z;

    j->initPos.x = x;
    j->initPos.y = y;
    j->initPos.z = z;

    j->mat128[3][0] = x;
    j->mat128[3][1] = y;
    j->mat128[3][2] = z;
}

/* 23D818 -> 23DA18 */
struct ObjJoint *make_joint(s32 flags, f32 x, f32 y, f32 z) {
    struct ObjJoint *j; // sp24
    struct ObjJoint *oldhead;

    j = (struct ObjJoint *) make_object(OBJ_TYPE_JOINTS);
    sJointCount++;
    oldhead = gGdJointList;
    gGdJointList = j;

    if (oldhead != NULL) {
        j->nextjoint = oldhead;
        oldhead->prevjoint = j;
    }
    gd_set_identity_mat4(&j->matE8);
    gd_set_identity_mat4(&j->mat128);
    set_joint_vecs(j, x, y, z);
    j->type = 0;
    j->id = sJointCount;
    j->flags = flags;

    if (!(j->flags & JOINT_FLAG_1)) {
        sJointNotF1Count++;
    }

    if (j->flags & JOINT_FLAG_1) {
        j->colourNum = COLOUR_RED;
    } else {
        j->colourNum = COLOUR_PINK;
    }

    j->boneGrp = NULL;
    j->shapePtr = NULL;
    j->scale.x = 1.0f;
    j->scale.y = 1.0f;
    j->scale.z = 1.0f;
    j->friction.x = 0.0f;
    j->friction.y = 0.0f;
    j->friction.z = 0.0f;
    j->updateFunc = NULL;

    return j;
}

/**
 * Creates a joint that can be grabbed by the cursor. When moved, this joint
 * drags the joints in its unk1F8 group along with it. The `shape` does not
 * actually get rendered due to the joint's OBJ_INVISIBLE flag being set.
 */
struct ObjJoint *make_grabber_joint(struct ObjShape *shape, s32 flags, f32 x, f32 y, f32 z) {
    struct ObjJoint *j;

    j = make_joint(0, x, y, z);
    j->shapePtr = shape;
    j->type = 5;
    j->flags |= flags;
    j->colourNum = COLOUR_PINK;
    j->header.drawFlags |= (OBJ_IS_GRABBABLE | OBJ_INVISIBLE);
    j->updateFunc = grabbable_joint_update_func;
    j->rootAnimator = NULL;

    return j;
}

/* 23DAF8 -> 23DC9C */
void func_8018F328(struct ObjBone *b) {
    struct ObjJoint *joint1;
    struct ObjJoint *joint2;
    struct ObjGroup *grp; // sp1C
    struct ListNode *link;   // sp18

    grp = b->jointGrp;
    link = grp->firstMember;
    joint1 = (struct ObjJoint *) link->obj;
    link = link->next;
    joint2 = (struct ObjJoint *) link->obj;

    // bone position is average of two connecting joints
    b->worldPos.x = (joint1->worldPos.x + joint2->worldPos.x) / 2.0f;
    b->worldPos.y = (joint1->worldPos.y + joint2->worldPos.y) / 2.0f;
    b->worldPos.z = (joint1->worldPos.z + joint2->worldPos.z) / 2.0f;

    b->direction.x = joint2->worldPos.x - joint1->worldPos.x;
    b->direction.y = joint2->worldPos.y - joint1->worldPos.y;
    b->direction.z = joint2->worldPos.z - joint1->worldPos.z;

    gd_normalize_vec3f(&b->direction);
    gd_create_origin_lookat(&b->matB0, &b->direction, 0.0f);
}

/* 23DC9C -> 23DCF0 */
void func_8018F4CC(struct ObjJoint *j) {
    if (j->flags & JOINT_FLAG_1000) {
        j->unkB4.x = D_801BA968.x;
        j->unkB4.y = D_801BA968.y;
        j->unkB4.z = D_801BA968.z;
    }
}

/* 23DCF0 -> 23E06C */
void func_8018F520(struct ObjBone *b) {
    struct ObjJoint *joint1;
    struct ObjJoint *joint2;
    struct GdVec3f sp90;
    struct GdVec3f sp84;
    struct GdVec3f sp78;
    struct GdVec3f sp6C;
    f32 sp68;
    f32 sp64;
    struct ObjGroup *grp; // sp60
    struct ListNode *link;
    Mat4f mtx; // sp1C

    grp = b->jointGrp;
    link = grp->firstMember;
    joint1 = (struct ObjJoint *) link->obj;
    link = link->next;
    joint2 = (struct ObjJoint *) link->obj;

    // bone position is average of two connecting joints
    b->worldPos.x = (joint1->worldPos.x + joint2->worldPos.x) / 2.0f;
    b->worldPos.y = (joint1->worldPos.y + joint2->worldPos.y) / 2.0f;
    b->worldPos.z = (joint1->worldPos.z + joint2->worldPos.z) / 2.0f;

    sp90.x = b->direction.x;
    sp90.y = b->direction.y;
    sp90.z = b->direction.z;

    sp6C.x = sp90.x;
    sp6C.y = sp90.y;
    sp6C.z = sp90.z;

    sp6C.x -= b->unk64.x;
    sp6C.y -= b->unk64.y;
    sp6C.z -= b->unk64.z;
    b->unk64.x = sp90.x;
    b->unk64.y = sp90.y;
    b->unk64.z = sp90.z;

    sp68 = 5.4f / b->length;
    sp6C.x *= sp68;
    sp6C.y *= sp68;
    sp6C.z *= sp68;
    sp90.x *= sp68;
    sp90.y *= sp68;
    sp90.z *= sp68;

    gd_cross_vec3f(&sp90, &sp6C, &sp78);
    sp84.x = sp78.x;
    sp84.y = sp78.y;
    sp84.z = sp78.z;

    gd_normalize_vec3f(&sp84);
    sp64 = gd_vec3f_magnitude(&sp78);
    gd_create_rot_mat_angular(&mtx, &sp84, sp64);
    gd_mult_mat4f(&b->mat70, &mtx, &b->mat70);
    D_801BA968.x = b->mat70[2][0];
    D_801BA968.y = b->mat70[2][1];
    D_801BA968.z = b->mat70[2][2];
    D_801BA964 = &b->mat70;

    apply_to_obj_types_in_group(OBJ_TYPE_JOINTS, (applyproc_t) func_8018F4CC, b->jointGrp);
}

/* 23E06C -> 23E238 */
void func_8018F89C(struct ObjBone *b) {
    struct ObjJoint *spAC;
    struct ObjJoint *spA8;
    struct ObjGroup *grp; // sp60
    struct ListNode *link;   // sp5c
    Mat4f mtx;            // sp1c

    grp = b->jointGrp;
    link = grp->firstMember;
    spAC = (struct ObjJoint *) link->obj;
    link = link->next;
    spA8 = (struct ObjJoint *) link->obj;

    b->worldPos.x = (spAC->worldPos.x + spA8->worldPos.x) / 2.0f;
    b->worldPos.y = (spAC->worldPos.y + spA8->worldPos.y) / 2.0f;
    b->worldPos.z = (spAC->worldPos.z + spA8->worldPos.z) / 2.0f;

    gd_mult_mat4f(&b->matB0, &gGdSkinNet->mat128, &mtx);
    gd_copy_mat4f(&mtx, &b->mat70);

    D_801BA968.x = -b->mat70[2][0];
    D_801BA968.y = -b->mat70[2][1];
    D_801BA968.z = -b->mat70[2][2];
    D_801BA964 = &b->mat70;

    apply_to_obj_types_in_group(OBJ_TYPE_JOINTS, (applyproc_t) func_8018F4CC, b->jointGrp);
}

/* 23E238 -> 23E298 */
void func_8018FA68(struct ObjBone *b) {
    if (b->flags & (BONE_FLAG_8 | BONE_FLAG_2)) {
        func_8018F89C(b);
    } else {
        func_8018F520(b);
    }
}

/* 23E298 -> 23E328; orig name: func_8018FAC8 */
s32 set_skin_weight(struct ObjJoint *j, s32 id, struct ObjVertex *vtx /* always NULL */, f32 weight) {
    struct ObjWeight *w;

    if (j->weightGrp == NULL) {
        j->weightGrp = make_group(0);
    }
    w = make_weight(0, id, vtx, weight);
    addto_group(j->weightGrp, &w->header);

    return TRUE;
}

/* 23E328 -> 23E474 */
void func_8018FB58(struct ObjBone *b) {
    struct GdVec3f vec;  // sp2c
    struct ObjJoint *j1; // sp28
    struct ObjJoint *j2;
    struct ListNode *link;
    struct ObjGroup *grp;

    grp = b->jointGrp;
    link = grp->firstMember;
    j1 = (struct ObjJoint *) link->obj;
    link = link->next;
    j2 = (struct ObjJoint *) link->obj;

    vec.x = j1->worldPos.x - j2->worldPos.x;
    vec.y = j1->worldPos.y - j2->worldPos.y;
    vec.z = j1->worldPos.z - j2->worldPos.z;
    b->length = gd_vec3f_magnitude(&vec);
    // b->length1 = b->length;
    // b->length2 = b->length;
    func_8018F328(b);
}

/* 23E474 -> 23E56C */
void add_joint2bone(struct ObjBone *b, struct ObjJoint *j) {
    if (j->header.type != OBJ_TYPE_JOINTS) {
        fatal_printf("add_joint2bone(): Can only add Joints to Bones");
    }

    if (b->jointGrp == NULL) {
        b->jointGrp = make_group(0);
    }
    addto_group(b->jointGrp, &j->header);

    if (j->boneGrp == NULL) {
        j->boneGrp = make_group(0);
    }
    addto_group(j->boneGrp, &b->header);

    if (b->jointGrp->memberCount == 2) {
        func_8018FB58(b);
    }
}

/* 23E56C -> 23E6E4 */
struct ObjBone *make_bone(s32 flags, struct ObjJoint *j1, struct ObjJoint *j2, UNUSED s32 a3) {
    struct ObjBone *b; // sp34
    struct ObjBone *oldhead;

    b = (struct ObjBone *) make_object(OBJ_TYPE_BONES);
    sBoneCount++;
    b->id = sBoneCount;
    oldhead = gGdBoneList;
    gGdBoneList = b;

    if (oldhead != NULL) {
        b->next = oldhead;
        oldhead->prev = b;
    }
    b->jointGrp = NULL;
    b->colourNum = COLOUR_BLACK;
    b->flags = flags;
    b->shapePtr = NULL;
    gd_set_identity_mat4(&b->mat70);
    b->spring = 0.8f;
    b->unk114 = 0.9f;
    b->length = 100.0f;

    if (j1 != NULL && j2 != NULL) {
        add_joint2bone(b, j1);
        add_joint2bone(b, j2);
    }

    printf("Made bone %d\n", b->id);
    return b;
}

/* 23E6F8 -> 23E758; not called */
UNUSED void func_8018FF28(struct ObjJoint *dst, struct ObjJoint *src) {
    if (src->flags & JOINT_FLAG_1) {
        dst->unk84.x -= src->unk84.x;
        dst->unk84.y -= src->unk84.y;
        dst->unk84.z -= src->unk84.z;
    }
}

/* 23E7B8 -> 23E938 */
s32 func_8018FFE8(struct ObjBone **dstBone, struct ObjJoint **dstJoint, struct ObjJoint *j1, struct ObjJoint *j2) {
    struct ObjBone *currBone; // 1C
    struct ObjJoint *currJoint;
    s32 numBones = 0;
    struct ObjGroup *bonegrp; // 10
    struct ObjGroup *grp;     // 0c
    struct ListNode *bonelink;   // 08
    struct ListNode *link;       // 04

    grp = j2->boneGrp;
    if (grp == NULL) {
        return 0;
    }

    link = grp->firstMember;
    if (link == NULL) {
        return 0;
    }

    while (link != NULL) {
        currBone = (struct ObjBone *) link->obj;

        if (currBone != NULL) {
            bonegrp = currBone->jointGrp;
            bonelink = bonegrp->firstMember;

            while (bonelink != NULL) {
                currJoint = (struct ObjJoint *) bonelink->obj;

                if (currJoint != j2 && currJoint != j1) {
                    dstJoint[numBones] = currJoint;
                    dstBone[numBones] = currBone;
                    numBones++;
                }

                bonelink = bonelink->next;
            }
        }
        link = link->next;
    }

    return numBones;
}

/* 23E938 -> 23EBB8 */
void gd_joints_stub(struct ObjBone *b, UNUSED struct ObjJoint *j1, UNUSED struct ObjJoint *j2,
                   struct GdVec3f *dst) {
    struct GdVec3f vec;
    f32 limit;
    f32 spring;
    f32 mag;

    return;

    //! dead code

    b->direction.x = vec.x;
    b->direction.y = vec.y;
    b->direction.z = vec.z;

    if (b->flags & BONE_FLAG_8) {
        mag = gd_vec3f_magnitude(&vec);
        if (mag == 0.0f) {
            mag = 1.0f;
        }
        limit = (b->length / mag) * b->spring;
    }

    if (b->flags & BONE_FLAG_4) {
        mag = gd_vec3f_magnitude(&vec);
        if (limit > mag) {
            spring = b->spring;
            dst->x *= spring;
            dst->y *= spring;
            dst->z *= spring;
        } else {
            dst->x = 0.0f;
            dst->y = 0.0f;
            dst->z = 0.0f;
        }
    }

    if (b->flags & BONE_FLAG_2) {
        mag = gd_vec3f_magnitude(&vec);
        if (limit < mag) {
            spring = b->spring;
            dst->x *= spring;
            dst->y *= spring;
            dst->z *= spring;
        } else {
            dst->x = 0.0f;
            dst->y = 0.0f;
            dst->z = 0.0f;
        }
    }
}

/* 23EBB8 -> 23ED44 */
void func_801903E8(struct ObjJoint *j, struct GdVec3f *a1, f32 x, f32 y, f32 z) {
    f32 sp14;
    struct GdVec3f sp8;

    if ((j->flags & JOINT_FLAG_1) || !(j->flags & JOINT_FLAG_1000)) {
        j->unk3C.x += x;
        j->unk3C.y += y;
        j->unk3C.z += z;
        a1->x = a1->y = a1->z = 0.0f;
    } else {
        sp14 = (j->unkB4.x * x) + (j->unkB4.y * y) + (j->unkB4.z * z); // dot
        sp8.x = j->unkB4.x * sp14;
        sp8.y = j->unkB4.y * sp14;
        sp8.z = j->unkB4.z * sp14;

        j->unk3C.x += sp8.x;
        j->unk3C.y += sp8.y;
        j->unk3C.z += sp8.z;

        a1->x = x - sp8.x;
        a1->y = y - sp8.y;
        a1->z = z - sp8.z;
    }
}

/* 23EBB8 -> 23F184 */
void func_80190574(s32 numNestings, struct ObjJoint *j1, struct ObjJoint *j2, f32 x, f32 y, f32 z) { // sp278
    struct ObjJoint *j2p;
    struct ObjJoint *midJoint; // mid-point of stack array?
    struct ObjJoint *currJoint; // jointstackarr[i]? curjoint?
    struct GdVec3f sp24C = { 0.0f, 0.0f, 0.0f };
    struct GdVec3f sp240;
    s32 boneIndex;
    s32 i;
    s32 flag = 1;
    s32 numBones;
    s32 remainingBones;
    struct ObjJoint *joints[0x40];
    struct ObjBone *bones[0x40];

    for (i = 1; i < numNestings; i *= 2) {
        flag = (flag * 2) + 1;
    }

    if (numNestings & 0x8000) {
        fatal_print("Too many nestings!\n");
    }

    printf("\n");
    printf("NIDmask: %d /  ", numNestings);

    j2->unk1C0 |= numNestings;
    numBones = func_8018FFE8(bones, joints, j1, j2); // get_num_bones?
    func_801903E8(j2, &sp240, x, y, z);
    for (boneIndex = 0; boneIndex < numBones; boneIndex++) {
        if (j1 != NULL) {
            printf("branch %d from j%d-j%d(%d): ", boneIndex, j2->id, j1->id, numBones);
        } else {
            printf("branch %d from j%d(%d): ", boneIndex, j2->id, numBones);
        }

        j2p = j2;
        currJoint = joints[boneIndex];
        gd_joints_stub(bones[boneIndex], j2p, currJoint, &sp24C);
        do {
            remainingBones = func_8018FFE8(&bones[0x20], &joints[0x20], j2p, currJoint);
            midJoint = joints[0x20];
            if (currJoint->unk1C0 & flag) {
                break;
            }

            if (remainingBones < 2) {
                if (currJoint->flags & JOINT_FLAG_1) {
                    sJointArrLen++;
                    sJointArr[sJointArrLen] = j2p;
                    sJointArrVecs[sJointArrLen].x = -sp24C.x;
                    sJointArrVecs[sJointArrLen].y = -sp24C.y;
                    sJointArrVecs[sJointArrLen].z = -sp24C.z;

                    currJoint->unk90.x += sp24C.x;
                    currJoint->unk90.y += sp24C.y;
                    currJoint->unk90.z += sp24C.z;

                    currJoint->unk90.x += sp240.x;
                    currJoint->unk90.y += sp240.y;
                    currJoint->unk90.z += sp240.z;

                    sp240.x = sp240.y = sp240.z = 0.0f;
                    break;
                } else {
                    sp24C.x += sp240.x;
                    sp24C.y += sp240.y;
                    sp24C.z += sp240.z;

                    func_801903E8(currJoint, &sp240, sp24C.x, sp24C.y, sp24C.z);
                }

                if (remainingBones == 1) {
                    gd_joints_stub(bones[0x20], currJoint, midJoint, &sp24C);
                }
            }

            if (remainingBones > 1) {
                func_80190574(numNestings * 2, j2p, currJoint, sp24C.x, sp24C.y, sp24C.z); // recursion
                break;
            }

            j2p = currJoint;
            currJoint = midJoint;
        } while (remainingBones);
        printf("Exit");
        // probably j2p(currJoint) because it would make sense to print
        // the iterations of both of these loops.
        printf(" %d(%d)", j2p->id, currJoint->id);
        printf("R ");
        printf("\n");
    }

    printf("\n\n");
}

/* 23F184 -> 23F1F0 */
void func_801909B4(void) {
    struct ObjJoint *node = gGdJointList;

    while (node != NULL) {
        node->unk1C0 = 0;
        node = node->nextjoint;
    }
}

/* 23F1F0 -> 23F324; not called */
UNUSED void func_80190A20(void) {
    struct ObjJoint *j; // sp3c
    struct GdVec3f vec; // sp2C
    struct ObjGroup *grp;
    struct ListNode *link;
    struct ObjBone *b;

    j = gGdJointList;
    while (j != NULL) {
        if (j->flags & JOINT_FLAG_40) {
            grp = j->boneGrp;
            link = grp->firstMember;
            b = (struct ObjBone *) link->obj;

            vec.z = b->unk40.x * 100.0f;
            vec.y = b->unk40.y * 100.0f;
            vec.x = b->unk40.z * 100.0f;
            func_80190574(1, NULL, j, vec.z, vec.y, vec.x);
        }

        j = j->nextjoint;
    }
}

/* 23F324 -> 23F638 */
void func_80190B54(struct ObjJoint *j0, struct ObjJoint *j1, struct GdVec3f *j2) { // b0
    struct GdVec3f spA4;
    struct GdVec3f sp8C;
    struct GdVec3f sp80;
    f32 mag;
    f32 invMag;
    Mat4f mtx;

    if (j1 != NULL) {
        spA4.x = j1->unk3C.x;
        spA4.y = j1->unk3C.y;
        spA4.z = j1->unk3C.z;

        spA4.x -= j0->unk3C.x;
        spA4.y -= j0->unk3C.y;
        spA4.z -= j0->unk3C.z;

        sp8C.x = spA4.x;
        sp8C.y = spA4.y;
        sp8C.z = spA4.z;
        gd_normalize_vec3f(&sp8C);

        mag = j1->unk228; // unk228 is never initialized, so mag is 0

        D_801BAAE0.x = spA4.x - (sp8C.x * mag);
        D_801BAAE0.y = spA4.y - (sp8C.y * mag);
        D_801BAAE0.z = spA4.z - (sp8C.z * mag);

        invMag = 5.4f / mag;
        D_801BAAD0.x *= invMag;
        D_801BAAD0.y *= invMag;
        D_801BAAD0.z *= invMag;

        spA4.x *= invMag;
        spA4.y *= invMag;
        spA4.z *= invMag;

        gd_cross_vec3f(&spA4, &D_801BAAD0, &sp80);
        invMag = gd_vec3f_magnitude(&sp80);
        gd_normalize_vec3f(&sp80);
        gd_create_rot_mat_angular(&mtx, &sp80, invMag);
        gd_mult_mat4f(&j0->matE8, &mtx, &j0->matE8);

    } else {
        D_801BAAE0.x = j2->x;
        D_801BAAE0.y = j2->y;
        D_801BAAE0.z = j2->z;
    }

    j0->unk3C.x += D_801BAAE0.x;
    j0->unk3C.y += D_801BAAE0.y;
    j0->unk3C.z += D_801BAAE0.z;

    D_801BAAD0.x = D_801BAAE0.x;
    D_801BAAD0.y = D_801BAAE0.y;
    D_801BAAD0.z = D_801BAAE0.z;
}

/* 23F638 -> 23F70C; not called */
UNUSED void func_80190E68(struct GdObj *obj, f32 x, f32 y, f32 z) {
    struct ObjJoint *j = NULL;
    struct GdVec3f vec;

    vec.x = x;
    vec.y = y;
    vec.z = z;

    while (obj != NULL) {
        if (obj->type != OBJ_TYPE_JOINTS) {
            break;
        }

        func_80190B54(((struct ObjJoint *) obj), j, &vec);
        j = ((struct ObjJoint *) obj);
        obj = ((struct ObjJoint *) obj)->attachedToObj;
    }
}

/* 23F70C -> 23F978 */
f32 func_80190F3C(struct ObjJoint *j, f32 x, f32 y, f32 z) {
    struct ObjJoint *curj;
    s32 i;
    struct GdVec3f vec;

    vec.x = j->unk3C.x;
    vec.y = j->unk3C.y;
    vec.z = j->unk3C.z;

    func_801909B4();
    sJointArrLen = 0;
    func_80190574(1, NULL, j, x, y, z);

    for (i = 1; i <= sJointArrLen; i++) {
        sJointArr2[i] = sJointArr[i];
        sJointArr2Vecs[i].x = sJointArrVecs[i].x;
        sJointArr2Vecs[i].y = sJointArrVecs[i].y;
        sJointArr2Vecs[i].z = sJointArrVecs[i].z;
    }
    printf("Num return joints (pass 1): %d\n", i);

    sJointArr2Len = sJointArrLen;
    sJointArrLen = 0;

    for (i = 1; i <= sJointArr2Len; i++) {
        func_801909B4();
        curj = sJointArr2[i];
        func_80190574(1, NULL, curj, sJointArr2Vecs[i].x, sJointArr2Vecs[i].y, sJointArr2Vecs[i].z);
    }
    printf("Num return joints (pass 2): %d\n", i);

    vec.x -= j->unk3C.x;
    vec.y -= j->unk3C.y;
    vec.z -= j->unk3C.z;

    return gd_vec3f_magnitude(&vec);
}

/* 23F978 -> 23F9F0 */
void func_801911A8(struct ObjJoint *j) {
    j->unkCC.x = j->shapeOffset.x;
    j->unkCC.y = j->shapeOffset.y;
    j->unkCC.z = j->shapeOffset.z;

    gd_rotate_and_translate_vec3f(&j->unkCC, &gGdSkinNet->mat128);
}

/* 23F9F0 -> 23FB90 */
void func_80191220(struct ObjJoint *j) {
    j->unk48.x = j->initPos.x; // storing "attached offset"?
    j->unk48.y = j->initPos.y;
    j->unk48.z = j->initPos.z;

    gd_mat4f_mult_vec3f(&j->unk48, &gGdSkinNet->mat128);
    j->unk3C.x = j->unk48.x;
    j->unk3C.y = j->unk48.y;
    j->unk3C.z = j->unk48.z;
    j->worldPos.x = gGdSkinNet->worldPos.x;
    j->worldPos.y = gGdSkinNet->worldPos.y;
    j->worldPos.z = gGdSkinNet->worldPos.z;

    j->worldPos.x += j->unk3C.x;
    j->worldPos.y += j->unk3C.y;
    j->worldPos.z += j->unk3C.z;
    j->unk1A8.x = j->unk1A8.y = j->unk1A8.z = 0.0f;
    gGdCounter.ctr0++;
}

/* 23FBC0 -> 23FCC8 */
void func_801913F0(struct ObjJoint *j) {
    // hmm...
    j->velocity.x = j->worldPos.x;
    j->velocity.y = j->worldPos.y;
    j->velocity.z = j->worldPos.z;

    j->velocity.x -= j->unk30.x;
    j->velocity.y -= j->unk30.y;
    j->velocity.z -= j->unk30.z;

    j->unk30.x = j->worldPos.x;
    j->unk30.y = j->worldPos.y;
    j->unk30.z = j->worldPos.z;

    gd_copy_mat4f(&gGdSkinNet->mat128, &j->matE8);
}

/* 23FCDC -> 23FDD4; not called */
UNUSED void func_8019150C(Mat4f *mtx, struct GdVec3f *vec) {
    struct GdVec3f translation;

    translation.x = (*mtx)[3][0] / 10.0f;
    translation.y = (*mtx)[3][1] / 10.0f;
    translation.z = (*mtx)[3][2] / 10.0f;

    vec->x += translation.x;
    vec->y += translation.y;
    vec->z += translation.z;
    gd_mat4f_mult_vec3f(vec, mtx);
}

/* 23FDD4 -> 23FFF4 */
void reset_joint(struct ObjJoint *j) {
    j->worldPos.x = j->initPos.x;
    j->worldPos.y = j->initPos.y;
    j->worldPos.z = j->initPos.z;

    j->unk30.x = j->initPos.x;
    j->unk30.y = j->initPos.y;
    j->unk30.z = j->initPos.z;

    j->unk3C.x = j->initPos.x;
    j->unk3C.y = j->initPos.y;
    j->unk3C.z = j->initPos.z;

    j->velocity.x = j->velocity.y = j->velocity.z = 0.0f;
    j->unk84.x = j->unk84.y = j->unk84.z = 0.0f;
    j->unk90.x = j->unk90.y = j->unk90.z = 0.0f;
    j->unk1A8.x = j->unk1A8.y = j->unk1A8.z = 0.0f;

    gd_set_identity_mat4(&j->mat168);
    gd_scale_mat4f_by_vec3f(&j->mat168, (struct GdVec3f *) &j->scale);
    gd_rot_mat_about_vec(&j->mat168, (struct GdVec3f *) &j->unk6C);
    gd_add_vec3f_to_mat4f_offset(&j->mat168, &j->attachOffset);
    gd_copy_mat4f(&j->mat168, &j->matE8);

    gd_set_identity_mat4(&j->mat128);
    gd_add_vec3f_to_mat4f_offset(&j->mat128, &j->initPos);
}

/* 23FFF4 -> 2400C4 */
void func_80191824(struct ObjJoint *j) {
    UNUSED struct ObjNet *sp14;

    sp14 = gGdSkinNet->unk1F0;
    if (j->flags & JOINT_FLAG_1) {
        j->worldPos.x = gGdSkinNet->worldPos.x;
        j->worldPos.y = gGdSkinNet->worldPos.y;
        j->worldPos.z = gGdSkinNet->worldPos.z;

        j->unk3C.x = gGdSkinNet->worldPos.x;
        j->unk3C.y = gGdSkinNet->worldPos.y;
        j->unk3C.z = gGdSkinNet->worldPos.z;
    }
}

/* 2400C4 -> 2401EC; not called */
UNUSED void func_801918F4(struct ObjJoint *j) {
    j->velocity.x = j->unk3C.x;
    j->velocity.y = j->unk3C.y;
    j->velocity.z = j->unk3C.z;

    j->velocity.x -= j->unk30.x;
    j->velocity.y -= j->unk30.y;
    j->velocity.z -= j->unk30.z;

    j->unk30.x = j->unk3C.x;
    j->unk30.y = j->unk3C.y;
    j->unk30.z = j->unk3C.z;

    if (!(j->flags & (JOINT_FLAG_1 | JOINT_FLAG_40))) {
        j->velocity.y -= 0.8f;

        j->unk3C.x += j->velocity.x;
        j->unk3C.y += j->velocity.y;
        j->unk3C.z += j->velocity.z;
    }
}

/* 2401EC -> 2403C8; not called */
UNUSED void func_80191A1C(struct ObjBone *b) {
    f32 sp3C;
    f32 sp38 = 0.0f;
    struct GdObj *argjoint;
    struct GdObj *tempjoint;
    struct GdVec3f sp24;
    struct GdVec3f sp18;

    if (gGdTempBone == NULL) {
        gGdTempBone = b;
    }
    sp3C = gd_dot_vec3f(&gGdTempBone->unk40, &b->unk40);
    b->unk118 = sp3C;

    sp3C -= sp38;

    if (sp3C < 0.0f) {
        tempjoint = gGdTempBone->jointGrp->firstMember->obj;
        argjoint = b->jointGrp->firstMember->next->obj;
        set_cur_dynobj(argjoint);
        d_get_rel_pos(&sp24);
        set_cur_dynobj(tempjoint);
        d_get_rel_pos(&sp18);

        sp24.x -= sp18.x;
        sp24.y -= sp18.y;
        sp24.z -= sp18.z;
        gd_normalize_vec3f(&sp24);

        sp3C *= -50.0f;
        if (!(((struct ObjJoint *) argjoint)->flags & JOINT_FLAG_1)) {
            func_80190F3C((struct ObjJoint *) argjoint, (sp24.x * sp3C), (sp24.y * sp3C), (sp24.z * sp3C));
        }
    }
    gGdTempBone = b;
}

/* 2403C8 -> 240530 */
void func_80191BF8(struct ObjJoint *j) {
    f32 sp1C;
    f32 sp18 = -2.0f;

    if (!(j->flags & JOINT_FLAG_1)) {
        j->unk3C.y += sp18;
    }

    sp1C = j->unk3C.y - (D_801A8058 + 30.0f);

    if (sp1C < 0.0f && j->velocity.y < 0.0f) {
        sp1C += j->velocity.y;
        sp1C *= 0.8f;
        func_80190F3C(j, -j->velocity.x * 0.7f, -sp1C, -j->velocity.z * 0.7f);
    }

    func_80190F3C(j, 0.0f, 0.0f, 0.0f);
}

/* 240530 -> 240624 */
void func_80191D60(struct ObjJoint *j) {
    j->velocity.x += j->unk3C.x - j->worldPos.x;
    j->velocity.y += j->unk3C.y - j->worldPos.y;
    j->velocity.z += j->unk3C.z - j->worldPos.z;

    j->velocity.x *= 0.9f;
    j->velocity.y *= 0.9f;
    j->velocity.z *= 0.9f;

    j->worldPos.x += j->velocity.x;
    j->worldPos.y += j->velocity.y;
    j->worldPos.z += j->velocity.z;
}

/* 240624 -> 240658 */
void func_80191E54(struct ObjJoint *j) {
    j->unk3C.x = j->worldPos.x;
    j->unk3C.y = j->worldPos.y;
    j->unk3C.z = j->worldPos.z;
}

/* 240658 -> 2406B8 */
void func_80191E88(struct ObjGroup *grp) {
    apply_to_obj_types_in_group(OBJ_TYPE_JOINTS, (applyproc_t) func_80191BF8, grp);
    apply_to_obj_types_in_group(OBJ_TYPE_JOINTS, (applyproc_t) func_80191D60, grp);
    apply_to_obj_types_in_group(OBJ_TYPE_JOINTS, (applyproc_t) func_80191E54, grp);
}

/* 2406B8 -> 2406E0; orig name: func_80191EE8 */
void reset_joint_counts(void) {
    sJointCount = 0;
    sJointNotF1Count = 0;
    sBoneCount = 0;
}
