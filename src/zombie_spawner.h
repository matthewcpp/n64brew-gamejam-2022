#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

#include "level.h"
#include "zombie.h"
#include "zombie_logic.h"

#define ZOMBIE_SPAWNER_COUNT 2

typedef struct {
    fw64Engine* engine;
    fw64Level* level;
    fw64Allocator* allocator;
    fw64Node* spawner_node;
    Zombie zombies[ZOMBIE_SPAWNER_COUNT];
    int next_index;
    fw64Mesh* zombie_mesh;
    fw64AnimationData* animation_data;
} ZombieSpawner;

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target, fw64Allocator* allocator);
void zombie_spawner_uninit(ZombieSpawner* spawner);
void zombie_spawner_update(ZombieSpawner* spawner);
void zombie_spawner_draw(ZombieSpawner* spawner);