/* pti */
#include "pti.h"

#include "../bank.h"
#include "entities.h"

#include "../lib/collisions.h"

namespace {
	constexpr float max_speed = 1.0f;
	constexpr float accel = 20.0f;
	constexpr float friction = 15.0f;
	constexpr float phy_vertical_max = 6.0f;
	constexpr float phy_vertical_grav = 0.24f;
	constexpr float phy_vertical_grav_fall = 0.4254f;
	constexpr float phy_vertical_grav_alt = 0.1940f;
	constexpr float phy_jump_strength = 6.1135f;
	constexpr float phy_bounce_strength = 4.5535f;
	constexpr float fire_rate = 1.0f;
}// namespace

void Bullet::Init(void) {
	bx = -4;
	by = -8;
	bw = 8;
	bh = 8;
	flags = (Entity::ENTITYFLAG_OVERLAP_CHECKS | Entity::ENTITYFLAG_HITS_SOLIDS);
	direction = 1;
}
void Bullet::Destroy(void) {
	// ...
}
void Bullet::Update(void) {
	if (Collisions::PlaceMeeting(*this, direction, 0)) {
		// entity::destroy(self);
	}
	_pti_appr(sx, direction * max_speed, accel * PTI_DELTA);
}

void Bullet::Draw(void) {
	pti_spr(bitmap_bullet, 0, x - 4, y - 4, false, false);
}

void Bullet::Overlap(Entity &other) {
	// entity::entity_t *other = ev->other;
	// if (other->type == entity_player) {
	// 	/* bounce */
	// 	if (other->sy > 0.0f) {
	// 		if ((other->flags & entity::ENTITYFLAG_GROUNDED) == 0) {
	// 			// other->state = PLAYER_STATE_JUMP;
	// 			other->flags &= ~entity::ENTITYFLAG_GROUNDED;
	// 			other->sy = -bullet::phy_jump_strength;
	// 			// TODO: proper interactions
	// 			entity::destroy(self);
	// 		}
	// 	} else {
	// 		/* if were not moving downwards we should take damage. */
	// 		std::printf("hello world\n");
	// 	}
	// }
}