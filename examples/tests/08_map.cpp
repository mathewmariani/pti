// engine
#include "pti.h"

// cute
#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

// stl
#include <string>
#include <vector>

pti_bank_t bank;

pti_tileset_t __create_tileset(const std::string &path) {
	ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);
	ase_tileset_t ase_tileset = ase->tileset;

	/* allocate tileset data */
	const size_t size = (ase_tileset.tile_w * (ase_tileset.tile_h * ase_tileset.tile_count)) * sizeof(ase_color_t);
	void *pixels = pti_alloc(&bank, size);

	memcpy(pixels, ase_tileset.pixels, size);

	/* release cute resources. */
	cute_aseprite_free(ase);

	return (pti_tileset_t) {
			.count = (int32_t) ase_tileset.tile_count,
			.width = (int16_t) ase_tileset.tile_w,
			.height = (int16_t) ase_tileset.tile_h,
			.pixels = pixels,
	};
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

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.cleanup_cb = cleanup,
			.frame_cb = frame,
			.memory_size = _pti_kilobytes(1024),
			.window =
					(pti_window) {
							.name = "pti - tilemap",
							.width = 240 * 2,
							.height = 136 * 2,
							.flags = PTI_SCALE2X,
					},
	};
}

static pti_tileset_t tileset;
static pti_tilemap_t tilemap;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	tilemap = __create_tilemap("assets/tilemap.ase");
	tileset = __create_tileset("assets/tilemap.ase");
	pti_load_bank(&bank);

	pti_clip(0, 0, 240 * 2, 136 * 2);
}

static void cleanup(void) {}

#include <math.h>

float t = 0.0f;
static void frame(void) {
	t += (1 / 60.0f);
	pti_camera((int) (100.0f * sinf(t)), 0);
	pti_cls(0x00000000);
	pti_map(&tilemap, &tileset, 0, 0);
}