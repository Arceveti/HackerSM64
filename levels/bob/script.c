#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

/* Fast64 begin persistent block [includes] */
/* Fast64 end persistent block [includes] */

#include "make_const_nonconst.h"
#include "levels/bob/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_bob_entry[] = {
	INIT_LEVEL(),
	LOAD_YAY0(0x07, _bob_segment_7SegmentRomStart, _bob_segment_7SegmentRomEnd), 
	LOAD_YAY0_TEXTURE(0x09, _generic_yay0SegmentRomStart, _generic_yay0SegmentRomEnd), 
	LOAD_YAY0(0x0A, _ccm_skybox_yay0SegmentRomStart, _ccm_skybox_yay0SegmentRomEnd), 
	LOAD_YAY0(0x05, _group3_yay0SegmentRomStart, _group3_yay0SegmentRomEnd), 
	LOAD_RAW(0x0C, _group3_geoSegmentRomStart, _group3_geoSegmentRomEnd), 
	LOAD_YAY0(0x06, _group14_yay0SegmentRomStart, _group14_yay0SegmentRomEnd), 
	LOAD_RAW(0x0D, _group14_geoSegmentRomStart, _group14_geoSegmentRomEnd), 
	LOAD_YAY0(0x08, _common0_yay0SegmentRomStart, _common0_yay0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	LOAD_YAY0(0xb, _effect_yay0SegmentRomStart, _effect_yay0SegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 
	JUMP_LINK(script_func_global_4), 
	JUMP_LINK(script_func_global_15), 
	LOAD_MODEL_FROM_GEO(MODEL_BOB_BUBBLY_TREE, bubbly_tree_geo), 
	LOAD_MODEL_FROM_GEO(MODEL_BOB_CHAIN_CHOMP_GATE, bob_geo_000440), 
	LOAD_MODEL_FROM_GEO(MODEL_BOB_SEESAW_PLATFORM, bob_geo_000458), 
	LOAD_MODEL_FROM_GEO(MODEL_BOB_BARS_GRILLS, bob_geo_000470), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, bob_area_1),
		WARP_NODE(0x0A, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF0, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0xF1, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		WARP_NODE(0x0B, LEVEL_WF, 0x01, 0x0A, WARP_CHECKPOINT),
		WARP_NODE(0xF3, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_NONE, 0, 11358, 0, 0, 0, 0, (10 << 24) | (DLG_BOOTING << 16), bhvDLG),
		OBJECT(MODEL_NONE, -1954, -960, 6228, 0, 0, 0, (10 << 24) | (DLG_DONT_FALL << 16), bhvDLG),
		OBJECT(MODEL_NONE, 4829, 8343, 3670, 0, 0, 0, (10 << 24) | (DLG_GO_LEFT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 3680, 3556, -7499, 0, 0, 0, (10 << 24) | (DLG_GO_LEFT << 16), bhvDLG),
		OBJECT(MODEL_NONE, -4868, -755, 2845, 0, 0, 0, (10 << 24) | (DLG_GO_LEFT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 3858, 9287, -5465, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, 4933, 8318, 1486, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, 5989, 8225, 7107, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, 6969, 5838, 448, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, -1665, 5159, 3530, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, -2229, 4332, -4088, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, -6775, 1154, -7659, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, 11673, -5168, 12012, 0, 0, 0, (10 << 24) | (DLG_GOOD << 16), bhvDLG),
		OBJECT(MODEL_NONE, -239, -7165, 8652, 0, 0, 0, (10 << 24) | (DLG_INTO_THE_CAVE << 16), bhvDLG),
		OBJECT(MODEL_NONE, 4869, 8606, -926, 0, 0, 0, (10 << 24) | (DLG_GO_JUMP << 16), bhvDLG),
		OBJECT(MODEL_NONE, -3105, 4857, 2875, 0, 0, 0, (10 << 24) | (DLG_JUMP_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 1599, 5260, 3255, 0, 0, 0, (10 << 24) | (DLG_GO_JUMP << 16), bhvDLG),
		OBJECT(MODEL_NONE, -4987, -647, -473, 0, 0, 0, (10 << 24) | (DLG_GO_JUMP << 16), bhvDLG),
		OBJECT(MODEL_NONE, -2630, -1478, 13590, 0, 0, 0, (10 << 24) | (DLG_GO_JUMP << 16), bhvDLG),
		OBJECT(MODEL_NONE, -742, -3763, 15459, 0, 0, 0, (10 << 24) | (DLG_GO_JUMP << 16), bhvDLG),
		OBJECT(MODEL_NONE, -154, -6344, 11440, 0, 0, 0, (10 << 24) | (DLG_GO_JUMP << 16), bhvDLG),
		OBJECT(MODEL_NONE, 10836, 7378, 8018, 0, 0, 0, (10 << 24) | (DLG_KEEP_LEFT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 5535, 3380, -11509, 0, 0, 0, (10 << 24) | (DLG_KEEP_LEFT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 5383, -5266, 18400, 0, 0, 0, (10 << 24) | (DLG_KEEP_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, -8844, 727, -8820, 0, 0, 0, (10 << 24) | (DLG_KEEP_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, -6116, -2119, 15497, 0, 0, 0, (10 << 24) | (DLG_KEEP_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 8088, -4582, 9334, 0, 0, 0, (10 << 24) | (DLG_LEFT_NO_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 0, -6053, 16002, 0, 0, 0, (10 << 24) | (DLG_NONE << 16), bhvDLG),
		OBJECT(MODEL_NONE, 1083, 2981, -11870, 0, 0, 0, (10 << 24) | (DLG_NONE << 16), bhvDLG),
		OBJECT(MODEL_NONE, -6465, -3002, 8343, 0, 0, 0, (10 << 24) | (DLG_NONE << 16), bhvDLG),
		OBJECT(MODEL_NONE, -5149, 108, -10880, 0, 0, 0, (10 << 24) | (DLG_NONE << 16), bhvDLG),
		OBJECT(MODEL_NONE, 0, 11071, -2669, 0, 0, 0, (10 << 24) | (DLG_START_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, -3941, 4118, -969, 0, 0, 0, (10 << 24) | (DLG_GO_RIGHT << 16), bhvDLG),
		OBJECT(MODEL_NONE, -1487, 2309, -8631, 0, 0, 0, (10 << 24) | (DLG_JUMP_ICICLES << 16), bhvDLG),
		OBJECT(MODEL_NONE, 3735, 5311, 0, 0, 0, 0, (10 << 24) | (DLG_WATCH_OUT << 16), bhvDLG),
		OBJECT(MODEL_NONE, 3388, 7564, 7432, 0, 0, 0, (10 << 24) | (DLG_WRONG_WAY << 16), bhvDLG),
		OBJECT(MODEL_NONE, 7365, 5764, -1298, 0, 0, 0, (10 << 24) | (DLG_WRONG_WAY << 16), bhvDLG),
		OBJECT(MODEL_NONE, -3829, 3901, -4928, 0, 0, 0, (10 << 24) | (DLG_WRONG_WAY << 16), bhvDLG),
		OBJECT(MODEL_NONE, 6825, 3093, -7927, 0, 0, 0, (10 << 24) | (DLG_WRONG_WAY << 16), bhvDLG),
		OBJECT(MODEL_NONE, -4512, -1094, 5913, 0, 0, 0, (10 << 24) | (DLG_WRONG_WAY << 16), bhvDLG),
		OBJECT(MODEL_NONE, 12777, -5143, 9413, 0, 0, 0, (10 << 24) | (DLG_WRONG_WAY << 16), bhvDLG),
		MARIO_POS(0x01, -180, 0, 11200, 0),
		OBJECT(MODEL_NONE, 0, 11100, 0, 0, 0, 0, 0x00000000, bhvRobot),
		OBJECT(MODEL_NONE, 0, 11200, 0, 0, -180, 0, (0x0A << 16), bhvSpinAirborneWarp),
		OBJECT(MODEL_NONE, 0, -7800, 2000, 0, 0, 0, (80 << 24) | (0x0B << 16), bhvWarp),
		TERRAIN(bob_area_1_collision),
		MACRO_OBJECTS(bob_area_1_macro_objs),
		SET_BACKGROUND_MUSIC(0x00, SEQ_LEVEL_SNOW),
		TERRAIN_TYPE(TERRAIN_SLIDE),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, -180, 0, 11200, 0),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
