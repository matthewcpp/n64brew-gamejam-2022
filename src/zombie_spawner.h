#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

#include "framework64/level.h"
#include "zombie.h"
#include "zombie_logic.h"

#define ZOMBIE_SPAWNER_MAX_COUNT 32 // might be very optimistic
#define ZOMBIE_SPAWNER_SMALL_GROUP 5
#define ZOMBIE_SPAWNER_BIG_GROUP 20

typedef struct {
    fw64Engine* engine;
    fw64Level* level;
    fw64Allocator* allocator;
    fw64Node* spawner_nodes[16]; // random guess. 16 active spawn points seems like plenty?
    int active_nodes;
    fw64Transform* target;
    Zombie zombies[ZOMBIE_SPAWNER_MAX_COUNT];
    unsigned int zombie_slot_active; // bitset. todo: make a proper clone procedure instead
    int active_zombies;
    fw64Mesh* zombie_mesh;
    fw64AnimationData* animation_data;
} ZombieSpawner;

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target, fw64Allocator* allocator);
void zombie_spawner_uninit(ZombieSpawner* spawner);
void zombie_spawner_update(ZombieSpawner* spawner);
void zombie_spawner_draw(ZombieSpawner* spawner);
void zombie_spawner_add_node(ZombieSpawner* spawner, fw64Node* node); // add new spawn point
void zombie_spawner_remove_node(ZombieSpawner* spawner, fw64Node* node);