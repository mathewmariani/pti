// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// engine
#include "pti.h"

// sweetie16 palette
uint32_t pal[] = {
    0xff1a1c2c, 0xff5d275d, 0xffb13e53, 0xffef7d57, 0xffffcd75, 0xffa7f070,
    0xff38b764, 0xff257179, 0xff29366f, 0xff3b5dc9, 0xff41a6f6, 0xff73eff7,
    0xfff4f4f4, 0xff94b0c2, 0xff566c86, 0xff333c57,
};

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

#define PTI_ARGB(a, r, g, b)                                                   \
  (uint32_t)(((uint8_t)(a) << 24) | ((uint8_t)(r) << 16) |                     \
             ((uint8_t)(g) << 8) | ((uint8_t)(b) << 0))

#define PTI_RGBA(r, g, b, a)                                                   \
  (uint32_t)(((uint8_t)(a) << 24) | ((uint8_t)(r) << 16) |                     \
             ((uint8_t)(g) << 8) | ((uint8_t)(b) << 0))

pti_desc pti_main(int argc, char *argv[]) {
  return (pti_desc){
      .init_cb = init,
      .cleanup_cb = cleanup,
      .frame_cb = frame,
      .memory_size = _pti_kilobytes(128),
      .window =
          (pti_window){
              .name = "pti - rosecurve",
              .width = 128,
              .height = 128,
              .flags = PTI_SCALE3X,
          },
  };
}

static void init(void) {
  // gfx state
  // pti_dither(0x5a5a);
  pti_dither(0xeae0);
  pti_clip(0, 0, 128, 128);
}

static void cleanup(void) {}

float t = 0.0f;
static void frame(void) {
  int a = 32;
  int n = 5;
  int b = 16;

  float rotation = sinf(t) + t;
  float growth = sinf(t) + t;
  float offset = 0.2f;

  const int width = 128;
  const int height = 128;
  const int cx = width / 2;
  const int cy = width / 2;

  for (int j = 0; j < width; j++) {
    for (int i = 0; i < height; i++) {
      // center:
      int x = cx - i;
      int y = cy - j;

      // rosecurve:
      float theta = atan2f(y, x); // + rotation;
      float d = sqrtf(x * x + y * y);
      float r = a * cosf(n * theta);

      // color:
      unsigned char c = (unsigned char)(r + d - growth) / 16;
      unsigned char low = (unsigned char)(c + offset) % 16;
      unsigned char high = (unsigned char)(c - offset) % 16;
      pti_pset(i, j, ((unsigned long int)pal[low] << 32) | pal[high]);
    }
  }
  t += 1.0f / 30.0f;
}