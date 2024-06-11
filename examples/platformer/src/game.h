#pragma once

#include "lib/assets.h"

namespace game {

	extern assets::tileset_t *_tileset;
	extern assets::tilemap_t *_tilemap;


	void load();
	void startup();
	void shutdown();
	void update();
	void render();

}// namespace game