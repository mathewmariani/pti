#pragma once

namespace entity {
	struct entity_t;
}

namespace collisions {

	constexpr int EN_COUNT = 64;
	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 32;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 256;
	constexpr int EN_GRID_SIZE = 8;

	bool is_grounded(const entity::entity_t *self);
	bool overlaps(const entity::entity_t *a, const entity::entity_t *b);
	bool place_meeting(const entity::entity_t *a, int dx, int dy);

}// namespace collisions
