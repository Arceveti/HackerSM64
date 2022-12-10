// ddd_sub.inc.c

void bhv_bowsers_sub_init(void) {
    if (gCurrActNum != ACT_INDEX_TO_NUM(ACT_INDEX_1)) {
        obj_mark_for_deletion(o);
    } else {
        load_object_static_model();
    }
}
