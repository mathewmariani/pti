/*
 * This is a tool to convert PNG files into a
 * simple Palette-based C array. It uses the standard
 * library for simplicity.
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdint.h>

// static void print_cell(FILE *file, int *grid, int cx, int cy) {
//   for (int y = 0; y < 8; ++y) {
//     int b = 0x00000000;
//     for (int x = 7; x >= 0; --x) {
//       int i = (x + (cx * 8)) + 64 * (y + (cy * 8));
//       b |= *(grid + i) << x;
//     }

//     fprintf(file, "0x%08x, ", b);
//   }

//   printf("we broke out of here\n");
// }

static void print_cell(FILE *file, int *grid, int cx, int cy) {
  for (int y = 0; y < 8; ++y) {
    uint32_t b = 0x00000000;

    for (int x = 7; x >= 0; --x) {
      int i = (x + (cx * 8)) + 64 * (y + (cy * 8));
      b |= *(grid + i) << x;
    }

    fprintf(file, "0x%08x, ", b);
  }

  printf("we broke out of here\n");
}

int main(int argc, char **argv) {
  int x, y, w, h, c, i, j;

  stbi_uc *pixels;
  pixels = stbi_load("spritesheet_2.png", &w, &h, &c, STBI_rgb_alpha);

  FILE *file;
  file = fopen("spritesheet_encoded.h", "w");

  fprintf(file, "#ifndef ROM_SPRITESHEET_H\n");
  fprintf(file, "#define ROM_SPRITESHEET_H\n\n");
  fprintf(file, "/* This file is generated automatically via `spritegen.c` in "
                "tools */\n\n");
  fprintf(file, "#define ROM_SPRITESHEET_WIDTH %i\n", w);
  fprintf(file, "#define ROM_SPRITESHEET_HEIGHT %i\n", h);
  fprintf(file, "const unsigned int rom_spritesheet_h[] = {\n");

  for (y = 0; y < h; y++) {
    uint32_t b = 0x00000000;
    fprintf(file, "    ");
    for (x = 0; x < w; x++) {
      i = x + y * w;
      j = i % 8;
      c = (int)((pixels[i * 4] / 256.0f) * 0xf);
      b |= (c << (j * 4));

      if (j == 7) {
        fprintf(file, "0x%08x, ", b);
        b = 0x00000000;
      }
    }
    fprintf(file, "\n");
  }

  fprintf(file, "};\n");
  fprintf(file, "#endif\n");
  fclose(file);
  return 0;
}