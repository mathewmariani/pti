#pragma once

#include "base.h"

constexpr float kBulletMaxSpeed = 1.0f;
constexpr float kBulletAcceleration = 20.0f;

enum BulletState {
	BulletNormal,
	BulletJump,
	BulletDeath,
};

struct Bullet : EntityBase {
	static constexpr auto cEntityType = EntityType::Bullet;

	Bullet() {
		bx = -4;
		by = -8;
		bw = 8;
		bh = 8;
		direction = -1;
	}

	void Update() override;
	void Render() override;

	const EntityReaction Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
};