#include "pti.h"

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

	// entity_t *create_ext(const entity_type type, int x, int y) {
	//   unsigned int id = 0;
	//   entity_t e;

	//   for (id = 0; id < EN_COUNT; id++) {
	//     if (manager.all[id].type == NULL) {
	//       /* reset entity */
	//       e = manager.all + id;
	//       *e = null_entity;

	//       e->type = type;
	//       e->x = x; // * EN_GRID_SIZE;
	//       e->y = y; // * EN_GRID_SIZE;
	//       e->timer = 0.0f;
	//       // ev.self->type(&ev);

	//       break;
	//     }
	//   }
	//   return (id >= 0 && id < EN_COUNT ? manager.all + id : NULL);
	// }


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

	bool overlaps(const entity_t *a, const entity_t *b) {
		if ((a->x + a->bx < b->x + b->bx + b->bw) &&
			(a->y + a->by < b->y + b->by + b->bh) &&
			(a->x + a->bx + a->bw > b->x + b->bx) &&
			(a->y + a->by + a->bh > b->y + b->by)) {
			return true;
		}
		return false;
	}

	bool place_meeting(const entity_t *a, int dx, int dy) {
#define ENTITY_TOP (a->y + a->by + dy)
#define ENTITY_LEFT (a->x + a->bx + dx)
#define ENTITY_BOTTOM (a->y + a->by + a->bh + dy)
#define ENTITY_RIGHT (a->x + a->bx + a->bw + dx)

		const int t = std::max(0, ENTITY_TOP / EN_GRID_SIZE);
		const int l = std::max(0, ENTITY_LEFT / EN_GRID_SIZE);
		const int b = std::min(EN_ROOM_ROWS - 1, (ENTITY_BOTTOM - 1) / EN_GRID_SIZE);
		const int r = std::min(EN_ROOM_COLS - 1, (ENTITY_RIGHT - 1) / EN_GRID_SIZE);

		/* FIXME: we should reference PTI api from here..*/
		int x, y, f;
		for (y = t; y <= b; y++) {
			for (x = l; x <= r; x++) {
				f = pti_fget(game::_tilemap, x, y);
				switch (f) {
					case 0:
					case 46:
					case 47:
						continue;

					/* slope */
					case 31: {
						/* shallow slope bottom (right): */
						int cx = ENTITY_RIGHT - (x * 8);
						float top = -0.5f * cx + ((y + 1) * 8);
						int e_bottom = ENTITY_BOTTOM;
						if (e_bottom > top) {
							return true;
						}
						continue;
					}
					case 32: {
						// shallow slope top (right):
						int cx = ENTITY_RIGHT - (x * 8);
						float top = -0.5f * cx + ((y + 1) * 8 - 4);
						if (ENTITY_BOTTOM > top) {
							return true;
						}
						continue;
					}
					case 33: {
						/* shallow slope top (left): */
						int cx = ENTITY_LEFT - (x * 8);
						float top = 0.5f * cx + (y * 8);
						if (ENTITY_BOTTOM > top) {
							return true;
						}
						continue;
					}
					case 34: {
						/* shallow slope bottom (left): */
						int cx = ENTITY_LEFT - (x * 8);
						int top = 0.5f * cx + ((y + 1) * 8 - 4);
						if (ENTITY_BOTTOM > top) {
							return true;
						}
						continue;
					}

					/* jump through */
					case 27:
					case 28:
					case 29:
					case 30:
					case 38:
					case 39:
					case 40:
					case 41:
						/* if, prior to the movement, the player was entirely above it (that is,
         * bottom-most coordinate of player was at least one pixel above
         * top-most coordinate of one-way platform). */
						if ((a->y + a->by + a->bh - a->sy) <= (y * 8)) {
							return true;
						}
						continue;
					default:
						return true;
				}
			}
		}
		return false;
	}

	inline bool can_wiggle(entity_t *self) {
		int i, j;
		for (i = 0; i < 4; i++) {
			for (j = -1; j <= 1; j += 2) {
				if (!entity::place_meeting(self, i * j, -1)) {
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
					if (entity::place_meeting(ev.self, dx, 0) &&
						!(entity::place_meeting(ev.self, dx, -1))) {
						ev.self->y -= 1;
					}
					/* moving down slope: */
					if (!(entity::place_meeting(ev.self, dx, 0)) &&
						!(entity::place_meeting(ev.self, dx, 1)) &&
						entity::place_meeting(ev.self, dx, 2)) {
						ev.self->y += 1;
					}

					/* always last */
					if (entity::place_meeting(ev.self, dx, 0)) {
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
					if (entity::place_meeting(ev.self, 0, dy)) {
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
				if (ev.self->type != NULL && entity::overlaps(ev.self, ev.other)) {
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