#pragma once

#include <algorithm>

#include <stdint.h>

enum class EntityType : uint8_t {
	Coin,
	Bullet,
	Goomba,
	Player,
	Shooter,

	// always last.
	Count,
	Null = 255,
};

enum EntityFlags {
	ENTITYFLAG_HITS_SOLIDS = (1 << 0),
	ENTITYFLAG_OVERLAP_CHECKS = (1 << 1),
	ENTITYFLAG_FACING_LEFT = (1 << 2),
	ENTITYFLAG_GROUNDED = (1 << 3),
};

using EntityId = uint8_t;

struct BoundingBox {
	uint8_t top;
	uint8_t left;
	uint8_t width;
	uint8_t height;
};


struct EntityBase {
	EntityType type;
	EntityId id;
	int32_t x, y;      // position
	uint8_t flags;     // general entity flags
	float sx, sy;      // speed
	float rx, ry;      // movement remainder
	int bx, by, bw, bh;// bounding box
	float timer;       // general timer, starts at 0 on create

	int8_t direction;

	template<typename T>
	bool Is() const;

	virtual void Update();
	virtual void Render();
	virtual void InteractWith(const EntityBase *other);

	void SetLocation(int x, int y);

	void Physics();
	bool IsGrounded() const;
	bool Overlaps(const EntityBase *other) const;
	bool PlaceMeeting(int dx, int dy) const;
	bool IsTouching() const;

	bool CanWiggle();
};