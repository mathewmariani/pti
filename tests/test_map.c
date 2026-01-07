// engine
#include "pti.h"

// assets
#include "assets.h"
#include "palettes.h"

pti_bank_t bank;

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(1024),
			.width = 240 * 2,
			.height = 136 * 2,
	};
}

static pti_tileset_t tileset;
static pti_tilemap_t tilemap;
static pti_bitmap_t bitmap;
static pti_palette_t palette;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	init_assets(&bank);
	tilemap = create_tilemap("assets/tilemap.ase");
	tileset = create_tileset("assets/tilemap.ase");
	bitmap = create_bitmap("assets/font.ase");

	pti_set_palette(&sweetie16);
	pti_set_tilemap(&tilemap);
	pti_set_tileset(&tileset);
	pti_set_font(&bitmap);
	pti_load_bank(&bank);
}

static void cleanup(void) {}

#include <math.h>

float t = 0.0f;
static void frame(void) {
	t += (1 / 60.0f);
	pti_camera((int) (100.0f * sinf(t)), 0);
	pti_cls(0);
	pti_map(0, 0);
}