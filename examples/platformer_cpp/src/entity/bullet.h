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
		flags = EntityFlags::ENTITYFLAG_OVERLAP_CHECKS | EntityFlags::ENTITYFLAG_HITS_SOLIDS;
		state = BulletState::BulletNormal;
	}

	void Update() override;
	void Render() override;
	void InteractWith(const EntityBase *other) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
};