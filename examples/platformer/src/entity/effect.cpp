#include "coin.h"
#include "registry.h"
#include "../gamestate.h"

#include "../bank.h"

#include "pti.h"

template<>
bool EntityBase::Is<Effect>() const {
	return type == EntityType::Effect;
}

void Effect::Create(const CoordXY<int> &location, Effect::Type type) {
	auto *effect = CreateEntity<Effect>();
	if (effect == nullptr) {
		return;
	}

	// effect->bx = kEffectBoundaryOffset;
	// effect->by = kEffectBoundaryOffset;
	// effect->bw = kEffectWidth;
	// effect->bh = kEffectHeight;
	effect->subtype = type;

	effect->SetLocation(location);
}

void Effect::Update() {
	// nothing.
	auto frame = static_cast<int>(timer * kEffectFrameCount);
	if (frame == (kEffectFrameMod - 1)) {
		flags |= EntityFlags::MarkedForGarbage;
	}
}

void Effect::PostUpdate() {
	if (flags & EntityFlags::MarkedForGarbage) {
		RemoveEntity(this);
	}
}

void Effect::Render() {
	auto frame = static_cast<int>(timer * kEffectFrameCount) % kEffectFrameMod;

	switch (subtype) {
		case Type::Collect:
			pti_spr(bitmap_fx_collect, frame, x, y, false, false);
			break;
		case Type::JumpDust:
			pti_spr(bitmap_fx_dust1, frame, x - 4, y + 2, false, false);
			break;
		case Type::LandDust:
			pti_spr(bitmap_fx_dust2, frame, x - 4, y + 2, false, false);
			break;
	}
}

const EntityReaction Effect::Interact(const EntityInteraction interaction, EntityBase *const from, const CoordXY<int> &dir) {
	return EntityReaction::None;
}