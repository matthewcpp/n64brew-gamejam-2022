#include "levels/test.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

static fw64Scene* load_scene(TestLevel* level);
static void init_weapon_pickups(TestLevel* level, fw64Scene* scene);

void test_level_init(TestLevel* level, fw64Engine* engine, GameData* game_data, fw64Allocator* state_allocator) {
    level_base_init(&level->base, engine, game_data, state_allocator);
    fw64Scene* scene = load_scene(level);
    pickups_add_from_scene(&level->base.pickups, scene);

    zombie_spawner_init(&level->zombie_spawner, engine, &level->base.level, &level->base.player.movement.camera.transform, level->base.allocator);
    zombie_spawner_add_node(&level->zombie_spawner, fw64_scene_get_node(scene, FW64_scene_spooky_level_node_Zombie_Spawn));
    fw64Node* player_spawn_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_Player_Spawn);
    player_set_position(&level->base.player, &player_spawn_node->transform.position);

    player_add_ammo(&level->base.player, WEAPON_TYPE_HANDGUN, 90);
    player_set_weapon(&level->base.player, WEAPON_TYPE_HANDGUN);

    fw64Renderer* renderer = engine->renderer;
    fw64_renderer_set_clear_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(renderer, 0.8, 1.0f);
}

static fw64Scene* load_scene(TestLevel* level) {
    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);

    info.scene_id = FW64_ASSET_scene_spooky_level;
    info.allocator = level->base.allocator;
    uint32_t chunk_handle = fw64_level_load_chunk(&level->base.level, &info);
    return fw64_level_get_chunk_by_handle(&level->base.level, chunk_handle);
}

void test_level_uninit(TestLevel* level) {
    level_base_uninit(&level->base);
    zombie_spawner_uninit(&level->zombie_spawner);
}

void test_level_update(TestLevel* level){
    level_base_update(&level->base);

    if (level->zombie_spawner.active_zombies < 5) {
        zombie_spawner_spawn_now(&level->zombie_spawner, 5 - level->zombie_spawner.active_zombies);
    }

    zombie_spawner_update(&level->zombie_spawner);
}

void test_level_draw(TestLevel* level) {
    fw64Renderer* renderer = level->base.engine->renderer;

    fw64_renderer_set_fog_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->base.player);
    pickups_draw(&level->base.pickups);
    zombie_spawner_draw(&level->zombie_spawner);    
    fw64_renderer_set_fog_enabled(renderer, 0);
    player_draw_weapon(&level->base.player);
    player_draw_damage(&level->base.player);
    ui_draw(&level->base.ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
