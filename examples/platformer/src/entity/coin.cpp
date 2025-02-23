#include "coin.h"
#include "registry.h"
#include "../gamestate.h"

#include "../bank.h"

#include "pti.h"

template<>
bool EntityBase::Is<Coin>() const {
	return type == EntityType::Coin;
}

void Coin::Create(const CoordXY<int> &location, Coin::Type type) {
	auto *coin = CreateEntity<Coin>();
	if (coin == nullptr) {
		return;
	}

	coin->bx = kCoinBoundaryOffset;
	coin->by = kCoinBoundaryOffset;
	coin->bw = kCoinWidth;
	coin->bh = kCoinHeight;
	coin->subtype = type;

	coin->SetLocation(location);
}

void Coin::Update() {
	// nothing.
}

void Coin::PostUpdate() {
	if (flags & EntityFlags::MarkedForGarbage) {
		Effect::Create({x, y});

		RemoveEntity(this);
		GetGameState().Coins++;
	}
}

void Coin::Render() {
	auto frame = static_cast<int>(timer * kCoinFrameCount) % kCoinFrameMod;
	pti_spr(bitmap_coin, frame, x, y, false, false);
}

const EntityReaction Coin::Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) {
	if (interaction == EntityInteraction::CollectDirect) {
		flags |= EntityFlags::MarkedForGarbage;
		return EntityReaction::Collected;
	}
	return EntityReaction::None;
}

void CoinUpdateAll() {
	// nothing.
}