/* pti */
#include "pti.h"

#include "../bank.h"
#include "entities.h"

#include "../lib/collisions.h"

namespace {
	constexpr float fire_rate = 1.0f;
}

void Shooter::Init(void) {
	bx = -4;
	by = -8;
	bw = 8;
	bh = 8;
	flags = (Entity::ENTITYFLAG_OVERLAP_CHECKS | Entity::ENTITYFLAG_HITS_SOLIDS);
}

void Shooter::Destroy(void) {
	// ...
}

void Shooter::Update(void) {
	shoot_timer -= PTI_DELTA;
	if (shoot_timer < 0.0f) {
		shoot_timer = fire_rate;
		// entity::entity_t *other;
		// other = entity::create(entity_bullet, self->x, self->y);
		// other = entity::create(entity_pop, self->x, self->y);
		// other->sx = 4;
		// other->sy = 4;
		// other = entity::create(entity_pop, self->x, self->y);
		// other->sx = -4;
		// other->sy = 4;
		// other = entity::create(entity_pop, self->x, self->y);
		// other->sx = 4;
		// other->sy = -4;
		// other = entity::create(entity_pop, self->x, self->y);
		// other->sx = -4;
		// other->sy = -4;
	}
}

void Shooter::Draw(void) {
	frame = ((int) (timer * 8) % 2);
	pti_spr(bitmap_shooter, frame, x - 8, y - 16, flags & Entity::ENTITYFLAG_FACING_LEFT ? 1 : 0, false);
}

void Shooter::Overlap(Entity &other) {
	// ...
}