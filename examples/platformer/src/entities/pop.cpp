/* pti */
#include "pti.h"

#include "entities.h"

void entity_pop(entity::event_t *ev) {
	entity::entity_t *self = ev->self;
	entity::entity_t *other;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT:
			self->bw = 0;
			self->bh = 0;
			break;
		case entity::EVENTTYPE_DESTROY:
			break;
		case entity::EVENTTYPE_UPDATE:
			_pti_appr(self->sx, 0, 32 * PTI_DELTA);
			_pti_appr(self->sy, 0, 32 * PTI_DELTA);
			if (self->timer > 0.20f) {
				entity::destroy(self);
			}
			break;
		case entity::EVENTTYPE_OVERLAP:
			break;
		case entity::EVENTTYPE_DRAW:
			pti_rect(self->x - 1, self->y - 1, 2, 2, 0xffffcd75);
			break;
	}
}