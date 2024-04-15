void scroll_gfx_mat_bob_dl_f3dlite_material_bg_aurora2_layer0() {
	Gfx *mat = segmented_to_virtual(mat_bob_dl_f3dlite_material_bg_aurora2_layer0);

	static int interval_tex_bob_dl_f3dlite_material_bg_aurora2_layer0 = 2;
	static int cur_interval_tex_bob_dl_f3dlite_material_bg_aurora2_layer0 = 2;
	shift_s(mat, 13, PACK_TILESIZE(0, 1));

	if (--cur_interval_tex_bob_dl_f3dlite_material_bg_aurora2_layer0 <= 0) {
		shift_s_down(mat, 18, PACK_TILESIZE(0, 1));
		cur_interval_tex_bob_dl_f3dlite_material_bg_aurora2_layer0 = interval_tex_bob_dl_f3dlite_material_bg_aurora2_layer0;
	}

};

void scroll_bob() {
	scroll_gfx_mat_bob_dl_f3dlite_material_bg_aurora2_layer0();
};
