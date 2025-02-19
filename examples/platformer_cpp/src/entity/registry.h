#pragma once

#include "base.h"
#include <algorithm>

constexpr uint8_t kMaxEntities = 64;

EntityBase *GetEntity(uint8_t sprite_idx);

// template<typename T>
// T *GetEntity(uint8_t sprite_idx) {
// 	auto e = GetEntity(sprite_idx);
// 	return std::get_if<T>(e);//spr != nullptr ? spr->As<T>() : nullptr;
// }

EntityBase *CreateEntity(EntityType type);

template<typename T>
T *CreateEntity() {
	return static_cast<T *>(CreateEntity(T::cEntityType));
}

void RemoveEntity(EntityBase *entity);

void ResetAllEntities();

void UpdateAllEntities();
void RenderAllEntities();