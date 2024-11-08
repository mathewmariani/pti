/* pti */
#include "pti.h"

/* stdlib */
#include <algorithm>
#include <math.h>
#include <stdbool.h>

#include "../lib/assets.h"
#include "entities.h"

static const assets::sprite_t *sprite;

void entity_coin(entity::event_t *ev) {
	entity::entity_t *self = ev->self;
	entity::entity_t *other;
	int frame;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT:
			sprite = assets::sprite("assets/coin.ase");
			self->bx = -2;
			self->by = -2;
			self->bw = 4;
			self->bh = 4;
			self->flags = entity::ENTITYFLAG_OVERLAP_CHECKS;
			break;
		case entity::EVENTTYPE_DESTROY:
			break;
		case entity::EVENTTYPE_UPDATE:
		case entity::EVENTTYPE_OVERLAP:
			break;
		case entity::EVENTTYPE_DRAW:
			frame = ((int) (self->timer * 8) % 2);
			// pti_sspr(sprite.pixels, frame, self->x - 2, self->y - 2, false, false);
			pti_plot(sprite->pixels, frame, self->x - 2, self->y - 2, 8, 8, false, false);

			break;
	}
}

// void create_coin(int x, int y) {
//   entity::entity_t* e = entity::create_ext()
//   sprite = pti_sprite_create("coin.ase");
//   e->bx = -2;
//   e->by = -2;
//   e->bw = 4;
//   e->bh = 4;
//   e->flags = entity::ENTITYFLAG_OVERLAP_CHECKS;

//   e->draw = [](entity::entity_t* self) {
//     auto frame = ((int)(self->timer * 8) % 2);
//     pti_plot_sprite(sprite, frame, self->x - 2, self->y - 2, false, false);
//   };
// }