#include "pti.h"
#include "entity.h"
#include "collisions.h"

namespace entity {
	entity_t *entity_state::create(const entity_type type, int x, int y) {
		event_t ev;

		auto it = std::find_if(manager.all.begin(), manager.all.end(), [](const entity_t &entity) {
			return entity.type == nullptr;
		});

		if (it != manager.all.end()) {
			ev.type = EVENTTYPE_INIT;
			ev.self = &(*it);
			*ev.self = null_entity;
			ev.self->type = type;
			ev.self->x = x;
			ev.self->y = y;
			ev.self->timer = 0.0f;
			ev.self->type(&ev);
			return &(*it);
		}

		return nullptr;
	}


	void entity_state::destroy(entity_t *entity) {
		auto it = std::find_if(manager.all.begin(), manager.all.end(), [entity](const entity_t &e) {
			return e.type == entity->type;
		});

		if (it != manager.all.end() && it->type != nullptr) {
			event_t ev;
			ev.type = EVENTTYPE_DESTROY;
			ev.self = &(*it);
			ev.self->type(&ev);
			ev.self->type = nullptr;
		}
	}

	inline bool can_wiggle(entity_t *entity) {
		for (int i = 0; i < 4; ++i) {
			for (int j : {-1, 1}) {
				if (!collisions::place_meeting(entity, i * j, -1)) {
					entity->x += (i * j);
					return true;
				}
			}
		}
		return false;
	}

	void entity_state::clear(void) {
		std::fill(manager.all.begin(), manager.all.end(), null_entity);
	}

	void entity_state::update(void) {
		entity::event_t ev = {
				.type = entity::EVENTTYPE_UPDATE,
		};

		/* Run entities */
		for (auto &entity : entity::manager.all) {
			if (entity.type == nullptr) {
				continue;
			}

			ev.self = &entity;
			ev.self->timer += 1.0f / 60.0f;

			/* Get move amount */
			int nx = ev.self->sx + ev.self->rx;
			int ny = ev.self->sy + ev.self->ry;

			/* Try to move */
			if ((ev.self->flags & entity::ENTITYFLAG_HITS_SOLIDS) != 0) {
				// Move in X direction
				int j = nx;
				int dx = _pti_sign(j);
				while (j != 0) {
					// Moving up slope:
					if (collisions::place_meeting(ev.self, dx, 0) &&
						!(collisions::place_meeting(ev.self, dx, -1))) {
						ev.self->y -= 1;
					}
					// Moving down slope:
					if (!(collisions::place_meeting(ev.self, dx, 0)) &&
						!(collisions::place_meeting(ev.self, dx, 1)) &&
						collisions::place_meeting(ev.self, dx, 2)) {
						ev.self->y += 1;
					}

					// Always last
					if (collisions::place_meeting(ev.self, dx, 0)) {
						ev.self->sx = ev.self->rx = nx = 0;
						break;
					}
					ev.self->x += dx;
					j -= dx;
				}

				// Move in Y direction
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

			/* Update move remainder */
			ev.self->rx = ev.self->sx + ev.self->rx - nx;
			ev.self->ry = ev.self->sy + ev.self->ry - ny;

			/* Call their update event */
			ev.self->type(&ev);
		}

		/* Do overlap tests */
		ev.type = entity::EVENTTYPE_OVERLAP;
		for (auto &self : entity::manager.all) {
			if ((self.flags & entity::ENTITYFLAG_OVERLAP_CHECKS) == 0 || self.type == nullptr || self.bw < 0 || self.bh <= 0) {
				continue;
			}

			for (auto &other : entity::manager.all) {
				if (&self == &other || (other.flags & entity::ENTITYFLAG_OVERLAP_CHECKS) == 0 || other.type == nullptr || other.bw < 0 || other.bh <= 0) {
					continue;
				}

				if (collisions::overlaps(&self, &other)) {
					ev.self = &self;
					ev.other = &other;
					self.type(&ev);
				}
			}
		}
	}


	void entity_state::draw(void) {
		entity::event_t ev = {
				.type = entity::EVENTTYPE_DRAW,
		};
		for (auto &entity : entity::manager.all) {
			if (entity.type == nullptr) {
				continue;
			}
			ev.self = &entity;
			entity.type(&ev);
		}
	}

}// namespace entity
