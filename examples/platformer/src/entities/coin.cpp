/* pti */
#include "pti.h"

#include "../bank.h"
#include "entities.h"

void Coin::Init(void) {
	bx = -2;
	by = -2;
	bw = 4;
	bh = 4;
	timer = 0.0f;
	flags = Entity::ENTITYFLAG_OVERLAP_CHECKS;
}

void Coin::Destroy(void) {
	// ...
}

void Coin::Update(void) {
	// ...
}

void Coin::Draw(void) {
	frame = ((int) (timer * 8) % 2);
	pti_spr(bitmap_coin, frame, x - 2, y - 2, false, false);
}

void Coin::Overlap(Entity &other) {
	// ...
}
