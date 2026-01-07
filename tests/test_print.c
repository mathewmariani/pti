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

	pti_set_palette(&sweetie16);
	pti_set_font(&bitmap);
	pti_load_bank(&bank);
}

static void cleanup(void) {}

char *btn_str[5] = {
		"0123456789",
		"HelloWorld",
		"Quick brown fox",
		"jumps over",
		"the lazy dog",
};

static void frame(void) {
	for (int i = 0; i < 5; i++) {
		pti_color(i);
		pti_print(btn_str[i], 4, (i * 8) + 4);
	}
}