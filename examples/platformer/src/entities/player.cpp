/* pti */
#include "pti.h"

#include "../bank.h"
#include "../lib/assets.h"
#include "../lib/collisions.h"
#include "entities.h"

namespace {
	constexpr float max_speed = 3.0f;
	constexpr float accel = 20.0f;
	constexpr float friction = 15.0f;
	constexpr float phy_vertical_max = 6.0f;
	constexpr float phy_vertical_grav = 0.24f;
	constexpr float phy_vertical_grav_fall = 0.4254f;
	constexpr float phy_vertical_grav_alt = 0.1940f;
	constexpr float phy_jump_strength = 6.1135f;
	constexpr float phy_bounce_strength = 4.5535f;

	enum PlayerState {
		Normal,
		Jump,
		Death,
	};

	struct player_t {
		PlayerState state = PlayerState::Normal;
	};

	static void handle_player_horizontal_movement(entity::entity_t *self) {
		if (pti_down(PTI_LEFT)) {
			_pti_appr(self->sx, -max_speed, accel * PTI_DELTA);
			self->flags |= entity::ENTITYFLAG_FACING_LEFT;
		} else if (pti_down(PTI_RIGHT)) {
			_pti_appr(self->sx, max_speed, accel * PTI_DELTA);
			self->flags &= ~entity::ENTITYFLAG_FACING_LEFT;
		} else {
			_pti_appr(self->sx, 0, friction * PTI_DELTA);
		}
	}

	static void handle_player_vertical_movement(entity::entity_t *self) {
		static int hang_time = 0;

		if ((self->flags & entity::ENTITYFLAG_GROUNDED) == 0) {
			if (self->state == PlayerState::Jump) {
				if (self->sy <= -0.5f) {
					hang_time = 3;
					self->sy += phy_vertical_grav_fall;
				} else {
					if (hang_time > 0) {
						--hang_time;
						self->sy = 0;
					} else {
						self->sy += phy_vertical_grav_fall;
					}
				}
			} else {
				self->sy += phy_vertical_grav_fall;
			}
		} else {
			self->sy = 0;
		}

		// Limit vertical speed
		if (self->sy > phy_vertical_max) {
			self->sy = phy_vertical_max;
		}
	}

	static void handle_player_jump(entity::entity_t *self) {
		bool kJump = pti_pressed(PTI_UP);

		// Full jump
		if (kJump && (self->flags & entity::ENTITYFLAG_GROUNDED)) {
			self->state = PlayerState::Jump;
			self->flags &= ~entity::ENTITYFLAG_GROUNDED;
			self->sy = -phy_jump_strength;
		}

		// Revert state
		if (self->state == PlayerState::Jump && (self->flags & entity::ENTITYFLAG_GROUNDED)) {
			self->state = PlayerState::Normal;
		}

		// Variable jump
		if (self->state == PlayerState::Jump && self->sy < -(phy_jump_strength * 0.5f)) {
			if (pti_released(PTI_UP)) {
				self->sy = -(phy_jump_strength * 0.5f);
			}
		}
	}
}// namespace

void entity_player(entity::event_t *ev) {
	auto *self = ev->self;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT: {
			auto *player = new player_t();
			self->bx = -4;
			self->by = -8;
			self->bw = 8;
			self->bh = 8;
			self->flags = entity::ENTITYFLAG_OVERLAP_CHECKS | entity::ENTITYFLAG_HITS_SOLIDS;
			self->state = PlayerState::Normal;
			self->userdata = static_cast<void *>(player);
		} break;

		case entity::EVENTTYPE_DESTROY: {
			delete static_cast<player_t *>(self->userdata);
		} break;

		case entity::EVENTTYPE_UPDATE: {
			// Ground check
			if (collisions::is_grounded(self)) {
				self->flags |= entity::ENTITYFLAG_GROUNDED;
			} else {
				self->flags &= ~entity::ENTITYFLAG_GROUNDED;
			}

			handle_player_horizontal_movement(self);
			handle_player_vertical_movement(self);
			handle_player_jump(self);

			// Camera movement
			constexpr int cx = 256;
			constexpr int cy = 128;

			int cam_x, cam_y;
			pti_get_camera(&cam_x, &cam_y);
			cam_x += ((self->x + self->bx + self->bw / 2.0f - cx / 2.0f) - cam_x) / 10;
			cam_y += ((self->y + self->by + self->bh / 2.0f - cy / 2.0f) - cam_y) / 10;
			pti_camera(cam_x, cam_y);
		} break;

		case entity::EVENTTYPE_OVERLAP: {
			if (ev->other->type == entity_coin) {
				entity::manager.destroy(ev->other);
			}
		} break;

		case entity::EVENTTYPE_DRAW: {
			auto frame = static_cast<int>(self->timer * 8) % 2;
			if (self->sx == 0 && self->sy == 0) {
				frame = 0;
			}
			pti_spr(bitmap_player, frame, self->x - 8, self->y - 16,
					self->flags & entity::ENTITYFLAG_FACING_LEFT ? 1 : 0, false);
		} break;
	}
}
