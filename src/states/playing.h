#pragma once

#include "game_data.h"

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#include "levels/level_base.h"
#include "levels/levels.h"
#include "levels/test.h"
#include "levels/church_hill.h"
#include "levels/tiles_test.h"
#include "levels/interior.h"
#include "levels/eldrich_interior.h"

typedef union {
    LevelBase current;
    TestLevel test_level;
    HillLevel church_hill;
    TilesTestLevel tiles_test;
    InteriorLevel interior;
    EldrichLevel eldrich_interior;
} Levels;

typedef struct {
    fw64Engine* engine;
    GameData* game_data;
    Level current_level;
    Levels levels;
    float return_to_level_select_time;
    fw64BumpAllocator bump_allocator;
} Playing;

void game_state_playing_init(Playing* state, fw64Engine* engine, GameData* game_data);
void game_state_playing_uninit(Playing* state);
void game_state_playing_update(Playing* state);
void game_state_playing_draw(Playing* state);