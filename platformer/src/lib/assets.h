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

	// struct tileset_t {
	//   int width;
	//   int height;
	//   int tiles;
	//   void *pixels;
	// };


	// struct tilemap_t {
	//   int width;
	//   int height;
	//   void *tiles;
	// };

	const sprite_t *sprite(const std::string &path);
	tileset_t *tileset(const std::string &path);
	tilemap_t *tilemap(const std::string &path);

}// namespace assets