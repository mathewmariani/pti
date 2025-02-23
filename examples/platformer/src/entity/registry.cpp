#include "registry.h"
#include "../gamestate.h"

#include <algorithm>
#include <array>
#include <variant>
#include <vector>
#include <cstdio>

static std::vector<uint8_t> _freeIdList;

namespace {
	auto GetEntityBase = [](auto &entity) -> EntityBase * { return &entity; };
}// namespace

#define CASE_FOR(T)                                                    \
	case EntityType::T:                                                \
		entity = (EntityBase *) &gameState.Entities[idx].emplace<T>(); \
		break

EntityBase *CreateEntity(EntityType type) {
	auto idx = _freeIdList.back();
	if (idx >= kMaxEntities) {
		return nullptr;
	}
	_freeIdList.pop_back();

	auto &gameState = GetGameState();
	EntityBase *entity;
	switch (type) {
		CASE_FOR(Coin);
		CASE_FOR(Effect);
		CASE_FOR(Goomba);
		CASE_FOR(Player);
		CASE_FOR(Projectile);
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
	auto &gameState = GetGameState();
	std::fill(std::begin(gameState.Entities), std::end(gameState.Entities), EntityBase());

	_freeIdList.clear();
	_freeIdList.resize(kMaxEntities);

	auto nextId = 0;
	std::for_each(std::rbegin(_freeIdList), std::rend(_freeIdList), [&](auto &elem) {
		elem = nextId;
		nextId++;
	});
}

bool CheckCollisionsWith(const EntityBase *self, EntityBase *&out, const CoordXY<int> &dir) {
	for (auto &a : GetGameState().Entities) {
		if (!self || self->type == EntityType::Null) {
			continue;
		}

		auto *other = std::visit(GetEntityBase, a);
		if (!other || other->type == EntityType::Null || self == other) {
			continue;
		}

		if (self->Overlaps(other, dir)) {
			out = other;
			return true;
		}
	}

	return false;
}

void CleanupEntities() {
	for (auto &e : GetGameState().Entities) {
		auto *entity = std::visit(GetEntityBase, e);
		if (entity && entity->type != EntityType::Null && entity->flags) {
			entity->Step();
		}
	}
}

void RenderAllEntities() {
	for (auto &e : GetGameState().Entities) {
		auto *entity = std::visit(GetEntityBase, e);
		if (entity && entity->type != EntityType::Null) {
			entity->Render();
		}
	}
}