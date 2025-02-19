#include "pti.h"

#include "bullet.h"
#include "goomba.h"
#include "registry.h"
#include "../bank.h"

template<>
bool EntityBase::Is<Bullet>() const {
	return type == EntityType::Bullet;
}

void Bullet::Update() {
	if (IsTouching()) {
		RemoveEntity(this);
	}

	Physics();
	HandleVerticalMovement();
}

void Bullet::Render() {
	auto flip = (flags & EntityFlags::ENTITYFLAG_FACING_LEFT) ? true : false;
	pti_spr(bitmap_bullet, 0, x - 4, y - 4, flip, false);
}

void Bullet::InteractWith(const EntityBase *other) {
	// nothing.
}

void Bullet::HandleHorizontalMovement() {
	// nothing.
}

void Bullet::HandleVerticalMovement() {
	_pti_appr(sx, direction * kBulletMaxSpeed, kBulletAcceleration * PTI_DELTA);
}