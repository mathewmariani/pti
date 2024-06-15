#pragma once

namespace entity {
	struct entity_t;
}

namespace collisions {
	bool overlaps(const entity::entity_t *a, const entity::entity_t *b);
	bool place_meeting(const entity::entity_t *a, int dx, int dy);
}// namespace collisions