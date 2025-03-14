// engine
#include "pti.h"

// cute
#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

pti_bank_t bank;

pti_bitmap_t __create_bitmap(const char *path) {
	ase_t *ase = cute_aseprite_load_from_file(path, NULL);

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
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(256),
			.window =
					(pti_window) {
							.width = 128,
							.height = 128,
							.flags = PTI_SCALE3X,
					}};
}

static pti_bitmap_t bitmap;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(256));
	bitmap = __create_bitmap("assets/font.ase");
	pti_load_bank(&bank);

	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

static void frame(void) {
	// pti_cls(0xff5d275d);
	pti_print(&bitmap, "0123456789", 0, 0);
	pti_print(&bitmap, "HelloWorld", 0, 16);
	pti_print(&bitmap, "Quick brown fox", 0, 32);
	pti_print(&bitmap, "jumps over the", 0, 48);
	pti_print(&bitmap, "lazy dog", 0, 64);
}