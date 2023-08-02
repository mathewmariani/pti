#include "assets.h"

#include <unordered_map>

#include "pti.h"

#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

namespace assets {

	static std::unordered_map<std::string, sprite_t> _sprite_cache;
	static std::unordered_map<std::string, tileset_t> _tileset_cache;
	static std::unordered_map<std::string, tilemap_t> _tilemap_cache;

	sprite_t __create_sprite(const std::string &path) {
		ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

		/* allocate sprite data */
		const size_t size = ase->w * ase->h * sizeof(ase_color_t);
		char *pixels = (char *) _pti_alloc(size * ase->frame_count);

		sprite_t sprite = {
				.width = ase->w,
				.height = ase->h,
				.frames = ase->frame_count,
				.pixels = pixels,
		};

		// sprite.durr.reserve(ase->frame_count);
		for (int i = 0; i < ase->frame_count; ++i, pixels += size) {
			ase_frame_t *frame = ase->frames + i;
			memcpy(pixels, frame->pixels, size);
			// sprite.durr.emplace_back(frame->duration_milliseconds);
		}

		cute_aseprite_free(ase);

		return sprite;
	}

	tileset_t __create_tileset(const std::string &path) {
		ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

		ase_tileset_t tileset = ase->tileset;

		const size_t size = (tileset.tile_w * (tileset.tile_h * tileset.tile_count)) * sizeof(ase_color_t);
		void *pixels = _pti_alloc(size);

		memcpy(pixels, tileset.pixels, size);

		/* release cute resources. */
		cute_aseprite_free(ase);

		return (tileset_t){
				.width = tileset.tile_w,
				.height = tileset.tile_h,
				.tiles = tileset.tile_count,
				.pixels = pixels,
		};
	}

	tilemap_t __create_tilemap(const std::string &path) {
		ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

		tilemap_t tilemap;
		for (int i = 0; i < ase->frame_count; ++i) {
			ase_frame_t *frame = ase->frames + i;
			for (int j = 0; j < frame->cel_count; ++j) {
				ase_cel_t *cel = frame->cels + j;
				if (cel->is_tilemap) {
					const size_t size = cel->w * cel->h * sizeof(int);
					void *tiles = _pti_alloc(size);
					memcpy(tiles, cel->tiles, size);
					tilemap = (tilemap_t){
							.width = cel->w,
							.height = cel->h,
							.tiles = tiles,
					};
				}
			}
		}

		/* release cute resources. */
		cute_aseprite_free(ase);

		return tilemap;
	}

	// << tilemap end

	const sprite_t *sprite(const std::string &path) {
		if (_sprite_cache.find(path) == _sprite_cache.end()) {
			_sprite_cache.emplace(std::make_pair(path, __create_sprite(path)));
		}
		return &_sprite_cache[path];
	}

	tileset_t *tileset(const std::string &path) {
		if (_tileset_cache.find(path) == _tileset_cache.end()) {
			_tileset_cache.emplace(std::make_pair(path, __create_tileset(path)));
		}
		return &_tileset_cache[path];
	}

	tilemap_t *tilemap(const std::string &path) {
		if (_tilemap_cache.find(path) == _tilemap_cache.end()) {
			_tilemap_cache.emplace(std::make_pair(path, __create_tilemap(path)));
		}
		return &_tilemap_cache[path];
	}
}// namespace assets