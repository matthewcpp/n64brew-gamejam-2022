#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#include "game_data.h"

typedef struct {
    fw64BumpAllocator bump_allocator;
    fw64Engine* engine;
    GameData* game_data;
    fw64Font* font;
    fw64Camera camera;
    Level selected_level;
    int title_pos_x;
} LevelSelect;

void game_state_level_select_init(LevelSelect* level_select, fw64Engine* engine, GameData* game_data);
void game_state_level_select_update(LevelSelect* level_select);
void game_state_level_select_draw(LevelSelect* level_select);
void game_state_level_select_uninit(LevelSelect* level_select);