#include "pti.h"

#include "collisions.h"
#include "entity.h"

void EntityManager::Create(std::unique_ptr<Entity> e, int x, int y) {
	e->x = x;
	e->y = y;
	e->Init();
	all.emplace_back(std::move(e));
}

void EntityManager::Destroy(Entity &e) {
}

void EntityManager::Clear(void) {
	all.clear();
}

void EntityManager::Update(void) {
	int i, j, nx, ny;
	/* update all */
	for (auto &e : all) {
		e->timer += 1.0f / 60.0f;

		/* get move amount */
		nx = e->sx + e->rx;
		ny = e->sy + e->ry;

		/* try to move */
		if ((e->flags & Entity::ENTITYFLAG_HITS_SOLIDS) != 0) {
			j = nx;
			int dx = _pti_sign(j);
			while (j != 0) {
				/* moving up slope: */
				if (Collisions::PlaceMeeting(*e, dx, 0) &&
					!(Collisions::PlaceMeeting(*e, dx, -1))) {
					e->y -= 1;
				}
				/* moving down slope: */
				if (!(Collisions::PlaceMeeting(*e, dx, 0)) &&
					!(Collisions::PlaceMeeting(*e, dx, 1)) &&
					Collisions::PlaceMeeting(*e, dx, 2)) {
					e->y += 1;
				}
				/* always last */
				if (Collisions::PlaceMeeting(*e, dx, 0)) {
					e->sx = e->rx = nx = 0;
					break;
				}
				e->x += dx;
				j -= dx;
			}

			j = ny;
			int dy = _pti_sign(j);
			while (j != 0) {
				if (e->sy < 0 && !Collisions::CanWiggle(*e)) {
					e->sy = e->ry = ny = 0;
					break;
				}
				if (Collisions::PlaceMeeting(*e, 0, dy)) {
					e->sy = e->ry = ny = 0;
					break;
				}
				e->y += dy;
				j -= dy;
			}
		} else {
			e->x += nx;
			e->y += ny;
		}

		/* update move remainder */
		e->rx = e->sx + e->rx - nx;
		e->ry = e->sy + e->ry - ny;

		/* call their update event */
		e->Update();
	}

	/* do overlap tests */
	for (auto &e : all) {
		if ((e->flags & Entity::ENTITYFLAG_OVERLAP_CHECKS) == 0) {
			continue;
		}
		if (e->bw < 0 || e->bh <= 0) {
			continue;
		}
		for (auto &other : all) {
			if (&other == &e || (other->flags & Entity::ENTITYFLAG_OVERLAP_CHECKS) == 0) {
				continue;
			}
			if (other->bw < 0 || other->bh <= 0) {
				continue;
			}
			if (Collisions::Overlaps(*e, *other)) {
				e->Overlap(*other);
			}
		}
	}
}

void EntityManager::Draw(void) {
	for (auto &e : all) {
		e->Draw();
	}
}