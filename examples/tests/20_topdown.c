// engine
#include "pti.h"

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

pti_desc pti_main(int argc, char *argv[]) {
  return (pti_desc){
      .init_cb = init,
      .cleanup_cb = cleanup,
      .frame_cb = frame,
      .memory_size = _pti_kilobytes(256),
      .window =
          (pti_window){
              .width = 128,
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

// entity types
void pti_player(pti_entity_event *);
void pti_coin(pti_entity_event *);
void pti_pop(pti_entity_event *);
void pti_cursor(pti_entity_event *);

// sweetie16 palette
unsigned int pal[] = {
    0xff1a1c2c, 0xff5d275d, 0xffb13e53, 0xffef7d57, 0xffffcd75, 0xffa7f070,
    0xff38b764, 0xff257179, 0xff29366f, 0xff3b5dc9, 0xff41a6f6, 0xff73eff7,
    0xfff4f4f4, 0xff94b0c2, 0xff566c86, 0xff333c57,
};

int score = 0;

// userdata
pti_sprite *player_sprite = NULL;
pti_sprite *coin_sprite = NULL;

static void init(void) {
  pti_load_bitmap("spritesheet.png");
  pti_load_font("font.png");

  // pti_plot_sprite(spitter, 2, 0, 0, false, false);
  player_sprite = pti_sprite_create("dog.ase");
  coin_sprite = pti_sprite_create("coin.ase");

  int i, j;

  /* Zero out entities */
  for (i = 0; i < PTI_ENTITIES; i++) {
    pti_entities.all[i] = pti_null_entity;
  }

  /* Zero out tiles with solids along room edges */
  for (i = 0; i < PTI_ROOM_COLS; i++) {
    for (j = 0; j < PTI_ROOM_ROWS; j++) {
      bool t = (i == 0 || j == 0 || i == PTI_ROOM_COLS - 1 ||
                j == PTI_ROOM_ROWS - 1);
      pti_entities.tiles[i][j] = t;
    }
  }

  /* Create some entities */
  pti_en_create(pti_player, PTI_ROOM_WIDTH / 2, PTI_ROOM_HEIGHT / 2);
  for (i = 0; i < 7; i++) {
    pti_en_create(pti_coin, 32 + i * 32, 32);
    pti_en_create(pti_coin, 32 + i * 32, PTI_ROOM_HEIGHT - 32);
  }

  // graphics state:
  pti_clip(0, 0, 128, 128);
  // pti_keycolor(0x00000000);
}

static void cleanup(void) {}

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
  const int width = 128;
  const int height = 128;

  int cam_x, cam_y;
  pti_get_camera(&cam_x, &cam_y);

  cam_x = _pti_max(0, _pti_min(PTI_ROOM_WIDTH - width, cam_x));
  cam_y = _pti_max(0, _pti_min(PTI_ROOM_HEIGHT - height, cam_y));

  pti_camera(cam_x, cam_y);

  /* draw bg */
  pti_cls(pal[0]);
  for (i = cam_x / 32; i <= (cam_x + width) / 32; i++) {
    pti_rect(i * 32, cam_y, 1, height, pal[4]);
  }

  for (i = cam_y / 32; i <= (cam_y + height) / 32; i++) {
    pti_rect(cam_x, i * 32, width, 1, pal[4]);
  }

  /* draw solids */
  for (i = 0; i < PTI_ROOM_COLS; i++) {
    for (j = 0; j < PTI_ROOM_ROWS; j++) {
      if (pti_entities.tiles[i][j]) {
        pti_rect(i * PTI_GRID_SIZE, j * PTI_GRID_SIZE, PTI_GRID_SIZE,
                 PTI_GRID_SIZE, pal[2]);
      }
    }
  }
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

void pti_player(pti_entity_event *ev) {
  const float max_speed = 3;
  const float accel = 20;
  const float friction = 15;

  pti_entity *self = ev->self;
  int frame = 0;

  switch (ev->type) {
  case PTI_EVENTTYPE_INIT:
    self->bx = -4;
    self->by = -8;
    self->bw = 8;
    self->bh = 8;
    self->flags = PTI_ENTITYFLAG_OVERLAP_CHECKS | PTI_ENTITYFLAG_HITS_SOLIDS;
    break;
  case PTI_EVENTTYPE_DESTROY:
    break;
  case PTI_EVENTTYPE_UPDATE:
    if (pti_down(PTI_LEFT)) {
      _pti_appr(self->sx, -max_speed, accel * PTI_DELTA);
      self->flags |= PTI_ENTITYFLAG_FACING_LEFT;
    } else if (pti_down(PTI_RIGHT)) {
      _pti_appr(self->sx, max_speed, accel * PTI_DELTA);
      self->flags &= ~PTI_ENTITYFLAG_FACING_LEFT;
    } else {
      _pti_appr(self->sx, 0, friction * PTI_DELTA);
    }

    if (pti_down(PTI_UP)) {
      _pti_appr(self->sy, -max_speed, accel * PTI_DELTA);
    } else if (pti_down(PTI_DOWN)) {
      _pti_appr(self->sy, max_speed, accel * PTI_DELTA);
    } else {
      _pti_appr(self->sy, 0, friction * PTI_DELTA);
    }

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

    break;
  case PTI_EVENTTYPE_OVERLAP:
    if (ev->other->type == pti_coin) {
      pti_en_destroy(ev->other);
    }
    break;
  case PTI_EVENTTYPE_DRAW:
    frame = ((int)(self->timer * 8) % 2);
    if (self->sx == 0 && self->sy == 0) {
      frame = 0;
    }

    // TODO: add userdata to entity
    // const pti_sprite* sprite = self->userdata;
    const pti_sprite *sprite = player_sprite;

    pti_plot_sprite(sprite, frame, self->x - 8, self->y - 16,
                    self->flags & PTI_ENTITYFLAG_FACING_LEFT ? 1 : 0, false);
    break;
  }
}

void pti_coin(pti_entity_event *ev) {
  pti_entity *self = ev->self;
  pti_entity *other;
  int frame;

  switch (ev->type) {
  case PTI_EVENTTYPE_INIT:
    self->bx = -2;
    self->by = -2;
    self->bw = 4;
    self->bh = 4;
    self->flags = PTI_ENTITYFLAG_OVERLAP_CHECKS;
    break;
  case PTI_EVENTTYPE_DESTROY:
    score++;
    other = pti_en_create(pti_pop, self->x, self->y);
    other->sx = 4;
    other->sy = 4;
    other = pti_en_create(pti_pop, self->x, self->y);
    other->sx = -4;
    other->sy = 4;
    other = pti_en_create(pti_pop, self->x, self->y);
    other->sx = 4;
    other->sy = -4;
    other = pti_en_create(pti_pop, self->x, self->y);
    other->sx = -4;
    other->sy = -4;
    break;
  case PTI_EVENTTYPE_UPDATE:
  case PTI_EVENTTYPE_OVERLAP:
    break;
  case PTI_EVENTTYPE_DRAW:
    frame = ((int)(self->timer * 8) % 2);

    // TODO: add userdata to entity
    // const pti_sprite* sprite = self->userdata;
    const pti_sprite *sprite = coin_sprite;
    pti_plot_sprite(coin_sprite, frame, self->x - 4, self->y - 4, false, false);

    break;
  }
}

void pti_pop(pti_entity_event *ev) {
  pti_entity *self = ev->self;

  switch (ev->type) {
  case PTI_EVENTTYPE_INIT:
    self->bw = 0;
    self->bh = 0;
    break;
  case PTI_EVENTTYPE_DESTROY:
    break;
  case PTI_EVENTTYPE_UPDATE:
    _pti_appr(self->sx, 0, 32 * PTI_DELTA);
    _pti_appr(self->sy, 0, 32 * PTI_DELTA);
    if (self->timer > 0.20f) {
      pti_en_destroy(self);
    }
    break;
  case PTI_EVENTTYPE_OVERLAP:
    break;
  case PTI_EVENTTYPE_DRAW:
    pti_rect(self->x - 1, self->y - 1, 2, 2, pal[4]);
    break;
  }
}