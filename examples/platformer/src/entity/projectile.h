#pragma once

#include "base.h"

constexpr float kProjectileMaxSpeed = 1.0f;
constexpr float kProjectileAcceleration = 20.0f;
constexpr int kProjectileBoundaryOffset = 2;
constexpr int kProjectileWidth = 4;
constexpr int kProjectileHeight = 4;

struct Projectile : EntityBase {
	enum class Type : uint8_t {
		Ball,
	};

	static constexpr auto cEntityType = EntityType::Projectile;

	Type subtype;

	static void Create(const CoordXY<int> &location, Type type = Type::Ball);

	void Update() override;
	void PostUpdate() override;
	void Render() override;

	const EntityReaction Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) override;

private:
	void HandleHorizontalMovement();
	void HandleVerticalMovement();
};