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

constexpr int kGoombaOffsetX = 4;
constexpr int kGoombaOffsetY = 8;
constexpr int kGoombaWidth = 8;
constexpr int kGoombaHeight = 8;
constexpr int kGoombaFrameCount = 8;
constexpr int kGoombaFrameMod = 2;

struct Goomba : EntityBase {
	static constexpr auto cEntityType = EntityType::Goomba;

	static void Create(const CoordXY<int> &locatio);

	void Update() override;
	void PostUpdate() override;
	void Render() override;

	const EntityReaction Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
};