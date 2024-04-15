#include "src/game/envfx_snow.h"

const GeoLayout scan_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, scan_Circle_mesh_layer_5),
		GEO_DISPLAY_LIST(LAYER_OPAQUE_DECAL, scan_Circle_mesh_layer_2),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
