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
			.memory_size = _pti_kilobytes(1024),
			.window =
					(pti_window){
							.name = "pti - tilemap",
							.width = 240 * 2,
							.height = 136 * 2,
							.flags = PTI_SCALE2X,
					},
	};
}

static void init(void) {
	/* NOTE: this allocates memory */
	pti_tilemap_create("tilemap.ase");
	pti_clip(0, 0, 240 * 2, 136 * 2);
}

static void cleanup(void) {}

#include <math.h>

float t = 0.0f;
static void frame(void) {
	t += (1 / 60.0f);
	pti_camera((int) (100.0f * sinf(t)), 0);
	pti_cls(0x00000000);
	pti_plot_tilemap(0, 0);
}