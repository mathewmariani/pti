#include "assets.h"

#include <unordered_map>

#include "pti.h"

#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

namespace assets {

	static std::unordered_map<std::string, pti_bitmap_t> _sprite_cache;
	static std::unordered_map<std::string, pti_tileset_t> _tileset_cache;
	static std::unordered_map<std::string, pti_tilemap_t> _tilemap_cache;

	pti_bank_t bank;

	void init() {
		pti_bank_init(&bank, _pti_kilobytes(256));
	}

	void reload() {
		pti_load_bank(&bank);
	}

	pti_bitmap_t __create_bitmap(const std::string &path) {
		auto *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

		/* allocate bitmap data */
		auto size = ase->w * ase->h * sizeof(ase_color_t);
		auto *data = (char *) pti_alloc(&bank, size * ase->frame_count);

		/* initialize */
		pti_bitmap_t bitmap;
		bitmap.frames = (int32_t) ase->frame_count;
		bitmap.width = (int16_t) ase->w;
		bitmap.height = (int16_t) ase->h;
		bitmap.pixels = data;

		for (int i = 0; i < ase->frame_count; ++i, data += size) {
			auto *frame = ase->frames + i;
			memcpy(data, frame->pixels, size);
		}

		/* release cute resources. */
		cute_aseprite_free(ase);

		return bitmap;
	}

	pti_tileset_t __create_tileset(const std::string &path) {
		ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);
		ase_tileset_t ase_tileset = ase->tileset;

		const size_t size = (ase_tileset.tile_w * (ase_tileset.tile_h * ase_tileset.tile_count)) * sizeof(ase_color_t);
		void *pixels = pti_alloc(&bank, size);

		pti_tileset_t tileset;

		/* initialize */
		tileset.count = (int32_t) ase_tileset.tile_count;
		tileset.width = (int16_t) ase_tileset.tile_w;
		tileset.height = (int16_t) ase_tileset.tile_h;
		tileset.pixels = (void *) pti_alloc(&bank, size);

		memcpy(tileset.pixels, ase_tileset.pixels, size);

		/* release cute resources. */
		cute_aseprite_free(ase);

		return tileset;
	}

	pti_tilemap_t __create_tilemap(const std::string &path) {
		ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

		pti_tilemap_t tilemap;
		for (int i = 0; i < ase->frame_count; ++i) {
			ase_frame_t *frame = ase->frames + i;
			for (int j = 0; j < frame->cel_count; ++j) {
				ase_cel_t *cel = frame->cels + j;
				if (cel->is_tilemap) {
					const size_t size = cel->w * cel->h * sizeof(int);
					tilemap.width = (int16_t) cel->w;
					tilemap.height = (int16_t) cel->h;
					tilemap.tiles = (int *) pti_alloc(&bank, size);
					memcpy(tilemap.tiles, cel->tiles, size);
				}
			}
		}

		/* release cute resources. */
		cute_aseprite_free(ase);

		return tilemap;
	}

	pti_bitmap_t *sprite(const std::string &path) {
		if (_sprite_cache.find(path) == _sprite_cache.end()) {
			_sprite_cache.emplace(std::make_pair(path, __create_bitmap(path)));
		}
		return &_sprite_cache[path];
	}

	pti_tileset_t *tileset(const std::string &path) {
		if (_tileset_cache.find(path) == _tileset_cache.end()) {
			_tileset_cache.emplace(std::make_pair(path, __create_tileset(path)));
		}
		return &_tileset_cache[path];
	}

	pti_tilemap_t *tilemap(const std::string &path) {
		if (_tilemap_cache.find(path) == _tilemap_cache.end()) {
			_tilemap_cache.emplace(std::make_pair(path, __create_tilemap(path)));
		}
		return &_tilemap_cache[path];
	}
}// namespace assets