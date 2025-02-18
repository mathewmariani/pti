/* engine */
#include "pti.h"

#include "bank.h"
#include "lib/assets.h"
#include "lib/entity.h"
#include "entities/entities.h"

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
}// namespace

#define XPOS(x) (x * entity::EN_GRID_SIZE)
#define YPOS(y) (y * entity::EN_GRID_SIZE)

static void load(void) {
	entity::manager.clear();
	assets::reload();

	int i, j, t;
	for (i = 0; i < entity::EN_ROOM_COLS; i++) {
		for (j = 0; j < entity::EN_ROOM_ROWS; j++) {
			t = pti_mget(tilemap, i, j);
			switch (t) {
				case 48:
					entity::manager.create(entity_player, XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
					break;
				case 49:
					entity::manager.create(entity_coin, XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
					break;
				case 50:
					entity::manager.create(entity_goomba, XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
					break;
				case 51:
					entity::manager.create(entity_shooter, XPOS(i), YPOS(j));
					pti_mset(tilemap, i, j, 0);
					break;
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

	entity::manager.update();

	// pti_cls(0xffef7d57);

	/* adjust camera */
	int cam_x, cam_y;
	pti_get_camera(&cam_x, &cam_y);
	pti_camera(_pti_max(0, _pti_min(entity::EN_ROOM_WIDTH - width, cam_x)),
			   _pti_max(0, _pti_min(entity::EN_ROOM_HEIGHT - height, cam_y)));

	//>> lissajous curves:
	auto half_width = width * 0.5f;
	auto half_height = height * 0.5f;
	t += (1 / 30.0f);

	auto cx1 = sinf(t / 3) * half_width;
	auto cy1 = cosf(t / 2) * half_height;
	auto cx2 = -cx1;
	auto cy2 = -cy1;

	for (auto y = 0; y < height; y++) {
		auto dy1 = (y - cy1 - half_height) * (y - cy1 - half_height);
		auto dy2 = (y - cy2 - half_height) * (y - cy2 - half_height);
		for (auto x = 0; x < width; x++) {
			auto dx1 = (x - cx1 - half_width) * (x - cx1 - half_width);
			auto dx2 = (x - cx2 - half_width) * (x - cx2 - half_width);
			int c = (((int) sqrtf(dx1 + dy1) ^ (int) sqrtf(dx2 + dy2)) >> 4);
			if (rand() % 5 == 1) {
				if ((c & 1) == 0) {
					pti_pset(x, y, (unsigned long int) pal[1] << 32 | pal[2]);
				} else {
					pti_pset(x, y, (unsigned long int) pal[3] << 32 | pal[4]);
				}
			}
		}
	}
	//>> =============

	pti_map(tilemap, tileset, 0, 0);
	entity::manager.draw();
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