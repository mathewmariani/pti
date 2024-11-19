/* pti */
#include "pti.h"

#include "../bank.h"
#include "entities.h"

#include "../lib/collisions.h"

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

	// FIXME: magic numbers
	constexpr int cx = 256;
	constexpr int cy = 128;
}// namespace

void Player::Init(void) {
	bx = -4;
	by = -8;
	bw = 8;
	bh = 8;
	timer = 0.0f;
	flags = (ENTITYFLAG_OVERLAP_CHECKS | ENTITYFLAG_HITS_SOLIDS);
	state = PLAYER_STATE_NORMAL;
}

void Player::Destroy(void) {
	// ...
}

void Player::Update(void) {
	/* ground check */
	if (Collisions::IsGroudned(*this)) {
		flags |= ENTITYFLAG_GROUNDED;
	} else {
		flags &= ~ENTITYFLAG_GROUNDED;
	}

	// handle_player_horizontal_movement
	if (pti_down(PTI_LEFT)) {
		_pti_appr(sx, -max_speed, accel * PTI_DELTA);
		flags |= ENTITYFLAG_FACING_LEFT;
	} else if (pti_down(PTI_RIGHT)) {
		_pti_appr(sx, max_speed, accel * PTI_DELTA);
		flags &= ~ENTITYFLAG_FACING_LEFT;
	} else {
		_pti_appr(sx, 0, friction * PTI_DELTA);
	}

	// handle_player_vertical_movement
	if ((flags & ENTITYFLAG_GROUNDED) == 0) {
		if (state == PLAYER_STATE_JUMP) {
			if (sy <= -0.5) {
				hang_time = 3;
				sy = sy + phy_vertical_grav_fall;
			} else {
				if (hang_time > 0) {
					hang_time = hang_time - 1;
					sy = 0;
				} else {
					sy = sy + phy_vertical_grav_fall;
				}
			}
		} else {
			sy = sy + phy_vertical_grav_fall;
		}
	} else {
		sy = 0;
	}
	/* limit: */
	if (sy > phy_vertical_max) {
		sy = phy_vertical_max;
	}

	// handle_player_jump
	bool kJump = pti_pressed(PTI_UP);

	/* full jump: */
	if (kJump && ((flags & ENTITYFLAG_GROUNDED) == ENTITYFLAG_GROUNDED)) {
		state = PLAYER_STATE_JUMP;
		flags &= ~ENTITYFLAG_GROUNDED;
		sy = -phy_jump_strength;
	}

	/* revert state: */
	if (state == PLAYER_STATE_JUMP && ((flags & ENTITYFLAG_GROUNDED) == ENTITYFLAG_GROUNDED)) {
		state = PLAYER_STATE_NORMAL;
	}

	/* variable jump: */
	if (state == PLAYER_STATE_JUMP) {
		if (sy < -(phy_jump_strength * 0.5)) {
			if (pti_released(PTI_UP)) {
				sy = -(phy_jump_strength * 0.5);
			}
		}
	}

	/* make camera approach player */
	int cam_x, cam_y;
	pti_get_camera(&cam_x, &cam_y);
	cam_x += ((x + bx + bw / 2.0f - cx / 2.0f) - cam_x) / 10;
	cam_y += ((y + by + bh / 2.0f - cy / 2.0f) - cam_y) / 10;
	pti_camera(cam_x, cam_y);
}

void Player::Draw(void) {
	frame = ((int) (timer * 8) % 2);
	if (sx == 0 && sy == 0) {
		frame = 0;
	}
	pti_spr(bitmap_player, frame, x - 8, y - 16, flags & ENTITYFLAG_FACING_LEFT ? 1 : 0, false);
}

void Player::Overlap(Entity &other) {
	if (dynamic_cast<Coin *>(&other)) {
		other.Destroy();
	}
}