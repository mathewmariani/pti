/* pti */
#include "pti.h"

#include "../lib/assets.h"
#include "../lib/collisions.h"
#include "entities.h"
#include <cstdio>
#include <cstdlib>

typedef struct {
	int direction;
} bullet_t;

static const assets::sprite_t *sprite;

namespace bullet {
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

	typedef enum {
		STATE_NORMAL,
		STATE_DEATH,
	} state_t;

	inline void vertical_movement(entity::entity_t *self) {
		auto dir = ((bullet_t *) self->userdata)->direction;
		_pti_appr(self->sx, dir * max_speed, accel * PTI_DELTA);
	}

	inline bool is_touching(const entity::entity_t *self) {
		auto dir = ((bullet_t *) self->userdata)->direction;
		return collisions::place_meeting(self, dir, 0);
	}

}// namespace bullet

/* the entry point for an entity. */
void entity_bullet(entity::event_t *ev) {
	entity::entity_t *self = ev->self;
	bullet_t *data = ((bullet_t *) self->userdata);

	int frame = 0;

	switch (ev->type) {
		case entity::EVENTTYPE_INIT: {
			auto *bullet = (bullet_t *) malloc(sizeof(bullet_t));

			/* FIXME: this will cause an insufficient memory error */
			// bullet->sprite = pti_sprite_create("bullet.ase");
			sprite = assets::sprite("assets/bullet.ase");
			bullet->direction = 1;

			self->bx = -4;
			self->by = -8;
			self->bw = 8;
			self->bh = 8;
			self->flags =
					entity::ENTITYFLAG_OVERLAP_CHECKS | entity::ENTITYFLAG_HITS_SOLIDS;
			self->userdata = bullet;
		} break;
		case entity::EVENTTYPE_DESTROY: {
			free(self->userdata);
			entity::entity_t *other;
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
		} break;
		case entity::EVENTTYPE_UPDATE: {
			if (bullet::is_touching(self)) {
				entity::destroy(self);
			}
			bullet::vertical_movement(self);
		} break;
		case entity::EVENTTYPE_OVERLAP: {
			entity::entity_t *other = ev->other;
			if (other->type == entity_player) {
				/* bounce */
				if (other->sy > 0.0f) {
					if ((other->flags & entity::ENTITYFLAG_GROUNDED) == 0) {
						// other->state = PLAYER_STATE_JUMP;
						other->flags &= ~entity::ENTITYFLAG_GROUNDED;
						other->sy = -bullet::phy_jump_strength;

						// TODO: proper interactions
						entity::destroy(self);
					}
				} else {
					/* if were not moving downwards we should take damage. */
					std::printf("hello world\n");
				}
			}
		} break;
		case entity::EVENTTYPE_DRAW:
			// pti_sprite *sprite = data->sprite;
			// pti_plot_sprite(sprite, 0, self->x - 4, self->y - 4,
			//                 self->flags & entity::ENTITYFLAG_FACING_LEFT ? 1 : 0,
			//                 false);

			pti_plot(sprite->pixels, 0, self->x - 4, self->y - 4, 8, 8, false, false);

			// pti_rect(self->x + self->bx, self->y + self->by, self->bw, self->bh,
			//          0xffff0000);

			break;
	}
}