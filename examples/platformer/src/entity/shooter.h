#pragma once

#include "base.h"

constexpr float kShooterFireRate = 1.0f;
constexpr int kShooterOffsetX = 4;
constexpr int kShooterOffsetY = 8;
constexpr int kShooterWidth = 8;
constexpr int kShooterHeight = 8;

struct Shooter : EntityBase {
	static constexpr auto cEntityType = EntityType::Shooter;

	Shooter() {
		bx = 0;
		by = 0;
		bw = kShooterWidth;
		bh = kShooterHeight;
		direction = -1;
		flags = EntityFlags::ProvidesStaticCollision;
		shoot_timer = kShooterFireRate;
	}

	void Update() override;
	void Render() override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();

private:
	float shoot_timer;
};