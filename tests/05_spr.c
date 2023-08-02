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
							.flags = (PTI_SCALE3X | PTI_FPS30),
					},
			.spritesheet =
					(pti_bitmap){
							.width = 64,
							.height = 64,
							.user_data = NULL,
					},
			.font_atlas =
					(pti_bitmap){
							.width = 64,
							.height = 64,
							.user_data = NULL,
					},
	};
}
static void init(void) {
	pti_load_bitmap("spritesheet.png");
	pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

static void frame(void) {
	pti_cls(0xff5d275d);

	bool flip_x = false, flip_y = false;
	if (pti_down(PTI_LEFT) || pti_down(PTI_RIGHT)) {
		flip_x = !flip_x;
	}
	if (pti_down(PTI_UP) || pti_down(PTI_DOWN)) {
		flip_y = !flip_y;
	}
	// pti_sspr(0, 0, 0, 8, 8, flip_x, flip_y);
}