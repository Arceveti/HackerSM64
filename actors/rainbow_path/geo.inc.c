#include "src/game/envfx_snow.h"

const GeoLayout rainbow_path_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT_INTER, rainbow_path_Rainbow_Road_mesh_layer_7),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
