#pragma once

#include "base.h"

constexpr int kEffectBoundaryOffset = 2;
constexpr int kEffectWidth = 4;
constexpr int kEffectHeight = 4;
constexpr int kEffectFrameCount = 8;
constexpr int kEffectFrameMod = 5;

struct Effect : EntityBase {
	enum class Type : uint8_t {
		Collect,
		JumpDust,
		LandDust,
	};

	static constexpr auto cEntityType = EntityType::Effect;

	Type subtype;

	static void Create(const CoordXY<int> &location, Type type = Type::Collect);

	void Update() override;
	void PostUpdate() override;
	void Render() override;

	const EntityReaction Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) override;
};