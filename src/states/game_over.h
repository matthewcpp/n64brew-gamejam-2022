#pragma once

#include "game_data.h"

#include <framework64/engine.h>
#include <framework64/util/bump_allocator.h>

typedef struct GameOver {
    fw64Engine* engine;
    GameData* game_data;
    fw64Camera* camera;
    fw64BumpAllocator bump_allocator;
} GameOver;

void game_state_game_over_init(GameOver* state, fw64Engine* engine, GameData* game_data);
void game_state_game_over_uninit(GameOver* state);
void game_state_game_over_update(GameOver* state);
void game_state_game_over_draw(GameOver* state);