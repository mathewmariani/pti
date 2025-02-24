#include "pti.h"

#include "goomba.h"
#include "effect.h"
#include "registry.h"
#include "../bank.h"

template<>
bool EntityBase::Is<Goomba>() const {
	return type == EntityType::Goomba;
}

void Goomba::Create(const CoordXY<int> &location) {
	auto *goomba = CreateEntity<Goomba>();
	if (goomba == nullptr) {
		return;
	}

	goomba->bx = 0;
	goomba->by = 0;
	goomba->bw = kGoombaHeight;
	goomba->bh = kGoombaWidth;
	goomba->direction = -1;

	goomba->SetLocation(location);
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

void Goomba::PostUpdate() {
	if (flags & EntityFlags::MarkedForGarbage) {
		Effect::Create({x, y});
		RemoveEntity(this);
	}
}

void Goomba::Render() {
	auto frame = static_cast<int>(timer * kGoombaFrameCount) % kGoombaFrameMod;
	if (sx == 0 && IsGrounded()) {
		frame = 0;
	}

	pti_spr(bitmap_goomba, frame, x - kGoombaOffsetX, y - kGoombaOffsetY, false, false);
}

const EntityReaction Goomba::Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) {
	if (dir.y > 0) {
		if (interaction == EntityInteraction::Touch) {
			flags |= EntityFlags::MarkedForGarbage;
			return EntityReaction::Bump;
		}
	}
	if (interaction == EntityInteraction::Touch) {
		return EntityReaction::Hurt;
	}
	return EntityReaction::None;
}

void Goomba::HandleHorizontalMovement() {
	_pti_appr(sx, direction * kGoombaMaxSpeed, kGoombaAcceleration * PTI_DELTA);
}

void Goomba::HandleVerticalMovement() {
	sy += kGoombaPhysicsVerticalGravFall;
	if (sy > kGoombaPhysicsVerticalMax) {
		sy = kGoombaPhysicsVerticalMax;
	}
}