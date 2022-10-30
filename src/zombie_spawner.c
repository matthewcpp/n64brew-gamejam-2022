#include "zombie_spawner.h"
#include "framework64/random.h"

#include "assert.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

static void spawn_next_zombie(ZombieSpawner* spawner, Vec3* spawn_position);
static int get_free_slot(ZombieSpawner* spawner);
static void set_free_slot(ZombieSpawner* spawner, int slot);

void zombie_spawner_init(ZombieSpawner* spawner, fw64Engine* engine, fw64Level* level, fw64Transform* target, fw64Allocator* allocator) {
    spawner->engine = engine;
    spawner->level = level;
    spawner->target = target;
    spawner->allocator = allocator;
    spawner->active_zombies = 0;
    spawner->zombie_slot_active = 0;
    spawner->active_nodes = 0;

    spawner->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_zombie, allocator);
    spawner->zombie_mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_zombie, allocator);

    for(int i = 0; i < 16; i++)
        spawner->spawner_nodes[i] = NULL;

    for (int i = 0; i < ZOMBIE_SPAWNER_MAX_COUNT; i++) {
        zombie_init(&spawner->zombies[i], spawner->engine, spawner->level, spawner->zombie_mesh, spawner->animation_data, spawner->allocator);
    }
}

void zombie_spawner_uninit(ZombieSpawner* spawner) {
    for(int i = 0; i < ZOMBIE_SPAWNER_MAX_COUNT; i++) {
        zombie_uninit(&spawner->zombies[i], spawner->allocator);
    }

    fw64_mesh_delete(spawner->engine->assets, spawner->zombie_mesh, spawner->allocator);
    fw64_animation_data_delete(spawner->animation_data, spawner->allocator);
}

void zombie_spawner_add_node(ZombieSpawner* spawner, fw64Node* node) {
    if(spawner->active_nodes >= 15)
        return;
    spawner->spawner_nodes[spawner->active_nodes] = node;
    spawner->active_nodes++;
}

void zombie_spawner_remove_node(ZombieSpawner* spawner, fw64Node* node) {
    if(spawner->active_nodes == 0)
        return;
    
    for(int i = 0; i < spawner->active_nodes; i++) {
        if(spawner->spawner_nodes[i] == node) {
            spawner->spawner_nodes[i] = spawner->spawner_nodes[spawner->active_nodes - 1];
            spawner->spawner_nodes[spawner->active_nodes - 1] = 0;
            spawner->active_nodes--;
            return;
        }
    }
}

void zombie_spawner_spawn_now(ZombieSpawner* spawner, uint8_t number_to_spawn) {
    if(spawner->active_nodes < 1)
        return;

    for(int i = 0; i < number_to_spawn; i++) {
        int node_index = fw64_random_int_in_range(0, spawner->active_nodes - 1);
        Vec3* pos = &spawner->spawner_nodes[node_index]->transform.position;
        spawn_next_zombie(spawner, pos);
    }
}

void zombie_spawner_spawn_at_pos(ZombieSpawner* spawner, uint8_t number_to_spawn, Vec3* pos) {
    for(int i = 0; i < number_to_spawn; i++) {
        spawn_next_zombie(spawner, pos);
    }
}

void spawn_next_zombie(ZombieSpawner* spawner, Vec3* spawn_position) {
    if(spawner->active_zombies >= ZOMBIE_SPAWNER_BIG_GROUP)
        return;
    
    int next_slot = get_free_slot(spawner);
    if(next_slot < 0)
        return;
    Zombie* zed = &spawner->zombies[next_slot];
    spawner->active_zombies++;

    zombie_reset(zed);
    fw64_level_add_dynamic_node(zed->level, &zed->node);
    zombie_ai_init(&zed->ai, zed->level, &zed->collider, &zed->node.transform, spawner->target);

    zed->health = 3;
    zed->node.transform.position = *spawn_position;
    float radius = 15.0f; //10.0f + (2.0f * spawner->active_zombies);
    Vec3 random_offset = {fw64_random_float_in_range(-radius,radius), spawn_position->y, fw64_random_float_in_range(-radius,radius)};
    vec3_add(&zed->node.transform.position, &zed->node.transform.position, &random_offset);
    zed->rotation = fw64_random_float_in_range(0.0f, 359.9f);
    quat_from_euler(&zed->node.transform.rotation, 0.0f, zed->rotation, 0.0f);
    zombie_set_to_ground_height(zed);
    fw64_node_update(&zed->node);
    
    zombie_set_new_state(zed, ZOMBIE_STATE_IDLE);
    zed->animation_controller.current_time = fw64_random_float();
    zed->animation_controller.speed = 0.5f + fw64_random_float();

    zombie_ai_set_logic_state(&zed->ai, ZLS_IDLE);
    zed->health = ZOMBIE_MAX_HEALTH;
}

void zombie_spawner_update(ZombieSpawner* spawner) {
    if(spawner->active_zombies < 1)
        return;

    int zombiesUpdated = 0;

    for(int i = 0; i < ZOMBIE_SPAWNER_MAX_COUNT; i++) {
        
        if(!((1<<i) & spawner->zombie_slot_active))
            continue;

        Zombie* zombie = &spawner->zombies[i];
        if(zombie_update(zombie)) {
            zombiesUpdated++;
        } else {
            set_free_slot(spawner, i);
            fw64_level_remove_dynamic_node(zombie->level, &zombie->node);
            spawner->active_zombies--;
        }
    }

    // if (zombiesUpdated < ZOMBIE_SPAWNER_SMALL_GROUP) {
    //     spawn_next_zombie(spawner);
    // }
}

void zombie_spawner_draw(ZombieSpawner* spawner) {
    if(spawner->active_zombies < 1)
        return;

    for(int i = 0; i < ZOMBIE_SPAWNER_MAX_COUNT; i++) {        
        if(!((1<<i) & spawner->zombie_slot_active))
            continue;

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
