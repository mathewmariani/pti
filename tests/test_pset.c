// stdlib
#include <math.h>
#include <stdlib.h>

// engine
#include "pti.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

#define PTI_ARGB(a, r, g, b)                                    \
	(uint32_t) (((uint8_t) (a) << 24) | ((uint8_t) (r) << 16) | \
				((uint8_t) (g) << 8) | ((uint8_t) (b) << 0))

#define PTI_RGBA(r, g, b, a)                                    \
	(uint32_t) (((uint8_t) (a) << 24) | ((uint8_t) (r) << 16) | \
				((uint8_t) (g) << 8) | ((uint8_t) (b) << 0))

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
}
static void cleanup(void) {}

static void frame(void) {
	int i, x, y, r;
	for (i = 0; i < 200; i++) {
		x = rand() % 128;
		y = rand() % 128;

		unsigned int c = PTI_RGBA(rand() % 0xff, rand() % 0xff, rand() % 0xff, 0xff);
		pti_pset(x, y, c);
	}
}