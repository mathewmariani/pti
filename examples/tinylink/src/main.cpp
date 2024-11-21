// engine
#include "pti.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(4096),
			.window =
					(pti_window) {
							.name = "pti - tinylink",
							.width = 240,
							.height = 136,
							.flags = PTI_SCALE2X,
					},
	};
}

#include "game.h"

TL::Game game;

void init() {
	// pti_load_bitmap("spritesheet.png");
	// pti_load_font("font.png");
	// pti_load_map("map.png");
	pti_camera(0, 0);
	pti_clip(0, 0, 240, 136);
	game.startup();
}

void cleanup() {
	game.shutdown();
}

void frame() {
	game.update();
	game.render();
}