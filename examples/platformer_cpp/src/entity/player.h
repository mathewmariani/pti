#pragma once

#include "base.h"

constexpr float kPlayerMaxSpeed = 3.0f;
constexpr float kPlayerAcceleration = 20.0f;
constexpr float kPlayerFriction = 15.0f;
constexpr float kPlayerPhysicsVerticalMax = 6.0f;
constexpr float kPlayerPhysicsVerticalGrav = 0.24f;
constexpr float kPlayerPhysicsVerticalGravFall = 0.4254f;
constexpr float kPlayerPhysicsVerticalGravAlt = 0.1940f;
constexpr float kPlayerPhysicsJumpStrength = 6.1135f;
constexpr float kPlayerPhysicsBounceStrength = 4.5535f;

enum PlayerState {
	PlayerNormal,
	PlayerJump,
	PlayerDeath,
};

struct Player : EntityBase {
	static constexpr auto cEntityType = EntityType::Player;

	Player() {
		bx = -4;
		by = -8;
		bw = 8;
		bh = 8;
		flags = EntityFlags::ENTITYFLAG_OVERLAP_CHECKS | EntityFlags::ENTITYFLAG_HITS_SOLIDS;
		state = PlayerState::PlayerNormal;
	}

	void Update() override;
	void Render() override;
	void InteractWith(const EntityBase *other) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
	void HandleJump();
};