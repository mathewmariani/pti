#pragma once

#include "../lib/entity.h"

void entity_bullet(entity::event_t *ev);
void entity_coin(entity::event_t *ev);
void entity_goomba(entity::event_t *ev);
void entity_player(entity::event_t *ev);
void entity_pop(entity::event_t *ev);
void entity_shooter(entity::event_t *ev);

void create_coin(entity::entity_t *e);