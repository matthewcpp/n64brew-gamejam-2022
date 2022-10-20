#pragma once

#include "framework64/engine.h"
#include "framework64/level.h"

#include "player.h"
#include "ui.h"
#include "zombie_spawner.h"
#include "trigger.h"
#include "projectile_controller.h"
#include "audio_controller.h"
#include "level_base.h"

#define HILL_LEVEL_TRIGGER_CROW 0
#define HILL_LEVEL_TRIGGER_HOWL 1
#define HILL_LEVEL_TRIGGER_COUNT 2

typedef struct {
    LevelBase base;
    ZombieSpawner zombie_spawner[2];
    TriggerBox triggers[HILL_LEVEL_TRIGGER_COUNT];

} HillLevel;

void hill_level_init(HillLevel* hill_level, fw64Engine* engine, GameData* game_data);
void hill_level_uninit(HillLevel* hill_level);
void hill_level_update(HillLevel* hill_level);
void hill_level_draw(HillLevel* hill_level);