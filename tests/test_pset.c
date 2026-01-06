// stdlib
#include <math.h>
#include <stdlib.h>

// engine
#include "pti.h"

// assets
#include "assets.h"
#include "palettes.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(128),
			.width = 128,
			.height = 128,
	};
}

static void init(void) {
	pti_clip(12, 12, 128 - 12, 128 - 12);
	pti_set_palette(&sweetie16);
}

static void cleanup(void) {}

static void frame(void) {
	int i, x, y, r;
	for (i = 0; i < 200; i++) {
		x = rand() % 128;
		y = rand() % 128;
		pti_pset(x, y, rand() % 0x0f);
	}
}