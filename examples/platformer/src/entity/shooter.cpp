#include "pti.h"

#include "shooter.h"
#include "projectile.h"
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
		Projectile::Create({x - 8, y}, Projectile::Type::Ball);
	}
}

void Shooter::Render() {
	pti_spr(bitmap_shooter, 0, x - kShooterOffsetX, y - kShooterOffsetY, false, false);
}

void Shooter::HandleHorizontalMovement() {
	// nothing.
}

void Shooter::HandleVerticalMovement() {
	// nothing.
}