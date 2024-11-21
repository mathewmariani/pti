#pragma once

#include <string>

typedef struct pti_tilemap_t pti_tilemap_t;
typedef struct pti_tileset_t pti_tileset_t;
typedef struct pti_bitmap_t pti_bitmap_t;

// extern pti_tilemap_t *tilemap;
// extern pti_tileset_t *tileset;
// extern pti_bitmap_t *bitmap_blob;
// extern pti_bitmap_t *bitmap_bramble;
// extern pti_bitmap_t *bitmap_bullet;
// extern pti_bitmap_t *bitmap_circle;
// extern pti_bitmap_t *bitmap_door;
// extern pti_bitmap_t *bitmap_ghostfrog;
// extern pti_bitmap_t *bitmap_heart;
// extern pti_bitmap_t *bitmap_mosquito;
// extern pti_bitmap_t *bitmap_player;
// extern pti_bitmap_t *bitmap_pop;
// extern pti_bitmap_t *bitmap_spitter;

namespace bank {
	void init_bank(void);
	void reload_bank(void);

	pti_bitmap_t *sprite(const std::string &path);
	pti_tileset_t *tileset(const std::string &path);
	pti_tilemap_t *tilemap(const std::string &path);
}// namespace bank
