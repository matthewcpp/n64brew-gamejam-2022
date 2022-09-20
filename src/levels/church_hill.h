#pragma once

#include "framework64/engine.h"

#include "level.h"
#include "player.h"
#include "ui.h"
#include "zombie_spawner.h"
#include "trigger.h"
#include "projectile_controller.h"

#define HILL_LEVEL_TRIGGER_CROW 0
#define HILL_LEVEL_TRIGGER_HOWL 1
#define HILL_LEVEL_TRIGGER_COUNT 2

typedef struct {
    fw64Engine* engine;
    fw64Level level;
    Player player;
    ProjectileController projectile_controller;
    UI ui;
    ZombieSpawner zombie_spawner[2];
    TriggerBox triggers[HILL_LEVEL_TRIGGER_COUNT];
    fw64Allocator* allocator;
    fw64SoundBank* sound;
} HillLevel;

void hill_level_init(HillLevel* hill_level, fw64Engine* engine);
void hill_level_uninit(HillLevel* hill_level);
void hill_level_update(HillLevel* hill_level);
void hill_level_draw(HillLevel* hill_level);