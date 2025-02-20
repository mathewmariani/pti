#pragma once

#include "base.h"

constexpr float kShooterFireRate = 1.0f;

enum ShooterState {
	ShooterNormal,
	ShooterJump,
	ShooterDeath,
};

struct Shooter : EntityBase {
	static constexpr auto cEntityType = EntityType::Shooter;

	Shooter() {
		bx = -4;
		by = -8;
		bw = 8;
		bh = 8;
		direction = -1;
		flags = EntityFlags::ENTITYFLAG_OVERLAP_CHECKS | EntityFlags::ENTITYFLAG_HITS_SOLIDS;
		shoot_timer = kShooterFireRate;
	}

	void Update() override;
	void Render() override;
	void InteractWith(const EntityBase *other) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();

private:
	float shoot_timer;
};