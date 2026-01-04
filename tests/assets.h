#pragma once

typedef struct pti_bitmap_t pti_bitmap_t;
typedef struct pti_tileset_t pti_tileset_t;
typedef struct pti_tilemap_t pti_tilemap_t;
typedef struct pti_sound_t pti_sound_t;
typedef struct pti_bank_t pti_bank_t;

void init_assets(pti_bank_t *bank);

pti_bitmap_t create_bitmap(const char *path);
pti_tileset_t create_tileset(const char *path);
pti_tilemap_t create_tilemap(const char *path);
pti_sound_t create_sfx(const char *path);

pti_sound_t create_sine_tone(float frequency, float amplitude, float duration_seconds, int sample_rate, int num_channels);