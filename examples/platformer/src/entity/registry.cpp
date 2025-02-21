#include "registry.h"

#include <algorithm>
#include <array>
#include <variant>
#include <vector>
#include <cstdio>

#include "bullet.h"
#include "coin.h"
#include "goomba.h"
#include "player.h"
#include "shooter.h"

static std::vector<uint8_t> _freeIdList;

using EntityVariant = std::variant<EntityBase, Bullet, Coin, Goomba, Player, Shooter>;
EntityVariant Entities[kMaxEntities];

auto GetEntityBase = [](auto &entity) -> EntityBase * { return &entity; };

EntityBase *GetEntity(uint8_t sprite_idx) {
	if (sprite_idx >= kMaxEntities) return nullptr;
	return std::get_if<EntityBase>(&Entities[sprite_idx]);
}

#define CASE_FOR(T)                                          \
	case EntityType::T:                                      \
		entity = (EntityBase *) &Entities[idx].emplace<T>(); \
		break

EntityBase *CreateEntity(EntityType type) {
	auto idx = _freeIdList.back();
	if (idx >= kMaxEntities) {
		return nullptr;
	}
	_freeIdList.pop_back();

	EntityBase *entity;
	switch (type) {
		CASE_FOR(Bullet);
		CASE_FOR(Coin);
		CASE_FOR(Goomba);
		CASE_FOR(Player);
		CASE_FOR(Shooter);
		default:
			return nullptr;
	}

	entity->type = type;
	entity->id = idx;
	entity->x = 0;
	entity->y = 0;

	return entity;
}

void RemoveEntity(EntityBase *entity) {
	// add to free list
	auto index = entity->id;
	_freeIdList.insert(std::upper_bound(std::rbegin(_freeIdList), std::rend(_freeIdList), index).base(), index);

	// reset entity
	*entity = EntityBase();
	entity->id = index;
	entity->type = EntityType::Null;
}

void ResetAllEntities() {
	std::fill(std::begin(Entities), std::end(Entities), EntityBase());

	_freeIdList.clear();
	_freeIdList.resize(kMaxEntities);

	auto nextId = 0;
	std::for_each(std::rbegin(_freeIdList), std::rend(_freeIdList), [&](auto &elem) {
		elem = nextId;
		nextId++;
	});
}

bool CheckCollisionsWith(const EntityBase *self, EntityBase *&out) {
	for (auto &a : Entities) {
		if (!self || self->type == EntityType::Null || !(self->flags & EntityFlags::ENTITYFLAG_OVERLAP_CHECKS)) {
			continue;
		}

		EntityBase *other = std::visit(GetEntityBase, a);
		if (!other || other->type == EntityType::Null || self == other | !(other->flags & EntityFlags::ENTITYFLAG_OVERLAP_CHECKS)) {
			continue;
		}

		if (self->Overlaps(other)) {
			out = other;
			return true;
		}
	}

	return false;
}

void UpdateAllEntities() {
	for (auto &e : Entities) {
		EntityBase *entity = std::visit(GetEntityBase, e);
		if (entity && entity->type != EntityType::Null) {
			entity->Step();
		}
	}

	// // check for collisions
	// for (auto &a : Entities) {
	// 	EntityBase *self = std::visit(GetEntityBase, a);
	// 	if (!self || self->type == EntityType::Null || !(self->flags & EntityFlags::ENTITYFLAG_OVERLAP_CHECKS) ||
	// 		self->bw < 0 || self->bh <= 0) {
	// 		continue;
	// 	}

	// 	for (auto &b : Entities) {
	// 		EntityBase *other = std::visit(GetEntityBase, b);
	// 		if (!other || other->type == EntityType::Null || self == other || !(other->flags & EntityFlags::ENTITYFLAG_OVERLAP_CHECKS) ||
	// 			other->bw < 0 || other->bh <= 0) {
	// 			continue;
	// 		}

	// 		if (self->Overlaps(other)) {
	// 			std::visit([&](auto &entity) { entity.InteractWith(other); }, a);
	// 		}
	// 	}
	// }
}


void RenderAllEntities() {
	for (auto &e : Entities) {
		EntityBase *entity = std::visit(GetEntityBase, e);
		if (entity && entity->type != EntityType::Null) {
			entity->Render();
		}
	}
}