// engine
#include "pti.h"

#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

#include <string>
#include <vector>

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

struct sprite_t {
	int width;
	int height;
	int frames;
	std::vector<float> durr;
	void *pixels;
};

pti_bank_t bank;

sprite_t __create_sprite(const std::string &path) {
	ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

	/* allocate sprite data */
	const size_t size = ase->w * ase->h * sizeof(ase_color_t);
	char *pixels = (char *) pti_bank_push(&bank, size * ase->frame_count);

	sprite_t sprite = {
			.width = ase->w,
			.height = ase->h,
			.frames = ase->frame_count,
			.pixels = pixels,
	};

	// sprite.durr.reserve(ase->frame_count);
	for (int i = 0; i < ase->frame_count; ++i, pixels += size) {
		ase_frame_t *frame = ase->frames + i;
		memcpy(pixels, frame->pixels, size);
		// sprite.durr.emplace_back(frame->duration_milliseconds);
	}

	cute_aseprite_free(ase);

	return sprite;
}

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.cleanup_cb = cleanup,
			.frame_cb = frame,
			.memory_size = _pti_kilobytes(1024),
			.window =
					(pti_window) {
							.name = "pti - sprites",
							.width = 128,
							.height = 128,
							.flags = PTI_SCALE3X,
					},
	};
}

static sprite_t sprite;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	sprite = __create_sprite("dog.ase");
	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

float t = 0.0f;
static void frame(void) {
	t += (1 / 60.0f);

	pti_cls(0x00000000);
	pti_plot(sprite.pixels, 0, 0, 0, 16, 16, false, false);
}
