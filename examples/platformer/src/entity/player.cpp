#include "player.h"
#include "effect.h"
#include "registry.h"
#include "../gamestate.h"
#include "../bank.h"
#include "pti.h"

template<>
bool EntityBase::Is<Player>() const {
	return type == EntityType::Player;
}

void Player::Update() {
	if (bumped) {
		state = PlayerState::Jump;
		flags &= ~EntityFlags::ENTITYFLAG_GROUNDED;
		sy = -kPlayerPhysicsJumpStrength;

		bumped = false;
	}

	HandleHorizontalMovement();
	HandleVerticalMovement();
	HandleJump();

	// Camera movement
	constexpr int cx = 256;
	constexpr int cy = 128;

	int cam_x, cam_y;
	pti_get_camera(&cam_x, &cam_y);
	cam_x += ((x + bx + bw / 2.0f - cx / 2.0f) - cam_x) / 10;
	cam_y += ((y + by + bh / 2.0f - cy / 2.0f) - cam_y) / 10;
	pti_camera(cam_x, cam_y);
}

void Player::PostUpdate() {
	if (flags & EntityFlags::MarkedForGarbage) {
		Effect::Create({x, y});
		RemoveEntity(this);
		GetGameState().Deaths++;
		GetGameState().PlayerIsDead = true;
	}
}

void Player::Render() {
	auto frame = static_cast<int>(timer * kPlayerFrameCount) % kPlayerFrameMod;
	if (sx == 0 && IsGrounded()) {
		frame = 0;
	}

	pti_spr(bitmap_player, frame, x - kPlayerOffsetX, y - kPlayerOffsetY, false, false);
}

bool Player::PreSolidCollisionWith(EntityBase *const other, const CoordXY<int> &dir) {
	auto reaction = other->Interact(EntityInteraction::CollectDirect, this, dir);

	if (reaction == EntityReaction::Collected) {
		return false;
	}

	if (reaction == EntityReaction::None) {
		reaction = other->Interact(EntityInteraction::Touch, this, dir);

		if (reaction == EntityReaction::Bump) {
			bumped = true;
			return true;
		} else if (reaction == EntityReaction::Hurt) {
			flags |= EntityFlags::MarkedForGarbage;
			return true;
		}
	}

	return false;
}

void Player::HandleHorizontalMovement() {
	if (pti_down(PTI_LEFT)) {
		_pti_appr(sx, -kPlayerMaxSpeed, kPlayerAcceleration * PTI_DELTA);
	} else if (pti_down(PTI_RIGHT)) {
		_pti_appr(sx, kPlayerMaxSpeed, kPlayerAcceleration * PTI_DELTA);
	} else {
		_pti_appr(sx, 0, kPlayerFriction * PTI_DELTA);
	}
}

void Player::HandleVerticalMovement() {
	static int hang_time = 0;

	if (IsGrounded() && state == PlayerState::Jump) {
		if (sy <= -0.5f) {
			hang_time = 3;
			sy += kPlayerPhysicsVerticalGravFall;
		} else {
			if (hang_time > 0) {
				--hang_time;
				sy = 0;
			} else {
				sy += kPlayerPhysicsVerticalGravFall;
			}
		}
	} else {
		sy += kPlayerPhysicsVerticalGravFall;
	}

	// Limit vertical speed
	if (sy > kPlayerPhysicsVerticalMax) {
		sy = kPlayerPhysicsVerticalMax;
	}
}

void Player::HandleJump() {
	bool kJump = pti_pressed(PTI_UP);

	// Full jump
	if (kJump && (flags & EntityFlags::ENTITYFLAG_GROUNDED)) {
		state = PlayerState::Jump;
		flags &= ~EntityFlags::ENTITYFLAG_GROUNDED;
		sy = -kPlayerPhysicsJumpStrength;

		Effect::Create({x, y}, Effect::Type::JumpDust);
	}

	// Revert state
	if (state == PlayerState::Jump && (flags & EntityFlags::ENTITYFLAG_GROUNDED)) {
		state = PlayerState::Normal;
		Effect::Create({x, y}, Effect::Type::LandDust);
	}

	// Variable jump
	if (state == PlayerState::Jump && sy < -(kPlayerPhysicsJumpStrength * 0.5f)) {
		if (pti_released(PTI_UP)) {
			sy = -(kPlayerPhysicsJumpStrength * 0.5f);
		}
	}
}