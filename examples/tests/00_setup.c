// engine
#include "pti.h"

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
  return (pti_desc){.init_cb = init,
                    .cleanup_cb = cleanup,
                    .frame_cb = frame,
                    .memory_size = _pti_kilobytes(256),
                    .window = (pti_window){
                        .name = "pti - window",
                        .width = 128,
                        .height = 128,
                        .flags = PTI_SCALE3X,
                    }};
}

void init() {}
void cleanup() {}
void frame() {}