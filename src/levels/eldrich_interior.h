#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "audio_controller.h"
#include "player.h"
#include "ui/ui.h"
#include "zombie_spawner.h"
#include "projectile_controller.h"
#include "pickups.h"

#include "levels/level_base.h"

#include "framework64/level.h"

typedef struct {
    LevelBase base;
    ZombieSpawner zombie_spawner;
} EldrichLevel;

#ifdef __cplusplus
extern "C" {
#endif

void eldrich_interior_level_init(EldrichLevel* level, fw64Engine* engine, GameData* game_data, fw64Allocator* state_allocator);
void eldrich_interior_level_uninit(EldrichLevel* level);
void eldrich_interior_level_update(EldrichLevel* level);
void eldrich_interior_level_draw(EldrichLevel* level);

#ifdef __cplusplus
}
#endif
