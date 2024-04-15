#include "src/game/envfx_snow.h"

const GeoLayout background_lava_geo[] = {
	GEO_CULLING_RADIUS(30000),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_FORCE, background_Background__Lava_mesh_layer_0),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
