#ifndef SURFACE_COLLISION_H
#define SURFACE_COLLISION_H

#include <PR/ultratypes.h>

#include "types.h"

#include "surface_load.h"

// The y coord is moved upward by this amount when finding floors.
// Vanilla value is 78.
#define FIND_FLOOR_BUFFER 78

#define CELL_HEIGHT_LIMIT            20000
#define FLOOR_LOWER_LIMIT           -11000
#define FLOOR_LOWER_LIMIT_MISC      (FLOOR_LOWER_LIMIT + 1000)

enum RaycastFlags {
    RAYCAST_FIND_FLOOR = BIT(SPATIAL_PARTITION_FLOORS),
    RAYCAST_FIND_CEIL  = BIT(SPATIAL_PARTITION_CEILS),
    RAYCAST_FIND_WALL  = BIT(SPATIAL_PARTITION_WALLS),
    RAYCAST_FIND_WATER = BIT(SPATIAL_PARTITION_WATER),
    RAYCAST_FIND_SOLID = (RAYCAST_FIND_FLOOR | RAYCAST_FIND_CEIL | RAYCAST_FIND_WALL),
    RAYCAST_FIND_ALL   = (0xFFFFFFFF)
};

struct WallCollisionData {
    /*0x00*/ f32 x, y, z;
    /*0x0C*/ f32 offsetY;
    /*0x10*/ f32 radius;
    /*0x14*/ s32 numWalls;
    /*0x18*/ struct Surface *walls[MAX_REFERENCED_WALLS];
};

s32 is_outside_level_bounds(s32 xPos, s32 zPos);
s32 get_cell_coord(s32 coord);
f32 get_surface_height_at_pos(f32 xPos, f32 zPos, struct Surface *surf);
s32 get_surface_yaw(struct Surface *surf);
s32 f32_find_wall_collision(f32 *xPtr, f32 *yPtr, f32 *zPtr, f32 offsetY, f32 radius);
s32 find_wall_collisions(struct WallCollisionData *colData);
void resolve_and_return_wall_collisions(Vec3f pos, f32 offset, f32 radius, struct WallCollisionData *collisionData);
void raycast_collision_walls(Vec3f pos, Vec3f intendedPos, f32 yOffset);
f32 find_ceil(f32 posX, f32 posY, f32 posZ, struct Surface **pceil);

// Finds the ceiling from a vec3f and a minimum height (with 3 unit vertical buffer).
ALWAYS_INLINE f32 find_mario_ceil(Vec3f pos, f32 height, struct Surface **ceil) {
#ifdef EXPOSED_CEILINGS_FIX
    return find_ceil(pos[0], (max(height, pos[1]) + 3.0f), pos[2], ceil);
#else
    return find_ceil(pos[0], (height + 3.0f), pos[2], ceil);
#endif
}

f32 find_floor_height(f32 x, f32 y, f32 z);
f32 find_floor(f32 xPos, f32 yPos, f32 zPos, struct Surface **pfloor);
f32 find_room_floor(f32 x, f32 y, f32 z, struct Surface **pfloor);
s32 get_room_at_pos(f32 x, f32 y, f32 z);
s32 find_water_level_and_floor(s32 x, s32 y, s32 z, struct Surface **pfloor);
s32 find_water_level(s32 x, s32 y, s32 z);
s32 find_poison_gas_level(s32 x, s32 z);
#ifdef VANILLA_DEBUG
void debug_surface_list_info(f32 xPos, f32 zPos);
#endif

// Backwards compatibility
#define GET_CELL_COORD                  get_cell_coord
#define get_surface_height_at_location  get_surface_height_at_pos
#define SURFACE_YAW                     get_surface_yaw

#endif // SURFACE_COLLISION_H
