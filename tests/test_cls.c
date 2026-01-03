// stdlib
#include <math.h>
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
			.width = 128,
			.height = 128,
	};
}

static void init(void) {
	// graphics state:
	pti_clip(0, 0, 128, 128);
}

#define CLAMP01(x) (_pti_clamp(x, 0.0f, 1.0f))

static void cleanup(void) {}

static void frame(void) {
	static float r = 1.0f;
	static float g = 0.0f;
	static float b = 0.0f;
	static int phase = 0;

	const float speed = 0.01f;

	switch (phase) {
		case 0:
			g += speed;
			r -= speed;
			if (g >= 1.0f) {
				g = 1.0f;
				r = 0.0f;
				phase = 1;
			}
			break;

		case 1:
			b += speed;
			g -= speed;
			if (b >= 1.0f) {
				b = 1.0f;
				g = 0.0f;
				phase = 2;
			}
			break;

		case 2:
			r += speed;
			b -= speed;
			if (r >= 1.0f) {
				r = 1.0f;
				b = 0.0f;
				phase = 0;
			}
			break;
	}

	uint8_t R = (uint8_t) (CLAMP01(r) * 255.0f);
	uint8_t G = (uint8_t) (CLAMP01(g) * 255.0f);
	uint8_t B = (uint8_t) (CLAMP01(b) * 255.0f);
	uint8_t A = 255;

	pti_cls(PTI_RGBA(R, G, B, A));
}