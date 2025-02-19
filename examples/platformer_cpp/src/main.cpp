/* engine */
#include "pti.h"

#include "bank.h"
#include "lib/assets.h"

#include "entity/coin.h"
#include "entity/goomba.h"
#include "entity/player.h"
#include "entity/registry.h"

#include <math.h>

namespace {
	constexpr int width = 240;
	constexpr int height = 135;
	auto t = 0.0f;

	// sweetie16 palette
	constexpr uint32_t pal[] = {
			0xff1a1c2c,
			0xff5d275d,
			0xffb13e53,
			0xffef7d57,
			0xffffcd75,
			0xffa7f070,
			0xff38b764,
			0xff257179,
			0xff29366f,
			0xff3b5dc9,
			0xff41a6f6,
			0xff73eff7,
			0xfff4f4f4,
			0xff94b0c2,
			0xff566c86,
			0xff333c57,
	};

	constexpr int EN_COUNT = 64;
	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 32;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 256;
	constexpr int EN_GRID_SIZE = 8;
}// namespace

#define XPOS(x) (x * EN_GRID_SIZE)
#define YPOS(y) (y * EN_GRID_SIZE)

static void load(void) {
	ResetAllEntities();
	assets::reload();

	int i, j, t;
	for (i = 0; i < EN_ROOM_COLS; i++) {
		for (j = 0; j < EN_ROOM_ROWS; j++) {
			t = pti_mget(tilemap, i, j);
			switch (t) {
				case 48: {
					auto *e = CreateEntity<Player>();
					e->SetLocation(XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
				} break;
				case 49: {
					auto *e = CreateEntity<Coin>();
					e->SetLocation(XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
				} break;
				case 50: {
					auto *e = CreateEntity<Goomba>();
					e->SetLocation(XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
				} break;
				case 51: {
					// CreateEntity<Shooter>();
					pti_mset(tilemap, i, j, 0);
				} break;
			}
		}
	}
}

static void init(void) {
	assets::init();
	tileset = assets::tileset("assets/tilemap.ase");
	tilemap = assets::tilemap("assets/tilemap.ase");
	bitmap_bullet = assets::sprite("assets/bullet.ase");
	bitmap_coin = assets::sprite("assets/coin.ase");
	bitmap_goomba = assets::sprite("assets/goomba.ase");
	bitmap_player = assets::sprite("assets/dog.ase");
	bitmap_shooter = assets::sprite("assets/goomba.ase");

	load();

	/* graphics state: */
	pti_dither(0x5a5a);
	pti_clip(0, 0, 240, 135);
}

static void cleanup(void) {
	/* ... */
}

static void frame(void) {
	if (pti_down(PTI_DBG)) {
		load();
	}

	UpdateAllEntities();

	pti_cls(0xffef7d57);

	/* adjust camera */
	int cam_x, cam_y;
	pti_get_camera(&cam_x, &cam_y);
	pti_camera(_pti_max(0, _pti_min(EN_ROOM_WIDTH - width, cam_x)),
			   _pti_max(0, _pti_min(EN_ROOM_HEIGHT - height, cam_y)));

	pti_map(tilemap, tileset, 0, 0);
	RenderAllEntities();
}

pti_desc pti_main(int argc, char *argv[]) {
	return (pti_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.memory_size = _pti_kilobytes(256), /* 256KB */
			.window =
					(pti_window) {
							.name = "pti - platformer",
							.width = 240,
							.height = 135,
							.flags = PTI_SCALE3X,
					},
	};
}