// water_splashes_and_waves.inc.c

// Water droplets from Mario jumping in a pool of water.
struct WaterDropletParams sWaterSplashDropletParams = {
    .flags                = WATER_DROPLET_FLAG_RAND_ANGLE,
    .model                = MODEL_WHITE_PARTICLE_SMALL,
    .behavior             = bhvWaterDroplet,
    .moveAngleRange       = 0, // Unused (flag-specific)
    .moveRange            = 0, // Unused (flag-specific)
    .randForwardVelOffset = 5.0f,
    .randForwardVelScale  = 3.0f,
    .randYVelOffset       = 30.0f,
    .randYVelScale        = 20.0f,
    .randSizeOffset       = 0.5f,
    .randSizeScale        = 1.0f
};

// Water droplets from Mario jumping in shallow water.
struct WaterDropletParams gShallowWaterSplashDropletParams = {
    .flags                = (WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL),
    .model                = MODEL_WHITE_PARTICLE_SMALL,
    .behavior             = bhvWaterDroplet,
    .moveAngleRange       = 0, // Unused (flag-specific)
    .moveRange            = 0, // Unused (flag-specific)
    .randForwardVelOffset = 2.0f,
    .randForwardVelScale  = 3.0f,
    .randYVelOffset       = 20.0f,
    .randYVelScale        = 20.0f,
    .randSizeOffset       = 0.5f,
    .randSizeScale        = 1.0f
};

// The fish particle easter egg from Mario jumping in shallow water.
struct WaterDropletParams sWaterDropletFishParams = {
    .flags                = (WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL),
    .model                = MODEL_FISH,
    .behavior             = bhvWaterDroplet,
    .moveAngleRange       = 0, // Unused (flag-specific)
    .moveRange            = 0, // Unused (flag-specific)
    .randForwardVelOffset = 2.0f,
    .randForwardVelScale  = 3.0f,
    .randYVelOffset       = 20.0f,
    .randYVelScale        = 20.0f,
    .randSizeOffset       = 1.0f,
    .randSizeScale        = 0.0f
};

// Water droplets from Mario running in shallow water.
struct WaterDropletParams gShallowWaterWaveDropletParams = {
    .flags                = (WATER_DROPLET_FLAG_RAND_ANGLE_INCR_BACKWARD | WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL),
    .model                = MODEL_WHITE_PARTICLE_SMALL,
    .behavior             = bhvWaterDroplet,
    .moveAngleRange       = 0x6000,
    .moveRange            = 0, // Unused (flag-specific)
    .randForwardVelOffset = 2.0f,
    .randForwardVelScale  = 8.0f,
    .randYVelOffset       = 10.0f,
    .randYVelScale        = 10.0f,
    .randSizeOffset       = 0.5f,
    .randSizeScale        = 1.0f
};

void bhv_align_to_water(void) {
    struct Surface *waterSurf = NULL;
    Vec3f pos;

    vec3_zero(&o->oFaceAngleVec);
    vec3f_copy(pos, &o->oPosVec);

    find_water_level_and_floor(pos[0], pos[1], pos[2], &waterSurf);
    if (waterSurf != NULL) {
        Vec3f normal;
        surface_normal_to_vec3f(normal, waterSurf);

        if (normal[1] < 1.0f) {
            mtxf_align_terrain_normal(o->transform, normal, pos, 0x0);
            o->header.gfx.throwMatrix = &o->transform;
        }
    }
}

void bhv_water_splash_spawn_droplets(void) {
    s32 i;
    if (o->oTimer == 0) {
        o->oPosY = find_water_level(o->oPosX, o->oPosY, o->oPosZ);
    }

    if (o->oPosY > FLOOR_LOWER_LIMIT_MISC) { // Make sure it is not at the default water level
        for (i = 0; i < 3; i++) {
            spawn_water_droplet(o, &sWaterSplashDropletParams);
        }
    }
}

void bhv_water_droplet_loop(void) {
    f32 waterLevel = find_water_level(o->oPosX, o->oPosY, o->oPosZ);

    if (o->oTimer == 0) {
        o->header.gfx.node.flags = COND_BIT(o->header.gfx.node.flags, GRAPH_RENDER_BILLBOARD, !cur_obj_has_model(MODEL_FISH));
        o->oFaceAngleYaw = random_u16();
    }
    // Apply gravity
    o->oVelY -= 4.0f;
    o->oPosY += o->oVelY;
    // Check if fallen back into the water
    if (o->oVelY < 0.0f) {
        if (waterLevel > o->oPosY) {
            // Create the smaller splash
            try_to_spawn_object(0, 1.0f, o, MODEL_SMALL_WATER_SPLASH, bhvWaterDropletSplash);
            obj_mark_for_deletion(o);
        } else if (o->oTimer > 20) {
            obj_mark_for_deletion(o);
        }
    }
    if (waterLevel < FLOOR_LOWER_LIMIT_MISC) {
        obj_mark_for_deletion(o);
    }
}

void bhv_idle_water_wave_loop(void) {
    obj_copy_pos(o, gMarioObject);
    o->oPosY = gMarioStates[0].waterLevel + 5;

    bhv_align_to_water();

    if (!(gMarioObject->oMarioParticleFlags & ACTIVE_PARTICLE_IDLE_WATER_WAVE)) {
        gMarioObject->oActiveParticleFlags &= (u16) ~ACTIVE_PARTICLE_IDLE_WATER_WAVE;
        obj_mark_for_deletion(o);
    }
}

void bhv_water_droplet_splash_init(void) {
    cur_obj_scale(random_float() + 1.5f);
}

void bhv_bubble_splash_init(void) {
    f32 waterLevel = find_water_level(o->oPosX, o->oPosY, o->oPosZ);
    obj_scale_xyz(o, 0.5f, 1.0f, 0.5f);
    o->oPosY = waterLevel + 5.0f;
}

void bhv_shallow_water_splash_init(void) {
    // Have a 1 in 256 chance to spawn the fish particle easter egg.
    if ((random_u16() & 0xFF) <= 0) { // Strange
        struct Object *fishObj = spawn_water_droplet(o, &sWaterDropletFishParams);
        obj_init_animation_with_sound(fishObj, blue_fish_seg3_anims_0301C2B0, BLUE_FISH_ANIM_DEFAULT);
    }
}

void bhv_wave_trail_shrink(void) {
    //! Destroy every other water wave to space them out (this is a terrible way of doing it)
    if ((o->oTimer == 0) && (gGlobalTimer & 1)) {
        obj_mark_for_deletion(o);
        return;
    }
    o->oPosY = find_water_level(o->oPosX, o->oPosY, o->oPosZ) + 5.0f;

    bhv_align_to_water();

    if (o->oTimer == 0) {
        o->oWaveTrailSize = o->header.gfx.scale[0];
    }

    if (o->oAnimState > 3) {
        o->oWaveTrailSize = o->oWaveTrailSize - 0.1f; // Shrink the wave
        if (o->oWaveTrailSize < 0.0f) {
            o->oWaveTrailSize = 0.0f;
        }
        o->header.gfx.scale[0] = o->oWaveTrailSize;
        o->header.gfx.scale[2] = o->oWaveTrailSize;
    }
}
