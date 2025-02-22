#pragma once

#include "entity/registry.h"
#include "entity/coin.h"
#include "entity/goomba.h"
#include "entity/player.h"
#include "entity/projectile.h"
#include "entity/shooter.h"

#include <variant>

using EntityVariant = std::variant<EntityBase, Coin, Goomba, Player, Projectile, Shooter>;

struct GameState_t {
	EntityVariant Entities[kMaxEntities];
};

GameState_t &GetGameState();