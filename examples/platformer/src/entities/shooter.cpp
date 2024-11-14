/* pti */
#include "pti.h"

#include "../bank.h"
#include "../lib/assets.h"
#include "../lib/collisions.h"
#include "entities.h"
#include <cstdio>
#include <cstdlib>

namespace shooter {

	constexpr float fire_rate = 1.0f;

	typedef enum {
		STATE_NORMAL,
		STATE_SHOOT,
		STATE_DEATH,
	} state_t;

}// namespace shooter

typedef struct {
	float shoot_timer;
} shooter_t;

/* the entry point for an entity. */
void entity_shooter(entity::event_t *ev) {
	entity::entity_t *self = ev->self;
	shooter_t *data = ((shooter_t *) self->userdata);

	int frame = 0;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT: {
			auto *shooter = (shooter_t *) malloc(sizeof(shooter_t));
			self->bx = -4;
			self->by = -8;
			self->bw = 8;
			self->bh = 8;
			self->flags =
					entity::ENTITYFLAG_OVERLAP_CHECKS | entity::ENTITYFLAG_HITS_SOLIDS;
			self->userdata = shooter;
		} break;
		case entity::EVENTTYPE_DESTROY:
			free(self->userdata);
			break;
		case entity::EVENTTYPE_UPDATE: {
			data->shoot_timer -= PTI_DELTA;
			if (data->shoot_timer < 0.0f) {
				/* TODO: we shoot. */
				data->shoot_timer = shooter::fire_rate;

				entity::entity_t *other;
				other = entity::create(entity_bullet, self->x, self->y);
				other = entity::create(entity_pop, self->x, self->y);
				other->sx = 4;
				other->sy = 4;
				other = entity::create(entity_pop, self->x, self->y);
				other->sx = -4;
				other->sy = 4;
				other = entity::create(entity_pop, self->x, self->y);
				other->sx = 4;
				other->sy = -4;
				other = entity::create(entity_pop, self->x, self->y);
				other->sx = -4;
				other->sy = -4;
			}
		} break;
		case entity::EVENTTYPE_OVERLAP: {
		} break;
		case entity::EVENTTYPE_DRAW:
			pti_spr(bitmap_shooter, frame, self->x - 8, self->y - 16, self->flags & entity::ENTITYFLAG_FACING_LEFT ? 1 : 0, false);
			break;
	}
}