#pragma once

#include <string>
#include <vector>

#include "pti.h"

namespace assets {

	typedef pti_tileset_t tileset_t;
	typedef pti_tilemap_t tilemap_t;

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
	tileset_t *tileset(const std::string &path);
	tilemap_t *tilemap(const std::string &path);

}// namespace assets