/*
 * This is a tool to convert PNG files into a
 * simple Palette-based C array. It uses the standard
 * library for simplicity.
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdint.h>

#define RGB(r, g, b)                                                           \
  (uint32_t)(((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | ((uint8_t)(b) << 0))

int main(int argc, char **argv) {
  int w, h, comp, x, y;

  uint32_t *pixels;
  pixels =
      (uint32_t *)stbi_load("maps/13x0.png", &w, &h, &comp, STBI_rgb_alpha);

  if (pixels == NULL) {
    printf("Failed to load asset 'doom-grunt.png'\n");
    return 0;
  }

  printf("Loaded file 'doom-grunt.png', %ix%i pixels\n", w, h);

  FILE *file;
  file = fopen("../13x0d.h", "w");

  fprintf(file, "#ifndef ROM_SPRITESHEET_H\n");
  fprintf(file, "#define ROM_SPRITESHEET_H\n\n");
  fprintf(file, "/* This file is generated automatically via `spritegen.c` in "
                "tools */\n\n");
  fprintf(file, "#define ROM_SPRITESHEET_WIDTH %i\n", w);
  fprintf(file, "#define ROM_SPRITESHEET_HEIGHT %i\n", h);
  fprintf(file, "const unsigned int rom_spritesheet_h[] = {\n");

  // int n = w * h * sizeof(uint32_t);
  // for (int i = 0; i < n; i += 4) {
  //   uint8_t b = pixels[i];
  //   pixels[i] = pixels[i + 2];
  //   pixels[i + 2] = b;
  // }

  uint32_t c;
  uint8_t r, g, b;
  for (y = 0; y < h; y++) {
    fprintf(file, "    ");
    for (x = 0; x < w; x++) {
      c = pixels[x + y * w];
      r = ((c >> 0) & 0x000000ff);
      g = ((c >> 8) & 0x000000ff);
      b = ((c >> 16) & 0x000000ff);
      fprintf(file, "0x%06x, ", RGB(r, g, b));
    }
    fprintf(file, "\n");
  }

  fprintf(file, "};\n");
  fprintf(file, "#endif\n");
  fclose(file);
  return 0;
}