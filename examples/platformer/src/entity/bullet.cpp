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

	HandleVerticalMovement();
}

void Bullet::Render() {
	pti_spr(bitmap_bullet, 0, x - 4, y - 4, false, false);
	pti_rect(x + bx, y + by, bw, bh, 0xff0000);
}

const EntityReaction Bullet::Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) {
	if (dir.y > 0) {
		if (interaction == EntityInteraction::Touch) {
			return EntityReaction::Bump;
		}
	}
	return EntityReaction::None;
}


void Bullet::HandleHorizontalMovement() {
	// nothing.
}

void Bullet::HandleVerticalMovement() {
	_pti_appr(sx, direction * kBulletMaxSpeed, kBulletAcceleration * PTI_DELTA);
}