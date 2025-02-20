#pragma once

#include "base.h"

constexpr int kCoinBoundaryOffset = -2;
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

	static void Create(int32_t x, int32_t y, Type type = Type::Gold);

	void Update() override;
	void Render() override;
	void InteractWith(const EntityBase *other) override;
};

void CoinUpdateAll();