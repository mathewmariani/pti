#pragma once

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

	typedef struct entity_t {
		entity_type type;   /* entity type; a pointer to a behavior function */
		int x, y;           /* position */
		float sx, sy;       /* speed */
		float rx, ry;       /* movement remainder */
		int bx, by, bw, bh; /* bounding box */
		float timer;        /* general timer, starts at 0 on create */
		int flags;          /* general entity flags */
		void *userdata;     /* general user data */
		int state;          /* state of the entity */

		/* interesting idea for future considerations */
		std::function<void(entity_t *)> init;
		std::function<void(entity_t *)> destroy;
		std::function<void(entity_t *)> update;
		std::function<void(entity_t *)> draw;
		std::function<void(entity_t *, entity_t *)> overlap;

	} entity_t;

	typedef enum entity_flag {
		ENTITYFLAG_HITS_SOLIDS = (1 << 0),
		ENTITYFLAG_OVERLAP_CHECKS = (1 << 1),
		ENTITYFLAG_FACING_LEFT = (1 << 2),
		ENTITYFLAG_GROUNDED = (1 << 3),
	} entity_flag;

	/* Entity Event */
	typedef enum event_type {
		EVENTTYPE_INIT,
		EVENTTYPE_UPDATE,
		EVENTTYPE_DRAW,
		EVENTTYPE_DESTROY,
		EVENTTYPE_OVERLAP
	} event_type;

	typedef struct event_t {
		event_type type;
		entity_t *self;
		entity_t *other;
	} event_t;

	typedef struct entity_state {
		entity_t all[EN_COUNT];
	} entity_state;

	// static
	extern entity_state manager;
	extern const entity_t null_entity;

	entity_t *create(entity_type type, int x, int y);
	void destroy(entity_t *entity);

	void clear();
	void update();
	void draw();

}// namespace entity