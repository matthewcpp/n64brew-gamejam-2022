#pragma once

#include "framework64/engine.h"
#include "framework64/node.h"

#include "level.h"
#include "zombie.h"
#include "zombie_logic.h"

#define ZOMBIE_SPAWNER_MAX_COUNT 8 // might be very optimistic
#define ZOMBIE_SPAWNER_SMALL_GROUP 5
#define ZOMBIE_SPAWNER_BIG_GROUP 20

#define ZOMBIE_SPAWNER_SPAWN_LOCATION_COUNT 8

typedef struct {
    fw64Engine* engine;
    fw64Level* level;
    fw64Allocator* allocator;

    /// These are nodes that the zombie spawner will spawn zombies in from
    fw64Node* spawn_locations[ZOMBIE_SPAWNER_SPAWN_LOCATION_COUNT];
    int active_nodes;
    fw64Transform* target;
    Zombie zombies[ZOMBIE_SPAWNER_MAX_COUNT];
    unsigned int zombie_slot_active; // bitset. todo: make a proper clone procedure instead
    int active_zombies;
    fw64SkinnedMesh* zombie_mesh;
} ZombieSpawner;

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target, fw64Allocator* allocator);
void zombie_spawner_uninit(ZombieSpawner* spawner);
void zombie_spawner_update(ZombieSpawner* spawner);
void zombie_spawner_draw(ZombieSpawner* spawner, fw64RenderPass* renderpass);
void zombie_spawner_add_node(ZombieSpawner* spawner, fw64Node* node); // add new spawn point
void zombie_spawner_remove_node(ZombieSpawner* spawner, fw64Node* node);
void zombie_spawner_spawn_now(ZombieSpawner* spawner, uint8_t number_to_spawn);