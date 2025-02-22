#pragma once

constexpr char kMaxEntities = 64;

#include "base.h"
#include "../world/coordinate.h"

EntityBase *CreateEntity(EntityType type);

template<typename T>
T *CreateEntity() {
	return static_cast<T *>(CreateEntity(T::cEntityType));
}

void RemoveEntity(EntityBase *entity);

void ResetAllEntities();

void UpdateAllEntities();
void RenderAllEntities();
bool CheckCollisionsWith(const EntityBase *self, EntityBase *&other, const CoordXY<int> &dir);