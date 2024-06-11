// engine
#include "pti.h"

// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
  return (pti_desc){
      .init_cb = init,
      .cleanup_cb = cleanup,
      .frame_cb = frame,
      .memory_size = _pti_kilobytes(256),
      .window =
          (pti_window){
              .width = 144,
              .height = 128,
              .flags = PTI_SCALE4X,
          },
      .spritesheet =
          (pti_bitmap){
              .width = 64,
              .height = 64,
          },
      .font_atlas =
          (pti_bitmap){
              .width = 96,
              .height = 182,
          },
  };
}

enum {
  field_width = 10,
  field_height = 20,
  piece_width = 4,
  piece_height = 4,
};

// (4 x 4):
static int tetromino[7][16] = {
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
};

typedef struct {
  int x; // x-position
  int y; // y-position
  int r; // rotation
  int n; // piece
} piece_t;

static struct {
  int t;         // timer
  piece_t curr;  // current piece
  piece_t next;  // next piece
  int grid[200]; // grid

  bool has_lines;
  int clear_count;
  int to_clear[4];

  bool game_over;

  int _score;
  int score;
  int lines;
  int level;
} state;

static inline piece_t new_piece() {
  int n = rand() % 7;
  return (piece_t){.x = 3, .y = 0, .r = 0, .n = n};
}

static void reset() {
  memset(&state.grid[0], 0, sizeof(int) * 200);
  memset(&state.to_clear[0], 0, sizeof(int) * 4);
  state.clear_count = 0;
  state.t = 0;

  state.curr = new_piece();
  state.next = new_piece();

  state.score = 0;
  state.lines = 0;
  state.level = 0;

  state.game_over = false;
  state.has_lines = false;
}

static void init(void) {
  pti_load_bitmap("spritesheet.png");
  pti_load_font("font.png");

  // gfx state
  pti_dither(0x0000);
  pti_clip(0, 0, 144, 128);
  pti_colorkey(0x00000000);

  reset();
}

static void cleanup(void) {}

static int get_score(int n, int level) {
  switch (n) {
  case 1:
    return 40 * (level + 1);
  case 2:
    return 100 * (level + 1);
  case 3:
    return 300 * (level + 1);
  case 4:
    return 1200 * (level + 1);
  default:
    return 0;
  }
}

static int rotate(int x, int y, int r) {
  r = r % 4;
  switch (r) {
  case 0:
    return y * 4 + x;
  case 1:
    return 12 + y - (x * 4);
  case 2:
    return 15 - (y * 4) - x;
  case 3:
    return 3 - y + (x * 4);
  }
  return 0;
}

// NOTE: p should already be rotated before this function is called
static bool does_piece_fit(const piece_t *p, int ox, int oy, int or) {
  int x = 0;
  int y = 0;
  int i = 0;
  int j = 0;
  int cx = 0;
  int cy = 0;

  int *t = tetromino[p->n];
  for (y = 0; y < piece_height; y++) {
    for (x = 0; x < piece_width; x++) {
      i = rotate(x, y, p->r + or);
      if (*(t + i) == 0) {
        continue;
      }

      cx = p->x + x + ox;
      cy = p->y + y + oy;

      j = cx + field_width * cy;

      if ((cx >= 0) && (cx < 10) && (cy >= 0) && (cy < 20)) {
        if (state.grid[j] != 0) {
          return false;
        }
      } else {
        return false;
      }
    }
  }
  return true;
}

static void copy_to_grid(const piece_t *p) {
  int x = 0;
  int y = 0;
  int i = 0;
  int j = 0;
  int cx = 0;
  int cy = 0;

  int *t = tetromino[p->n];
  for (y = 0; y < piece_height; y++) {
    for (x = 0; x < piece_width; x++) {
      cx = p->x + x;
      cy = p->y + y;
      i = rotate(x, y, p->r);
      if (*(t + i) != 0) {
        state.grid[cx + field_width * cy] = p->n + 1;
      }
    }
  }
}

static void draw_tetromino(const piece_t *p, int x, int y) {
  // draw current tetromino:
  int *t = tetromino[p->n];
  for (int j = 0; j < piece_width; j++) {
    for (int i = 0; i < piece_height; i++) {
      if (*(t + rotate(i, j, p->r)) == 1) {
        pti_sspr(p->n + 1, (x + i) * 8, (y + j) * 8, 1, 1, false, false);
      }
    }
  }
}

static void handle_movement(piece_t *p) {
  // handle movement:
  if (pti_pressed(PTI_LEFT)) {
    if (does_piece_fit(p, -1, 0, 0)) {
      p->x--;
    }
  } else if (pti_pressed(PTI_RIGHT)) {
    if (does_piece_fit(p, 1, 0, 0)) {
      p->x++;
    }
  } else if (pti_pressed(PTI_UP)) {
    if (does_piece_fit(p, 0, -1, 0)) {
      p->y--;
    }
  } else if (pti_pressed(PTI_DOWN)) {
    if (does_piece_fit(p, 0, 1, 0)) {
      p->y++;
    }
  } else if (pti_pressed(PTI_A)) {
    if (does_piece_fit(p, 0, 0, 1)) {
      p->r++;
    }
  } else if (pti_pressed(PTI_B)) {
    if (does_piece_fit(p, 0, 0, -1)) {
      p->r--;
    }
  }
}

static void frame(void) {
  if (state.game_over) {
    return;
  }

  // current piece
  piece_t *p = &state.curr;

  int i = 0;
  int j = 0;

  handle_movement(p);

  // slowly move the piece down
  if (state.t >= 24) {
    state.t = 0;
    if (does_piece_fit(p, 0, 1, 0)) {
      p->y++;
    } else {
      copy_to_grid(p);

      // check for lines
      int *l = &state.to_clear[0];
      for (j = 0; j < 4; j++) {
        if ((p->y + j) < field_height) {
          bool is_line = true;
          for (i = 0; i < field_width; i++) {
            is_line &= state.grid[i + field_width * (p->y + j)] != 0;
          }

          if (is_line) {
            state.clear_count++;
            state.has_lines = true;
            for (i = 0; i < field_width; i++) {
              state.grid[i + field_width * (p->y + j)] = 8;
            }
            *(l++) = p->y + j;
          }
        }
      }

      // switch to the next piece
      state.curr = state.next;
      state.next = new_piece();

      // update score
      state._score += get_score(state.clear_count, state.level);
      state.level = _pti_min(state.lines / 10, 10);
      state.lines += state.clear_count;

      // check for gameover
      if (!does_piece_fit(&state.curr, 0, 0, 0)) {
        state.game_over = true;
      }
    }
  } else {
    state.t++;
  }

  // graphics:
  pti_cls(0xff0e0f0f);

  // update stats
  if (state.score < state._score) {
    state.score += 5;
  }

  // draw grid:
  int *grid = &state.grid[40];
  for (j = 0; j < field_height - 4; j++) {
    for (i = 0; i < field_width; i++) {
      int n = *(grid + (i + field_width * j));
      if (n > 0) {
        pti_sspr(n, i * 8, j * 8, 1, 1, false, false);
      }
    }
  }

  const piece_t *curr = &state.curr;
  draw_tetromino(curr, curr->x, curr->y - 4);
  draw_tetromino(&state.next, 12, 4);

  // lines:
  if (state.has_lines) {
    state.clear_count = 0;
    state.has_lines = false;
    for (j = 0; j < piece_height; j++) {
      for (i = 0; i < field_width; i++) {
        if (state.to_clear[j] == 0) {
          continue;
        }
        for (int k = state.to_clear[j]; k > 0; k--) {
          state.grid[i + field_width * k] =
              state.grid[i + field_width * (k - 1)];
        }
        state.grid[i] = 0;
      }
      state.to_clear[j] = 0;
    }
  }

  // draw stats
  char str_lines[7];
  char str_score[7];
  char str_level[7];
  sprintf(str_lines, "%06d", state.lines);
  sprintf(str_score, "%06d", state.score);
  sprintf(str_level, "%02d", state.level);

  pti_print("LINES:", 88, 8 /*, 0xff6a6654*/);
  pti_print(str_lines, 88, 16 /*, 0xfff6f6e3*/);

  pti_print("SCORE:", 88, 72 /*, 0xff6a6654*/);
  pti_print(str_score, 88, 80 /*, 0xfff6f6e3*/);
  pti_print("LEVEL:", 88, 96 /*, 0xff6a6654*/);
  pti_print(str_level, 120, 104 /*, 0xfff6f6e3*/);

  if (state.game_over) {
    pti_rect(0, 56, 80, 24, 0xff0e0f0f);
    pti_print("GAME  OVER", 0, 56 /*, 0xff6a6654*/);
    pti_print("PRESS  'R'", 0, 64 /*, 0xfff6f6e3*/);
    pti_print(" TO RESET ", 0, 72 /*, 0xfff6f6e3*/);
  }

  // TODO: remove the lines from grid and array.
}