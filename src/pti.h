#if defined(PTI_IMPL) && !defined(PTI_API_IMPL)
#define PTI_API_IMPL
#endif
#ifndef PTI_INCLUDED
/*
    pti.h -- docs
*/
#define PTI_INCLUDED (1)

// >>includes
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// >>>
// FIXME: FPS scales don't work (PTI_FPS30, PTI_FPS60, PTI_FPS144)
// FIXME: `pti_rect` and `pti_rectf` are two different filled rectangle algorithms.
// FIXME: `pti_rect` should be a rectangle line.
// TODO: implement `pti_stat`
// TODO: convert int types into stdint types
// TODO: implement bank switching
// <<<

#define _pti_kilobytes(n) (1024 * (n))
#define _pti_megabytes(n) (1024 * _pti_kilobytes(n))
#define _pti_gigabytes(n) (1024 * _pti_megabytes(n))
#define _pti_align_to(value, N) ((value + (N - 1)) & ~(N - 1))

#define _pti_min(x, y) ((x) < (y) ? (x) : (y))
#define _pti_max(x, y) ((x) > (y) ? (x) : (y))
#define _pti_clamp(x, a, b) (_pti_max(a, _pti_min(x, b)))
#define _pti_abs(n) ((n < 0) ? (-n) : (n))
#define _pti_appr(val, tar, delta)                   \
	(val = (val > (tar) ? _pti_max(tar, val - delta) \
						: _pti_min(tar, val + delta)))
#define _pti_sign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
#define _pti_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#define PTI_FRAMERATE 30
#define PTI_DELTA (1.0f / PTI_FRAMERATE)

#ifdef __cplusplus
extern "C" {
#endif

enum {
	PTI_SCALE2X = (1 << 0),
	PTI_SCALE3X = (1 << 1),
	PTI_SCALE4X = (1 << 2),
	PTI_HIDECURSOR = (1 << 3),
	PTI_FPS30 = (1 << 4),
	PTI_FPS60 = (1 << 5),
	PTI_FPS144 = (1 << 6),
	PTI_FPSINF = (1 << 7),
};

typedef enum pti_button {
	PTI_LEFT,
	PTI_RIGHT,
	PTI_UP,
	PTI_DOWN,
	PTI_A,
	PTI_B,

	PTI_DBG,

	// always last.
	PTI_BUTTON_COUNT
} pti_button;

typedef struct pti_window {
	const char *name;
	int width;
	int height;
	int flags;
} pti_window;

typedef struct pti_bitmap_t {
	int width;
	int height;
	void *pixels;
} pti_bitmap_t;

typedef struct pti_tileset_t {
	int width;
	int height;
	int tile_count;
	void *pixels;
} pti_tileset_t;

typedef struct pti_tilemap_t {
	int width;
	int height;
	void *tiles;
} pti_tilemap_t;

// typedef struct {
// 	int bitmap_count;
// 	pti_bitmap_t *bitmaps;
// 	pti_tileset_t tileset;
// 	pti_tilemap_t tilemap;
// } pti_bank_t;

typedef struct {
	uint8_t *begin;
	uint8_t *end;
	uint8_t *it;
	uint8_t *cap;
} pti_bank_t;

typedef struct pti_desc {
	void (*init_cb)(void);
	void (*frame_cb)(void);
	void (*cleanup_cb)(void);

	int memory_size;
	pti_window window;
} pti_desc;

/* user-provided function */
extern pti_desc pti_main(int argc, char *argv[]);

/* public functions */
void pti_init(const pti_desc *desc);

/* api functions */

void *_pti__alloc(const uint32_t size);

//>> virutal machine api
void pti_bank_init(pti_bank_t *bank, uint32_t capacity);
void *pti_bank_push(pti_bank_t *bank, uint32_t size);

//>> memory api
void pti_reload(const pti_bank_t *bank);
void pti_cstore(void *dst, const void *src, size_t len);
void pti_memcpy(void *dst, const void *src, size_t len);
void pti_memset(void *dst, const int value, size_t len);
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
uint32_t pti_mget(const pti_tilemap_t *tilemap, int x, int y);
void pti_mset(pti_tilemap_t *tilemap, int x, int y, int value);
short pti_fget(const pti_tilemap_t *tilemap, int x, int y);

uint16_t pti_prand(void);

//>> gfx api
void pti_camera(int x, int y);
void pti_get_camera(int *x, int *y);
void pti_cls(const uint32_t color);
void pti_colorkey(const uint32_t color);
void pti_dither(const uint16_t bstr);
void pti_clip(const int x0, const int y0, const int x1, const int y1);
void pti_pset(const int x, const int y, uint64_t color);
void pti_line(int x0, int y0, int x1, int y1, uint64_t color);
void pti_rect(int x, int y, int w, int h, uint64_t color);
void pti_rectf(int x0, int y0, int x1, int y1, uint64_t color);
void pti_plot(void *pixels, int n, int x, int y, int w, int h, bool flip_x, bool flip_y);
void pti_map(const pti_tilemap_t *tilemap, const pti_tileset_t *tileset, int x, int y);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif// PTI_INCLUDED

#ifdef PTI_API_IMPL
#define PTI_API_IMPL_INCLUDED (1)

// >>implementation

#ifndef _PTI_PRIVATE
#if defined(__GNUC__) || defined(__clang__)
#define _PTI_PRIVATE __attribute__((unused)) static
#else
#define _PTI_PRIVATE static
#endif
#endif
#ifndef _PTI_UNUSED
#define _PTI_UNUSED(x) (void) (x)
#endif

#if !defined(PTI_ASSERT)
#include <assert.h>
#define PTI_ASSERT(c) assert(c)
#endif

#if defined(_WIN32)
#include <memoryapi.h>
#else
#include <sys/mman.h>
#endif

typedef struct {
	uint8_t *begin;
	uint8_t *end;
	uint8_t *it;
	uint8_t *cap;
} _pti__memory_t;

typedef struct {
	int16_t clip_x0, clip_y0;
	int16_t clip_x1, clip_y1;
	int16_t cam_x, cam_y;
	uint16_t dither;
	uint32_t ckey;
} _pti__drawstate_t;

typedef struct {
	uint8_t bank_id;
	uint8_t btn_state[PTI_BUTTON_COUNT];
	uint8_t rnd_reg[4];
} _pti__hardwarestate_t;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint32_t *vram;
} _pti__screen_t;

typedef struct {
	_pti__drawstate_t draw;
	_pti__hardwarestate_t hardware;
	_pti__screen_t screen;
} _pti__vm_t;

// <<< new stuff

typedef struct {
	pti_desc desc;
	_pti__memory_t memory;
	void *bank;
	_pti__vm_t vm;
} _pti__t;
static _pti__t _pti;

_PTI_PRIVATE void *_pti__virtual_reserve(void *ptr, const uint32_t size) {
#if defined(_WIN32)
	/* Reserves a range of the process's virtual offsetess space without
	* allocating any actual physical storage in memory or in the paging file on
  * disk. */
	/* Disables all access to the committed region of pages. An attempt to read
   * from, write to, or execute the committed region results in an access
   * violation.
	 */
	ptr = VirtualAlloc(ptr, size, MEM_RESERVE, PAGE_NOACCESS);
	PTI_ASSERT(ptr);
#else
	/* Create a private copy-on-write mapping. */
	/* The mapping is not backed by any file; */
	/* its contents are initialized to zero. */
	uint16_t flags = (MAP_PRIVATE | MAP_ANON);
	ptr = mmap((void *) ptr, size, (PROT_READ | PROT_WRITE), flags, -1, 0);
	PTI_ASSERT(ptr != MAP_FAILED);
	msync(ptr, size, (MS_SYNC | MS_INVALIDATE));
#endif
	return ptr;
}

_PTI_PRIVATE void *_pti__virtual_commit(void *ptr, const uint32_t size) {
#if defined(_WIN32)
	/* Enables read-only or read/write access to the committed region of pages. */
	ptr = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
	PTI_ASSERT(ptr);
#else
	uint16_t flags = (MAP_FIXED | MAP_SHARED | MAP_ANON);
	ptr = mmap(ptr, size, (PROT_READ | PROT_WRITE), flags, -1, 0);
	PTI_ASSERT(ptr != MAP_FAILED);
	/* Requests an update and waits for it to complete. */
	/* Asks to invalidate other mappings of the same file (so
   * that they can be updated with the fresh values just written).
   */
	msync(ptr, size, (MS_SYNC | MS_INVALIDATE));
#endif
	return ptr;
}

_PTI_PRIVATE void *_pti__virtual_decommit(void *ptr, const uint32_t size) {
#if defined(_WIN32)
	VirtualFree(ptr, size, LMEM_DECOMMIT);
#else
	uint16_t flags = (MAP_FIXED | MAP_SHARED | MAP_ANON);
	mmap(ptr, size, PROT_NONE, flags, -1, 0);
	msync(ptr, size, (MS_SYNC | MS_INVALIDATE));
#endif
	return ptr;
}

_PTI_PRIVATE void *_pti__virtual_alloc(void *ptr, const uint32_t size) {
	return _pti__virtual_commit(_pti__virtual_reserve(ptr, size), size);
}

_PTI_PRIVATE void _pti__virtual_free(void *ptr, const uint32_t size) {
#if defined(_WIN32)
	_PTI_UNUSED(size);
	VirtualFree((void *) ptr, 0, LMEM_RELEASE);
#else
	/* Requests an update and waits for it to complete. */
	msync(ptr, size, MS_SYNC);
	munmap(ptr, size);
#endif
}

void *_pti__alloc(const uint32_t size) {
	void *ptr;
	_pti__memory_t *bank = &_pti.memory;
	if ((bank->end - bank->it) < size) {
		PTI_ASSERT((bank->cap - bank->it) >= size);
		uint32_t additional_size = size - (uint32_t) (bank->end - bank->it);
		additional_size = _pti_min((bank->cap - bank->it), _pti_align_to(additional_size, 4096));
		ptr = _pti__virtual_commit(bank->end, additional_size);
		bank->end += additional_size;
	}
	ptr = bank->it;
	bank->it += size;
	return ptr;
}

_PTI_PRIVATE void _pti__free(_pti__memory_t *bank) {
	_pti__virtual_free(bank, (uint32_t) (bank->cap - bank->begin));
	memset(bank, 0, sizeof(_pti__memory_t));
}

// >>input
enum {
	_PTI_KEY_STATE = (1 << 0),
	_PTI_KEY_PRESSED = (1 << 1),
	_PTI_KEY_RELEASED = (1 << 2),
};

_PTI_PRIVATE bool _pti__check_input_flag(uint32_t idx, int flag) {
	return _pti.vm.hardware.btn_state[idx] & flag ? true : false;
}

bool pti_down(pti_button btn) {
	return _pti__check_input_flag(btn, _PTI_KEY_STATE);
}

bool pti_pressed(pti_button btn) {
	return _pti__check_input_flag(btn, _PTI_KEY_PRESSED);
}

bool pti_released(pti_button btn) {
	return _pti__check_input_flag(btn, _PTI_KEY_RELEASED);
}

// >>graphics

// >>random

// >>internals
_PTI_PRIVATE void _pti__scale_size_by_flags(int *w, int *h, int flags) {
	if (flags & PTI_SCALE2X) {
		*w *= 2;
		*h *= 2;
	} else if (flags & PTI_SCALE3X) {
		*w *= 3;
		*h *= 3;
	} else if (flags & PTI_SCALE4X) {
		*w *= 4;
		*h *= 4;
	}
}

_PTI_PRIVATE void _pti__random_init(const pti_desc *desc) {
	for (int i = 0; i < 4; ++i) {
		_pti.vm.hardware.rnd_reg[i] = 0x0;
	}
}

// >>public
void pti_init(const pti_desc *desc) {
	/* cache description */
	_pti.desc = *desc;

	/* allocate memory */
	const size_t size = desc->memory_size;
	void *ptr = _pti__virtual_reserve(NULL, size);

	_pti.memory = (_pti__memory_t){
			.begin = ptr,
			.it = ptr,
			.end = ptr,
			.cap = ptr + size,
	};

	/* allocate virtual machine */
	_pti.bank = _pti__alloc(_pti_kilobytes(512));
	_pti.vm = *(_pti__vm_t *) _pti__alloc(sizeof(_pti__vm_t));

	/* allocate screen */
	const int window_w = desc->window.width;
	const int window_h = desc->window.height;
	const int screen_size = window_w * window_h * sizeof(uint32_t);

	_pti.vm.screen = (_pti__screen_t){
			.width = window_w,
			.height = window_h,
			.vram = _pti__alloc(screen_size),
	};
}

// >>memory api

void pti_bank_init(pti_bank_t *bank, uint32_t capacity) {
	/* allocate memory */
	void *ptr = _pti__virtual_reserve(NULL, capacity);
	bank->begin = ptr;
	bank->it = ptr;
	bank->end = ptr;
	bank->cap = ptr + capacity;
}

void *pti_bank_push(pti_bank_t *bank, uint32_t size) {
	void *ptr;
	if ((bank->end - bank->it) < size) {
		PTI_ASSERT((bank->cap - bank->it) >= size);
		uint32_t additional_size = size - (uint32_t) (bank->end - bank->it);
		additional_size = _pti_min((bank->cap - bank->it), _pti_align_to(additional_size, 4096));
		ptr = _pti__virtual_commit(bank->end, additional_size);
		bank->end += additional_size;
	}
	ptr = bank->it;
	bank->it += size;
	return ptr;
}

void pti_reload(const pti_bank_t *bank) {
	/* TODO: implementation. */
	pti_memcpy(_pti.bank, bank->begin, (uint32_t) (bank->cap - bank->begin));
}

void pti_cstore(void *dst, const void *src, size_t len) {
	/* TODO: implementation. */
	pti_memcpy(dst, src, len);
}

void pti_memcpy(void *dst, const void *src, size_t len) {
	memcpy(dst, src, len);
}

void pti_memset(void *dst, const int value, size_t len) {
	memset(dst, value, len);
}

const uint8_t pti_peek(const uint32_t offset, const uint32_t index) {
	const void *dst = (void *) _pti.memory.begin;
	return *(const uint8_t *) (dst + offset + index);
}

const uint16_t pti_peek2(const uint32_t offset, const uint32_t index) {
	const uint8_t b0 = pti_peek(offset, index + 0);
	const uint8_t b1 = pti_peek(offset, index + 1);

	/* combine 16-bit value */
	return (uint16_t) ((b0 << 8) | (b1 << 0));
}

const uint32_t pti_peek4(const uint32_t offset, const uint32_t index) {
	const uint8_t b0 = pti_peek(offset, index + 0);
	const uint8_t b1 = pti_peek(offset, index + 1);
	const uint8_t b2 = pti_peek(offset, index + 2);
	const uint8_t b3 = pti_peek(offset, index + 3);

	/* combine 32-bit value */
	return (uint32_t) ((b0 << 24) | (b1 << 16) | (b2 << 8) | (b3 << 0));
}

void pti_poke(const uint32_t offset, const uint32_t index, const uint8_t value) {
	const void *dst = (void *) _pti.memory.begin;
	*(uint8_t *) (dst + offset + index) = value;
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
	return ((uint16_t) * (reg + 2) << 0x8) | *(reg + 3);
}

uint32_t pti_mget(const pti_tilemap_t *tilemap, int x, int y) {
	return *((int *) tilemap->tiles + x + y * tilemap->width);
}

void pti_mset(pti_tilemap_t *tilemap, int x, int y, int value) {
	*((int *) tilemap->tiles + x + y * tilemap->width) = value;
}

short pti_fget(const pti_tilemap_t *tilemap, int x, int y) {
	return (short) pti_mget(tilemap, x, y);
}

_PTI_PRIVATE bool _pti__get_dither_bit(const int x, const int y) {
	const uint8_t i = 0xf - ((x & 0x3) + 0x4 * (y & 0x3));
	return ((_pti.vm.draw.dither & (1 << i)) >> i) != 0;
}

_PTI_PRIVATE void _pti__transform(int *x, int *y) {
	*x -= _pti.vm.draw.cam_x;
	*y -= _pti.vm.draw.cam_y;
}

_PTI_PRIVATE void _pti__set_pixel(int x, int y, uint64_t c) {
	const int16_t clip_x0 = _pti.vm.draw.clip_x0;
	const int16_t clip_y0 = _pti.vm.draw.clip_y0;
	const int16_t clip_x1 = _pti.vm.draw.clip_x1;
	const int16_t clip_y1 = _pti.vm.draw.clip_y1;

	if (x < clip_x0 || x >= clip_x1 || y < clip_y0 || y >= clip_y1) {
		return;
	}

	uint32_t *vram = _pti.vm.screen.vram;
	const int screen_w = _pti.vm.screen.width;
	*(vram + (x + y * screen_w)) = _pti__get_dither_bit(x, y) ? (c >> 32) & 0xffffffff : (c >> 0) & 0xffffffff;
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
	const size_t size = screen_w * screen_h * sizeof(uint32_t);
	pti_memset(_pti.vm.screen.vram, color, size);
}

void pti_colorkey(const uint32_t color) {
	_pti.vm.draw.ckey = color;
}

void pti_dither(const uint16_t bstr) {
	_pti.vm.draw.dither = bstr;
}

void pti_clip(const int x0, const int y0, const int x1, const int y1) {
	_pti.vm.draw.clip_x0 = x0;
	_pti.vm.draw.clip_y0 = y0;
	_pti.vm.draw.clip_x1 = x1;
	_pti.vm.draw.clip_y1 = y1;
}

void pti_pset(const int x, const int y, uint64_t color) {
	_pti__set_pixel(x, y, color);
}

void pti_line(int x0, int y0, int x1, int y1, uint64_t c) {
	bool steep = false;
	if (_pti_abs(x1 - x0) < _pti_abs(y1 - y0)) {
		_pti_swap(x0, y0);
		_pti_swap(x1, y1);
		steep = true;
	}

	if (x0 > x1) {
		_pti_swap(x0, x1);
		_pti_swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int de = 2 * _pti_abs(dy);
	int err = 0;
	int y = y0;

	for (int x = x0; x <= x1; x++) {
		if (steep) {
			_pti__set_pixel(y, x, c);
		} else {
			_pti__set_pixel(x, y, c);
		}
		err += de;
		if (err > dx) {
			y += y1 > y0 ? 1 : -1;
			err -= dx * 2;
		}
	}
}

void pti_rect(int x, int y, int w, int h, uint64_t color) {
	int px, py, l, t, r, b;

	_pti__transform(&x, &y);

	l = _pti_max(_pti.vm.draw.clip_x0, x);
	t = _pti_max(_pti.vm.draw.clip_y0, y);
	r = _pti_min(_pti.vm.draw.clip_x1, x + w);
	b = _pti_min(_pti.vm.draw.clip_y1, y + h);

	for (py = t; py < b; py++) {
		for (px = l; px < r; px++) {
			_pti__set_pixel(px, py, color);
		}
	}
}

void pti_rectf(int x0, int y0, int x1, int y1, uint64_t color) {
	if (x0 > x1) {
		_pti_swap(x0, x1);
	}
	if (y0 > y1) {
		_pti_swap(y0, y1);
	}

	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			_pti__set_pixel(x, y, color);
		}
	}
}

void pti_plot(void *pixels, int n, int x, int y, int w, int h, bool flip_x, bool flip_y) {
	// adjust camera:
	_pti__transform(&x, &y);

	int src_x = 0;
	int src_y = 0;
	int dst_x1 = x;
	int dst_y1 = y;
	int dst_x2 = x + w - 1;
	int dst_y2 = y + h - 1;
	int src_x1 = src_x;
	int src_y1 = src_y;

	// clip:
	const int16_t clip_x0 = _pti.vm.draw.clip_x0;
	const int16_t clip_y0 = _pti.vm.draw.clip_y0;
	const int16_t clip_x1 = _pti.vm.draw.clip_x1;
	const int16_t clip_y1 = _pti.vm.draw.clip_y1;

	if (dst_x1 >= clip_x1 || dst_x2 < clip_x0) {
		return;
	}
	if (dst_y1 >= clip_y1 || dst_y2 < clip_y0) {
		return;
	}

	if (dst_x1 < clip_x0) {
		src_x1 -= dst_x1;
		dst_x1 = clip_x0;
	}
	if (dst_y1 < clip_y0) {
		src_y1 -= dst_y1;
		dst_y1 = clip_y0;
	}
	if (dst_x2 >= clip_x1) {
		dst_x2 = clip_x1 - 1;
	}
	if (dst_y2 >= clip_y1) {
		dst_y2 = clip_y1 - 1;
	}

	const int ix = flip_x ? -1 : 1;
	const int iy = flip_y ? -1 : 1;

	if (flip_x) {
		src_x1 += w - 1;
	}
	if (flip_y) {
		src_y1 += h - 1;
	}

	const size_t size = w * h * sizeof(int);
	uint32_t *src = pixels + size * n;
	uint32_t *dst = _pti.vm.screen.vram;

	const int dst_width = _pti.desc.window.width;
	const int src_width = w;

	int clipped_width = dst_x2 - dst_x1 + 1;
	int dst_next_row = dst_width - clipped_width;
	int src_next_row = (flip_x && flip_y)
							   ? (src_width - clipped_width)
					   : (flip_x || flip_y)
							   ? (src_width + clipped_width)
							   : (src_width - clipped_width);

	uint32_t *dst_pixel = dst + dst_y1 * dst_width + dst_x1;
	uint32_t *src_pixel = src + src_y1 * src_width + src_x1;
	uint32_t color_key = _pti.vm.draw.ckey;
	for (int dst_y = dst_y1; dst_y <= dst_y2; dst_y++) {
		for (int i = 0; i < clipped_width; i++) {
			uint32_t src_color = *src_pixel;
			uint32_t dst_color = *dst_pixel;
			*dst_pixel = src_color != color_key ? src_color : dst_color;
			src_pixel += ix;
			dst_pixel++;
		}
		dst_pixel += dst_next_row;
		src_pixel += src_next_row * iy;
	}
}

/* FIXME: implement full functionality. */
/** celx  : The column location of the map cell in the upper left corner of the region to draw, where 0 is the leftmost column. */
/** cely  : The row location of the map cell in the upper left corner of the region to draw, where 0 is the topmost row. */
/** sx    : The x coordinate of the screen to place the upper left corner. */
/** sy    : The y coordinate of the screen to place the upper left corner. */
/** celw  : The number of map cells wide in the region to draw. */
/** celh  : The number of map cells tall in the region to draw. */
void pti_map(const pti_tilemap_t *tilemap, const pti_tileset_t *tileset, int x, int y) {
	const int map_w = tilemap->width;
	const int map_h = tilemap->height;
	const int tile_w = tileset->width;
	const int tile_h = tileset->height;
	int i, j, t;
	for (j = 0; j < map_h; j++) {
		for (i = 0; i < map_w; i++) {
			t = *((int *) tilemap->tiles + i + j * map_w);
			if (t == 0) {
				continue;
			}
			pti_plot(tileset->pixels, t, x + (i * tile_h), y + (j * tile_w), tile_h, tile_w, false, false);
		}
	}
}

// FIXME: get font stuff from mac backup.
// TODO: get font stuff from mac backup.

#endif// PTI_API_IMPL
