// engine
#include "pti.h"

// tests
#include "assets.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(0),
			.width = 128,
			.height = 128,
	};
}

static void init(void) {}
static void cleanup(void) {}

static void frame(void) {
	pti_cls(0x00000000);
	if (pti_down(PTI_LEFT)) {
		pti_quit();
	}
}
