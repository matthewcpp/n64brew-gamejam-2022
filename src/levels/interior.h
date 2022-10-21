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
#include "zombie_spawner.h"

#include "framework64/level.h"

#define ROOM_COUNT 9

typedef enum {
    BUILDING_EXIT,
    STAIRS_UP,
    STAIRS_DOWN
} INTERIOR_EXITS;

typedef struct {
    LevelBase base;
    fw64BumpAllocator allocators[ROOM_COUNT];
    uint32_t room_handles[ROOM_COUNT];
    int current_floor;
    int total_floors;
    int has_exit_type[3];
    Vec3 exits[3];
    ZombieSpawner zombie_spawner;
} InteriorLevel;

void interior_level_init(InteriorLevel* level, fw64Engine* engine, GameData* game_data);
void interior_level_uninit(InteriorLevel* level);
void interior_level_update(InteriorLevel* level);
void interior_level_draw(InteriorLevel* level);