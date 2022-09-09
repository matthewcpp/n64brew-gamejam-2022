#pragma once

#include "game_data.h"

#include "framework64/engine.h"

#include "levels/levels.h"
#include "levels/test.h"
#include "levels/church_hill.h"

typedef union {
    TestLevel test_level;
    HillLevel church_hill;
} Levels;

typedef struct {
    fw64Engine* engine;
    GameData* game_data;
    Level current_level;
    Levels levels;
    float return_to_level_select_time;
} Playing;

void game_state_playing_init(Playing* state, fw64Engine* engine, GameData* game_data);
void game_state_playing_uninit(Playing* state);
void game_state_playing_update(Playing* state);
void game_state_playing_draw(Playing* state);