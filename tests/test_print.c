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
			.memory_size = _pti_kilobytes(256),
			.width = 128,
			.height = 128,
	};
}

static pti_bitmap_t bitmap;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	init_assets(&bank);
	bitmap = create_bitmap("assets/font.ase");

	pti_set_font(&bitmap);
	pti_load_bank(&bank);

	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

static void frame(void) {
	// pti_cls(0xff5d275d);
	pti_print("0123456789", 0, 0);
	pti_print("HelloWorld", 0, 16);
	pti_print("Quick brown fox", 0, 32);
	pti_print("jumps over the", 0, 48);
	pti_print("lazy dog", 0, 64);
}