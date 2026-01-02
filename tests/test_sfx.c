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

pti_audio_t tone;
bool playing = false;

static void init(void) {
	// graphics state:
	pti_clip(0, 0, 128, 128);

	tone.num_frames = 44100;// 1 second
	tone.num_channels = 1;
	tone.samples = malloc(tone.num_frames * sizeof(float));

	float frequency = 440.0f;
	float amplitude = 0.3f;
	float phase = 0.0f;

	for (int i = 0; i < tone.num_frames; i++) {
		tone.samples[i] = sinf(phase) * amplitude;
		phase += 2.0f * 3.14159265f * frequency / 44100.0f;
		if (phase >= 2.0f * 3.14159265f) phase -= 2.0f * 3.14159265f;
	}
}

static void cleanup(void) {
	free(tone.samples);
}

static void frame(void) {
	pti_rectf(0, 0, 128, 128, 0xFF7F00FF);
	if (pti_down(PTI_LEFT) && pti_pressed(PTI_LEFT)) {
		pti_sfx(&tone, 0, 0);
	}
}