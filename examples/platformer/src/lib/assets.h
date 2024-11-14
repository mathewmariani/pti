#pragma once

#include <string>
#include <vector>

#include "pti.h"

namespace assets {

	void init();
	void reload();

	pti_bitmap_t *sprite(const std::string &path);
	pti_tileset_t *tileset(const std::string &path);
	pti_tilemap_t *tilemap(const std::string &path);

}// namespace assets