// engine
#include "../engine/engine_entity.h"
#include "../engine/engine_gfx.h"
#include "../engine/engine_input.h"
#include "../engine/engine_platform.h"
#include "../engine/engine_types.h"

// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// sweetie16 palette
static uint32_t pal[] = {
    0xff1a1c2c, 0xff5d275d, 0xffb13e53, 0xffef7d57, 0xffffcd75, 0xffa7f070,
    0xff38b764, 0xff257179, 0xff29366f, 0xff3b5dc9, 0xff41a6f6, 0xff73eff7,
    0xfff4f4f4, 0xff94b0c2, 0xff566c86, 0xff333c57,
};

// (4 x 4):
static uint16_t tetromino[7] = {
    0b0010001000100010, 0b0010011000100000, 0b0000011001100000,
    0b0010011001000000, 0b0100011000100000, 0b0100010001100000,
    0b0010001001100000,
};

// FIXME: standard tetris board in (10x18) might be time to switch to boolean
// array (8 x 16):
static uint8_t grid[] = {
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b01111111,
};

int8_t n = 0;
int8_t x = 0;
int8_t y = 0;

uint32_t piece;
int8_t lines[4] = {-1, -1, -1, -1};

static void nb_init(void) {
#include "../spritesheet.h"
  gfx_decode_spritesheet(&rom_spritesheet_h[0], sizeof(rom_spritesheet_h));

#include "../font.h"
  gfx_decode_font(&rom_font_h[0], sizeof(rom_font_h));

  // gfx state
  // nb_dither(0x5a5a);
  nb_dither(0x0000);
  nb_clip(0, 0, 128, 128);
  nb_keycolor(0x00000000);

  // game state:
  x = 2;
  y = 0;
  piece = tetromino[rand() % 7];
}

#define field_width (8)
#define field_height (16)
#define piece_size (4)

#define __byte_to_binary(b)                                                    \
  (((b)&0x80) ? '1' : '0'), (((b)&0x40) ? '1' : '0'),                          \
      (((b)&0x20) ? '1' : '0'), (((b)&0x10) ? '1' : '0'),                      \
      (((b)&0x08) ? '1' : '0'), (((b)&0x04) ? '1' : '0'),                      \
      (((b)&0x02) ? '1' : '0'), (((b)&0x01) ? '1' : '0')
#define print_byte(b) printf("0b%c%c%c%c%c%c%c%c\n", __byte_to_binary(b))

// NOTE: p should already be rotated before this function is called
static inline bool does_piece_fit(uint32_t piece, uint8_t x, uint8_t y) {
  // early check:
  if (x < 0 || (x + 4) > field_width || y < 0 || (y + 4) > field_height) {
    return false;
  }
  uint8_t *grid_ptr = &grid[0];
  for (int8_t j = 3; j >= 0; j--) {

    // 0b0001001001001000
    // 0b0000000000000001
    uint8_t nibble = (piece >> (j * 4)) & 0xf;
    for (int8_t i = 3; i >= 0; i--) {
      uint8_t mask = (nibble << (4 - x));
      uint8_t row = *(grid_ptr + y + (4 - j));
      if (((row & mask) & mask) != 0) {
        return false;
      }
    }
  }
  return true;
}

int8_t timer = 0;

static void nb_step(void) {

  // == logic:===============================================================

  if (timer < 24) {
    timer += 1;
    return;
  }

  timer = 0;

  int8_t i = 0;
  int8_t j = 0;
  uint8_t *grid_ptr = &grid[0];

  // handle input:
  if (nb_down(NB_LEFT)) {
    x--;
  } else if (nb_down(NB_RIGHT)) {
    x++;
  }

  // move piece:
  if (does_piece_fit(piece, x, y)) {
    y++;
  } else {
    // lock piece to grid:

    for (j = 3; j >= 0; j--) {
      uint8_t nibble = (piece >> (j * 4)) & 0xf;
      uint8_t mask = (nibble << (4 - x));
      *(grid_ptr + y + (4 - j - 1)) |= mask;
    }

    // FIXME: we only need to check lines in our current area (y -> y+4)
    // check for lines:
    for (j = 0, i = 0; j < 16; j++) {
      if ((*(grid_ptr + j) & 0xff) == 0xff) {
        lines[i++] = j;
      }
    }

    x = 2, y = 0;
    piece = tetromino[rand() % 7];
  }

  nb_cls(pal[0]);

  // draw grid:
  for (j = 0; j < 16; j++) {
    for (i = 0; i < 8; i++) {
      if ((*(grid_ptr + j) & (0x80 >> i)) == 0) {
        nb_spr(5, i * 8, j * 8, 1, 1);
      } else {
        nb_spr(4, i * 8, j * 8, 1, 1);
      }
    }
  }

  // draw tetromino:
  for (j = 0; j < 4; j++) {
    for (i = 0; i < 4; i++) {
      if (((piece << (i + j * 4)) & 0x8000) != 0) {
        nb_spr(4, (x + i) * 8, (y + j) * 8, 1, 1);
      }
    }
  }

  // TODO: remove the lines from grid and array.

  nb_clamp(x, 0, 4);
}