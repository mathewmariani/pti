// engine
#include "pti.h"

// tests
#include "assets.h"

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

	pti_clip(0, 0, 240 * 2, 136 * 2);
}
static void cleanup(void) {}

float t = 0.0f;
static void frame(void) {
	t += (1 / 60.0f);

	pti_cls(0x00000000);
	pti_spr(&bitmap, 0, 0, 0, false, false);
}
