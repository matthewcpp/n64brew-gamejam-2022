#pragma once

#include "framework64/level.h"
#include "player.h"
#include "zombie_spawner.h"

#define MAX_SPAWN_NODE_COUNT 4
#define INFOS_CAPACITY 9

typedef struct {
    fw64LevelChunckRef* ref;
    fw64Node* nodes[MAX_SPAWN_NODE_COUNT];
    int spawned[MAX_SPAWN_NODE_COUNT];
    uint32_t count;
} ZombieSpawnInfo;

typedef struct {
    Player* player;
    ZombieSpawner* spawner;
    ZombieSpawnInfo infos[INFOS_CAPACITY];
    uint32_t count;
} ZombieControl;

void zombie_control_init(ZombieControl* control, Player* player, ZombieSpawner* spawner);
ZombieSpawnInfo* zombie_control_get_info(ZombieControl* control, Vec3* point);
void zombie_control_update(ZombieControl* control);
void zombie_control_add_ref(ZombieControl* control, fw64LevelChunckRef* ref);
void zombie_control_remove_ref(ZombieControl* control, fw64LevelChunckRef* ref);