#include "coin.h"
#include "registry.h"

#include "../bank.h"

#include "pti.h"

template<>
bool EntityBase::Is<Coin>() const {
	return type == EntityType::Coin;
}

void Coin::Create(int32_t x, int32_t y, Coin::Type type) {
	auto *coin = CreateEntity<Coin>();
	if (coin == nullptr) {
		return;
	}

	coin->bx = kCoinBoundaryOffset;
	coin->by = kCoinBoundaryOffset;
	coin->bw = kCoinWidth;
	coin->bh = kCoinHeight;
	coin->flags = EntityFlags::ENTITYFLAG_OVERLAP_CHECKS;
	coin->subtype = type;

	coin->SetLocation(x, y);
}

void Coin::Update() {
	// nothing.
}

void Coin::Render() {
	auto frame = static_cast<int>(timer * kCoinFrameCount) % kCoinFrameMod;
	pti_spr(bitmap_coin, frame, x - kCoinWidth / 2, y - kCoinHeight / 2, false, false);
}

void Coin::InteractWith(const EntityBase *other) {
	// nothing.
}

void CoinUpdateAll() {
	// nothing.
}