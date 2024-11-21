#include "game.h"
#include "assets/sprite.h"
#include "components/collider.h"
#include "components/mover.h"
#include "components/player.h"
#include "components/tilemap.h"
#include "bank.h"
#include "factory.h"
#include "masks.h"
#include <_types/_uint32_t.h>
#include <cstdlib>

#include "pti.h"

using namespace TL;

namespace {
	constexpr float transition_duration = 0.4f;
}

void Game::startup() {
	world.game = this;

	// load our content
	// Content::load();

	// framebuffer for the game
	// buffer = Target::create(width, height);

	// set batcher to use Nearest Filter
	// batch.default_sampler = TextureSampler(TextureFilter::Nearest);
	m_draw_colliders = false;

	// load first room
	load_room(Point(0, 0));
	camera = Vec2f(room.x * width, room.y * height);
	fullscreen = false;
}

void Game::load_room(Point cell, bool is_reload) {
	// BLAH_ASSERT(grid, "Room doesn't exist!");
	room = cell;

	// get room offset
	auto offset = Point(cell.x * width, cell.y * height);

	// make the floor
	auto floor = world.add_entity(offset);
	auto tilemap = floor->add(Tilemap(8, 8, columns, rows));
	auto solids = floor->add(Collider::make_grid(8, 40, 23));
	solids->mask = Mask::solid;

	// loop over the room grid
	for (int x = 0; x < columns; x++)
		for (int y = 0; y < rows; y++) {
			Point world_position = offset + Point(x * tile_width, y * tile_height) +
								   Point(tile_width / 2, tile_height);
			// uint32_t col = grid[x + y * columns];
			uint32_t col = pti_mget(cell.x * columns + x, cell.y * rows + y);
			switch (col) {
				// black does nothing
				case 0x000000:
					break;

				// castle tiles
				case 0xffffff:
					tilemap->set_cell(x, y, 3);
					solids->set_cell(x, y, true);
					break;

				// grass tiles
				case 0x8f974a:
					tilemap->set_cell(x, y, 1);
					solids->set_cell(x, y, true);
					break;

				// plants tiles
				case 0x4b692f:
					tilemap->set_cell(x, y, 4);
					break;

				// back tiles
				// 45283c
				case 0x45283c:
					tilemap->set_cell(x, y, 2);
					break;

				// jumpthru tiles
				case 0xdf7126: {
					tilemap->set_cell(x, y, 5);
					auto jumpthru_en = world.add_entity(offset + Point(x * tile_width, y * tile_height));
					auto jumpthru_col = jumpthru_en->add(Collider::make_rect(Recti(0, 0, 8, 4)));
					jumpthru_col->mask = Mask::jumpthru;
					break;
				}

				// player (only if it doesn't already exist)
				case 0x6abe30:
					if (!world.first<Player>())
						Factory::player(&world, world_position + (is_reload ? Point(0, -16) : Point::zero));
					break;

				// brambles
				case 0xd77bba:
					Factory::bramble(&world, world_position);
					break;

				// spitter plant
				case 0xac3232:
					Factory::spitter(&world, world_position);
					break;

				// mosquito
				case 0xfbf236:
					Factory::mosquito(&world, world_position + Point(0, -8));
					break;

				// door
				case 0x9badb7:
					Factory::door(&world, world_position);
					break;

				// closing door
				case 0x847e87:
					Factory::door(&world, world_position, !is_reload);
					break;

				// blob
				case 0x3f3f74:
					Factory::blob(&world, world_position);
					break;

				case 0x76428a:
					Factory::ghost_frog(&world, world_position + Point(-4, 0));
					break;
			}
		}
}

void Game::shutdown() {}

void Game::update() {

	// Toggle Collider Render
	// if (Input::pressed(Key::F1))
	//   m_draw_colliders = !m_draw_colliders;

	// Reload Current Room
	// if (Input::pressed(Key::F2)) {
	//   m_transition = false;
	//   world.clear();
	//   load_room(room);
	// }

	// Reload First Room
	// if (Input::pressed(Key::F9))
	// {
	// 	m_transition = false;
	// 	world.clear();
	// 	load_room(Point(0, 0));
	// 	camera = Vec2f(0, 0);
	// }

	// Toggle Fullscreen
	// if (Input::pressed(Key::F4))
	// 	App::fullscreen(fullscreen = !fullscreen);

	// Normal Update
	if (!m_transition) {
		// Screen Shake
		m_shake_timer -= Time::delta;
		if (m_shake_timer > 0) {
			if (Time::on_interval(0.05f)) {
				m_shake.x = rand_int(0, 2) == 0 ? -1 : 1;
				m_shake.y = rand_int(0, 2) == 0 ? -1 : 1;
			}
		} else
			m_shake = Point::zero;

		// Upodate Objects
		world.update();

		// Check for transition / death
		auto player = world.first<Player>();
		if (player) {
			auto pos = player->entity()->position;
			auto bounds = Recti(room.x * width, room.y * height, width, height);
			if (!bounds.contains(pos)) {
				// target room
				Point next_room = Point(pos.x / width, pos.y / height);
				if (pos.x < 0)
					next_room.x--;
				if (pos.y < 0)
					next_room.y--;

				// see if room exists
				if (player->health > 0 && true /* Content::find_room(next_room) */ &&
					next_room.x >= room.x) {
					Time::pause_for(0.1f);

					// transiton to it!
					m_transition = true;
					m_next_ease = 0;
					m_next_room = next_room;
					m_last_room = room;

					// store entities from the previous room
					m_last_entities.clear();
					Entity *e = world.first_entity();
					while (e) {
						m_last_entities.push_back(e);
						e = e->next();
					}

					// load contents of the next room
					load_room(next_room);
				}
				// doesn't exist, clamp player
				else {
					player->entity()->position =
							Point(Calc::clamp(pos.x, bounds.x, bounds.x + bounds.w),
								  Calc::clamp(pos.y, bounds.y, bounds.y + bounds.h + 100));

					// reload if they fell out the bottom
					if (player->entity()->position.y > bounds.y + bounds.h + 64) {
						world.clear();
						load_room(room, true);
					}
				}
			}

			// death ... delete everything except the player
			// then when they fall out of the screen, we reset
			if (player->health <= 0) {
				Entity *e = world.first_entity();
				while (e) {
					auto next = e->next();
					if (!e->get<Player>())
						world.destroy_entity(e);
					e = next;
				}
			}
		}
	}
	// Room Transition routine
	else {
		// increment ease
		m_next_ease = Calc::approach(m_next_ease, 1.0f, Time::delta / transition_duration);

		// get last & next camera position
		auto last_cam = Vec2f(m_last_room.x * width, m_last_room.y * height);
		auto next_cam = Vec2f(m_next_room.x * width, m_next_room.y * height);

		// LERP camera position
		camera = last_cam + (next_cam - last_cam) * Ease::cube_in_out(m_next_ease);

		// Finish Transition
		if (m_next_ease >= 1.0f) {
			// boost player on vertical up rooms
			if (m_next_room.y < m_last_room.y) {
				auto player = world.first<Player>();
				if (player)
					player->get<Mover>()->speed = Vec2f(0, -150);
			}

			// delete old objects (except player!)
			for (auto &it : m_last_entities) {
				if (!it->get<Player>())
					world.destroy_entity(it);
			}

			Time::pause_for(0.1f);
			m_transition = false;
		}
	}
}

void Game::render() {
	pti_cls(0xff150e22);
	// // draw gameplay stuff
	// {
	//   buffer->clear(0x150e22);

	//   // push camera offset
	//   batch.push_matrix(Mat3x2f::create_translation(-camera + m_shake));

	//   // draw gameplay objects
	pti_camera(camera.x + m_shake.x, camera.y + m_shake.y);
	world.render();

	//   // draw debug colliders
	if (m_draw_colliders) {
		auto collider = world.first<Collider>();
		while (collider) {
			collider->render();
			collider = (Collider *) collider->next();
		}
	}

	// hacky start / end screen text
	if (room == Point(0, 0) || m_last_room == Point(0, 0)) {
		// auto w = Content::font.width_of(title);
		// auto pos = Point((width - w) / 2, 20);
		// auto pos = Point(20, 20);
		// pti_print(title, pos.x, pos.y + 1);
		// pti_print(title, pos.x, pos.y + 0);
		// pti_print(controls, pos.x, pos.y + 20);
		// batch.str(Content::font, title, pos + Point(0, 1), Color::black);
		// batch.str(Content::font, title, pos, Color::white);

		// w = Content::font.width_of(controls);
		// pos.x = (width - w) / 2;
		// pos.y += 20;
		// batch.str(Content::font, controls, pos, Color::white * 0.25f);
	} else if (room == Point(13, 0)) {
		// auto w = Content::font.width_of(ending);
		// auto pos = Point(room.x * width + width / 2, room.y * height + 20);
		// batch.str(Content::font, ending, pos + Point(0, 1), Vec2f(0.0, 0.0), 8,
		//           Color::black);
		// batch.str(Content::font, ending, pos, Vec2f(0.0, 0.0), 8, Color::white);
	}

	//   // end camera offset
	//   batch.pop_matrix();

	pti_camera(0, 0);

	// draw the health
	auto player = world.first<Player>();
	if (player) {
		auto hearts = bank::sprite("assets/heart.ase");
		// auto hearts = Content::find_sprite("heart");
		// auto full = hearts->get_animation("full");
		// auto empty = hearts->get_animation("empty");

		Point pos = Point(0, height - 16);
		// batch.rect(Rect(pos.x, pos.y + 7, 40, 4), Color::black);
		pti_rect(pos.x, pos.y + 7, 40, 4, 0xff000000);

		int n, w, h;
		for (int i = 0; i < Player::max_health; i++) {
			if (player->health >= i + 1) {
				pti_spr(hearts, n, pos.x, pos.y, false, false);
			} else {
				pti_spr(hearts, n, pos.x, pos.y, false, false);
			}
			pos.x += 12;
		}
	}
}

void Game::shake(float time) { m_shake_timer = time; }

int Game::rand_int(int min, int max) { return min + (rand() % (max - min)); }
