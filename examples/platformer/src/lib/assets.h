#pragma once

#include <string>
#include <vector>

#include "pti.h"

namespace assets {

	struct sprite_t {
		int width;
		int height;
		int frames;
		std::vector<float> durr;
		void *pixels;
	};

	void init();
	void reload();

	sprite_t *sprite(const std::string &path);
	pti_tileset_t *tileset(const std::string &path);
	pti_tilemap_t *tilemap(const std::string &path);

}// namespace assets