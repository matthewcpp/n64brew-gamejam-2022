#pragma once

#include "levels/levels.h"
#include "states/game_states.h"

typedef struct {
    Level transition_to_level;
    GameState transition_to_state;
} GameData;

void game_data_init(GameData* game_data);