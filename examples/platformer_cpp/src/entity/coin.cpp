#include "coin.h"

#include "pti.h"

template<>
bool EntityBase::Is<Coin>() const {
	return type == EntityType::Coin;
}

void Coin::Update() {}

void Coin::Render() {
	pti_rect(x + bx, y + by, bw, bh, 0xff0000);
}

void Coin::InteractWith(const EntityBase *other) {
}

void CoinUpdateAll() {
	// for (auto coin : EntityList<Coin>()) {
	// 	coin->Update();
	// }
}