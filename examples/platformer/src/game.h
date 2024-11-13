#pragma once

#include "lib/assets.h"

namespace game {

	extern pti_tileset_t *_tileset;
	extern pti_tilemap_t *_tilemap;

	void load();
	void startup();
	void shutdown();
	void update();
	void render();

}// namespace game