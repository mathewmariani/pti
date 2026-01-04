// cute
#define CUTE_ASEPRITE_IMPLEMENTATION
#include "cute_aseprite.h"

// stb
#include "stb_vorbis.c"

// engine
#include "pti.h"

#include "assets.h"

#include <math.h>

pti_bank_t *_bank;

void init_assets(pti_bank_t *bank) {
	_bank = bank;
}

pti_bitmap_t create_bitmap(const char *path) {
	ase_t *ase = cute_aseprite_load_from_file(path, NULL);

	/* allocate bitmap data */
	const size_t size = ase->w * ase->h * sizeof(ase_color_t);
	char *data = (char *) pti_alloc(_bank, size * ase->frame_count);

	/* initialize */
	pti_bitmap_t bitmap;
	bitmap.frames = (int32_t) ase->frame_count;
	bitmap.width = (int16_t) ase->w;
	bitmap.height = (int16_t) ase->h;
	bitmap.pixels = data;

	for (int i = 0; i < ase->frame_count; ++i, data += size) {
		ase_frame_t *frame = ase->frames + i;
		memcpy(data, frame->pixels, size);
	}

	/* release cute resources. */
	cute_aseprite_free(ase);

	return bitmap;
}

pti_tileset_t create_tileset(const char *path) {
	ase_t *ase = cute_aseprite_load_from_file(path, NULL);
	ase_tileset_t ase_tileset = ase->tileset;

	int tile_w = ase_tileset.tile_w;
	int tile_h = ase_tileset.tile_h;
	int count = ase_tileset.tile_count;
	const ase_color_t *src_pixels = ase_tileset.pixels;

	int tiles_per_row = (int) ceilf(sqrtf((float) count));
	int atlas_w = tiles_per_row * tile_w;
	int atlas_h = ((count + tiles_per_row - 1) / tiles_per_row) * tile_h;

	ase_color_t *atlas_pixels = pti_alloc(_bank, atlas_w * atlas_h * sizeof(ase_color_t));
	memset(atlas_pixels, 0, atlas_w * atlas_h * sizeof(ase_color_t));

	for (int i = 0; i < count; ++i) {
		int tile_x = (i % tiles_per_row) * tile_w;
		int tile_y = (i / tiles_per_row) * tile_h;

		for (int y = 0; y < tile_h; ++y) {
			const ase_color_t *src_row = &src_pixels[i * tile_w * tile_h + y * tile_w];
			ase_color_t *dst_row = &atlas_pixels[(tile_y + y) * atlas_w + tile_x];
			memcpy(dst_row, src_row, tile_w * sizeof(ase_color_t));
		}
	}

	/* release cute resources. */
	cute_aseprite_free(ase);

	return (pti_tileset_t) {
			.count = count,
			.width = atlas_w,
			.height = atlas_h,
			.tile_w = tile_w,
			.tile_h = tile_h,
			.pixels = atlas_pixels,
	};
}

pti_tilemap_t create_tilemap(const char *path) {
	ase_t *ase = cute_aseprite_load_from_file(path, NULL);

	pti_tilemap_t tilemap;
	for (int i = 0; i < ase->frame_count; ++i) {
		ase_frame_t *frame = ase->frames + i;
		for (int j = 0; j < frame->cel_count; ++j) {
			ase_cel_t *cel = frame->cels + j;
			if (cel->is_tilemap) {
				const size_t size = cel->w * cel->h * sizeof(int);
				tilemap.width = (int16_t) cel->w;
				tilemap.height = (int16_t) cel->h;
				tilemap.tiles = (int *) pti_alloc(_bank, size);
				memcpy(tilemap.tiles, cel->tiles, size);
			}
		}
	}

	/* release cute resources. */
	cute_aseprite_free(ase);

	return tilemap;
}

pti_sound_t create_sfx(const char *path) {
	pti_sound_t sound = {0};

	int error = 0;
	stb_vorbis *stream = stb_vorbis_open_filename(path, &error, 0);
	if (!stream) return sound;

	stb_vorbis_info info = stb_vorbis_get_info(stream);
	sound.rate = info.sample_rate;
	sound.channels = info.channels;
	sound.samples_count = stb_vorbis_stream_length_in_samples(stream);

	size_t bytes_needed = sound.samples_count * sound.channels * sizeof(int16_t);
	sound.samples = (int16_t *) pti_alloc(_bank, bytes_needed);
	if (!sound.samples) {
		stb_vorbis_close(stream);
		return sound;
	}

	int16_t buffer[1024 * info.channels];// per channel buffer
	int16_t *it = sound.samples;
	int samples_read_per_channel;

	while ((samples_read_per_channel =
					stb_vorbis_get_samples_short_interleaved(stream, info.channels, buffer, 1024)) > 0) {
		memcpy(it, buffer, samples_read_per_channel * info.channels * sizeof(int16_t));
		it += samples_read_per_channel * info.channels;
	}

	stb_vorbis_close(stream);
	return sound;
}

pti_sound_t create_sine_tone(float frequency, float amplitude, float duration_seconds, int sample_rate, int num_channels) {
	pti_sound_t tone = {0};

	tone.samples_count = (int) (duration_seconds * sample_rate);
	tone.channels = num_channels;

	tone.samples = (int16_t *) pti_alloc(
			_bank,
			sizeof(int16_t) * tone.samples_count * num_channels);

	if (!tone.samples) {
		tone.samples_count = 0;
		tone.channels = 0;
		return tone;// allocation failed
	}

	if (amplitude > 1.0f) amplitude = 1.0f;
	if (amplitude < 0.0f) amplitude = 0.0f;

	float phase = 0.0f;
	float phase_inc = 2.0f * 3.14159265f * frequency / (float) sample_rate;

	for (int i = 0; i < tone.samples_count; i++) {
		float value = sinf(phase) * amplitude;

		// clamp
		if (value > 1.0f) value = 1.0f;
		if (value < -1.0f) value = -1.0f;

		int16_t sample = (int16_t) (value * 32767.0f);

		for (int ch = 0; ch < num_channels; ch++) {
			tone.samples[i * num_channels + ch] = sample;
		}

		phase += phase_inc;
		if (phase >= 2.0f * 3.14159265f) phase -= 2.0f * 3.14159265f;
	}

	return tone;
}