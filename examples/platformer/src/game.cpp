/* engine */
#include "pti.h"

#include "game.h"

#include "lib/assets.h"
#include "lib/entity.h"

// entity types
#include "entities/entities.h"

namespace {
	constexpr float transition_duration = 0.4f;
	constexpr int width = 240;
	constexpr int height = 135;
}// namespace

namespace game {
	assets::tileset_t *_tileset;
	assets::tilemap_t *_tilemap;
}// namespace game

#define XPOS(x) (x * entity::EN_GRID_SIZE)
#define YPOS(y) (y * entity::EN_GRID_SIZE)

void game::startup() {
	assets::init();

	_tileset = assets::tileset("assets/tilemap.ase");
	_tilemap = assets::tilemap("assets/tilemap.ase");

	game::load();
}

void game::load() {
	assets::reload();
	entity::clear();

	int i, j, t;
	for (i = 0; i < entity::EN_ROOM_COLS; i++) {
		for (j = 0; j < entity::EN_ROOM_ROWS; j++) {
			t = pti_mget(_tilemap, i, j);
			switch (t) {
				case 48:
					entity::create(entity_player, XPOS(i), YPOS(j));
					pti_mset(_tilemap, i, j, 0);
					break;
				case 49:
					entity::create(entity_coin, XPOS(i), YPOS(j));
					pti_mset(_tilemap, i, j, 0);
					break;
				case 50:
					entity::create(entity_goomba, XPOS(i), YPOS(j));
					pti_mset(_tilemap, i, j, 0);
					break;
				case 51:
					entity::create(entity_shooter, XPOS(i), YPOS(j));
					pti_mset(_tilemap, i, j, 0);
					break;
			}
		}
	}
}

void game::shutdown() {}

void game::update() {
	if (pti_down(PTI_DBG)) {
		game::load();
	}

	entity::update();
}

void game::render() {
	pti_cls(0xffef7d57);

	/* adjust camera */
	int cam_x, cam_y;
	pti_get_camera(&cam_x, &cam_y);
	pti_camera(_pti_max(0, _pti_min(entity::EN_ROOM_WIDTH - width, cam_x)),
			   _pti_max(0, _pti_min(entity::EN_ROOM_HEIGHT - height, cam_y)));

	/* draw tilemap */
	// pti_plot_tilemap(0, 0);
	pti_map(_tilemap, _tileset, 0, 0);

	/* draw entities */
	entity::draw();
}

#undef CREATE