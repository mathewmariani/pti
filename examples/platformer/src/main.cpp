/* engine */
#include "pti.h"

#include "bank.h"
#include "gamestate.h"
#include "lib/assets.h"

#include "entity/coin.h"
#include "entity/goomba.h"
#include "entity/player.h"
#include "entity/shooter.h"
#include "entity/registry.h"

#include <math.h>

#include <string>

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

	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 46;
	constexpr int EN_GRID_SIZE = 8;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 368;


	float resetTimer = 0.0f;
}// namespace

#define XPOS(x) (x * EN_GRID_SIZE)
#define YPOS(y) (y * EN_GRID_SIZE)

static void load(void) {
	GameStateInit();
	assets::reload();

	int i, j, t;
	for (i = 0; i < EN_ROOM_COLS; i++) {
		for (j = 0; j < EN_ROOM_ROWS; j++) {
			t = pti_mget(tilemap, i, j);
			switch (t) {
				case 48: {
					if (auto *e = CreateEntity<Player>(); e) {
						e->SetLocation({XPOS(i), YPOS(j)});
						pti_mset(tilemap, i, j, 0);
					}
				} break;
				case 49:
					Coin::Create({XPOS(i), YPOS(j)});
					pti_mset(tilemap, i, j, 0);
					break;
				case 50: {
					Goomba::Create({XPOS(i), YPOS(j)});
					pti_mset(tilemap, i, j, 0);
				} break;
				case 51: {
					if (auto *e = CreateEntity<Shooter>(); e) {
						e->SetLocation({XPOS(i), YPOS(j)});
						pti_mset(tilemap, i, j, 0);
					}
				} break;
			}
		}
	}
}

static void init(void) {
	assets::init();
	tileset = assets::tileset("assets/tilemap_2.ase");
	tilemap = assets::tilemap("assets/tilemap_2.ase");

	bitmap_bullet = assets::sprite("assets/bullet.ase");
	bitmap_coin = assets::sprite("assets/coin.ase");
	bitmap_goomba = assets::sprite("assets/goomba.ase");
	bitmap_player = assets::sprite("assets/dog.ase");
	bitmap_shooter = assets::sprite("assets/goomba.ase");
	bitmap_font = assets::sprite("assets/font.ase");

	bitmap_fx_collect = assets::sprite("assets/collect.ase");
	bitmap_fx_dust1 = assets::sprite("assets/dust1.ase");
	bitmap_fx_dust2 = assets::sprite("assets/dust2.ase");

	load();

	/* graphics state: */
	pti_dither(0x5a5a);
	pti_clip(0, 0, 240, 135);
}

static void cleanup(void) {
	/* ... */
}

static void frame(void) {
	auto &gameState = GetGameState();
	if (pti_down(PTI_DBG)) {
		load();
		return;
	}

	if (gameState.PlayerIsDead) {
		gameState.ResetTimer += PTI_DELTA;
		if (gameState.ResetTimer >= kDeathResetTimer) {
			gameState.PlayerIsDead = false;
			gameState.ResetTimer = 0.0f;
			load();
			return;
		}
	}

	GameStateTick();

	pti_cls(0xffef7d57);

	/* adjust camera */
	int cam_x, cam_y;
	pti_get_camera(&cam_x, &cam_y);
	pti_camera(_pti_max(0, _pti_min(EN_ROOM_WIDTH - width, cam_x)),
			   _pti_max(0, _pti_min(EN_ROOM_HEIGHT - height, cam_y)));

	pti_map(tilemap, tileset, 0, 0);
	RenderAllEntities();

	/* render ui */
	// const auto coin_str = std::format("coins: &d\n", coins);
	char buffer[100];
	std::snprintf(buffer, sizeof(buffer), "coins: %d\n", gameState.Coins);
	std::string coin_str(buffer);

	pti_print(bitmap_font, coin_str.c_str(), 0, 64);
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