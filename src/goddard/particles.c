#include <PR/ultratypes.h>

#include "debug_utils.h"
#include "draw_objects.h"
#include "dynlist_proc.h"
#include "gd_macros.h"
#include "gd_math.h"
#include "gd_types.h"
#include "macros.h"
#include "objects.h"
#include "particles.h"
#include "renderer.h"
#include "skin.h"

// static types
typedef union {
    struct ObjVertex *vtx;
    struct ObjParticle *ptc;
} VtxPtc;

struct Connection {
    struct GdObj header;  // this header is never used
    u8 filler[8];
    VtxPtc node1;  // first connected vertex or particle
    VtxPtc node2;  // second connected vertex or particle
    f32 dist;
    u32 unk28; // union tag? 0 = vertex; 1 = particle?
};

// data
static s32 D_801A81D4[25] = {
    /*  ID?    X    Y    Z */
         9,    3,  12, -14,
        25,    5,  16, -25,
        42,    4,  15, -39,
        55,   -6,  20, -23,
        70,   -2,  20, -23,
        135,   0,  16,   0,
        0 /* Terminator */
};
static s32 D_801A8238[5] = {
    /*  ID?    X    Y    Z */
        15,    0,  22,   0,
        0 /* Terminator */
};

// static bss
static struct ObjFace *D_801B9EF0;

// fn declarations
struct Connection *make_connection(struct ObjVertex *vtx1, struct ObjVertex *vtx2);
void func_80181EB0(struct Connection *cxn);
void func_80182088(struct Connection *cxn);
void move_particle(struct ObjParticle *ptc);
s32 func_80182778(struct ObjParticle *ptc);
void func_80182A08(struct ObjParticle *ptc, struct GdVec3f *b);

static void connect_vertices(struct ObjVertex *vtx1, struct ObjVertex *vtx2) {
    struct Connection *newConn;
    struct ListNode *link;

    if (vtx1 == vtx2) {
        return;
    }
    link = gGdSkinNet->unk1C0->firstMember;
    while (link != NULL) {
        // FIXME: types
        struct Connection *conn = (struct Connection *) link->obj;

        if ((conn->node1.vtx == vtx1 || conn->node1.vtx == vtx2)
         && (conn->node2.vtx == vtx1 || conn->node2.vtx == vtx2)) {
            break;
        }
        link = link->next;
    }
    if (link == NULL) {
        newConn = make_connection(vtx1, vtx2);
        //! make_connection never sets the header, so not sure what happens here
        addto_group(gGdSkinNet->unk1C0, &newConn->header);
    }
}

/* 2304E4 -> 230680 */
void Unknown80181D14(struct ObjFace *face) {
    s32 i, j;
    struct ObjVertex *vtx1;
    struct ObjVertex *vtx2;

    for (i = 0; i < face->vtxCount - 1; i++) {
        vtx1 = face->vertices[i];
        vtx2 = face->vertices[i + 1];
        connect_vertices(vtx1, vtx2);
    }
    if (D_801B9EF0 != NULL) {
        for (i = 0; i < face->vtxCount; i++) {
            vtx1 = face->vertices[i];
            for (j = 0; j < D_801B9EF0->vtxCount; j++) {
                vtx2 = D_801B9EF0->vertices[j];
                connect_vertices(vtx1, vtx2);
            }
        }
    }
    D_801B9EF0 = face;
}

/* 230680 -> 230858 */
void func_80181EB0(struct Connection *cxn) {
    struct GdVec3f vec;
    struct ObjParticle *ptc1 = cxn->node1.ptc;
    struct ObjParticle *ptc2 = cxn->node2.ptc;

    vec.x = 0.0f;
    vec.y = ptc1->pos.y - ptc2->pos.y;
    vec.z = 0.0f;
    vec.y *= 0.01f;
    ptc1->unk38.x -= vec.x;
    ptc1->unk38.y -= vec.y;
    ptc1->unk38.z -= vec.z;
    ptc2->unk38.x += vec.x;
    ptc2->unk38.y += vec.y;
    ptc2->unk38.z += vec.z;
    if (!(ptc1->flags & PTC_FLAG_2)) {
        ptc1->pos.x -= vec.x;
        ptc1->pos.y -= vec.y;
        ptc1->pos.z -= vec.z;
    }
    if (!(ptc2->flags & PTC_FLAG_2)) {
        ptc2->pos.x += vec.x;
        ptc2->pos.y += vec.y;
        ptc2->pos.z += vec.z;
    }
}

/* @ 230858 -> 230B70 */
void func_80182088(struct Connection *cxn) {
    struct GdVec3f vec;
    f32 diff;
    f32 dist;
    struct ObjParticle *ptc1;
    struct ObjParticle *ptc2;

    if (cxn->unk28 & 0x1) {
        func_80181EB0(cxn);
        return;
    }
    ptc1 = cxn->node1.ptc;
    ptc2 = cxn->node2.ptc;
    vec.x = ptc1->pos.x - ptc2->pos.x;
    vec.y = ptc1->pos.y - ptc2->pos.y;
    vec.z = ptc1->pos.z - ptc2->pos.z;
    dist = gd_vec3f_magnitude(&vec);
    diff = dist - cxn->dist;
    vec.x /= dist;
    vec.y /= dist;
    vec.z /= dist;
    vec.x *= diff * 0.1f;
    vec.y *= diff * 0.1f;
    vec.z *= diff * 0.1f;
    ptc1->unk38.x -= vec.x;
    ptc1->unk38.y -= vec.y;
    ptc1->unk38.z -= vec.z;
    ptc2->unk38.x += vec.x;
    ptc2->unk38.y += vec.y;
    ptc2->unk38.z += vec.z;
    if (!(ptc1->flags & PTC_FLAG_2)) {
        ptc1->pos.x -= vec.x;
        ptc1->pos.y -= vec.y;
        ptc1->pos.z -= vec.z;
    }
    if (!(ptc2->flags & PTC_FLAG_2)) {
        ptc2->pos.x += vec.x;
        ptc2->pos.y += vec.y;
        ptc2->pos.z += vec.z;
    }
}

/* 230B70 -> 230CC0 */
void func_801823A0(struct ObjNet *net) {
    struct ListNode *link;
    struct Connection *cxn;

    gGdSkinNet = net;
    switch (net->unk3C) {
        case 1: // Shape; Are these flags the same as net->netType (+0x1EC)?
            net->unk1C8 = net->shapePtr->vtxGroup;
            net->unk1C0 = make_group(0);
            D_801B9EF0 = NULL;

            apply_to_obj_types_in_group(OBJ_TYPE_FACES, (applyproc_t) Unknown80181D14,
                                        net->shapePtr->faceGroup);
            net->unk3C = 2;
            break;
        case 2:
            link = net->unk1C0->firstMember;
            while (link != NULL) {
                // FIXME: types
                cxn = (struct Connection *) link->obj;
                func_80182088(cxn);
                link = link->next;
            }
            apply_to_obj_types_in_group(OBJ_TYPE_PARTICLES, (applyproc_t) move_particle, net->unk1C8);
            apply_to_obj_types_in_group(OBJ_TYPE_PLANES,    (applyproc_t) reset_plane,   net->unk1CC);
            break;
    }
}

/* 230CC0 -> 230DCC */
struct ObjParticle *make_particle(u32 flags, s32 colourNum, f32 x, f32 y, f32 z) {
    struct ObjParticle *particle = (struct ObjParticle *) make_object(OBJ_TYPE_PARTICLES);

    particle->pos.x = x;
    particle->pos.y = y;
    particle->pos.z = z;
    particle->unk38.x = particle->unk38.y = particle->unk38.z = 0.0f;
    particle->colourNum = colourNum;
    particle->flags = (flags | PTC_FLAG_8);
    particle->timeout = -1;
    particle->id = sParticleId; /* should this be sParticleId++? */
    particle->shapePtr = NULL;
    particle->unkB0 = 1;
    return particle;
}

/* 230DCC -> 230F48 */
struct Connection *make_connection(struct ObjVertex *vtx1, struct ObjVertex *vtx2) {
    struct Connection *conn = gd_malloc_perm(sizeof(struct Connection));
    struct GdVec3f vtx1Pos;
    struct GdVec3f vtx2Pos;

    if (conn == NULL) {
        fatal_print("Cant allocate connection memory!");
    }
    conn->node1.vtx = vtx1;
    conn->node2.vtx = vtx2;
    d_stash_dynobj();
    set_cur_dynobj((struct GdObj *)vtx1);
    d_get_world_pos(&vtx1Pos);
    set_cur_dynobj((struct GdObj *)vtx2);
    d_get_world_pos(&vtx2Pos);
    vtx1Pos.x -= vtx2Pos.x;
    vtx1Pos.y -= vtx2Pos.y;
    vtx1Pos.z -= vtx2Pos.z;
    conn->dist = gd_vec3f_magnitude(&vtx1Pos);
    // Duplicate conditional. Possibly should've checked `vtx2`;
    // Also, this shouldn't be called with particle types...
    if (vtx1->header.type == OBJ_TYPE_PARTICLES
     && vtx2->header.type == OBJ_TYPE_PARTICLES) {
        if ((((struct ObjParticle *) vtx1)->flags & PTC_FLAG_4)
         && (((struct ObjParticle *) vtx2)->flags & PTC_FLAG_4)) {
            conn->unk28 |= 0x1;
        }
    }
    d_unstash_dynobj();
    return conn;
}

/* 230F48 -> 2311D8 */
s32 func_80182778(struct ObjParticle *ptc) {
    s32 i = 0;

    if (ptc->unk7C->animSeqNum == 2 && ptc->unk74 == 1) {
        while (D_801A81D4[i] != 0) {
            if (D_801A81D4[i] == ptc->unk7C->frame) {
                ptc->pos.x = D_801A81D4[i + 1] * 10.0f;
                ptc->pos.y = D_801A81D4[i + 2] * 10.0f;
                ptc->pos.z = D_801A81D4[i + 3] * 10.0f;
                return TRUE;
            }
            i += 4;
        }
    }
    if (ptc->unk7C->animSeqNum == 1 && ptc->unk74 == 1) {
        while (D_801A8238[i] != 0) {
            if (D_801A8238[i] == ptc->unk7C->frame) {
                ptc->pos.x = D_801A8238[i + 1] * 10.0f;
                ptc->pos.y = D_801A8238[i + 2] * 10.0f;
                ptc->pos.z = D_801A8238[i + 3] * 10.0f;
                return TRUE;
            }
            i += 4;
        }
    }
    return FALSE;
}

/* 2311D8 -> 231454 */
void func_80182A08(struct ObjParticle *ptc, struct GdVec3f *b) {
    struct ListNode *link;
    struct ObjParticle *currPtc;

    if (ptc->subParticlesGrp != NULL) {
        link = ptc->subParticlesGrp->firstMember;
        while (link != NULL) {
            // FIXME: types
            currPtc = (struct ObjParticle *) link->obj;
            if (currPtc->timeout <= 0) {
                currPtc->pos.x = ptc->pos.x;
                currPtc->pos.y = ptc->pos.y;
                currPtc->pos.z = ptc->pos.z;
                currPtc->timeout = 12.0f - gd_rand_float() * 5.0f;
                do {
                    currPtc->unk38.x = gd_rand_float() * 50.0f - 25.0f;
                    currPtc->unk38.y = gd_rand_float() * 50.0f - 25.0f;
                    currPtc->unk38.z = gd_rand_float() * 50.0f - 25.0f;
                } while (gd_vec3f_magnitude(&currPtc->unk38) > 30.0f);
                currPtc->unk38.x += b->x;
                currPtc->unk38.y += b->y;
                currPtc->unk38.z += b->z;
                currPtc->header.drawFlags &= ~OBJ_INVISIBLE;
                currPtc->flags |= PTC_FLAG_8;
            }
            link = link->next;
        }
    }
}

/* 231454 -> 231D40; orig name: Unknown80182C84 */
void move_particle(struct ObjParticle *ptc) {
    struct GdVec3f sp64;
    struct ObjParticle *currPtc;
    s32 i;
    struct ObjCamera *camera;
    struct GdVec3f sp40;
    struct GdVec3f sp34;

    if (ptc->flags & PTC_FLAG_2) {
        return;
    }
    if (!(ptc->flags & PTC_FLAG_8)) {
        return;
    }
    if (ptc->unk60 == 3) {
        sp40.x = -gViewUpdateCamera->unkE8[2][0] *  50.0f;
        sp40.y = -gViewUpdateCamera->unkE8[2][1] *  50.0f;
        sp40.z =  gViewUpdateCamera->unkE8[2][2] *  50.0f;
        sp34.x =  gViewUpdateCamera->unkE8[2][0] * -20.0f;
        sp34.y =  gViewUpdateCamera->unkE8[2][1] * -20.0f;
        sp34.z =  gViewUpdateCamera->unkE8[2][2] * -20.0f;
    }
    if (ptc->attachedToObj != NULL) {
        set_cur_dynobj(ptc->attachedToObj);
        if (ptc->unk60 == 3 && ptc->unk64 == 3) {
            camera = (struct ObjCamera *) ptc->attachedToObj;
            // Camera->view = ObjView here
            if (camera->view->pickedObj != NULL) {
                set_cur_dynobj(camera->view->pickedObj);
                ptc->flags |= PTC_FLAG_20;
            } else {
                ptc->flags &= ~(PTC_FLAG_10 | PTC_FLAG_20);
            }
        }
        d_get_world_pos(&sp64);
        ptc->pos.x = sp64.x;
        ptc->pos.y = sp64.y;
        ptc->pos.z = sp64.z;
    }
    ptc->pos.x += ptc->unk38.x;
    ptc->pos.y += ptc->unk38.y;
    ptc->pos.z += ptc->unk38.z;
    if (ptc->flags & PTC_FLAG_1) {
        ptc->unk38.y -= 0.4f;
    }
    switch (ptc->unkB0) {
        case 1:
            ptc->unkB0 = 2;
            if (ptc->unk60 == 3) {
                switch (ptc->unk64) {
                    case 1:
                        ptc->subParticlesGrp = make_group(0);
                        for (i = 0; i < 50; i++) {
                            currPtc = make_particle(PTC_FLAG_1, -1, ptc->pos.x, ptc->pos.y, ptc->pos.z);
                            currPtc->shapePtr = ptc->shapePtr;
                            addto_group(ptc->subParticlesGrp, &currPtc->header);
                            currPtc->flags &= ~PTC_FLAG_8;
                        }
                        break;
                    case 2:
                    case 3:
                        ptc->subParticlesGrp = make_group(0);
                        for (i = 0; i < 30; i++) {
                            currPtc = make_particle(PTC_FLAG_1, -1, ptc->pos.x, ptc->pos.y, ptc->pos.z);
                            currPtc->shapePtr = ptc->shapePtr;
                            addto_group(ptc->subParticlesGrp, &currPtc->header);
                            currPtc->flags &= ~PTC_FLAG_8;
                        }
                        break;
                }
            }
            break;
        default:
            break;
    }
    ptc->unk38.x *= 0.9f;
    ptc->unk38.y *= 0.9f;
    ptc->unk38.z *= 0.9f;
    if (ptc->unk60 == 3) {
        switch (ptc->unk64) {
            case 1:
                if (func_80182778(ptc) && ptc->subParticlesGrp != NULL) {
                    struct ListNode *link;

                    if (ptc->unk80 != NULL) {
                        ptc->unk80->unk3C |= LIGHT_UNK01;
                        ptc->unk80->position.x = ptc->pos.x;
                        ptc->unk80->position.y = ptc->pos.y;
                        ptc->unk80->position.z = ptc->pos.z;
                    }
                    link = ptc->subParticlesGrp->firstMember;
                    while (link != NULL) {
                        struct ObjParticle *linkPtc = (struct ObjParticle *) link->obj;

                        linkPtc->pos.x = ptc->pos.x;
                        linkPtc->pos.y = ptc->pos.y;
                        linkPtc->pos.z = ptc->pos.z;
                        linkPtc->timeout = 20;
                        do {
                            linkPtc->unk38.x = gd_rand_float() * 64.0f - 32.0f;
                            linkPtc->unk38.y = gd_rand_float() * 64.0f - 32.0f;
                            linkPtc->unk38.z = gd_rand_float() * 64.0f - 32.0f;
                        } while (gd_vec3f_magnitude(&linkPtc->unk38) > 32.0f);
                        linkPtc->unk30 = gd_rand_float() * 180.0f;
                        linkPtc->header.drawFlags &= ~OBJ_INVISIBLE;
                        linkPtc->flags |= PTC_FLAG_8;
                        link = link->next;
                    }
                }
                break;
            case 3:
                if ((ptc->flags & PTC_FLAG_20) && !(ptc->flags & PTC_FLAG_10)) {
                    func_80182A08(ptc, &sp40);
                    ptc->flags |= PTC_FLAG_10;
                }
                break;
            case 2:
                func_80182A08(ptc, &sp34);
                break;
        }
        apply_to_obj_types_in_group(OBJ_TYPE_PARTICLES, (applyproc_t) move_particle, ptc->subParticlesGrp);
    }
    if (ptc->timeout >= 0) {
        if (ptc->timeout-- <= 0) {
            ptc->header.drawFlags |= OBJ_INVISIBLE;
            ptc->flags &= ~PTC_FLAG_8;
        }
    }
}

/* 231D40 -> 231D98; orig name: func_80183570 */
void move_particles_in_grp(struct ObjGroup *group) {
    start_timer("particles");
    gGdSkinNet = NULL;
    apply_to_obj_types_in_group(OBJ_TYPE_PARTICLES, (applyproc_t) move_particle, group);
    stop_timer("particles");
}

/* 231D98 -> 232040 */
UNUSED void Unknown801835C8(struct ObjParticle *ptc) {
    struct GdVec3f d;
    f32 mag;
    struct ListNode *link;

    gd_printf("p(%d)=", ptc->attachedObjsGrp->memberCount);
    link = ptc->attachedObjsGrp->firstMember;
    while (link != NULL) {
        // FIXME: types
        struct ObjParticle *sp48 = (struct ObjParticle *) link->obj;

        d.x = sp48->pos.x - ptc->pos.x;
        d.y = sp48->pos.y - ptc->pos.y;
        d.z = sp48->pos.z - ptc->pos.z;
        mag = 150.0f - (ABS(d.x) + ABS(d.y) + ABS(d.z));
        gd_printf(",%f ", mag);
        mag *= 0.00000005f;
        ptc->pos.x += mag * d.x;
        ptc->pos.y += mag * d.y;
        ptc->pos.z += mag * d.z;
        sp48->pos.x -= mag * d.x;
        sp48->pos.y -= mag * d.y;
        sp48->pos.z -= mag * d.z;
        link = link->next;
    }
    gd_printf("\n");
}
