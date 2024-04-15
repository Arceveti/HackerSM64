#include "src/game/envfx_snow.h"

const GeoLayout black_hole_geo[] = {
	GEO_CULLING_RADIUS(30000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, black_hole_Black_Hole_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_INTER, black_hole_Black_Hole_mesh_layer_7),
		GEO_BILLBOARD_WITH_PARAMS(LAYER_TRANSPARENT_INTER, 0, 0, 0),
		GEO_OPEN_NODE(),
			// GEO_Z_OFFSET(4800),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT_INTER, black_hole_Black_Hole_Glow_mesh_layer_7),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
