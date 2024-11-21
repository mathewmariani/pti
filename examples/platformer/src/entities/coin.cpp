/* pti */
#include "pti.h"

#include "../bank.h"
#include "entities.h"

namespace {
	constexpr int COIN_BOUNDARY_OFFSET = -2;
	constexpr int COIN_WIDTH = 4;
	constexpr int COIN_HEIGHT = 4;
	constexpr int FRAME_COUNT = 8;
	constexpr int FRAME_MOD = 2;
}// namespace

void entity_coin(entity::event_t *ev) {
	auto *self = ev->self;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT:
			self->bx = COIN_BOUNDARY_OFFSET;
			self->by = COIN_BOUNDARY_OFFSET;
			self->bw = COIN_WIDTH;
			self->bh = COIN_HEIGHT;
			self->flags = entity::ENTITYFLAG_OVERLAP_CHECKS;
			break;

		case entity::EVENTTYPE_DESTROY:
			break;

		case entity::EVENTTYPE_UPDATE:
		case entity::EVENTTYPE_OVERLAP:
			break;

		case entity::EVENTTYPE_DRAW:
			auto frame = static_cast<int>(self->timer * FRAME_COUNT) % FRAME_MOD;
			pti_spr(bitmap_coin, frame, self->x - COIN_WIDTH / 2, self->y - COIN_HEIGHT / 2, false, false);
			break;
	}
}
