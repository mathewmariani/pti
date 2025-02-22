#include "pti.h"

#include "projectile.h"
#include "registry.h"
#include "../bank.h"

template<>
bool EntityBase::Is<Projectile>() const {
	return type == EntityType::Projectile;
}

void Projectile::Create(const CoordXY<int> &location, Projectile::Type type) {
	auto *projectile = CreateEntity<Projectile>();
	if (projectile == nullptr) {
		return;
	}

	projectile->bx = kProjectileBoundaryOffset;
	projectile->by = kProjectileBoundaryOffset;
	projectile->bw = kProjectileWidth;
	projectile->bh = kProjectileHeight;
	projectile->subtype = type;
	projectile->direction = -1;

	projectile->SetLocation(location);
}

void Projectile::Update() {
	HandleVerticalMovement();
}

void Projectile::PostUpdate() {
	if (flags & EntityFlags::MarkedForGarbage || IsTouching()) {
		RemoveEntity(this);
	}
}

void Projectile::Render() {
	pti_spr(bitmap_bullet, 0, x, y, false, false);
	pti_rect(x + bx, y + by, bw, bh, 0xff0000);
}

const EntityReaction Projectile::Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) {
	if (dir.y > 0) {
		if (interaction == EntityInteraction::Touch) {
			flags |= EntityFlags::MarkedForGarbage;
			return EntityReaction::Bump;
		}
	}
	return EntityReaction::None;
}


void Projectile::HandleHorizontalMovement() {
	// nothing.
}

void Projectile::HandleVerticalMovement() {
	_pti_appr(sx, direction * kProjectileMaxSpeed, kProjectileAcceleration * PTI_DELTA);
}