#include "pti.h"

#include "goomba.h"
#include "coin.h"
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

	HandleHorizontalMovement();
	HandleVerticalMovement();
}

void Goomba::Render() {
	auto frame = static_cast<int>(timer * kGoombaFrameCount) % kGoombaFrameMod;
	if (sx == 0 && sy == 0) {
		frame = 0;
	}

	pti_spr(bitmap_goomba, frame, x - 8, y - 16, false, false);
	pti_rect(x + bx, y + by, bw, bh, 0xff0000ff);
}

const EntityReaction Goomba::Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) {
	if (interaction == EntityInteraction::Touch) {
		return EntityReaction::Bump;
	}
	return EntityReaction::None;
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