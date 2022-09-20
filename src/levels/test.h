#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "player.h"
#include "ui.h"
#include "zombie_spawner.h"
#include "projectile_controller.h"

#include "level.h"

typedef struct {
    fw64Engine* engine;
    Player player;
    ProjectileController projectile_controller;
    UI ui;
    ZombieSpawner zombie_spawner;
    fw64Level level;
    fw64MusicBank* music;
    fw64SoundBank* sound;
    fw64Allocator* allocator;
} TestLevel;

#ifdef __cplusplus
extern "C" {
#endif

void test_level_init(TestLevel* level, fw64Engine* engine);
void test_level_uninit(TestLevel* level);
void test_level_update(TestLevel* level);
void test_level_draw(TestLevel* level);

#ifdef __cplusplus
}
#endif
