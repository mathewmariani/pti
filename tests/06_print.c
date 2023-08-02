// engine
#include "pti.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc){
			.init_cb = init,
			.cleanup_cb = cleanup,
			.frame_cb = frame,
			.memory_size = _pti_kilobytes(256),
			.window =
					(pti_window){
							.width = 128,
							.height = 128,
							.flags = PTI_SCALE3X,
					},
			.spritesheet =
					(pti_bitmap){
							.width = 64,
							.height = 64,
					},
			.font_atlas =
					(pti_bitmap){
							.width = 96,
							.height = 182,
					},
	};
}
static void init(void) {
	pti_load_bitmap("spritesheet.png");
	pti_load_font("font.png");
	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

static void frame(void) {
	// pti_cls(0xff5d275d);
	pti_print("0123456789", 0, 0);
	pti_print("HelloWorld", 0, 16);
	pti_print("Quick brown fox", 0, 32);
	pti_print("jumps over the", 0, 48);
	pti_print("lazy dog", 0, 64);
}