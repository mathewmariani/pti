#pragma once

#include "base.h"

constexpr int kCoinBoundaryOffset = 2;
constexpr int kCoinWidth = 4;
constexpr int kCoinHeight = 4;
constexpr int kCoinFrameCount = 8;
constexpr int kCoinFrameMod = 2;

struct Coin : EntityBase {
	enum class Type : uint8_t {
		Gold,
		Red,
		Blue,
		Purple,
	};

	static constexpr auto cEntityType = EntityType::Coin;

	Type subtype;

	static void Create(const CoordXY<int> &location, Type type = Type::Gold);

	void Update() override;
	void PostUpdate() override;
	void Render() override;

	const EntityReaction Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) override;
};

void CoinUpdateAll();