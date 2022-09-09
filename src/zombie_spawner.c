#include "zombie_spawner.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

static void spawn_next_zombie(ZombieSpawner* spawner);

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target, fw64Allocator* allocator) {
    spawner->engine = engine;
    spawner->level = level;
    spawner->allocator = allocator;
    spawner->spawner_node = fw64_scene_get_node(level->scene, FW64_scene_spooky_level_node_Zombie_Spawn);
    spawner->next_index = 0;

    spawner->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_zombie, allocator);
    spawner->zombie_mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_zombie, allocator);

    for (int i = 0; i < ZOMBIE_SPAWNER_COUNT; i++) {
        Zombie* zombie = &spawner->zombies[i];
        zombie_init(zombie, engine, level, spawner->zombie_mesh, spawner->animation_data);
        zombie_set_target(zombie, target);
    }

    spawn_next_zombie(spawner);
}

void zombie_spawner_uninit(ZombieSpawner* spawner) {
    fw64_mesh_delete(spawner->engine->assets, spawner->zombie_mesh, spawner->allocator);
    fw64_animation_data_delete(spawner->animation_data, spawner->allocator);
}

void spawn_next_zombie(ZombieSpawner* spawner) {
    Zombie* zombie = &spawner->zombies[spawner->next_index];
    zombie->health = 3;
    zombie->node.transform.position = spawner->spawner_node->transform.position;
    fw64_node_update(&zombie->node);
    zombie_set_new_state(zombie, spawner->next_index == 0 ? ZOMBIE_STATE_WALKING : ZOMBIE_STATE_RUNNING);
    zombie->health = ZOMBIE_MAX_HEALTH;
    spawner->next_index = spawner->next_index == 0 ? 1 : 0;
}

void zombie_spawner_update(ZombieSpawner* spawner) {
    int need_to_spawn = 1;

    for (int i = 0; i < ZOMBIE_SPAWNER_COUNT; i++) {
        Zombie* zombie = &spawner->zombies[i];
        zombie_update(zombie);

        if (zombie->state > ZOMBIE_STATE_DEAD)
            need_to_spawn = 0;
    }

    if (need_to_spawn)
        spawn_next_zombie(spawner);
}

void zombie_spawner_draw(ZombieSpawner* spawner) {
    for (int i = 0; i < ZOMBIE_SPAWNER_COUNT; i++) {
        Zombie* zombie = &spawner->zombies[i];

        if (zombie->state == ZOMBIE_STATE_INACTIVE)
            continue;

        zombie_draw(zombie);
    }
}