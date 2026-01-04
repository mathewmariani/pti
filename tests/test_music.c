// stdlib
#include <math.h>
#include <stdlib.h>

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
			.memory_size = _pti_megabytes(5),
			.width = 128,
			.height = 128,
	};
}

static pti_bitmap_t bitmap;
static pti_sound_t tone;
static pti_sound_t track;
bool playing = false;

static void init(void) {
	pti_bank_init(&bank, _pti_megabytes(5));
	init_assets(&bank);
	bitmap = create_bitmap("assets/font.ase");
	tone = create_sine_tone(440.0f, 0.3f, 0.5f, 44100.0f, 1);
	track = create_sfx("assets/track-main.ogg");

	pti_set_font(&bitmap);
	pti_load_bank(&bank);
}

static void cleanup(void) {
	free(tone.samples);
}

char *channel_str[PTI_NUM_CHANNELS] = {
		"CH..0: ",
		"CH..1: ",
		"CH..2: ",
		"CH..3: ",
};

static void frame(void) {
	pti_cls(0xFF7F00FF);
	if (pti_down(PTI_LEFT) && pti_pressed(PTI_LEFT)) {
		playing = !playing;
		if (playing) {
			pti_music(&track);
		} else {
			pti_music(NULL);
		}
	}
	if (pti_down(PTI_RIGHT) && pti_pressed(PTI_RIGHT)) {
		pti_sfx(&tone, -1, 0);
	}

	pti_color(0xffffffff);
	pti_print("PRESS LEFT FOR MUSIC", 4, 4);
	pti_print("PRESS RIGHT FOR SFX", 4, 12);

	for (int i = 0; i < PTI_NUM_CHANNELS; i++) {
		pti_print(channel_str[i], 4, (i * 8) + 32);
		pti_print(pti_stat(PTI_STATTYPE_SFX_CHANNEL0 + i).ptr ? "PLAYING" : "SILENT", 40, (i * 8) + 32);
	}
}