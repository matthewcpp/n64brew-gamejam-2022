#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "audio_controller.h"
#include "player.h"
#include "ui.h"
#include "zombie_spawner.h"
#include "projectile_controller.h"
#include "weapon_pickup.h"

#include "framework64/level.h"

typedef struct {
    fw64Engine* engine;
    Player player;
    ProjectileController projectile_controller;
    UI ui;
    ZombieSpawner zombie_spawner;
    AudioController audio_controller;
    WeaponPickups weapon_pickups;
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
