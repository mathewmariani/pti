// engine
#include "pti.h"

// cute
#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

// stl
#include <string>
#include <vector>

pti_bank_t bank;

pti_bitmap_t __create_bitmap(const std::string &path) {
	ase_t *ase = cute_aseprite_load_from_file(path.c_str(), NULL);

	/* allocate bitmap data */
	const size_t size = ase->w * ase->h * sizeof(ase_color_t);
	char *pixels = (char *) pti_alloc(&bank, size * ase->frame_count);
	char *pixels_start = pixels;

	for (int i = 0; i < ase->frame_count; ++i, pixels += size) {
		ase_frame_t *frame = ase->frames + i;
		memcpy(pixels, frame->pixels, size);
	}

	/* release cute resources. */
	cute_aseprite_free(ase);

	return (pti_bitmap_t) {
			.frames = (int32_t) ase->frame_count,
			.width = (int16_t) ase->w,
			.height = (int16_t) ase->h,
			.pixels = pixels_start,
	};
}

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.cleanup_cb = cleanup,
			.frame_cb = frame,
			.memory_size = _pti_kilobytes(1024),
			.window =
					(pti_window) {
							.name = "pti - bitmaps",
							.width = 128,
							.height = 128,
							.flags = PTI_SCALE3X,
					},
	};
}

static pti_bitmap_t bitmap;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	bitmap = __create_bitmap("assets/dog.ase");
	pti_load_bank(&bank);

	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

float t = 0.0f;
static void frame(void) {
	t += (1 / 60.0f);

	pti_cls(0x00000000);
	pti_spr(&bitmap, 0, 0, 0, false, false);
}
