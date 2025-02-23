#pragma once

#include "../world/coordinate.h"

#include <algorithm>
#include <stdint.h>

enum class EntityType : uint8_t {
	Coin,
	Effect,
	Goomba,
	Player,
	Projectile,
	Shooter,

	// always last.
	Count,
	Null = 255,
};

enum class EntityInteraction : uint8_t {
	CollectDirect,
	Touch,

	// always last.
	Count,
	Null = 255,
};

enum class EntityReaction : uint8_t {
	None,
	Collected,
	Hurt,
	Bump,

	// always last.
	Count,
	Null = 255,
};

enum EntityFlags : uint8_t {
	ProvidesStaticCollision = (1 << 0),
	ENTITYFLAG_GROUNDED = (1 << 1),
	MarkedForGarbage = (1 << 2),
	PhysicsBumped = (1 << 3),
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

	void Step();
	void Physics();

	virtual void Update();
	virtual void PostUpdate();
	virtual void Render();

	virtual bool PreSolidCollisionWith(EntityBase *const other, const CoordXY<int> &dir);
	virtual const EntityReaction Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir);

	void SetLocation(const CoordXY<int> &newLocation);


	bool IsGrounded() const;
	bool ProvidesStaticCollision() const;

	bool Overlaps(const EntityBase *other, const CoordXY<int> &dir) const;
	bool PlaceMeeting(const CoordXY<int> &dir) const;
	bool IsTouching() const;
	bool CanWiggle();
};