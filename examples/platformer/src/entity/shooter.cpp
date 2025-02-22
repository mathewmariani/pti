#include "pti.h"

#include "shooter.h"
#include "bullet.h"
#include "registry.h"
#include "../bank.h"

template<>
bool EntityBase::Is<Shooter>() const {
	return type == EntityType::Shooter;
}

void Shooter::Update() {
	shoot_timer -= PTI_DELTA;
	if (shoot_timer < 0.0f) {
		shoot_timer = kShooterFireRate;
		auto *e = CreateEntity<Bullet>();
		e->x = x;
		e->y = y;
		e->direction = -1;
	}
}

void Shooter::Render() {
	pti_spr(bitmap_shooter, 0, x - 8, y - 16, false, false);
}

void Shooter::HandleHorizontalMovement() {
	// nothing.
}

void Shooter::HandleVerticalMovement() {
	// nothing.
}