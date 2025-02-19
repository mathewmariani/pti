#include "pti.h"

#include "goomba.h"

template<>
bool EntityBase::Is<Goomba>() const {
	return type == EntityType::Goomba;
}

void Goomba::Update() {
	Physics();
	HandleHorizontalMovement();
	HandleVerticalMovement();

	if (IsTouching()) {
		direction *= -1;
		flags ^= EntityFlags::ENTITYFLAG_FACING_LEFT;
	}
}

void Goomba::Render() {
	pti_rect(x + bx, y + by, bw, bh, 0xff0000);
}

void Goomba::InteractWith(const EntityBase *other) {
	switch (other->type) {
		case EntityType::Coin:
			break;
		case EntityType::Goomba:
			direction *= -1;
			x += direction;
			sx = -sx;
			flags ^= EntityFlags::ENTITYFLAG_FACING_LEFT;
			break;
		case EntityType::Player:
			break;
		default:
			break;
	}
}

void Goomba::HandleHorizontalMovement() {
	_pti_appr(sx, direction * kGoombaMaxSpeed, kGoombaAcceleration * PTI_DELTA);
}

void Goomba::HandleVerticalMovement() {
	if ((flags & EntityFlags::ENTITYFLAG_GROUNDED) == 0) {
		sy += kGoombaPhysicsVerticalGravFall;
		if (sy > kGoombaPhysicsVerticalMax) {
			sy = kGoombaPhysicsVerticalMax;
		}
	} else {
		sy = 0;
	}
}