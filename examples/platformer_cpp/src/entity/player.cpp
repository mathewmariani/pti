#include "player.h"

#include "pti.h"

template<>
bool EntityBase::Is<Player>() const {
	return type == EntityType::Player;
}

void Player::Update() {
	Physics();
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

void Player::Render() {
	pti_rect(x + bx, y + by, bw, bh, 0xff0000);
}

void Player::InteractWith(const EntityBase *other) {
	switch (other->type) {
		case EntityType::Coin:
			break;
		case EntityType::Goomba:
			if (sy > 0.0f) {
				if ((flags & EntityFlags::ENTITYFLAG_GROUNDED) == 0) {
					flags &= ~EntityFlags::ENTITYFLAG_GROUNDED;
					sy = -kPlayerPhysicsJumpStrength;
				}
			}
		case EntityType::Player:
			break;
		default:
			break;
	}
}

void Player::HandleHorizontalMovement() {
	if (pti_down(PTI_LEFT)) {
		_pti_appr(sx, -kPlayerMaxSpeed, kPlayerAcceleration * PTI_DELTA);
		flags |= EntityFlags::ENTITYFLAG_FACING_LEFT;
	} else if (pti_down(PTI_RIGHT)) {
		_pti_appr(sx, kPlayerMaxSpeed, kPlayerAcceleration * PTI_DELTA);
		flags &= ~EntityFlags::ENTITYFLAG_FACING_LEFT;
	} else {
		_pti_appr(sx, 0, kPlayerFriction * PTI_DELTA);
	}
}

void Player::HandleVerticalMovement() {
	static int hang_time = 0;

	if ((flags & EntityFlags::ENTITYFLAG_GROUNDED) == 0) {
		if (state == PlayerState::PlayerJump) {
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
	} else {
		sy = 0;
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
		state = PlayerState::PlayerJump;
		flags &= ~EntityFlags::ENTITYFLAG_GROUNDED;
		sy = -kPlayerPhysicsJumpStrength;
	}

	// Revert state
	if (state == PlayerState::PlayerJump && (flags & EntityFlags::ENTITYFLAG_GROUNDED)) {
		state = PlayerState::PlayerNormal;
	}

	// Variable jump
	if (state == PlayerState::PlayerJump && sy < -(kPlayerPhysicsJumpStrength * 0.5f)) {
		if (pti_released(PTI_UP)) {
			sy = -(kPlayerPhysicsJumpStrength * 0.5f);
		}
	}
}