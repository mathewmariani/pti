#pragma once

#include <vector>

namespace entity {
	constexpr int EN_COUNT = 64;
	constexpr int EN_ROOM_COLS = 64;
	constexpr int EN_ROOM_ROWS = 32;
	constexpr int EN_ROOM_WIDTH = 512;
	constexpr int EN_ROOM_HEIGHT = 256;
	constexpr int EN_GRID_SIZE = 8;
}// namespace entity

class Entity {
	friend class EntityManager;
	friend class Collisions;

public:
	enum Flags {
		ENTITYFLAG_HITS_SOLIDS = (1 << 0),
		ENTITYFLAG_OVERLAP_CHECKS = (1 << 1),
		ENTITYFLAG_FACING_LEFT = (1 << 2),
		ENTITYFLAG_GROUNDED = (1 << 3),
	};

public:
	Entity() = default;
	virtual ~Entity() = default;

	virtual void Init(void) = 0;
	virtual void Destroy(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void Overlap(Entity &other) = 0;

public:
	int x, y;           /* position */
	float sx, sy;       /* speed */
	float rx, ry;       /* movement remainder */
	int bx, by, bw, bh; /* bounding box */
	float timer;        /* general timer, starts at 0 on create */
	int flags;          /* general entity flags */
	int state;          /* state of the entity */
	int frame;
	int direction;
};

class EntityManager {
public:
	EntityManager() = default;
	void Clear(void);
	void Update(void);
	void Draw(void);

	void Create(std::unique_ptr<Entity> e, int x, int y);
	void Destroy(Entity &e);

private:
	std::vector<std::unique_ptr<Entity>> all;
	std::vector<std::unique_ptr<Entity>> cached;
};