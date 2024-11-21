#include "pti.h"

#include "collisions.h"
#include "entity.h"
#include "../bank.h"

namespace {
	constexpr int EN_COUNT = 64;
	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 32;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 256;
	constexpr int EN_GRID_SIZE = 8;
}// namespace

namespace collisions {
	bool is_grounded(const entity::entity_t *self) {
		return collisions::place_meeting(self, 0, 1) || (self->sy >= 0 && collisions::place_meeting(self, 0, 1));
	}

	// Check if two entities overlap using their bounding boxes
	bool overlaps(const entity::entity_t *a, const entity::entity_t *b) {
		return (a->x + a->bx < b->x + b->bx + b->bw) &&
			   (a->y + a->by < b->y + b->by + b->bh) &&
			   (a->x + a->bx + a->bw > b->x + b->bx) &&
			   (a->y + a->by + a->bh > b->y + b->by);
	}

	// Check if an entity collides with the environment after a movement
	bool place_meeting(const entity::entity_t *a, int dx, int dy) {
		// Calculate the bounding box of the entity in grid coordinates
		const int top = std::max(0, (a->y + a->by + dy) / EN_GRID_SIZE);
		const int left = std::max(0, (a->x + a->bx + dx) / EN_GRID_SIZE);
		const int bottom = std::min(EN_ROOM_ROWS - 1, (a->y + a->by + a->bh + dy - 1) / EN_GRID_SIZE);
		const int right = std::min(EN_ROOM_COLS - 1, (a->x + a->bx + a->bw + dx - 1) / EN_GRID_SIZE);

		// Iterate through the grid cells within the entity's bounding box
		for (int y = top; y <= bottom; ++y) {
			for (int x = left; x <= right; ++x) {
				// Fetch tile properties from the tilemap
				int flags = pti_fget(tilemap, x, y);

				switch (flags) {
					// Non-colliding tiles
					case 0:
					case 46:
					case 47:
						continue;

					// Slope handling
					case 31: {// Shallow slope bottom (right)
						int cx = (a->x + a->bx + a->bw + dx) - (x * EN_GRID_SIZE);
						float top = -0.5f * cx + ((y + 1) * EN_GRID_SIZE);
						if (a->y + a->by + a->bh + dy > top) return true;
						continue;
					}
					case 32: {// Shallow slope top (right)
						int cx = (a->x + a->bx + a->bw + dx) - (x * EN_GRID_SIZE);
						float top = -0.5f * cx + ((y + 1) * EN_GRID_SIZE - 4);
						if (a->y + a->by + a->bh + dy > top) return true;
						continue;
					}
					case 33: {// Shallow slope top (left)
						int cx = (a->x + a->bx + dx) - (x * EN_GRID_SIZE);
						float top = 0.5f * cx + (y * EN_GRID_SIZE);
						if (a->y + a->by + a->bh + dy > top) return true;
						continue;
					}
					case 34: {// Shallow slope bottom (left)
						int cx = (a->x + a->bx + dx) - (x * EN_GRID_SIZE);
						float top = 0.5f * cx + ((y + 1) * EN_GRID_SIZE - 4);
						if (a->y + a->by + a->bh + dy > top) return true;
						continue;
					}

					// One-way platforms
					case 27:
					case 28:
					case 29:
					case 30:
					case 38:
					case 39:
					case 40:
					case 41: {
						// Allow collision only if the entity was entirely above the platform before the movement
						if ((a->y + a->by + a->bh - a->sy) <= (y * EN_GRID_SIZE)) return true;
						continue;
					}

					// Solid tiles
					default:
						return true;
				}
			}
		}

		return false;
	}

}// namespace collisions
