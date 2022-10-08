#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "framework64/util/bump_allocator.h"

#include "player.h"
#include "ui.h"
#include "audio_controller.h"
#include "projectile_controller.h"
#include "level_base.h"

#include "framework64/vec3.h"
#include "framework64/level.h"
#define TILE_ROW_CELLS 3
#define TILE_COL_CELLS 3
#define ACTIVE_TILE_COUNT (TILE_ROW_CELLS * TILE_COL_CELLS)

// N -Z, S +Z
// E +X, W -X
typedef enum {
    NORTH, EAST, SOUTH, WEST
} CompassDirections;

typedef struct {
    LevelBase base;
    fw64BumpAllocator allocators[ACTIVE_TILE_COUNT];
    uint32_t chunk_handles[ACTIVE_TILE_COUNT];
    float next_row_pos[4];
    float next_row_trigger[4];
    int handle_nw, handle_ne, handle_sw, handle_se;
    Vec3 player_prev_position;
} TilesTestLevel;

void tiles_test_level_init(TilesTestLevel* level, fw64Engine* engine);
void tiles_test_level_uninit(TilesTestLevel* level);
void tiles_test_level_update(TilesTestLevel* level);
void tiles_test_level_draw(TilesTestLevel* level);