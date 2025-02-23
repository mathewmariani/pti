#include "gamestate.h"

#include <memory>

static auto _gameState = std::make_unique<GameState_t>();

GameState_t &GetGameState() {
	return *_gameState;
}

void GameStateInit() {
	ResetAllEntities();
}

void GameStateTick() {
	auto GetEntityBase = [](auto &entity) -> EntityBase * { return &entity; };

	for (auto &e : GetGameState().Entities) {
		auto *entity = std::visit(GetEntityBase, e);
		if (entity && entity->type != EntityType::Null) {
			entity->Step();
		}
	}

	for (auto &e : GetGameState().Entities) {
		auto *entity = std::visit(GetEntityBase, e);
		if (entity && entity->type != EntityType::Null) {
			entity->PostUpdate();
		}
	}
}