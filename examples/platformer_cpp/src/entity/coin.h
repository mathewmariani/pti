#pragma once

#include "base.h"

constexpr int kCoinBoundaryOffset = -2;
constexpr int kCoinWidth = 4;
constexpr int kCoinHeight = 4;
constexpr int kCoinFrameCount = 8;
constexpr int kCoinFrameMod = 2;

struct Coin : EntityBase {
	static constexpr auto cEntityType = EntityType::Coin;

	Coin() {
		bx = kCoinBoundaryOffset;
		by = kCoinBoundaryOffset;
		bw = kCoinWidth;
		bh = kCoinHeight;
		flags = EntityFlags::ENTITYFLAG_OVERLAP_CHECKS;
	}

	void Update() override;
	void Render() override;
	void InteractWith(const EntityBase *other) override;
};

void CoinUpdateAll();