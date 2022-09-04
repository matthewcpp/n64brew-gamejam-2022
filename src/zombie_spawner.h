#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

#include "level.h"
#include "zombie.h"

#define ZOMBIE_SPAWNER_COUNT 2

typedef struct {
    fw64Engine* engine;
    fw64Level* level;
    fw64Node* spawner_node;
    Zombie zombies[ZOMBIE_SPAWNER_COUNT];
    int next_index;
} ZombieSpawner;

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target);
void zombie_spawner_update(ZombieSpawner* spawner);
void zombie_spawner_draw(ZombieSpawner* spawner);