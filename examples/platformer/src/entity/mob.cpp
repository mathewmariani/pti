#include "pti.h"

#include "goomba.h"
#include "registry.h"
#include "../bank.h"

template<>
bool EntityBase::Is<Goomba>() const {
	return type == EntityType::Goomba;
}

void Goomba::Update() {
	if (IsTouching()) {
		direction *= -1;
		x += direction;
		sx = -sx;
	}

	Physics();
	HandleHorizontalMovement();
	HandleVerticalMovement();
}

void Goomba::Render() {
	auto frame = static_cast<int>(timer * kGoombaFrameCount) % kGoombaFrameMod;
	if (sx == 0 && sy == 0) {
		frame = 0;
	}

	pti_spr(bitmap_goomba, frame, x - 8, y - 16, false, false);
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