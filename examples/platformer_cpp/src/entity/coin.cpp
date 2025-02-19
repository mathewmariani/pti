#include "coin.h"
#include "../bank.h"

#include "pti.h"

template<>
bool EntityBase::Is<Coin>() const {
	return type == EntityType::Coin;
}

void Coin::Update() {}

void Coin::Render() {
	auto frame = static_cast<int>(timer * kCoinFrameCount) % kCoinFrameMod;
	pti_spr(bitmap_coin, frame, x - kCoinWidth / 2, y - kCoinHeight / 2, false, false);
}

void Coin::InteractWith(const EntityBase *other) {
}

void CoinUpdateAll() {
	// for (auto coin : EntityList<Coin>()) {
	// 	coin->Update();
	// }
}