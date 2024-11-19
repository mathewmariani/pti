/* pti */
#include "pti.h"

#include "../bank.h"
#include "entities.h"

#include "../lib/collisions.h"

namespace {
	static const float max_speed = 1.0f;
	static const float accel = 20.0f;
	static const float friction = 15.0f;
	static const float phy_vertical_max = 6.0f;
	static const float phy_vertical_grav = 0.24f;
	static const float phy_vertical_grav_fall = 0.4254f;
	static const float phy_vertical_grav_alt = 0.1940f;
	static const float phy_jump_strength = 6.1135f;
	static const float phy_bounce_strength = 4.5535f;
}// namespace

void Goomba::Init(void) {
	bx = -4;
	by = -8;
	bw = 8;
	bh = 8;
	timer = 0.0f;
	direction = 1;
	flags = (ENTITYFLAG_OVERLAP_CHECKS | ENTITYFLAG_HITS_SOLIDS);
}

void Goomba::Destroy(void) {
	// ...
}

void Goomba::Update(void) {
	/* ground check */
	if (Collisions::IsGroudned(*this)) {
		flags |= ENTITYFLAG_GROUNDED;
	} else {
		flags &= ~ENTITYFLAG_GROUNDED;
	}

	// handle_horizontal_movement
	_pti_appr(sx, direction * max_speed, accel * PTI_DELTA);

	// handle_vertical_movement
	/* simple gravity */
	if ((flags & ENTITYFLAG_GROUNDED) == 0) {
		sy = sy + phy_vertical_grav_fall;
		if (sy > phy_vertical_max) {
			sy = phy_vertical_max;
		}
	} else {
		sy = 0;
	}

	if (Collisions::PlaceMeeting(*this, direction, 0)) {
		direction *= -1;
		flags ^= ENTITYFLAG_FACING_LEFT;
	}
}

void Goomba::Draw(void) {
	frame = ((int) (timer * 8) % 2);
	if (sx == 0 && sy == 0) {
		frame = 0;
	}
	pti_spr(bitmap_goomba, frame, x - 8, y - 16, (flags & ENTITYFLAG_FACING_LEFT) ? 1 : 0, false);
}

void Goomba::Overlap(Entity &other) {
	if (dynamic_cast<Goomba *>(&other)) {
		direction *= -1;
		x += direction;
		flags ^= ENTITYFLAG_FACING_LEFT;
		sx *= -1;
	} else if (dynamic_cast<Player *>(&other)) {
		/* bounce */
		if (other.sy > 0.0f) {
			if ((other.flags & ENTITYFLAG_GROUNDED) == 0) {
				// other.state = Player::PLAYER_STATE_JUMP;
				other.flags &= ~ENTITYFLAG_GROUNDED;
				other.sy = -phy_jump_strength;
				// TODO: proper interactions
				// entity::destroy(self);
			}
		} else {
			/* if were not moving downwards we should take damage. */
			printf("hello world\n");
		}
	}
}