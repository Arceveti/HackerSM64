#ifndef GD_DYNLIST_MACROS_H
#define GD_DYNLIST_MACROS_H

enum GdDynListCommands {
    /*0x00*/ DYNLIST_CMD_USE_INTEGER_NAMES,
    /*0x01*/ DYNLIST_CMD_SET_INITIAL_POSITION,
    /*0x02*/ DYNLIST_CMD_SET_RELATIVE_POSITION,
    /*0x03*/ DYNLIST_CMD_SET_WORLD_POSITION,
    /*0x04*/ DYNLIST_CMD_SET_NORMAL,
    /*0x05*/ DYNLIST_CMD_SET_SCALE,
    /*0x06*/ DYNLIST_CMD_SET_ROTATION,
    /*0x07*/ DYNLIST_CMD_SET_DRAW_FLAG,
    /*0x08*/ DYNLIST_CMD_SET_FLAG,
    /*0x09*/ DYNLIST_CMD_CLEAR_FLAG,
    /*0x0A*/ DYNLIST_CMD_SET_FRICTION,
    /*0x0B*/ DYNLIST_CMD_SET_SPRING,
    /*0x0C*/ DYNLIST_CMD_CALL_LIST,
    /*0x0D*/ DYNLIST_CMD_SET_COLOR_NUM,
    /*0x0E*/ DYNLIST_CMD_UNK_0E,
    /*0x0F*/ DYNLIST_CMD_MAKE_DYN_OBJ,
    /*0x10*/ DYNLIST_CMD_START_GROUP,
    /*0x11*/ DYNLIST_CMD_END_GROUP,
    /*0x12*/ DYNLIST_CMD_ADD_TO_GROUP,
    /*0x13*/ DYNLIST_CMD_SET_TYPE,
    /*0x14*/ DYNLIST_CMD_SET_MATERIAL_GROUP,
    /*0x15*/ DYNLIST_CMD_SET_NODE_GROUP,
    /*0x16*/ DYNLIST_CMD_SET_SKIN_SHAPE,
    /*0x17*/ DYNLIST_CMD_SET_PLANE_GROUP,
    /*0x18*/ DYNLIST_CMD_SET_SHAPE_PTR_PTR,
    /*0x19*/ DYNLIST_CMD_SET_SHAPE_PTR,
    /*0x1A*/ DYNLIST_CMD_SET_SHAPE_OFFSET,
    /*0x1B*/ DYNLIST_CMD_SET_CENTER_OF_GRAVITY,
    /*0x1C*/ DYNLIST_CMD_LINK_WITH,
    /*0x1D*/ DYNLIST_CMD_LINK_WITH_PTR,
    /*0x1E*/ DYNLIST_CMD_USE_OBJ,
    /*0x1F*/ DYNLIST_CMD_SET_CONTROL_TYPE,
    /*0x20*/ DYNLIST_CMD_SET_SKIN_WEIGHT,
    /*0x21*/ DYNLIST_CMD_SET_AMBIENT,
    /*0x22*/ DYNLIST_CMD_SET_DIFFUSE,
    /*0x23*/ DYNLIST_CMD_SET_ID,
    /*0x24*/ DYNLIST_CMD_SET_MATERIAL,
    /*0x25*/ DYNLIST_CMD_MAP_MATERIALS,
    /*0x26*/ DYNLIST_CMD_MAP_VERTICES,
    /*0x27*/ DYNLIST_CMD_ATTACH,
    /*0x28*/ DYNLIST_CMD_ATTACH_TO,
    /*0x29*/ DYNLIST_CMD_SET_ATTACH_OFFSET,
    /*0x2A*/ DYNLIST_CMD_UNK_2A,
    /*0x2B*/ DYNLIST_CMD_SET_NAME_SUFFIX,
    /*0x2C*/ DYNLIST_CMD_SET_PARAM_F,
    /*0x2D*/ DYNLIST_CMD_SET_PARAM_PTR,
    /*0x2E*/ DYNLIST_CMD_MAKE_NET_WITH_SUB_GROUP,
    /*0x2F*/ DYNLIST_CMD_MAKE_ATTACHED_JOINT,
    /*0x30*/ DYNLIST_CMD_END_NET_WITH_SUB_GROUP,
    /*0x31*/ DYNLIST_CMD_MAKE_VERTEX,
    /*0x32*/ DYNLIST_CMD_MAKE_VAL_PTR,
    /*0x33*/ DYNLIST_CMD_UNK_33,
    /*0x34*/ DYNLIST_CMD_USE_TEXTURE,
    /*0x35*/ DYNLIST_CMD_SET_TEXTURE_ST,
    /*0x36*/ DYNLIST_CMD_MAKE_NET_FROM_SHAPE,
    /*0x37*/ DYNLIST_CMD_MAKE_NET_FROM_SHAPE_PTR,
    /*0x38*/ DYNLIST_CMD_UNK_38,
    /*0x39*/ DYNLIST_CMD_UNK_39,
    /*0x3A*/ DYNLIST_CMD_END,
    /*0x3B*/ DYNLIST_CMD_UNK_3B,
    /*0x3C*/ DYNLIST_CMD_UNK_3C,
    /*0x3D*/ DYNLIST_CMD_UNK_3D,
    /*0x3E*/ DYNLIST_CMD_UNK_3E,
    /*0x3F*/ DYNLIST_CMD_UNK_3F,
    /*0xD1D4*/ DYNLIST_CMD_BEGIN = 53716,
};

/* DynListCmd Macros */

/**
 * Must be the first command in a dynlist.
 */
#define BeginList() \
    { DYNLIST_CMD_BEGIN, {0}, {0}, {0.0, 0.0, 0.0} }

/**
 * Must be the last command in a dynlist.
 */
#define EndList() \
    { DYNLIST_CMD_END, {0}, {0}, {0.0, 0.0, 0.0} }

/**
 * If `enable` is TRUE, then subsequent object names are treated as integers
 * rather than strings.
 */
#define UseIntegerNames(enable) \
    { DYNLIST_CMD_USE_INTEGER_NAMES, {0}, {(void *)(enable)}, {0.0, 0.0, 0.0} }

/**
 * Set the initial position of the current object
 * Supported Objs: joints, particles, nets, vertices, cameras
 */
#define SetInitialPosition(x, y, z) \
    { DYNLIST_CMD_SET_INITIAL_POSITION, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the relative position of the current object
 * Supported Objs: joints, particles, vertices, cameras, labels
 */
#define SetRelativePosition(x, y, z) \
    { DYNLIST_CMD_SET_RELATIVE_POSITION, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the world position of the current object
 * Supported Objs: joints, nets, vertices, cameras, gadgets, views
 */
#define SetWorldPosition(x, y, z) \
    { DYNLIST_CMD_SET_WORLD_POSITION, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the normal of the current object
 * Supported Objs: vertices
 */
#define SetNormal(x, y, z) \
    { DYNLIST_CMD_SET_NORMAL, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the scale of the current object
 * Supported Objs: joints, particles, nets, gadgets, views, lights
 */
#define SetScale(x, y, z) \
    { DYNLIST_CMD_SET_SCALE, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the rotation of the current object
 * Supported Objs: joints, nets
 */
#define SetRotation(x, y, z) \
    { DYNLIST_CMD_SET_ROTATION, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the specified bits in the object's `drawFlags` field
 * Supported Objs: all
 */
#define SetDrawFlag(flags) \
    { DYNLIST_CMD_SET_DRAW_FLAG, {0}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

/**
 * Set the specified bits in the object specific flag
 * Supported Objs: bones, joints, particles, shapes, nets, cameras, views, lights
 */
#define SetFlag(flags) \
    { DYNLIST_CMD_SET_FLAG, {0}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

/**
 * Clear the specified bits in the object specific flag
 * Supported Objs: bones, joints, particles, nets, cameras
 */
#define ClearFlag(flags) \
    { DYNLIST_CMD_CLEAR_FLAG, {0}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

/**
 * Set the friction vector of a Joint
 * Supported Objs: joints
 */
#define SetFriction(x, y, z) \
    { DYNLIST_CMD_SET_FRICTION, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the spring value of a Bone
 * Supported Objs: bones
 */
#define SetSpring(spring) \
    { DYNLIST_CMD_SET_SPRING, {0}, {0}, {(spring), 0.0, 0.0} }

/**
 * Jump to pointed dynlist. Once that list has finished processing, flow returns
 * to the current list.
 */
#define CallList(list) \
    { DYNLIST_CMD_CALL_LIST, {(void *)(list)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Sets the object's color to one of the predefined colors (see draw_objects.h
 * for the list of colors.
 * Supported Objs: joints, particles, nets, faces, gadgets
 */
#define SetColourNum(colourNum) \
    { DYNLIST_CMD_SET_COLOR_NUM, {0}, {(void *)(colourNum)}, {0.0, 0.0, 0.0} }

/**
 * Make an object of the specified type and name, and set it as the current
 * object.
 */
#define MakeDynObj(type, name) \
    { DYNLIST_CMD_MAKE_DYN_OBJ, {(void *)(name)}, {(void *)(type)}, {0.0, 0.0, 0.0} }

/**
 * Make a group that will contain all subsequently created objects once the
 * EndGroup command is called.
 */
#define StartGroup(grpName) \
    { DYNLIST_CMD_START_GROUP, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * End a group. All objects created between StartGroup and EndGroup are added to
 * the group.
 */
#define EndGroup(grpName) \
    { DYNLIST_CMD_END_GROUP, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Add the current object to the specified group.
 * Supported Objs: all
 */
#define AddToGroup(grpName) \
    { DYNLIST_CMD_ADD_TO_GROUP, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set an object specific type flag.
 * Supported Objs: groups, joints, particles, nets, materials, gadgets
 */
#define SetType(type) \
    { DYNLIST_CMD_SET_TYPE, {0}, {(void *)(type)}, {0.0, 0.0, 0.0} }

/**
 * Set the current shape's material group to the specified group.
 * Supported Objs: shapes
 */
#define SetMaterialGroup(mtlGrpName) \
    { DYNLIST_CMD_SET_MATERIAL_GROUP, {(void *)(mtlGrpName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Assign the specified group to the current object. The purpose of the group
 * depends on the current object's type. For shapes, it sets the vertex data.
 * For animators, it sets the animation data. For nets, it sets ???. For
 * gadgets, it sets ???.
 * Supported Objs: shapes, nets, gadgets, animators
 */
#define SetNodeGroup(grpName) \
    { DYNLIST_CMD_SET_NODE_GROUP, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the skin group of the current Net object with the vertices from the
 * specified shape.
 * Supported Objs: nets
 */
#define SetSkinShape(shapeName) \
    { DYNLIST_CMD_SET_SKIN_SHAPE, {(void *)(shapeName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the plane (face) group of the current object.
 * Supported Objs: shapes, nets
 */
#define SetPlaneGroup(planeGrpName) \
    { DYNLIST_CMD_SET_PLANE_GROUP, {(void *)(planeGrpName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the current object's shape, where `shapePtr` is a pointer to an
 * `ObjShape`.
 * Supported Objs: bones, joints, particles, nets, gadgets, lights
 */
#define SetShapePtrPtr(shapePtr) \
    { DYNLIST_CMD_SET_SHAPE_PTR_PTR, {(void *)(shapePtr)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the current object's shape, where `shapeName` is the name of a shape
 * object.
 * Supported Objs: bones, joints, particles, nets, gadgets
 */
#define SetShapePtr(shapeName) \
    { DYNLIST_CMD_SET_SHAPE_PTR, {(void *)(shapeName)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set offset of the connected shape
 * Supported Objs: joints
 */
#define SetShapeOffset(x, y, z) \
    { DYNLIST_CMD_SET_SHAPE_OFFSET, {0}, {0}, {(x), (y), (z)} }

/**
 * Set the center of gravity of the current Net object
 * Supported Objs: nets
 */
#define SetCenterOfGravity(x, y, z) \
    { DYNLIST_CMD_SET_CENTER_OF_GRAVITY, {0}, {0}, {(x), (y), (z)} }

// TODO:

/* Link Object ID to the current dynobj */
/* Supported Objs: groups, bones, faces, cameras, views, labels, animators */
#define LinkWith(w1) \
    { DYNLIST_CMD_LINK_WITH, {(void *)(w1)}, {0}, {0.0, 0.0, 0.0} }

/* Link Object pointer to the current dynobj */
/* Supported Objs: groups, bones, faces, cameras, views, labels, animators */
#define LinkWithPtr(w1) \
    { DYNLIST_CMD_LINK_WITH_PTR, {(void *)(w1)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the specified object as the current object.
 * Supported Objs: all
 */
#define UseObj(name) \
    { DYNLIST_CMD_USE_OBJ, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the current Net object's control type field. Control type is never used
 * for anything, so this command effectively does nothing.
 * Supported Objs: nets
 */
#define SetControlType(w2) \
    { DYNLIST_CMD_SET_CONTROL_TYPE, {0}, {(void *)(w2)}, {0.0, 0.0, 0.0} }

/**
 * Set the weight percentage of the specified vertex controlled by the current
 * Joint object.
 * Supported Objs: joints
 */
#define SetSkinWeight(vtxNum, weight) \
    { DYNLIST_CMD_SET_SKIN_WEIGHT, {0}, {(void *)(vtxNum)}, {(weight), 0.0, 0.0} }

/**
 * Set the ambient color of the current Material object.
 * Supported Objs: materials
 */
#define SetAmbient(r, g, b) \
    { DYNLIST_CMD_SET_AMBIENT, {0}, {0}, {(r), (g), (b)} }

/**
 * Set the diffuse color of the current Material or Light object.
 * Supported Objs: materials, lights
 */
#define SetDiffuse(r, g, b) \
    { DYNLIST_CMD_SET_DIFFUSE, {0}, {0}, {(r), (g), (b)} }

/**
 * Set the object specific ID field.
 * Supported Objs: joints, vertices, materials, lights
 */
#define SetId(id) \
    { DYNLIST_CMD_SET_ID, {0}, {(void *)(id)}, {0.0, 0.0, 0.0} }

/**
 * Set the material id of the current Face
 * Supported Objs: faces
 */
#define SetMaterial(id) \
    { DYNLIST_CMD_SET_MATERIAL, {0}, {(void *)(id)}, {0.0, 0.0, 0.0} }

/**
 * For all faces in the current Group, resolve their material IDs to actual
 * `ObjMaterial`s.
 * Supported Objs: groups
 */
#define MapMaterials(name) \
    { DYNLIST_CMD_MAP_MATERIALS, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * For all faces in the current Group, resolve their vertex indices to pointers
 * to actual `ObjVertex`es. Calculate normals for all vertices in the the group
 * specified by `name`
 * Supported Objs: groups
 */
#define MapVertices(name) \
    { DYNLIST_CMD_MAP_VERTICES, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Stub command (does nothing).
 * Supported Objs: joints
 */
#define Attach(name) \
    { DYNLIST_CMD_ATTACH, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Attach the current object to the specified object, using the specified flags.
 * Supported Objs: joints, particles, nets, animators
 */
#define AttachTo(flags, name) \
    { DYNLIST_CMD_ATTACH_TO, {(void *)(name)}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

/**
 * Set the point at which the current object is attached to its parent object
 * Supported Objs: joints, particles, nets
 */
#define SetAttachOffset(x, y, z) \
    { DYNLIST_CMD_SET_ATTACH_OFFSET, {0}, {0}, {(x), (y), (z)} }

/**
 * Set a "suffix" to use with dynobj names. All commands that take a name as a
 * parameter will have this suffix appended to the name.
 */
#define SetNameSuffix(suffix) \
    { DYNLIST_CMD_SET_NAME_SUFFIX, {(void *)(suffix)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Set the float paramter `param` to `value`.
 * For Shapes, the following parameters are supported:
 *     PARM_F_ALPHA - the alpha (opacity) of the shape
 * For Gadgets, the following parameters are supported:
 *     PARM_F_RANGE_MIN - the minimum value of the gadget
 *     PARM_F_RANGE_MAX - the maximum value of the gadget
 *     PARM_F_VARVAL - the current value of the gadget
 * For Vertices, the following parameters are supported:
 *     PARM_F_ALPHA - the alpha (opacity) of the vertex
 * Supported Objs: shapes, vertices, gadgets
 */
#define SetParamF(param, value) \
    { DYNLIST_CMD_SET_PARAM_F, {0}, {(void *)(param)}, {(value), 0.0, 0.0} }

/**
 * Set pointer paramter `param` to `value`
 * For Labels, the following parameters are supported:
 *     PARM_PTR_CHAR - the format string for the label text
 * For Views, the following parameters are supported:
 *     PARM_PTR_CHAR - the name of the view
 * For Faces, the following parameters are supported:
 *     PARM_PTR_OBJ_VTX - (not actually a pointer) index of a vertex created with `MakeVertex`.
 * Supported Objs: faces, views, labels */
#define SetParamPtr(param, value) \
    { DYNLIST_CMD_SET_PARAM_PTR, {(void *)(value)}, {(void *)(param)}, {0.0, 0.0, 0.0} }

/**
 * Create a Net with the specified name, and add a group to it.
 */
#define MakeNetWithSubGroup(name) \
    { DYNLIST_CMD_MAKE_NET_WITH_SUB_GROUP, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Make a Joint and attach it to the Net created with "MakeNetWithSubGroup".
 */
#define MakeAttachedJoint(name) \
    { DYNLIST_CMD_MAKE_ATTACHED_JOINT, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * End a Net that was created with "MakeNetWithSubGroup"
 */
#define EndNetWithSubGroup(name) \
    { DYNLIST_CMD_END_NET_WITH_SUB_GROUP, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

/**
 * Add a Vertex dynobj
 */
#define MakeVertex(x, y, z) \
    { DYNLIST_CMD_MAKE_VERTEX, {0}, {0}, {(x), (y), (z)} }

/**
 * Add a ValPtr dynobj
 */
#define MakeValPtr(id, flags, type, offset) \
    { DYNLIST_CMD_MAKE_VAL_PTR, {(void *)(id)}, {(void *)(type)}, {(offset), (flags), 0.0} }

/**
 * Set the texture of the current Material dynobj. Note that textures are not
 * actually supported.
 * Supported Objs: materials
 */
#define UseTexture(texture) \
    { DYNLIST_CMD_USE_TEXTURE, {0}, {(void *)(texture)}, {0.0, 0.0, 0.0} }

/**
 * Stub command (does nothing).
 * Supported Objs: vertices
 */
#define SetTextureST(s, t) \
    { DYNLIST_CMD_SET_TEXTURE_ST, {0}, {0}, {(s), (t), 0.0} }

/* Make a new Net from Shape ID */
#define MakeNetFromShape(shape) \
    { DYNLIST_CMD_MAKE_NET_FROM_SHAPE, {(void *)(shape)}, {0}, {0.0, 0.0, 0.0} }

/* Make a new Net from Shape double pointer PTR */
#define MakeNetFromShapePtrPtr(w1) \
    { DYNLIST_CMD_MAKE_NET_FROM_SHAPE_PTR, {(void *)(w1)}, {0}, {0.0, 0.0, 0.0} }

#endif // GD_DYNLIST_MACROS_H
