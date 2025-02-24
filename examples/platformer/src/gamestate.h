#pragma once

#include "entity/registry.h"
#include "entity/coin.h"
#include "entity/effect.h"
#include "entity/goomba.h"
#include "entity/player.h"
#include "entity/projectile.h"
#include "entity/shooter.h"

#include <variant>

constexpr float kDeathResetTimer = 5.0f;

using EntityVariant = std::variant<EntityBase, Coin, Effect, Goomba, Player, Projectile, Shooter>;

struct GameState_t {
	EntityVariant Entities[kMaxEntities]{};
	uint8_t Coins = 0;
	uint8_t Deaths = 0;

	bool PlayerIsDead = false;
	float ResetTimer = 0.0f;
};

GameState_t &GetGameState();

void GameStateInit();
void GameStateTick();