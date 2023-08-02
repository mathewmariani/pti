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
              .name = "pti - sprites",
              .width = 128,
              .height = 128,
              .flags = PTI_SCALE3X,
          },
  };
}

pti_sprite *spitter = NULL;

static void init(void) {
  /* NOTE: this allocates memory */
  spitter = pti_sprite_create("spitter.ase");
  pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

#include <math.h>

float t = 0.0f;
static void frame(void) {
  t += (1 / 60.0f);
  pti_camera(10.0f * sinf(t), 10.0f * cosf(t));
  pti_cls(0x00000000);
  pti_plot_sprite(spitter, 2, 0, 0, false, false);
}
