#pragma once

#include <array>
#include <algorithm>
#include <functional>

namespace entity {

	constexpr int EN_COUNT = 64;
	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 32;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 256;
	constexpr int EN_GRID_SIZE = 8;

	struct event_t;
	typedef void (*entity_type)(struct event_t *);

	struct entity_t {
		entity_type type;   /* entity type; a pointer to a behavior function */
		int x, y;           /* position */
		float sx, sy;       /* speed */
		float rx, ry;       /* movement remainder */
		int bx, by, bw, bh; /* bounding box */
		float timer;        /* general timer, starts at 0 on create */
		int flags;          /* general entity flags */
		void *userdata;     /* general user data */
		int state;          /* state of the entity */
	};

	enum entity_flag {
		ENTITYFLAG_HITS_SOLIDS = (1 << 0),
		ENTITYFLAG_OVERLAP_CHECKS = (1 << 1),
		ENTITYFLAG_FACING_LEFT = (1 << 2),
		ENTITYFLAG_GROUNDED = (1 << 3),
	};

	/* Entity Event */
	enum event_type {
		EVENTTYPE_INIT,
		EVENTTYPE_UPDATE,
		EVENTTYPE_DRAW,
		EVENTTYPE_DESTROY,
		EVENTTYPE_OVERLAP
	};

	struct event_t {
		event_type type;
		entity_t *self;
		entity_t *other;
	};

	struct entity_state {
		std::array<entity_t, EN_COUNT> all;
		entity_t *create(entity_type type, int x, int y);
		void destroy(entity_t *entity);

		void clear(void);
		void update(void);
		void draw(void);
	};

	// static
	inline entity_state manager;
	inline constexpr entity_t null_entity{};
}// namespace entity