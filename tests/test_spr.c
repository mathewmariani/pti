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
			.width = 128,
			.height = 128,
	};
}

static pti_bitmap_t bitmap;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	init_assets(&bank);
	bitmap = create_bitmap("assets/dog.ase");

	pti_load_bank(&bank);

	pti_set_palette(&sweetie16);
	pti_dither(0x5a5a);
	// pti_dither(0xeae0);
}
static void cleanup(void) {}

static void frame(void) {
	pti_spr(&bitmap, 0, 0, 0, false, false);
}
