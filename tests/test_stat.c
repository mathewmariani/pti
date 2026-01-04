// stdlib
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// engine
#include "pti.h"

// tests
#include "assets.h"

pti_bank_t bank;

// forward declarations
static void init(void);
static void cleanup(void);
static void frame(void);

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(1024),
			.width = 128,
			.height = 128,
	};
}

static pti_bitmap_t bitmap;

static void init(void) {
	pti_bank_init(&bank, _pti_kilobytes(128));
	init_assets(&bank);
	bitmap = create_bitmap("assets/font.ase");

	pti_set_font(&bitmap);
	pti_load_bank(&bank);
}

static void cleanup(void) {}

void pti_format_date(char *out, size_t out_size) {
	int year = pti_stat(PTI_STATTYPE_LOCAL_YEAR).i;
	int month = pti_stat(PTI_STATTYPE_LOCAL_MONTH).i;
	int day = pti_stat(PTI_STATTYPE_LOCAL_DAY).i;
	snprintf(out, out_size, "%04d-%02d-%02d", year, month, day);
}

void pti_format_time(char *out, size_t out_size) {
	int hour = pti_stat(PTI_STATTYPE_LOCAL_HOUR).i;
	int min = pti_stat(PTI_STATTYPE_LOCAL_MIN).i;
	int sec = pti_stat(PTI_STATTYPE_LOCAL_SEC).i;
	snprintf(out, out_size, "%02d:%02d:%02d", hour, min, sec);
}

static char buf[64];

static void frame(void) {
	pti_cls(0x00000000);
	pti_color(0xffffffff);

	pti_format_date(buf, sizeof(buf));
	pti_print(buf, 4, 4);

	pti_format_time(buf, sizeof(buf));
	pti_print(buf, 4, 12);

	// capacity
	snprintf(buf, sizeof(buf), "capacity: %zu kb", pti_stat(PTI_STATTYPE_CART_CAPACITY).zu / 1024);
	pti_print(buf, 4, 20);

	// usage
	snprintf(buf, sizeof(buf), "usage: %zu kb", pti_stat(PTI_STATTYPE_CART_USAGE).zu / 1024);
	pti_print(buf, 4, 28);

	// remaining
	snprintf(buf, sizeof(buf), "remaining: %zu kb", pti_stat(PTI_STATTYPE_CART_REMAINING).zu / 1024);
	pti_print(buf, 4, 36);
}