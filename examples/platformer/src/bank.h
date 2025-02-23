#pragma once

typedef struct pti_tilemap_t pti_tilemap_t;
typedef struct pti_tileset_t pti_tileset_t;
typedef struct pti_bitmap_t pti_bitmap_t;

// tilemap + tileset
extern pti_tilemap_t *tilemap;
extern pti_tileset_t *tileset;

// entities
extern pti_bitmap_t *bitmap_bullet;
extern pti_bitmap_t *bitmap_coin;
extern pti_bitmap_t *bitmap_goomba;
extern pti_bitmap_t *bitmap_player;
extern pti_bitmap_t *bitmap_pop;

// font
extern pti_bitmap_t *bitmap_shooter;
extern pti_bitmap_t *bitmap_font;

// effects
extern pti_bitmap_t *bitmap_fx_collect;
extern pti_bitmap_t *bitmap_fx_dust1;
extern pti_bitmap_t *bitmap_fx_dust2;