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

static void stereo_to_mono_i16(const int16_t *src, int16_t *dst, int frames) {
	for (int i = 0; i < frames; i++) {
		int l = src[i * 2 + 0];
		int r = src[i * 2 + 1];
		dst[i] = (int16_t) ((l + r) / 2);
	}
}

static int resample_i16(const int16_t *src, int src_frames, int src_rate, int16_t *dst, int dst_rate) {
	float ratio = (float) src_rate / (float) dst_rate;
	int dst_frames = (int) (src_frames / ratio);

	for (int i = 0; i < dst_frames; i++) {
		int src_i = (int) (i * ratio);
		if (src_i >= src_frames) src_i = src_frames - 1;
		dst[i] = src[src_i];
	}

	return dst_frames;
}

static void i16_to_float(const int16_t *src, float *dst, int frames) {
	for (int i = 0; i < frames; i++) {
		dst[i] = src[i] / 32768.0f;
	}
}

pti_audio_t create_sfx(const char *path) {
	short *pcm = NULL;
	int src_rate = 0;
	int channels = 0;

	int src_frames = stb_vorbis_decode_filename(path, &channels, &src_rate, &pcm);

	pti_audio_t out = {0};
	if (src_frames <= 0) return out;

	// --- Step 1: convert to mono ---
	int16_t *mono = NULL;

	if (channels == 2) {
		mono = malloc(sizeof(int16_t) * src_frames);
		stereo_to_mono_i16(pcm, mono, src_frames);
	} else {
		mono = pcm;// already mono
	}

	// --- Step 2: resample to 22050 ---
	int target_rate = 22050;
	int max_dst_frames = (int) ((float) src_frames * target_rate / src_rate) + 1;

	int16_t *resampled = malloc(sizeof(int16_t) * max_dst_frames);
	int dst_frames = resample_i16(
			mono,
			src_frames,
			src_rate,
			resampled,
			target_rate);

	// --- Step 3: allocate final float buffer ---
	out.num_channels = 1;
	out.num_frames = dst_frames;

	out.samples = (float *) pti_alloc(_bank, sizeof(float) * dst_frames);
	i16_to_float(resampled, out.samples, dst_frames);

	// --- cleanup ---
	if (channels == 2) free(mono);
	free(resampled);
	free(pcm);

	return out;
}

pti_audio_t create_sine_tone(float frequency, float amplitude, float duration_seconds, int sample_rate, int num_channels) {
	pti_audio_t tone;
	tone.num_frames = (int) (duration_seconds * sample_rate);
	tone.num_channels = num_channels;
	tone.samples = (float *) pti_alloc(_bank, sizeof(float) * tone.num_frames * num_channels);

	if (!tone.samples) {
		tone.num_frames = 0;
		tone.num_channels = 0;
		return tone;// allocation failed
	}

	float phase = 0.0f;
	float phase_increment = 2.0f * 3.14159265f * frequency / (float) sample_rate;

	for (int i = 0; i < tone.num_frames; i++) {
		float value = sinf(phase) * amplitude;

		for (int ch = 0; ch < num_channels; ch++) {
			tone.samples[i * num_channels + ch] = value;
		}

		phase += phase_increment;
		if (phase >= 2.0f * 3.14159265f) {
			phase -= 2.0f * 3.14159265f;
		}
	}

	return tone;
}