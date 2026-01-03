#pragma once
/*
    pti.h -- docs
*/

//>> includes
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define _pti_kilobytes(n) (1024 * (n))
#define _pti_megabytes(n) (1024 * _pti_kilobytes(n))
#define _pti_gigabytes(n) (1024 * _pti_megabytes(n))
#define _pti_align_to(value, N) ((value + (N - 1)) & ~(N - 1))

#define _pti_min(x, y) ((x) < (y) ? (x) : (y))
#define _pti_max(x, y) ((x) > (y) ? (x) : (y))
#define _pti_clamp(x, a, b) (_pti_max(a, _pti_min(x, b)))
#define _pti_round(x) ((int) ((x) + ((x) >= 0 ? 0.5f : -0.5f)))
#define _pti_abs(x) ((x < 0) ? (-x) : (x))
#define _pti_appr(val, tar, delta)                   \
	(val = (val > (tar) ? _pti_max(tar, val - delta) \
						: _pti_min(tar, val + delta)))
#define _pti_sign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
#define _pti_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#ifdef __cplusplus
extern "C" {
#endif

enum {
	PTI_KEY_STATE = (1 << 0),
	PTI_KEY_PRESSED = (1 << 1),
	PTI_KEY_RELEASED = (1 << 2),

	PTI_REQUEST_NONE = (1 << 0),
	PTI_REQUEST_SHUTDOWN = (1 << 1),
};

typedef enum pti_event_type {
	PTI_EVENTTYPE_INVALID,
	PTI_EVENTTYPE_KEY_DOWN,
	PTI_EVENTTYPE_KEY_UP,
	_PTI_EVENTTYPE_NUM,
} pti_event_type;

typedef enum pti_button {
	PTI_LEFT,
	PTI_RIGHT,
	PTI_UP,
	PTI_DOWN,
	PTI_A,
	PTI_B,
	PTI_X,
	PTI_Y,

	PTI_DBG,

	// always last.
	PTI_BUTTON_COUNT
} pti_button;

typedef struct pti_bitmap_t {
	uint32_t frames;
	uint32_t width;
	uint32_t height;
	void *pixels;// (width) x (height x frames)
} pti_bitmap_t;

typedef struct pti_tileset_t {
	uint32_t count;
	uint16_t width;
	uint16_t height;
	uint16_t tile_w;
	uint16_t tile_h;
	void *pixels;// (width) x (height x count)
} pti_tileset_t;

typedef struct pti_tilemap_t {
	uint16_t width;
	uint16_t height;
	int *tiles;
} pti_tilemap_t;

typedef struct pti_bank_t {
	uint8_t *begin;
	uint8_t *end;
	uint8_t *it;
	uint8_t *cap;
} pti_bank_t;

typedef struct pti_trace_hooks {
	void (*set_tilemap)(pti_tilemap_t *ptr);
	void (*set_tileset)(pti_tileset_t *ptr);
	void (*set_font)(pti_bitmap_t *ptr);
} pti_trace_hooks;

typedef struct pti_desc {
	void (*init_cb)(void);
	void (*frame_cb)(void);
	void (*cleanup_cb)(void);
	void (*debug_cb)(void);

	int memory_size;
	int width;
	int height;
} pti_desc;

// user-provided function
extern pti_desc pti_main(int argc, char *argv[]);

// public functions
void pti_init(const pti_desc *desc);
void pti_install_trace_hooks(const pti_trace_hooks *trace_hooks);

void pti_tick(double dt);
void pti_event(pti_event_type type, int value);
void pti_quit(void);

// api functions

//>> virutal machine api
void pti_bank_init(pti_bank_t *bank, const size_t capacity);
void pti_load_bank(pti_bank_t *bank);
void *pti_alloc(pti_bank_t *bank, const size_t size);
void pti_reload(void);
void pti_memcpy(void *dst, const void *src, size_t len);
void pti_memset(void *dst, const int value, size_t len);
void pti_set_tilemap(pti_tilemap_t *ptr);
void pti_set_tileset(pti_tileset_t *ptr);
void pti_set_font(pti_bitmap_t *ptr);

//>> memory api
const uint8_t pti_peek(const uint32_t offset, const uint32_t index);
const uint16_t pti_peek2(const uint32_t offset, const uint32_t index);
const uint32_t pti_peek4(const uint32_t offset, const uint32_t index);
void pti_poke(const uint32_t offset, const uint32_t index, const uint8_t value);
void pti_poke2(const uint32_t offset, const uint32_t index, const uint16_t value);
void pti_poke4(const uint32_t offset, const uint32_t index, const uint32_t value);

//>> input api
bool pti_down(pti_button btn);
bool pti_pressed(pti_button btn);
bool pti_released(pti_button btn);

//>> map api
uint32_t pti_mget(int x, int y);
void pti_mset(int x, int y, int value);
uint16_t pti_fget(int x, int y);

//>> random api
uint16_t pti_prand(void);

//>> graphics api
void pti_camera(int x, int y);
void pti_get_camera(int *x, int *y);
void pti_cls(const uint32_t color);
void pti_color(const uint64_t color);
void pti_colorkey(const uint32_t color);
void pti_dither(const uint16_t bstr);
void pti_clip(int x0, int y0, int x1, int y1);
void pti_pset(int x, int y, uint64_t color);
void pti_circ(int x, int y, int r, uint64_t color);
void pti_circf(int x, int y, int r, uint64_t color);
void pti_line(int x0, int y0, int x1, int y1, uint64_t color);
void pti_rect(int x, int y, int w, int h, uint64_t color);
void pti_rectf(int x, int y, int w, int h, uint64_t color);
void pti_map(int x, int y);
void pti_spr(const pti_bitmap_t *bitmap, int n, int x, int y, bool flip_x, bool flip_y);
void pti_print(const char *text, int x, int y);

#ifdef __cplusplus
}// extern "C"

#include <string>

// reference-based equivalents for C++
inline void pti_set_tilemap(pti_tilemap_t &tilemap) { pti_set_tilemap(&tilemap); }
inline void pti_set_tileset(pti_tileset_t &tileset) { pti_set_tileset(&tileset); }
inline void pti_set_font(pti_bitmap_t &bitmap) { pti_set_font(&bitmap); }
inline void pti_spr(const pti_bitmap_t &bitmap, int n, int x, int y, bool flip_x, bool flip_y) { pti_spr(&bitmap, n, x, y, flip_x, flip_y); }
inline void pti_print(const std::string &text, int x, int y) { pti_print(text.c_str(), x, y); }

#endif

#ifdef PTI_IMPL

#include <stdlib.h>// malloc, free

#if defined(__APPLE__)
// apple
#define _PTI_APPLE (1)
#elif defined(__EMSCRIPTEN__)
// emscripten (asm.js or wasm)
#define _PTI_EMSCRIPTEN (1)
#elif defined(_WIN32)
// Windows
#define _PTI_WIN32 (1)
#elif defined(__linux__) || defined(__unix__)
// Linux
#define _PTI_LINUX (1)
#else
#error "pti.h: Unknown platform"
#endif

//>> implementation

#ifndef _PTI_PRIVATE
#if defined(__GNUC__) || defined(__clang__)
#define _PTI_PRIVATE __attribute__((unused)) static
#else
#define _PTI_PRIVATE static
#endif
#endif
#ifndef _PTI_UNUSED
#define _PTI_UNUSED(x) ((void) (x))
#endif

#ifndef PTI_ASSERT
#include <assert.h>
#define PTI_ASSERT(c) assert(c)
#endif

#if defined(_PTI_WINDOWS)
#include <memoryapi.h>
#elif defined(_PTI_LINUX)
#include <sys/mman.h>
#endif

#if defined(PTI_TRACE_HOOKS)
#define _PTI_TRACE_ARGS(fn, ...) \
	if (_pti.hooks.fn) { _pti.hooks.fn(__VA_ARGS__); }
#define _PTI_TRACE_NOARGS(fn) \
	if (_pti.hooks.fn) { _pti.hooks.fn(); }
#else
#define _PTI_TRACE_ARGS(fn, ...)
#define _PTI_TRACE_NOARGS(fn)
#endif

#if defined(PTI_SIMD)
#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#endif

#define PTI_FRAMERATE (30.0)
#define PTI_DELTA (1.0 / PTI_FRAMERATE)
#define TICK_DURATION_NS (PTI_DELTA * 1e9)
#define TICK_TOLERANCE_NS (1000000)

typedef struct {
	struct {
		uint16_t width;
		uint16_t height;
	} screen;

	pti_tilemap_t *tilemap;

	struct {
		pti_tileset_t *tileset;
		pti_bitmap_t *font;
		int16_t clip_x0, clip_y0;
		int16_t clip_x1, clip_y1;
		int16_t cam_x, cam_y;
		uint16_t dither;
		uint32_t ckey;
		struct {
			uint32_t low;
			uint32_t high;
		} color;
	} draw;

	struct {
		uint8_t btn_state[PTI_BUTTON_COUNT];
		uint8_t rnd_reg[4];

		struct {
			unsigned int tick;
			int tick_accum;
		} timing;
	} hardware;

	uint8_t flags;
} _pti__vm_t;

typedef struct {
	pti_desc desc;
	pti_bank_t ram;
	pti_bank_t cart;
	_pti__vm_t vm;
	uint32_t *screen;
	void *data;
#if defined(PTI_TRACE_HOOKS)
	pti_trace_hooks hooks;
#endif
} _pti__t;
static _pti__t _pti;

//>> internals

_PTI_PRIVATE inline void *_pti__ptr_to_bank(void *ptr) {
	return (void *) ((uintptr_t) ptr + ((uintptr_t) _pti.data - (uintptr_t) _pti.cart.begin));
}

_PTI_PRIVATE void *_pti__virtual_reserve(void *ptr, const size_t size) {
#if defined(_PTI_WIN32)
	// Reserves a range of the process's virtual offsetess space without
	// allocating any actual physical storage in memory or in the paging file on
	// disk.
	// Disables all access to the committed region of pages. An attempt to read
	// from, write to, or execute the committed region results in an access
	// violation.

	ptr = VirtualAlloc(ptr, size, MEM_RESERVE, PAGE_NOACCESS);
	PTI_ASSERT(ptr);
#elif defined(_PTI_LINUX)
	// Create a private copy-on-write mapping.
	// The mapping is not backed by any file;
	// its contents are initialized to zero.
	uint16_t flags = (MAP_PRIVATE | MAP_ANON);
	uint16_t prot = (PROT_READ | PROT_WRITE);
	ptr = mmap((void *) ptr, size, prot, flags, -1, 0);
	PTI_ASSERT(ptr != MAP_FAILED);
	msync(ptr, size, (MS_SYNC | MS_INVALIDATE));
#else
	ptr = malloc(size);
	PTI_ASSERT(ptr);
#endif
	return ptr;
}

_PTI_PRIVATE void *_pti__virtual_commit(void *ptr, const uint32_t size) {
#if defined(_PTI_WIN32)
	// Enables read-only or read/write access to the committed region of pages.
	ptr = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
	PTI_ASSERT(ptr);
#elif defined(_PTI_LINUX)
	uint16_t flags = (MAP_FIXED | MAP_SHARED | MAP_ANON);
	ptr = mmap(ptr, size, (PROT_READ | PROT_WRITE), flags, -1, 0);
	PTI_ASSERT(ptr != MAP_FAILED);
	// Requests an update and waits for it to complete.
	// Asks to invalidate other mappings of the same file (so
	// that they can be updated with the fresh values just written).
	msync(ptr, size, (MS_SYNC | MS_INVALIDATE));
#else
	if (!ptr) {
		ptr = malloc(size);
		PTI_ASSERT(ptr);
	}
#endif
	return ptr;
}

_PTI_PRIVATE void *_pti__virtual_decommit(void *ptr, const uint32_t size) {
#if defined(_PTI_WIN32)
	VirtualFree(ptr, size, LMEM_DECOMMIT);
#elif defined(_PTI_LINUX)
	uint16_t flags = (MAP_FIXED | MAP_SHARED | MAP_ANON);
	mmap(ptr, size, PROT_NONE, flags, -1, 0);
	msync(ptr, size, (MS_SYNC | MS_INVALIDATE));
#else
	(void) ptr;
	(void) size;
#endif
	return ptr;
}

_PTI_PRIVATE void *_pti__virtual_alloc(void *ptr, const uint32_t size) {
	void *data = _pti__virtual_reserve(ptr, size);
	return _pti__virtual_commit(data, size);
}

_PTI_PRIVATE void _pti__virtual_free(void *ptr, const uint32_t size) {
#if defined(_PTI_WIN32)
	_PTI_UNUSED(size);
	VirtualFree((void *) ptr, 0, LMEM_RELEASE);
#elif defined(_PTI_LINUX)
	// Requests an update and waits for it to complete.
	msync(ptr, size, MS_SYNC);
	munmap(ptr, size);
#else
	free(ptr);
	(void) size;
#endif
}

void pti_init(const pti_desc *desc) {
	// cache description
	_pti.desc = *desc;

	_pti.vm.screen.width = desc->width;
	_pti.vm.screen.height = desc->height;

	// calculate sizes
	const size_t vm_size = sizeof(_pti__vm_t);
	const size_t vram_size = desc->width * desc->height * sizeof(uint32_t);
	const size_t capacity = desc->memory_size + vram_size;

	// init memory
	pti_bank_init(&_pti.ram, capacity);

	// allocate virtual machine
	_pti.screen = (uint32_t *) pti_alloc(&_pti.ram, vram_size);
	_pti.data = pti_alloc(&_pti.ram, desc->memory_size);

	// init random
	for (int i = 0; i < 4; ++i) {
		_pti.vm.hardware.rnd_reg[i] = 0x0;
	}

	// init gfx state
	pti_clip(0, 0, _pti.vm.screen.width, _pti.vm.screen.height);
}

void pti_install_trace_hooks(const pti_trace_hooks *trace_hooks) {
#if defined(PTI_TRACE_HOOKS)
	_pti.hooks = *trace_hooks;
#else
#warning "pti.h: PTI_TRACE_HOOKS not defined."
#endif
}

void pti_tick(double dt) {
	if (dt > TICK_DURATION_NS) {
		dt = TICK_DURATION_NS;
	}

	_pti.vm.hardware.timing.tick_accum += dt;
	while (_pti.vm.hardware.timing.tick_accum + TICK_TOLERANCE_NS >= TICK_DURATION_NS) {
		_pti.vm.hardware.timing.tick_accum -= TICK_DURATION_NS;
		_pti.vm.hardware.timing.tick++;

		if (_pti.desc.frame_cb != NULL) {
			_pti.desc.frame_cb();
		}

		for (int i = 0; i < PTI_BUTTON_COUNT; i++) {
			_pti.vm.hardware.btn_state[i] &= ~(PTI_KEY_PRESSED | PTI_KEY_RELEASED);
		}
	}
}

void pti_event(pti_event_type type, int pti_key) {
	if (type == PTI_EVENTTYPE_KEY_DOWN) {
		_pti.vm.hardware.btn_state[pti_key] |= (PTI_KEY_STATE | PTI_KEY_PRESSED);
		_pti.vm.hardware.btn_state[pti_key] &= ~PTI_KEY_RELEASED;
	} else if (type == PTI_EVENTTYPE_KEY_UP) {
		_pti.vm.hardware.btn_state[pti_key] &= ~(PTI_KEY_STATE | PTI_KEY_PRESSED);
		_pti.vm.hardware.btn_state[pti_key] |= PTI_KEY_RELEASED;
	}
}

void pti_quit(void) {
	_pti.vm.flags = PTI_REQUEST_SHUTDOWN;
}

// api functions

//>> virutal machine

void pti_bank_init(pti_bank_t *bank, const size_t capacity) {
	// allocate memory
	uint8_t *ptr = (uint8_t *) _pti__virtual_reserve(NULL, capacity);
	bank->begin = ptr;
	bank->it = ptr;
	bank->end = ptr;
	bank->cap = ptr + capacity;
}

void pti_load_bank(pti_bank_t *bank) {
	_pti.cart = *bank;
	pti_reload();
}

void *pti_alloc(pti_bank_t *bank, const size_t size) {
	void *ptr;
	if ((bank->end - bank->it) < size) {
		PTI_ASSERT((bank->cap - bank->it) >= size);
		size_t additional_size = size - (bank->end - bank->it);
		additional_size = _pti_min((bank->cap - bank->it), _pti_align_to(additional_size, 4096));
		ptr = _pti__virtual_commit(bank->end, additional_size);
		bank->end += additional_size;
	}
	ptr = bank->it;
	bank->it += size;
	return ptr;
}

_PTI_PRIVATE void pti_free(pti_bank_t *bank) {
	_pti__virtual_free(bank, (bank->cap - bank->begin));
	memset(bank, 0, sizeof(pti_bank_t));
}

void pti_reload(void) {
	pti_memcpy(_pti.data, _pti.cart.begin, (uint32_t) (_pti.cart.cap - _pti.cart.begin));
}

void pti_memcpy(void *dst, const void *src, size_t len) {
	memcpy(dst, src, len);
}

void pti_memset(void *dst, const int value, size_t len) {
	memset(dst, value, len);
}

void pti_set_tilemap(pti_tilemap_t *ptr) {
	_PTI_TRACE_ARGS(set_tilemap, ptr);
	_pti.vm.tilemap = ptr;
}

void pti_set_tileset(pti_tileset_t *ptr) {
	_PTI_TRACE_ARGS(set_tileset, ptr);
	_pti.vm.draw.tileset = ptr;
}

void pti_set_font(pti_bitmap_t *ptr) {
	_PTI_TRACE_ARGS(set_font, ptr);
	_pti.vm.draw.font = ptr;
}

//>> memory api

const uint8_t pti_peek(const uint32_t offset, const uint32_t index) {
	return *(_pti.ram.begin + offset + index);
}

const uint16_t pti_peek2(const uint32_t offset, const uint32_t index) {
	const uint8_t b0 = pti_peek(offset, index + 0);
	const uint8_t b1 = pti_peek(offset, index + 1);

	// combine 16-bit value
	return (uint16_t) ((b0 << 8) | (b1 << 0));
}

const uint32_t pti_peek4(const uint32_t offset, const uint32_t index) {
	const uint8_t b0 = pti_peek(offset, index + 0);
	const uint8_t b1 = pti_peek(offset, index + 1);
	const uint8_t b2 = pti_peek(offset, index + 2);
	const uint8_t b3 = pti_peek(offset, index + 3);

	// combine 32-bit value
	return (uint32_t) ((b0 << 24) | (b1 << 16) | (b2 << 8) | (b3 << 0));
}

void pti_poke(const uint32_t offset, const uint32_t index, const uint8_t value) {
	*(_pti.ram.begin + offset + index) = value;
}

void pti_poke2(const uint32_t offset, const uint32_t index, const uint16_t value) {
	pti_poke(offset, index + 0, (uint8_t) (value >> 0));
	pti_poke(offset, index + 1, (uint8_t) (value >> 8));
}

void pti_poke4(const uint32_t offset, const uint32_t index, const uint32_t value) {
	pti_poke(offset, index + 0, (uint8_t) (value >> 0));
	pti_poke(offset, index + 1, (uint8_t) (value >> 8));
	pti_poke(offset, index + 2, (uint8_t) (value >> 16));
	pti_poke(offset, index + 3, (uint8_t) (value >> 24));
}

//>> input

_PTI_PRIVATE inline bool _pti__check_input_flag(uint32_t idx, int flag) {
	return _pti.vm.hardware.btn_state[idx] & flag ? true : false;
}

bool pti_down(pti_button btn) {
	return _pti__check_input_flag(btn, PTI_KEY_STATE);
}

bool pti_pressed(pti_button btn) {
	return _pti__check_input_flag(btn, PTI_KEY_PRESSED);
}

bool pti_released(pti_button btn) {
	return _pti__check_input_flag(btn, PTI_KEY_RELEASED);
}

//>> map

uint32_t pti_mget(int x, int y) {
	int *tiles = (int *) _pti__ptr_to_bank((void *) _pti.vm.tilemap->tiles);
	return *(tiles + x + y * _pti.vm.tilemap->width);
}

void pti_mset(int x, int y, int value) {
	int *tiles = (int *) _pti__ptr_to_bank((void *) _pti.vm.tilemap->tiles);
	*(tiles + x + y * _pti.vm.tilemap->width) = value;
}

uint16_t pti_fget(int x, int y) {
	return (uint16_t) pti_mget(x, y);
}

//>> random

_PTI_PRIVATE void _pti__random_tick(int i) {
	uint8_t *reg = &_pti.vm.hardware.rnd_reg[0];
	*(reg + 0) = 5 * *(reg + 0) + 1;
	*(reg + 1) = ((*(reg + 1) & 0x80) == (*(reg + 1) & 0x10))
						 ? 2 * *(reg + 1) + 1
						 : 2 * *(reg + 1);
	*(reg + (2 + i)) = (*(reg + 0) ^ *(reg + 1));
}

uint16_t pti_prand(void) {
	for (int i = 1; i >= 0; --i) {
		_pti__random_tick(i);
	}
	uint8_t *reg = &_pti.vm.hardware.rnd_reg[0];
	return ((uint16_t) *(reg + 2) << 0x8) | *(reg + 3);
}

//>> graphics

_PTI_PRIVATE inline bool _pti__get_dither_bit(const int x, const int y) {
	const uint8_t i = 0xf - ((x & 0x3) + 0x4 * (y & 0x3));
	return ((_pti.vm.draw.dither & (1 << i)) >> i) != 0;
}

_PTI_PRIVATE inline void _pti__transform(int *x, int *y) {
	*x -= _pti.vm.draw.cam_x;
	*y -= _pti.vm.draw.cam_y;
}

_PTI_PRIVATE inline void _pti__set_pixel(int x, int y, uint64_t c) {
	const int16_t clip_x0 = _pti.vm.draw.clip_x0;
	const int16_t clip_y0 = _pti.vm.draw.clip_y0;
	const int16_t clip_x1 = _pti.vm.draw.clip_x1;
	const int16_t clip_y1 = _pti.vm.draw.clip_y1;

	if ((x >= clip_x1 || x < clip_x0) || (y >= clip_y1 || y < clip_y0)) {
		return;
	}

	*(_pti.screen + (x + y * _pti.vm.screen.width)) = _pti__get_dither_bit(x, y) ? (c >> 32) & 0xffffffff : (c >> 0) & 0xffffffff;
}

_PTI_PRIVATE void _pti__plot(void *pixels, int n, int dst_x, int dst_y, int dst_w, int dst_h, int src_x, int src_y, int src_w, int src_h, bool flip_x, bool flip_y) {
	const int16_t clip_x0 = _pti.vm.draw.clip_x0;
	const int16_t clip_y0 = _pti.vm.draw.clip_y0;
	const int16_t clip_x1 = _pti.vm.draw.clip_x1;
	const int16_t clip_y1 = _pti.vm.draw.clip_y1;

	int dst_x1 = dst_x;
	int dst_y1 = dst_y;
	int dst_x2 = dst_x + dst_w - 1;
	int dst_y2 = dst_y + dst_h - 1;

	if ((dst_x2 < clip_x0 || dst_x1 >= clip_x1) || (dst_y2 < clip_y0 || dst_y1 >= clip_y1)) {
		return;
	}

	if (dst_x1 < clip_x0) {
		src_x += (clip_x0 - dst_x1);
		dst_x1 = clip_x0;
	}
	if (dst_y1 < clip_y0) {
		src_y += (clip_y0 - dst_y1);
		dst_y1 = clip_y0;
	}

	if (dst_x2 >= clip_x1) {
		dst_x2 = clip_x1 - 1;
	}
	if (dst_y2 >= clip_y1) {
		dst_y2 = clip_y1 - 1;
	}

	int ix = flip_x ? -1 : 1;
	int iy = flip_y ? -1 : 1;
	if (flip_x) {
		src_x += (dst_x2 - dst_x1);
	}
	if (flip_y) {
		src_y += (dst_y2 - dst_y1);
	}

	uint32_t *src = (uint32_t *) pixels + n * (src_w * src_h);
	uint32_t *dst = _pti.screen;
	uint32_t color_key = _pti.vm.draw.ckey;
	uint32_t color_cur = _pti.vm.draw.color.low;

	const int dst_width = _pti.desc.width;
	const int clipped_width = dst_x2 - dst_x1 + 1;

	for (int y = dst_y1; y <= dst_y2; y++) {
		int src_row = src_y + (y - dst_y1) * iy;
		uint32_t *src_pixel = src + src_row * src_w + src_x;
		for (int x = dst_x1; x <= dst_x2; x++) {
			uint32_t src_color = *src_pixel;
			if (src_color != color_key) {
				uint64_t c = ((uint64_t) _pti.vm.draw.color.high << 32) | src_color;
				_pti__set_pixel(x, y, c);
			}
			src_pixel += ix;
		}
	}
}

void pti_camera(int x, int y) {
	_pti.vm.draw.cam_x = x;
	_pti.vm.draw.cam_y = y;
}

void pti_get_camera(int *x, int *y) {
	if (x) {
		*x = _pti.vm.draw.cam_x;
	}
	if (y) {
		*y = _pti.vm.draw.cam_y;
	}
}

void pti_cls(const uint32_t color) {
	const int screen_w = _pti.vm.screen.width;
	const int screen_h = _pti.vm.screen.height;
	const size_t pixel_count = screen_w * screen_h;

	uint32_t *pixels = (uint32_t *) _pti.screen;
	for (size_t i = 0; i < pixel_count; i++) {
		pixels[i] = color;
	}
}

void pti_color(const uint64_t color) {
	_pti.vm.draw.color = {
			.low = (uint32_t) (color & 0xFFFFFFFF),
			.high = (uint32_t) (color >> 32),
	};
}

void pti_colorkey(const uint32_t color) {
	_pti.vm.draw.ckey = color;
}

void pti_dither(const uint16_t bstr) {
	_pti.vm.draw.dither = bstr;
}

void pti_clip(int x0, int y0, int x1, int y1) {
	_pti.vm.draw.clip_x0 = x0;
	_pti.vm.draw.clip_y0 = y0;
	_pti.vm.draw.clip_x1 = x1;
	_pti.vm.draw.clip_y1 = y1;
}

void pti_pset(int x, int y, uint64_t color) {
	_pti__set_pixel(x, y, color);
}

void pti_circ(int x, int y, int r, uint64_t color) {
	_pti__transform(&x, &y);
	int32_t dx = r;
	int32_t dy = 0;
	int32_t err = 1 - dx;
	while (dx >= dy) {
		_pti__set_pixel(dx + x, dy + y, color);
		_pti__set_pixel(-dx + x, dy + y, color);
		_pti__set_pixel(dx + x, -dy + y, color);
		_pti__set_pixel(-dx + x, -dy + y, color);

		_pti__set_pixel(dy + x, dx + y, color);
		_pti__set_pixel(-dy + x, dx + y, color);
		_pti__set_pixel(dy + x, -dx + y, color);
		_pti__set_pixel(-dy + x, -dx + y, color);
		dy++;
		if (err < 0) {
			err += 2 * dy + 1;
		} else {
			dx--;
			err += 2 * (dy - dx + 1);
		}
	}
}

void pti_circf(int x, int y, int r, uint64_t color) {
	_pti__transform(&x, &y);
	int32_t dx = r;
	int32_t dy = 0;
	int32_t err = 1 - dx;
	while (dx >= dy) {
#define FILL_ROW(startx, endx, starty, c)                         \
	do {                                                          \
		int32_t sx = (startx);                                    \
		int32_t ex = (endx);                                      \
		int32_t sy = (starty);                                    \
		if (sy < 0 || sy >= _pti.vm.screen.height) break;         \
		if (sx < 0) sx = 0;                                       \
		if (sx > _pti.vm.screen.width) sx = _pti.vm.screen.width; \
		if (ex < 0) ex = 0;                                       \
		if (ex > _pti.vm.screen.width) ex = _pti.vm.screen.width; \
		if (sx == ex) break;                                      \
		for (int32_t i = 0; i < (ex - sx); i++) {                 \
			_pti__set_pixel(sx + i, sy, c);                       \
		}                                                         \
	} while (0)

		FILL_ROW(-dx + x, dx + x, dy + y, color);
		FILL_ROW(-dx + x, dx + x, -dy + y, color);
		FILL_ROW(-dy + x, dy + x, dx + y, color);
		FILL_ROW(-dy + x, dy + x, -dx + y, color);
#undef FILL_ROW

		dy++;
		if (err < 0) {
			err += 2 * dy + 1;
		} else {
			dx--;
			err += 2 * (dy - dx + 1);
		}
	}
}

void pti_line(int x0, int y0, int x1, int y1, uint64_t c) {
	bool steep = _pti_abs(x1 - x0) < _pti_abs(y1 - y0);
	if (steep) {
		_pti_swap(x0, y0);
		_pti_swap(x1, y1);
	}

	if (x0 > x1) {
		_pti_swap(x0, x1);
		_pti_swap(y0, y1);
	}

	const int dx = x1 - x0;
	const int dy = y1 - y0;
	const int de = 2 * _pti_abs(dy);
	int err = 0;
	int y = y0;

	for (int x = x0; x <= x1; x++) {
		int px = steep ? y : x;
		int py = steep ? x : y;

		_pti__transform(&px, &py);
		_pti__set_pixel(px, py, c);

		err += de;
		if (err > dx) {
			y += y1 > y0 ? 1 : -1;
			err -= dx * 2;
		}
	}
}

void pti_rect(int x, int y, int w, int h, uint64_t color) {
	pti_line(x, y, x + w, y, color);
	pti_line(x, y + h, x + w, y + h, color);
	pti_line(x, y, x, y + h, color);
	pti_line(x + w, y, x + w, y + h, color);
}

void pti_rectf(int x, int y, int w, int h, uint64_t color) {
	_pti__transform(&x, &y);
	int x2 = x + w;
	int y2 = y + h;

	for (int i = y; i <= y2; i++) {
		for (int j = x; j <= x2; j++) {
			_pti__set_pixel(j, i, color);
		}
	}
}

void pti_map(int x, int y) {
	const int map_w = _pti.vm.tilemap->width;
	const int map_h = _pti.vm.tilemap->height;

	const pti_tileset_t *tileset = _pti.vm.draw.tileset;
	const int tile_w = tileset->tile_w;
	const int tile_h = tileset->tile_h;

	const int tiles_per_row = tileset->width / tile_w;

	const int *tiles = (int *) _pti__ptr_to_bank((void *) _pti.vm.tilemap->tiles);
	void *pixels = (void *) _pti__ptr_to_bank((void *) tileset->pixels);

	_pti__transform(&x, &y);

	for (int j = 0; j < map_h; j++) {
		for (int i = 0; i < map_w; i++) {
			int t = tiles[i + j * map_w];
			if (t <= 0) {
				continue;
			}

			int src_x = (t % tiles_per_row) * tileset->tile_w;
			int src_y = (t / tiles_per_row) * tileset->tile_h;

			_pti__plot(
					tileset->pixels, 0,
					x + i * tileset->tile_w,
					y + j * tileset->tile_h,
					tileset->tile_w, tileset->tile_h,
					src_x, src_y,
					tileset->width, tileset->height,
					false, false);
		}
	}
}

void pti_spr(const pti_bitmap_t *sprite, int n, int x, int y, bool flip_x, bool flip_y) {
	const int bmp_w = sprite->width;
	const int bmp_h = sprite->height;
	void *pixels = (void *) _pti__ptr_to_bank((void *) sprite->pixels);
	_pti__transform(&x, &y);
	_pti__plot(pixels, n, x, y, bmp_w, bmp_h, 0, 0, bmp_w, bmp_h, flip_x, flip_y);
}

uint32_t _pti__next_utf8_code_point(const char *data, uint32_t *index, uint32_t end) {
	static const uint32_t utf8_offsets[6] = {
			0x00000000UL,
			0x00003080UL,
			0x000E2080UL,
			0x03C82080UL,
			0xFA082080UL,
			0x82082080UL,
	};

	uint32_t character = 0;
	const unsigned char *bytes = (const unsigned char *) data;
	int num_bytes = 0;
	do {
		character <<= 6;
		character += bytes[(*index)++];
		num_bytes++;
	} while (*index != end && ((bytes[*index]) & 0xC0) == 0x80);
	character -= utf8_offsets[num_bytes - 1];

	return character;
}

// FIXME: remove magic numbers
#define FONT_GLYPH_WIDTH (6)
#define FONT_GLYPH_HEIGHT (13)
#define FONT_GLYPHS_PER_ROW (96 / FONT_GLYPH_WIDTH)
#define FONT_TAB_SIZE (3)

void pti_print(const char *text, int x, int y) {
	void *pixels = (void *) _pti__ptr_to_bank((void *) _pti.vm.draw.font->pixels);
	int cursor_x = x;
	int cursor_y = y;
	uint32_t text_length = strlen(text);
	uint32_t index = 0;
	while (index < text_length) {
		uint32_t c = _pti__next_utf8_code_point(text, &index, text_length);
		if (c == '\t') {
			cursor_x += FONT_TAB_SIZE * FONT_GLYPH_WIDTH;
			continue;
		}
		if (c == '\n') {
			cursor_x = x;
			cursor_y += FONT_GLYPH_HEIGHT;
			continue;
		}
		if (c < 32 || c > 255) {
			cursor_x += FONT_GLYPH_WIDTH;
			continue;
		}

		int glyph_index = c - 32;
		int glyph_x = (glyph_index % FONT_GLYPHS_PER_ROW);
		int glyph_y = (glyph_index - glyph_x) / FONT_GLYPHS_PER_ROW;
		glyph_x *= FONT_GLYPH_WIDTH;
		glyph_y *= FONT_GLYPH_HEIGHT;

		int width = _pti.vm.draw.font->width;
		int height = _pti.vm.draw.font->height;
		_pti__plot(pixels, 0, cursor_x, cursor_y, FONT_GLYPH_WIDTH, FONT_GLYPH_HEIGHT, glyph_x, glyph_y, width, height, false, false);

		cursor_x += FONT_GLYPH_WIDTH;
	}
}

#endif// PTI_IMPL
