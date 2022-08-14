// breakable_wall.inc.c

void bhv_wf_breakable_wall_loop(void) {
    if (gMarioState->action == ACT_SHOT_FROM_CANNON) {
        cur_obj_become_tangible();

        if (obj_check_if_collided_with_object(o, gMarioObject)) {

            create_sound_spawner(SOUND_GENERAL_WALL_EXPLOSION);

            o->oInteractType = INTERACT_DAMAGE;
            o->oDamageOrCoinValue = 1;

            obj_explode_and_spawn_coins(80.0f, COIN_TYPE_NONE);

            if (cur_obj_has_behavior(bhvWfBreakableWallRight)) {
                play_puzzle_jingle();
#ifdef WF_BREAKABLE_WALL_SPAWNS_STAR
                spawn_default_star(590.0f, 2560.0f, 2650.0f);
#endif
            }
        }
    } else {
#ifdef WF_BREAKABLE_WALL_SPAWNS_STAR
        load_object_collision_model();
#else
        cur_obj_become_intangible();
#endif
    }
}
