#ifndef GD_OLD_MENU_H
#define GD_OLD_MENU_H

#include <PR/ultratypes.h>

#include "gd_types.h"
#include "macros.h"

void get_objvalue(union ObjVarVal *dst, enum ValPtrType type, void *base, size_t offset);
struct ObjGadget *make_gadget(UNUSED s32 a0, s32 a1);
void reset_gadget(struct ObjGadget *gdgt);
void reset_gadgets_in_grp(struct ObjGroup *grp);
struct ObjLabel *make_label(struct ObjValPtr *ptr, char *str, s32 flags, f32 x, f32 y, f32 z);

#endif // GD_OLD_MENU_H
