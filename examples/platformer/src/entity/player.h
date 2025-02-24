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

constexpr int kPlayerOffsetX = 4;
constexpr int kPlayerOffsetY = 8;
constexpr int kPlayerWidth = 8;
constexpr int kPlayerHeight = 8;
constexpr int kPlayerFrameCount = 8;
constexpr int kPlayerFrameMod = 2;


enum class PlayerState : uint8_t {
	Normal,
	Jump,
	Death,
};

struct Player : EntityBase {
	static constexpr auto cEntityType = EntityType::Player;

	Player() {
		bx = 0;
		by = 0;
		bw = kPlayerWidth;
		bh = kPlayerHeight;
		state = PlayerState::Normal;
	}

	void Update() override;
	void PostUpdate() override;
	void Render() override;
	bool PreSolidCollisionWith(EntityBase *const other, const CoordXY<int> &dir) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
	void HandleJump();

private:
	PlayerState state;

	bool bumped;
};