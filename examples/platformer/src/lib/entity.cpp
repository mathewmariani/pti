#include "pti.h"

#include "collisions.h"
#include "entity.h"
#include "../game.h"

#include <algorithm>

namespace entity {

	entity_state manager;
	const entity_t null_entity{};

	entity_t *create(const entity_type type, int x, int y) {
		unsigned int id = 0;
		event_t ev;

		for (id = 0; id < EN_COUNT; id++) {
			if (manager.all[id].type == NULL) {
				ev.type = EVENTTYPE_INIT;
				ev.self = manager.all + id;
				*ev.self = null_entity;
				ev.self->type = type;
				ev.self->x = x;// * EN_GRID_SIZE;
				ev.self->y = y;// * EN_GRID_SIZE;
				ev.self->timer = 0.0f;
				ev.self->type(&ev);
				break;
			}
		}
		return (id >= 0 && id < EN_COUNT ? manager.all + id : NULL);
	}

	void destroy(entity_t *entity) {
		event_t ev;
		int id = entity - manager.all;
		if (id >= 0 && id < EN_COUNT && manager.all[id].type != NULL) {
			ev.type = EVENTTYPE_DESTROY;
			ev.self = manager.all + id;
			ev.self->type(&ev);
			ev.self->type = NULL;
		}
	}

	inline bool can_wiggle(entity_t *self) {
		int i, j;
		for (i = 0; i < 4; i++) {
			for (j = -1; j <= 1; j += 2) {
				if (!collisions::place_meeting(self, i * j, -1)) {
					self->x += (i * j);
					return true;
				}
			}
		}
		return false;
	}

	void clear() {
		for (int i = 0; i < EN_COUNT; i++) {
			manager.all[i] = null_entity;
		}
	}

	void update() {
		int i, j, nx, ny;
		entity::event_t ev = {
				.type = entity::EVENTTYPE_UPDATE,
		};

		/* run entities */
		for (i = 0; i < entity::EN_COUNT; i++) {
			ev.self = entity::manager.all + i;

			if (ev.self->type == NULL) {
				continue;
			}

			ev.self->timer += 1.0f / 60.0f;

			/* get move amount */
			nx = ev.self->sx + ev.self->rx;
			ny = ev.self->sy + ev.self->ry;

			/* try to move */
			if ((ev.self->flags & entity::ENTITYFLAG_HITS_SOLIDS) != 0) {
				j = nx;
				int dx = _pti_sign(j);
				while (j != 0) {
					// moving up slope:
					if (collisions::place_meeting(ev.self, dx, 0) &&
						!(collisions::place_meeting(ev.self, dx, -1))) {
						ev.self->y -= 1;
					}
					/* moving down slope: */
					if (!(collisions::place_meeting(ev.self, dx, 0)) &&
						!(collisions::place_meeting(ev.self, dx, 1)) &&
						collisions::place_meeting(ev.self, dx, 2)) {
						ev.self->y += 1;
					}

					/* always last */
					if (collisions::place_meeting(ev.self, dx, 0)) {
						ev.self->sx = ev.self->rx = nx = 0;
						break;
					}
					ev.self->x += dx;
					j -= dx;
				}

				j = ny;
				int dy = _pti_sign(j);
				while (j != 0) {

					if (ev.self->sy < 0 && !can_wiggle(ev.self)) {
						ev.self->sy = ev.self->ry = ny = 0;
						break;
					}
					if (collisions::place_meeting(ev.self, 0, dy)) {
						ev.self->sy = ev.self->ry = ny = 0;
						break;
					}
					ev.self->y += dy;
					j -= dy;
				}
			} else {
				ev.self->x += nx;
				ev.self->y += ny;
			}

			/* update move remainder */
			ev.self->rx = ev.self->sx + ev.self->rx - nx;
			ev.self->ry = ev.self->sy + ev.self->ry - ny;

			/* call their update event */
			ev.self->type(&ev);
		}

		/* do overlap tests */
		ev.type = entity::EVENTTYPE_OVERLAP;
		for (i = 0; i < entity::EN_COUNT; i++) {
			ev.self = entity::manager.all + i;
			if ((ev.self->flags & entity::ENTITYFLAG_OVERLAP_CHECKS) == 0) {
				continue;
			}
			if (ev.self->type == NULL || ev.self->bw < 0 || ev.self->bh <= 0) {
				continue;
			}

			for (j = 0; j < entity::EN_COUNT; j++) {
				ev.other = entity::manager.all + j;
				if (i == j ||
					(ev.other->flags & entity::ENTITYFLAG_OVERLAP_CHECKS) == 0) {
					continue;
				}
				if (ev.other->type == NULL || ev.other->bw < 0 || ev.other->bh <= 0) {
					continue;
				}
				if (ev.self->type != NULL && collisions::overlaps(ev.self, ev.other)) {
					ev.self->type(&ev);
				}
			}
		}
	}
	void draw() {
		int i;
		entity::event_t ev = {
				.type = entity::EVENTTYPE_DRAW,
		};
		/* draw entities */
		for (i = 0; i < entity::EN_COUNT; i++) {
			ev.self = entity::manager.all + i;
			if (ev.self->type != NULL) {
				ev.self->type(&ev);
			}
		}
	}

}// namespace entity