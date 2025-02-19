#include "registry.h"

#include <algorithm>
#include <array>
#include <variant>
#include <vector>
#include <cstdio>

#include "coin.h"
#include "goomba.h"
#include "player.h"

static std::vector<uint8_t> _freeIdList;

using EntityVariant = std::variant<EntityBase, Coin, Goomba, Player>;
EntityVariant Entities[kMaxEntities];

EntityBase *GetEntity(uint8_t sprite_idx) {
	if (sprite_idx >= kMaxEntities) return nullptr;
	return std::get_if<EntityBase>(&Entities[sprite_idx]);
}

EntityBase *CreateEntity(EntityType type) {
	auto idx = _freeIdList.back();
	if (idx >= kMaxEntities) {
		return nullptr;
	}
	_freeIdList.pop_back();

	EntityBase *entity;
	switch (type) {
		case EntityType::Coin:
			entity = (EntityBase *) &Entities[idx].emplace<Coin>();
			break;
		case EntityType::Goomba:
			entity = (EntityBase *) &Entities[idx].emplace<Goomba>();
			break;
		case EntityType::Player:
			entity = (EntityBase *) &Entities[idx].emplace<Player>();
			break;
		default:
			return nullptr;
	}

	entity->type = type;
	entity->id = idx;
	entity->x = 0;
	entity->y = 0;

	return entity;
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

void UpdateAllEntities() {
	for (auto &entity : Entities) {
		// clang-format off
		EntityBase *e = std::visit([](auto &obj) -> EntityBase * {
			if constexpr (std::is_base_of_v<EntityBase, std::decay_t<decltype(obj)>>) {
				return &obj;
			}
			return nullptr;
		}, entity);
		// clang-format on

		if (!e || e->type == EntityType::Null) {
			continue;
		}

		switch (e->type) {
			case EntityType::Coin:
				std::get_if<Coin>(&entity)->Update();
				break;
			case EntityType::Goomba:
				std::get_if<Goomba>(&entity)->Update();
				break;
			case EntityType::Player:
				std::get_if<Player>(&entity)->Update();
				break;
			default:
				break;
		}
	}

	// check for collisions
	for (auto &a : Entities) {
		// clang-format off
		EntityBase *self = std::visit([](auto &obj) -> EntityBase * {
			if constexpr (std::is_base_of_v<EntityBase, std::decay_t<decltype(obj)>>) {
				return &obj;
			}
			return nullptr;
		}, a);
		// clang-format on

		if (!self || self->type == EntityType::Null) {
			continue;
		}
		if ((self->flags & EntityFlags::ENTITYFLAG_OVERLAP_CHECKS) == 0 || self->bw < 0 || self->bh <= 0) {
			continue;
		}

		for (auto &b : Entities) {
			// clang-format off
			EntityBase *other = std::visit([](auto &obj) -> EntityBase * {
				if constexpr (std::is_base_of_v<EntityBase, std::decay_t<decltype(obj)>>) {
					return &obj;
				}
				return nullptr;
			}, b);
			// clang-format on

			if (!other || other->type == EntityType::Null || self == other) {
				continue;
			}
			if ((other->flags & EntityFlags::ENTITYFLAG_OVERLAP_CHECKS) == 0 || other->bw < 0 || other->bh <= 0) {
				continue;
			}

			if (self->Overlaps(other)) {
				switch (self->type) {
					case EntityType::Coin:
						std::get_if<Coin>(&a)->InteractWith(other);
						break;
					case EntityType::Goomba:
						std::get_if<Goomba>(&a)->InteractWith(other);
						break;
					case EntityType::Player:
						std::get_if<Player>(&a)->InteractWith(other);
						break;
					default:
						break;
				}
			}
		}
	}
}


void RenderAllEntities() {
	for (auto &entity : Entities) {
		// clang-format off
		EntityBase *e = std::visit([](auto &obj) -> EntityBase * {
			if constexpr (std::is_base_of_v<EntityBase, std::decay_t<decltype(obj)>>) {
				return &obj;
			}
			return nullptr;
		}, entity);
		// clang-format on

		if (!e || e->type == EntityType::Null) {
			continue;
		}

		switch (e->type) {
			case EntityType::Coin:
				std::get_if<Coin>(&entity)->Render();
				break;
			case EntityType::Goomba:
				std::get_if<Goomba>(&entity)->Render();
				break;
			case EntityType::Player:
				std::get_if<Player>(&entity)->Render();
				break;
			default:
				break;
		}
	}
}