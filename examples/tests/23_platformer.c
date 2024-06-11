// engine
#include "pti.h"
#include "../engine/pti_sprite.h"

#define ENGINE_ENTITY_IMPL
#include "../engine/pti_entity.h"

// stdlib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

#define MAGIC_WIDTH (240)
#define MAGIC_HEIGHT (135)

pti_desc pti_main(int argc, char *argv[]) {
  return (pti_desc){
      .init_cb = init,
      .cleanup_cb = cleanup,
      .frame_cb = frame,
      .memory_size = _pti_kilobytes(256),
      .window =
          (pti_window){
              .name = "pti - platformer",
              .width = 240,
              .height = 135,
              .flags = PTI_SCALE2X,
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

// entity types
void pti_player(pti_entity_event *);

// sweetie16 palette
unsigned int pal[] = {
    0xff1a1c2c, 0xff5d275d, 0xffb13e53, 0xffef7d57, 0xffffcd75, 0xffa7f070,
    0xff38b764, 0xff257179, 0xff29366f, 0xff3b5dc9, 0xff41a6f6, 0xff73eff7,
    0xfff4f4f4, 0xff94b0c2, 0xff566c86, 0xff333c57,
};

int score = 0;

// userdata
pti_sprite *player_sprite = NULL;

static void init(void) {
  // pti_load_bitmap("spritesheet.png");
  pti_load_font("font.png");

  pti_tilemap_create("tilemap_2.ase");

  player_sprite = pti_sprite_create("dog.ase");

  int i, j;

  /* Zero out entities */
  for (i = 0; i < PTI_ENTITIES; i++) {
    pti_entities.all[i] = pti_null_entity;
  }

  /* Zero out tiles with solids along room edges */
  for (i = 0; i < PTI_ROOM_COLS; i++) {
    for (j = 0; j < PTI_ROOM_ROWS; j++) {
      pti_entities.tiles[i][j] = pti_mget(i, j);
    }
  }

  /* Create some entities */
  pti_en_create(pti_player, PTI_ROOM_WIDTH / 2, PTI_ROOM_HEIGHT / 2);

  // graphics state:
  pti_dither(0xeae0);
  pti_clip(0, 0, 240, 135);
  // pti_keycolor(0x00000000);
}

static void cleanup(void) {}

float t = 0.0f;
static void frame(void) {
  int i, j, nx, ny;
  pti_entity_event ev;

  /* run entities */
  ev.type = PTI_EVENTTYPE_UPDATE;
  for (i = 0; i < PTI_ENTITIES; i++) {
    ev.self = pti_entities.all + i;

    if (ev.self->type == NULL) {
      continue;
    }

    ev.self->timer += 1.0f / 60.0f;

    /* get move amount */
    nx = ev.self->sx + ev.self->rx;
    ny = ev.self->sy + ev.self->ry;

    /* try to move */
    if ((ev.self->flags & PTI_ENTITYFLAG_HITS_SOLIDS) != 0) {
      j = nx;
      while (j != 0) {
        ev.self->x += _pti_sign(j);
        if (pti_en_solids(ev.self)) {
          ev.self->sx = ev.self->rx = nx = 0;
          ev.self->x -= _pti_sign(j);
          break;
        }
        j -= _pti_sign(j);
      }

      j = ny;
      while (j != 0) {
        ev.self->y += _pti_sign(j);
        if (pti_en_solids(ev.self)) {
          ev.self->sy = ev.self->ry = ny = 0;
          ev.self->y -= _pti_sign(j);
          break;
        }
        j -= _pti_sign(j);
      }
    } else {
      ev.self->x += nx;
      ev.self->y += ny;
    }

    /* update move remainder */
    ev.self->rx = ev.self->sx + ev.self->rx - nx;
    ev.self->ry = ev.self->sy + ev.self->ry - ny;

    /* call their update event */
    ev.self->type(&ev);
  }

  /* do overlap tests */
  ev.type = PTI_EVENTTYPE_OVERLAP;
  for (i = 0; i < PTI_ENTITIES; i++) {
    ev.self = pti_entities.all + i;
    if (ev.self->type == NULL || ev.self->bw < 0 || ev.self->bh <= 0 ||
        (ev.self->flags & PTI_ENTITYFLAG_OVERLAP_CHECKS) == 0) {
      continue;
    }

    for (j = 0; j < PTI_ENTITIES; j++) {
      ev.other = pti_entities.all + j;
      if (i == j || ev.other->type == NULL || ev.other->bw < 0 ||
          ev.other->bh <= 0 ||
          (ev.other->flags & PTI_ENTITYFLAG_OVERLAP_CHECKS) == 0) {
        continue;
      }
      if (pti_en_overlaps(ev.self, ev.other)) {
        ev.self->type(&ev);
      }
    }
  }

  /* make sure camera is inside the room bounds */
  const int width = MAGIC_WIDTH;
  const int height = MAGIC_HEIGHT;

  //>> =============

  // lissajous curves:
  t += (1 / 30.0f);
  float cx1 = (MAGIC_WIDTH * 0.5f) - sinf(t / 3) * (MAGIC_WIDTH * 0.5f);
  float cy1 = (MAGIC_HEIGHT * 0.5f) - cosf(t / 2) * (MAGIC_HEIGHT * 0.5f);
  float cx2 = (MAGIC_WIDTH * 0.5f) + sinf(t / 3) * (MAGIC_WIDTH * 0.5f);
  float cy2 = (MAGIC_HEIGHT * 0.5f) + cosf(t / 2) * (MAGIC_HEIGHT * 0.5f);

  int x, y;
  for (y = 0; y < MAGIC_HEIGHT; y++) {
    float dy1 = (y - cy1) * (y - cy1);
    float dy2 = (y - cy2) * (y - cy2);
    for (x = 0; x < MAGIC_WIDTH; x++) {
      float dx1 = (x - cx1) * (x - cx1);
      float dx2 = (x - cx2) * (x - cx2);
      int c = (((int)sqrtf(dx1 + dy1) ^ (int)sqrtf(dx2 + dy2)) >> 4);
      if ((c & 1) == 0) {
        if (rand() % 5 == 1) {
          pti_pset(x, y, ((unsigned long int)pal[1] << 32) | pal[2]);
        }
      } else {
        if (rand() % 5 == 1) {
          pti_pset(x, y, ((unsigned long int)pal[3] << 32) | pal[4]);
        }
      }
    }
  }

  //>> =============

  /* adjust camera */
  int cam_x, cam_y;
  pti_get_camera(&cam_x, &cam_y);
  pti_camera(_pti_max(0, _pti_min(PTI_ROOM_WIDTH - width, cam_x)),
             _pti_max(0, _pti_min(PTI_ROOM_HEIGHT - height, cam_y)));

  /* draw tilemap */
  pti_plot_tilemap(0, 0);

  /* draw entities */
  ev.type = PTI_EVENTTYPE_DRAW;
  for (i = 0; i < PTI_ENTITIES; i++) {
    ev.self = pti_entities.all + i;
    if (ev.self->type != NULL) {
      ev.self->type(&ev);
    }
  }

  /* print letters and shit */
  char ui_score[11];
  sprintf(ui_score, "SCORE: %03d", score);
  pti_print(ui_score, 5, 0);
}

/* Begin Entity Types */

typedef enum {
  PLAYER_STATE_NORMAL,
  PLAYER_STATE_JUMP,
  PLAYER_STATE_DEATH,
} player_state_t;

static bool is_grounded(const pti_entity *self) {
  return pti_en_place_meeting(self, 0, 1) ||
         (self->sy >= 0 && (pti_en_place_meeting(self, 0, 1) &&
                            pti_en_place_meeting(self, 0, 1)));
}

static void handle_player_horizontal_movement(pti_entity *self) {
  static const float max_speed = 3;
  static const float accel = 20;
  static const float friction = 15;

  if (pti_down(PTI_LEFT)) {
    _pti_appr(self->sx, -max_speed, accel * PTI_DELTA);
    self->flags |= PTI_ENTITYFLAG_FACING_LEFT;
  } else if (pti_down(PTI_RIGHT)) {
    _pti_appr(self->sx, max_speed, accel * PTI_DELTA);
    self->flags &= ~PTI_ENTITYFLAG_FACING_LEFT;
  } else {
    _pti_appr(self->sx, 0, friction * PTI_DELTA);
  }
}

static void handle_player_vertical_movement(pti_entity *self) {
  static const float max_speed = 3;
  static const float accel = 20;
  static const float friction = 15;
  static const float gravity = 20;

  // static const float phy_vertical_grav_fall = 0;
  // static const float phy_vertical_max = 0;

  static const float phy_vertical_max = 6.0f;
  static const float phy_vertical_grav = 0.24f;
  static const float phy_vertical_grav_fall = 0.4254f;
  static const float phy_vertical_grav_alt = 0.1940f;

  static int hang_time = 0;

  /* simple gravity */
  // if ((self->flags & PTI_ENTITYFLAG_GROUNDED) == 0) {
  //   self->sy = self->sy + gravity;
  //   if (self->sy > max_speed) {
  //     self->sy = max_speed;
  //   }
  // }

  if ((self->flags & PTI_ENTITYFLAG_GROUNDED) == 0) {
    if (self->state == PLAYER_STATE_JUMP) {
      if (self->sy <= -0.5) {
        hang_time = 3;
        self->sy = self->sy + phy_vertical_grav_fall;
      } else {
        if (hang_time > 0) {
          hang_time = hang_time - 1;
          self->sy = 0;
        } else {
          self->sy = self->sy + phy_vertical_grav_fall;
        }
      }
    } else {
      self->sy = self->sy + phy_vertical_grav_fall;
    }
  } else {
    self->sy = 0;
  }

  /* limit: */
  if (self->sy > phy_vertical_max) {
    self->sy = phy_vertical_max;
  }
}

static void handle_player_jump(pti_entity *self) {
  static const float phy_jump_strength = 6.1135f;
  static const float phy_bounce_strength = 4.5535f;

  bool kJump = pti_pressed(PTI_UP);

  /* full jump: */
  if (kJump &&
      ((self->flags & PTI_ENTITYFLAG_GROUNDED) == PTI_ENTITYFLAG_GROUNDED)) {
    self->state = PLAYER_STATE_JUMP;
    self->flags &= ~PTI_ENTITYFLAG_GROUNDED;
    self->sy = -phy_jump_strength;
  }

  /* revert state: */
  if (self->state == PLAYER_STATE_JUMP &&
      ((self->flags & PTI_ENTITYFLAG_GROUNDED) == PTI_ENTITYFLAG_GROUNDED)) {
    self->state = PLAYER_STATE_NORMAL;
  }

  /* variable jump: */
  if (self->state == PLAYER_STATE_JUMP) {
    if (self->sy < -(phy_jump_strength * 0.5)) {
      if (pti_released(PTI_UP)) {
        self->sy = -(phy_jump_strength * 0.5);
      }
    }
  }
}

void pti_player(pti_entity_event *ev) {
  pti_entity *self = ev->self;
  int frame = 0;

  switch (ev->type) {
  case PTI_EVENTTYPE_INIT:
    self->bx = -4;
    self->by = -8;
    self->bw = 8;
    self->bh = 8;
    self->flags = PTI_ENTITYFLAG_OVERLAP_CHECKS | PTI_ENTITYFLAG_HITS_SOLIDS;
    self->state = PLAYER_STATE_NORMAL;
    break;
  case PTI_EVENTTYPE_DESTROY:
    break;
  case PTI_EVENTTYPE_UPDATE: {
    /* ground check */
    if (is_grounded(self)) {
      self->flags |= PTI_ENTITYFLAG_GROUNDED;
    } else {
      self->flags &= ~PTI_ENTITYFLAG_GROUNDED;
    }

    handle_player_horizontal_movement(self);
    handle_player_vertical_movement(self);

    handle_player_jump(self);

    /* make camera approach player */
    const int width = 128;
    const int height = 128;

    int cam_x, cam_y;
    pti_get_camera(&cam_x, &cam_y);
    cam_x +=
        ((self->x + self->bx + self->bw / 2.0f - width / 2.0f) - cam_x) / 10;
    cam_y +=
        ((self->y + self->by + self->bh / 2.0f - height / 2.0f) - cam_y) / 10;
    pti_camera(cam_x, cam_y);

  } break;
  case PTI_EVENTTYPE_OVERLAP:
    break;
  case PTI_EVENTTYPE_DRAW:
    frame = ((int)(self->timer * 8) % 2);
    if (self->sx == 0 && self->sy == 0) {
      frame = 0;
    }

    // TODO: add userdata to entity
    const pti_sprite *sprite = player_sprite;
    pti_plot_sprite(sprite, frame, self->x - 8, self->y - 16,
                    self->flags & PTI_ENTITYFLAG_FACING_LEFT ? 1 : 0, false);

    break;
  }
}