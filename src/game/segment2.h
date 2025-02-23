#ifndef SEGMENT2_H
#define SEGMENT2_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

extern Texture* puppyprint_font_lut[];
extern u8* puppyprint_kerning_lut[][95];
extern struct PPTextFont* gPuppyPrintFontTable[];

extern u8* seg2_course_name_table[];
extern u8* seg2_act_name_table[];
extern Gfx dl_rgba16_text_begin[];
extern Gfx dl_rgba16_text_end[];
extern Gfx dl_ia_text_begin[];
extern Gfx dl_ia_text_end[];
extern Texture texture_radial_light[];
extern Texture texture_transition_star_half[];
extern Texture texture_transition_circle_half[];
extern Texture texture_transition_mario[];
extern Texture texture_transition_bowser_half[];
extern Texture texture_waterbox_water[];
extern Texture texture_waterbox_mist[];
extern Texture texture_waterbox_jrb_water[];
extern Texture texture_waterbox_unknown_water[];
extern Texture texture_waterbox_lava[];
extern Gfx dl_proj_mtx_fullscreen[];
extern Gfx dl_draw_quad_verts_0123[];
extern Gfx dl_screen_transition_end[];
extern Gfx dl_transition_draw_filled_region[];
extern Gfx dl_shadow_begin_decal[];
extern Gfx dl_shadow_begin_non_decal[];
extern Gfx dl_shadow_circle[];
extern Gfx dl_shadow_square[];
extern Gfx dl_shadow_4_verts[];
extern Gfx dl_shadow_end[];
#if STAR_GLOW
extern Gfx dl_star_glow[];
#endif
extern Gfx dl_skybox_begin[];
extern Gfx dl_skybox_tile_tex_settings[];
extern Gfx dl_skybox_end[];
extern Gfx dl_waterbox_ia16_begin[];
extern Gfx dl_waterbox_rgba16_begin[];
extern Gfx dl_waterbox_end[];
#ifdef ENABLE_STATUS_REPOLLING_GUI
extern Texture texture_controller_port[];
extern Texture texture_controller_unknown[];
extern Texture texture_controller_n64_normal[];
extern Texture texture_controller_n64_mouse[];
extern Texture texture_controller_n64_voice[];
extern Texture texture_controller_n64_keyboard[];
extern Texture texture_controller_gba[];
extern Texture texture_controller_gcn_normal[];
extern Texture texture_controller_gcn_receiver[];
extern Texture texture_controller_gcn_wavebird[];
extern Texture texture_controller_gcn_wheel[];
extern Texture texture_controller_gcn_keyboard[];
extern Texture texture_controller_gcn_dancepad[];
#endif // ENABLE_STATUS_REPOLLING_GUI
#ifdef ENABLE_RUMBLE
//! TODO: Move rumble pak graphic textures to src/menu/intro_geo.c once build order is fixed.
extern Texture title_texture_rumble_pak_en[];
//! TODO: Use these after ASCII/multilang is merged.
 #if MULTILANG
  #ifdef ENABLE_FRENCH
extern Texture title_texture_rumble_pak_fr[];
  #endif // ENABLE_FRENCH
  #ifdef ENABLE_GERMAN
extern Texture title_texture_rumble_pak_de[];
  #endif // ENABLE_GERMAN
  #ifdef ENABLE_JAPANESE
extern Texture title_texture_rumble_pak_jp[];
  #endif // ENABLE_JAPANESE
  #if defined(ENABLE_SPANISH_SPAIN) || defined(ENABLE_SPANISH_LATIN_AMERICA)
extern Texture title_texture_rumble_pak_es[];
  #endif // (ENABLE_SPANISH_SPAIN || ENABLE_SPANISH_LATIN_AMERICA)
 #endif // MULTILANG
extern Texture title_texture_rumble_pak_controller[];
#endif // ENABLE_RUMBLE
extern Gfx dl_paintings_ripple_triangles[];
extern Gfx dl_paintings_rippling_begin[];
extern Gfx dl_paintings_rippling_end[];
extern Gfx dl_paintings_textured_shaded_begin[];
extern Gfx dl_paintings_textured_vertex_colored_begin[];
extern Gfx dl_paintings_textured_end[];
extern Gfx dl_paintings_env_mapped_begin[];
extern Gfx dl_paintings_env_mapped_end[];
extern PaintingData painting_data_vertices[];
extern PaintingData painting_data_triangles[];
extern Texture* main_hud_lut[59];
extern Gfx dl_hud_img_load_tex_block[];
extern Gfx dl_hud_img_begin[];
extern Gfx dl_hud_img_end[];
extern Texture* main_font_lut[];
extern Gfx dl_ia_text_tex_settings[];
extern Gfx dl_rgba16_load_tex_block[];
extern Texture* main_credits_font_lut[];
extern Texture* main_hud_camera_lut[6];
extern Gfx dl_shade_screen_begin[];
extern Gfx dl_shade_screen_end[];
extern Gfx dl_draw_text_bg_box[];
extern Gfx dl_draw_triangle[];
extern struct DialogEntry* seg2_dialog_table[];
extern Gfx dl_billboard_num_0[];
extern Gfx dl_billboard_num_1[];
extern Gfx dl_billboard_num_2[];
extern Gfx dl_billboard_num_3[];
extern Gfx dl_billboard_num_4[];
extern Gfx dl_billboard_num_5[];
extern Gfx dl_billboard_num_6[];
extern Gfx dl_billboard_num_7[];
extern Gfx dl_billboard_num_8[];
extern Gfx dl_billboard_num_9[];
#ifdef DIALOG_INDICATOR
extern Gfx dl_billboard_num_A[];
extern Gfx dl_billboard_num_B[];
extern Gfx dl_billboard_num_C[];
extern Gfx dl_billboard_num_D[];
extern Gfx dl_billboard_num_E[];
extern Gfx dl_billboard_num_F[];
#endif

#endif // SEGMENT2_H
