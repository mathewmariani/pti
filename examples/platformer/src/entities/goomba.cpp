/* pti */
#include "pti.h"

#include "../bank.h"
#include "../lib/assets.h"
#include "../lib/collisions.h"
#include "entities.h"

namespace {
	static constexpr float max_speed = 1.0f;
	static constexpr float accel = 20.0f;
	static constexpr float friction = 15.0f;
	static constexpr float phy_vertical_max = 6.0f;
	static constexpr float phy_vertical_grav = 0.24f;
	static constexpr float phy_vertical_grav_fall = 0.4254f;
	static constexpr float phy_vertical_grav_alt = 0.1940f;
	static constexpr float phy_jump_strength = 6.1135f;
	static constexpr float phy_bounce_strength = 4.5535f;

	struct goomba_t {
		int direction = 1;
	};

	static bool is_touching(const entity::entity_t *self) {
		auto *goomba = static_cast<goomba_t *>(self->userdata);
		return collisions::place_meeting(self, goomba->direction, 0);
	}

	static void handle_horizontal_movement(entity::entity_t *self) {
		auto *goomba = static_cast<goomba_t *>(self->userdata);
		_pti_appr(self->sx, goomba->direction * max_speed, accel * PTI_DELTA);
	}

	static void handle_vertical_movement(entity::entity_t *self) {
		if ((self->flags & entity::ENTITYFLAG_GROUNDED) == 0) {
			self->sy += phy_vertical_grav_fall;
			if (self->sy > phy_vertical_max) {
				self->sy = phy_vertical_max;
			}
		} else {
			self->sy = 0;
		}
	}
}// namespace

void entity_goomba(entity::event_t *ev) {
	auto *self = ev->self;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT: {
			auto *goomba = new goomba_t();
			goomba->direction = 1;

			self->bx = -4;
			self->by = -8;
			self->bw = 8;
			self->bh = 8;
			self->flags = entity::ENTITYFLAG_OVERLAP_CHECKS | entity::ENTITYFLAG_HITS_SOLIDS;
			self->userdata = static_cast<void *>(goomba);
		} break;

		case entity::EVENTTYPE_DESTROY: {
			std::vector<std::pair<int, int>> velocities = {{4, 4}, {-4, 4}, {4, -4}, {-4, -4}};
			for (const auto &[vx, vy] : velocities) {
				if (auto pop = entity::manager.create(entity_pop, self->x, self->y)) {
					pop->sx = vx;
					pop->sy = vy;
				}
			}

			delete static_cast<goomba_t *>(self->userdata);
		} break;

		case entity::EVENTTYPE_UPDATE: {
			// Ground check
			if (collisions::is_grounded(self)) {
				self->flags |= entity::ENTITYFLAG_GROUNDED;
			} else {
				self->flags &= ~entity::ENTITYFLAG_GROUNDED;
			}

			handle_horizontal_movement(self);
			handle_vertical_movement(self);

			if (is_touching(self)) {
				auto *goomba = static_cast<goomba_t *>(self->userdata);
				goomba->direction *= -1;
				self->flags ^= entity::ENTITYFLAG_FACING_LEFT;
			}
		} break;

		case entity::EVENTTYPE_OVERLAP: {
			auto *other = ev->other;
			if (other->type == entity_goomba) {
				auto *goomba = static_cast<goomba_t *>(self->userdata);
				goomba->direction *= -1;
				self->x += goomba->direction;
				self->sx = -self->sx;
				self->flags ^= entity::ENTITYFLAG_FACING_LEFT;
			} else if (other->type == entity_player) {
				if (other->sy > 0.0f) {
					if ((other->flags & entity::ENTITYFLAG_GROUNDED) == 0) {
						other->flags &= ~entity::ENTITYFLAG_GROUNDED;
						other->sy = -phy_jump_strength;
						entity::manager.destroy(self);
					}
				} else {
					std::printf("hello world\n");
				}
			}
		} break;

		case entity::EVENTTYPE_DRAW:
			auto frame = static_cast<int>(self->timer * 8) % 2;
			if (self->sx == 0 && self->sy == 0) {
				frame = 0;
			}
			pti_spr(bitmap_goomba, frame, self->x - 8, self->y - 16, self->flags & entity::ENTITYFLAG_FACING_LEFT ? 1 : 0, false);
			break;
	}
}
