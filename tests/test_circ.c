// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// engine
#include "pti.h"

#define PTI_ARGB(a, r, g, b)                                    \
	(uint32_t) (((uint8_t) (a) << 24) | ((uint8_t) (r) << 16) | \
				((uint8_t) (g) << 8) | ((uint8_t) (b) << 0))

#define PTI_RGBA(r, g, b, a)                                    \
	(uint32_t) (((uint8_t) (a) << 24) | ((uint8_t) (r) << 16) | \
				((uint8_t) (g) << 8) | ((uint8_t) (b) << 0))

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
			.window =
					(pti_window) {
							.width = 128,
							.height = 128,
							.flags = PTI_SCALE3X,
					},
	};
}

static void init(void) {
	// graphics state:
	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

static void frame(void) {
	const int width = 128;
	const int height = 128;

	unsigned int c = PTI_RGBA(rand() % 0xff, rand() % 0xff, rand() % 0xff, 0xff);
	pti_circ(rand() % width, rand() % height, rand() % width, c);
}