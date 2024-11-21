/* pti */
#include "pti.h"

#include "../bank.h"
#include "../lib/assets.h"
#include "../lib/collisions.h"
#include "entities.h"

namespace {
	constexpr float fire_rate = 1.0f;

	struct shooter_t {
		float shoot_timer = 0.0f;
	};
}// namespace

/* The entry point for an entity. */
void entity_shooter(entity::event_t *ev) {
	auto *self = ev->self;
	auto *data = static_cast<shooter_t *>(self->userdata);

	int frame = 0;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT: {
			auto *shooter = new shooter_t();
			self->bx = -4;
			self->by = -8;
			self->bw = 8;
			self->bh = 8;
			self->flags = entity::ENTITYFLAG_OVERLAP_CHECKS | entity::ENTITYFLAG_HITS_SOLIDS;
			self->userdata = static_cast<void *>(shooter);
		} break;

		case entity::EVENTTYPE_DESTROY: {
			delete static_cast<shooter_t *>(self->userdata);
		} break;

		case entity::EVENTTYPE_UPDATE: {
			data->shoot_timer -= PTI_DELTA;
			if (data->shoot_timer < 0.0f) {
				// Reset shoot timer
				data->shoot_timer = fire_rate;

				// Create entities for shooting and effects
				entity::manager.create(entity_bullet, self->x, self->y);

				auto *other = entity::manager.create(entity_pop, self->x, self->y);
				other->sx = 4;
				other->sy = 4;

				other = entity::manager.create(entity_pop, self->x, self->y);
				other->sx = -4;
				other->sy = 4;

				other = entity::manager.create(entity_pop, self->x, self->y);
				other->sx = 4;
				other->sy = -4;

				other = entity::manager.create(entity_pop, self->x, self->y);
				other->sx = -4;
				other->sy = -4;
			}
		} break;

		case entity::EVENTTYPE_OVERLAP: {
			// Handle overlap events here (currently empty)
		} break;

		case entity::EVENTTYPE_DRAW: {
			pti_spr(bitmap_shooter, frame, self->x - 8, self->y - 16,
					(self->flags & entity::ENTITYFLAG_FACING_LEFT) ? 1 : 0, false);
		} break;
	}
}
