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

#include "levels/level_base.h"

#include "framework64/level.h"

typedef struct {
    LevelBase base;
    ZombieSpawner zombie_spawner;
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
