#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "framework64/util/bump_allocator.h"

#include "player.h"
#include "ui.h"
#include "audio_controller.h"
#include "projectile_controller.h"

#include "framework64/level.h"

#define ACTIVE_TILE_COUNT 6

typedef struct {
    fw64Engine* engine;
    fw64Level level;
    Player player;
    ProjectileController projectile_controller;
    AudioController audio_controller;
    fw64BumpAllocator allocators[ACTIVE_TILE_COUNT];
    uint32_t chunk_handles[ACTIVE_TILE_COUNT];
    float next_row_pos;
    float next_row_trigger;
    int handle_index;
    float player_previous_z;
} TilesTestLevel;

void tiles_test_level_init(TilesTestLevel* level, fw64Engine* engine);
void tiles_test_level_uninit(TilesTestLevel* level);
void tiles_test_level_update(TilesTestLevel* level);
void tiles_test_level_draw(TilesTestLevel* level);