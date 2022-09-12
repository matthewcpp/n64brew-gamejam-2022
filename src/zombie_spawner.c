#include "zombie_spawner.h"
#include "framework64/random.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

static void spawn_next_zombie(ZombieSpawner* spawner);
static int get_free_slot(ZombieSpawner* spawner);
static void set_free_slot(ZombieSpawner* spawner, int slot);

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target, fw64Allocator* allocator) {
    spawner->engine = engine;
    spawner->level = level;
    spawner->target = target;
    spawner->allocator = allocator;
    spawner->spawner_node = fw64_scene_get_node(level->scene, FW64_scene_spooky_level_node_Zombie_Spawn);

    spawner->active_zombies = 0;
    spawner->zombie_slot_active = 0;
    spawner->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_zombie, allocator);
    spawner->zombie_mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_zombie, allocator);

    for(int i = 0; i < ZOMBIE_SPAWNER_SMALL_GROUP; i++) {
        spawn_next_zombie(spawner);
    }    
}

void zombie_spawner_uninit(ZombieSpawner* spawner) {
    fw64_mesh_delete(spawner->engine->assets, spawner->zombie_mesh, spawner->allocator);
    fw64_animation_data_delete(spawner->animation_data, spawner->allocator);
}

void spawn_next_zombie(ZombieSpawner* spawner) {
    
    if(spawner->active_zombies >= ZOMBIE_SPAWNER_BIG_GROUP)
        return;
    int next_slot = get_free_slot(spawner);
    if(next_slot < 0)
        return;
    Zombie* zed = &spawner->zombies[next_slot];
    spawner->active_zombies++;
    zombie_init(zed, spawner->engine, spawner->level, spawner->zombie_mesh, spawner->animation_data);
    zombie_ai_init(&zed->ai, &zed->node.transform, spawner->target);

    zed->health = 3;
    zed->node.transform.position = spawner->spawner_node->transform.position; //spawn in more intelligent locations
    float radius = 10.0f + (2.0f * spawner->active_zombies);
    Vec3 random_offset = {fw64_random_float_in_range(-radius,radius), 0.0f, fw64_random_float_in_range(-radius,radius)};
    vec3_add(&zed->node.transform.position, &zed->node.transform.position, &random_offset);
    fw64_node_update(&zed->node);
    
    zombie_set_new_state(zed, ZOMBIE_STATE_IDLE);
    zombie_ai_set_logic_state(&zed->ai, ZLS_IDLE);
    zed->health = ZOMBIE_MAX_HEALTH;
}

void zombie_spawner_update(ZombieSpawner* spawner) {
    if(spawner->active_zombies < 1)
        return;

    int zombiesUpdated = 0;

    for(int i = 0; i < spawner->active_zombies; i++) {
        Zombie* zombie = &spawner->zombies[i];
        if(zombie_update(zombie)) {
            zombiesUpdated++;
        } else {
            set_free_slot(spawner, i);
            zombie_uninit(&spawner->zombies[i]);
            spawner->active_zombies--;
        }
    }

    if (zombiesUpdated < ZOMBIE_SPAWNER_SMALL_GROUP) {
        spawn_next_zombie(spawner);
    }
}

void zombie_spawner_draw(ZombieSpawner* spawner) {
    if(spawner->active_zombies < 1)
        return;

    for(int i = 0; i < spawner->active_zombies; i++) {
        Zombie* zombie = &spawner->zombies[i];
        zombie_draw(zombie);
    }
}

static int get_free_slot(ZombieSpawner* spawner) {
    for(int i = 0; i < ZOMBIE_SPAWNER_MAX_COUNT; i++) {
        if(!((1<<i) & (spawner->zombie_slot_active))) {
            spawner->zombie_slot_active |= (1<<i);
            return i;
        }
    }
    return -1;
}

static void set_free_slot(ZombieSpawner* spawner, int slot) {
    if(slot < 0 || slot > ZOMBIE_SPAWNER_MAX_COUNT)
        return;
    
    spawner->zombie_slot_active &= ~(1<<slot);
}