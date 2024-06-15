#include "pti.h"

#include "entity.h"
#include "collisions.h"
#include "../game.h"

#include <algorithm>

namespace collisions {

	constexpr int EN_COUNT = 64;
	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 32;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 256;
	constexpr int EN_GRID_SIZE = 8;

	bool overlaps(const entity::entity_t *a, const entity::entity_t *b) {
		if ((a->x + a->bx < b->x + b->bx + b->bw) &&
			(a->y + a->by < b->y + b->by + b->bh) &&
			(a->x + a->bx + a->bw > b->x + b->bx) &&
			(a->y + a->by + a->bh > b->y + b->by)) {
			return true;
		}
		return false;
	}

	bool place_meeting(const entity::entity_t *a, int dx, int dy) {
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

}// namespace collisions