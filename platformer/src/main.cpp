#include "pti.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

namespace {
	// sweetie16 palette
	constexpr unsigned int pal[] = {
			0xff1a1c2c,
			0xff5d275d,
			0xffb13e53,
			0xffef7d57,
			0xffffcd75,
			0xffa7f070,
			0xff38b764,
			0xff257179,
			0xff29366f,
			0xff3b5dc9,
			0xff41a6f6,
			0xff73eff7,
			0xfff4f4f4,
			0xff94b0c2,
			0xff566c86,
			0xff333c57,
	};
}// namespace

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc){
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(1024), /* 256KB */
			.window =
					(pti_window){
							.name = "pti - platformer",
							.width = 240,
							.height = 135,
							.flags = PTI_SCALE2X,
					},
	};
}

#include "game.h"

static void init(void) {
	/* graphics state: */
	pti_dither(0x5a5a);
	pti_clip(0, 0, 240, 135);

	game::startup();
}

static void cleanup(void) {
	game::shutdown();
}

static void frame(void) {
	game::update();
	game::render();
}
