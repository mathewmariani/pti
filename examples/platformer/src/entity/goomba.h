#pragma once

#include "base.h"

constexpr float kGoombaMaxSpeed = 1.0f;
constexpr float kGoombaAcceleration = 20.0f;
constexpr float kGoombaFriction = 15.0f;
constexpr float kGoombaPhysicsVerticalMax = 6.0f;
constexpr float kGoombaPhysicsVerticalGrav = 0.24f;
constexpr float kGoombaPhysicsVerticalGravFall = 0.4254f;
constexpr float kGoombaPhysicsVerticalGravAlt = 0.1940f;
constexpr float kGoombaPhysicsJumpStrength = 6.1135f;
constexpr float kGoombaPhysicsBounceStrength = 4.5535f;
constexpr int kGoombaFrameCount = 8;
constexpr int kGoombaFrameMod = 2;

enum GoombaState {
	GoombaNormal,
	GoombaJump,
	GoombaDeath,
};

struct Goomba : EntityBase {
	static constexpr auto cEntityType = EntityType::Goomba;

	Goomba() {
		bx = -4;
		by = -8;
		bw = 8;
		bh = 8;
		direction = -1;
		flags = EntityFlags::ENTITYFLAG_OVERLAP_CHECKS | EntityFlags::ENTITYFLAG_HITS_SOLIDS | EntityFlags::ENTITYFLAG_FACING_LEFT;
	}

	void Update() override;
	void Render() override;
	void InteractWith(const EntityBase *other) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
};