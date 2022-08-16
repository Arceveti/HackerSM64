// castle_cannon_grate.inc.c

void bhv_castle_cannon_grate_init(void) {
    if (save_file_get_total_star_count(
            SAVE_NUM_TO_INDEX(gCurrSaveFileNum),
            COURSE_NUM_TO_INDEX(COURSE_MIN),
            COURSE_NUM_TO_INDEX(COURSE_MAX)
        ) >= 120
    ) {
        obj_mark_for_deletion(o);
    }
}
