#include <stdio.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PACK_COLOR(r, g, b)                \
  ((r << 24) + (g << 16) + (b << 8) + 255)

static void print_cell(FILE *file, int *grid, int cx, int cy) {
  for (int y = 0; y < 8; ++y) {
    int b = 0b00000000;
    for (int x = 7; x >= 0; --x) {
      int i = (x + (cx * 8)) + 128 * (y + (cy * 8));
      b |= *(grid + i) << x;
    }
    fprintf(file, "0x%02x, ", b);
  }
}

int main(int argc, char** argv) {
  const char *input = argv[1];
  const char *output = argv[2];

  int w, h, comp, i, j;
  unsigned char *pixels = stbi_load(input, &w, &h, &comp, STBI_rgb);

  int b[128 * 64];
  for (i = 0, j = 0;i < (w * h) * 3; i += 3, j++) {
    int rgb = PACK_COLOR(
      *(pixels + i + 0),
      *(pixels + i + 1),
      *(pixels + i + 2));

    *(b + j) = (rgb == 0x000000ff) ? 1 : 0;
  }

  FILE *file = fopen(output, "w");
   
  fprintf(file, "#ifndef NB_FONT_H\n");
  fprintf(file, "#define NB_FONT_H\n\n");
  fprintf(file, "/* This file is generated automatically via `fontgen.c` in tools */\n\n");
  fprintf(file, "#define NB_FONT_WIDTH %i\n", w);
  fprintf(file, "#define NB_FONT_HEIGHT %i\n", h);
  fprintf(file, "const unsigned char nb_font[] = {\n");

  for (i = 0; j < 8; ++j) {
    for (j = 0; i < 16; ++i) {
      fprintf(file, "  ");
      print_cell(file, b, i, j);
      fprintf(file, "\n");
    }
  }
  fprintf(file, "};\n");
  fprintf(file, "#endif\n");
  fclose(file);

  return 0;
}