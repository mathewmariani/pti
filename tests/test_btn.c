// engine
#include "pti.h"

// tests
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

	pti_set_font(&bitmap);
	pti_load_bank(&bank);
}

static void cleanup(void) {}

char *btn_str[PTI_BUTTON_COUNT] = {
		"PTI_LEFT",
		"PTI_RIGHT",
		"PTI_UP",
		"PTI_DOWN",
		"PTI_A",
		"PTI_B",
		"PTI_X",
		"PTI_Y",
		"PTI_DBG",
};

static void frame(void) {
	uint32_t color;
	for (int i = 0; i < PTI_BUTTON_COUNT; i++) {
		if (pti_down((pti_button) i)) {
			color = sweetie16[1];
		} else if (pti_pressed((pti_button) i)) {
			color = sweetie16[2];
		} else if (pti_released((pti_button) i)) {
			color = sweetie16[3];
		} else {
			color = sweetie16[0];
		}
		pti_color(color);
		pti_print(btn_str[i], 4, (i * 8) + 4);
	}
}