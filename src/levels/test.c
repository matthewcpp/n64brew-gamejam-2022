#include "levels/test.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

#include "framework64/n64/controller_button.h"

#include "framework64/util/renderer_util.h"

#define ROTATION_SPEED 90.0f

static fw64Scene* setup_level(TestLevel* level);
static void init_weapon_pickups(TestLevel* level, fw64Scene* scene);

void test_level_init(TestLevel* level, fw64Engine* engine) {
    level_base_init(&level->base, engine, fw64_default_allocator(), FW64_ASSET_musicbank_music, FW64_ASSET_soundbank_sounds);

    fw64Scene* scene = setup_level(level);
    init_weapon_pickups(level, scene);

    zombie_spawner_init(&level->zombie_spawner, engine, &level->base.level, FW64_scene_spooky_level_node_Zombie_Spawn, &level->base.player.movement.camera.transform, level->base.allocator);

    fw64Renderer* renderer = engine->renderer;
    fw64_renderer_set_clear_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(renderer, 0.45, 0.9f);
}


// todo move this into weapon pickups system
void init_weapon_pickups(TestLevel* level, fw64Scene* scene) {
    fw64Engine* engine = level->base.engine;
    fw64Allocator* allocator = level->base.allocator;
    WeaponPickups* weapon_pickups = &level->base.weapon_pickups;

    fw64Mesh* shotgun_pickup = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_shotgun_pickup, allocator);
    fw64Mesh* uzi_pickup = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_uzi_pickup, allocator);
    fw64Mesh* ar15_pickup = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_ar15_pickup, allocator);

    fw64Node* shotgun_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_z_shotgun_ammo_spawn);
    fw64_node_set_mesh(shotgun_node, shotgun_pickup);

    fw64Node* uzi_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_z_uzi_ammo_spawn);
    fw64_node_set_mesh(uzi_node, uzi_pickup);

    fw64Node* ar15_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_z_ar15_ammo_spawn);
    fw64_node_set_mesh(ar15_node, ar15_pickup);

    weapon_pickups_add(weapon_pickups, WEAPON_TYPE_AR15, 180, ar15_node);
    weapon_pickups_add(weapon_pickups, WEAPON_TYPE_SHOTGUN, 40, shotgun_node);
    weapon_pickups_add(weapon_pickups, WEAPON_TYPE_UZI, 320, uzi_node);
}

static fw64Scene* setup_level(TestLevel* level) {
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
    zombie_spawner_update(&level->zombie_spawner);
}

void test_level_draw(TestLevel* level) {
    weapon_pickups_draw(&level->base.weapon_pickups);
    fw64Renderer* renderer = level->base.engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->base.player);
    zombie_spawner_draw(&level->zombie_spawner);
    
    player_draw_weapon(&level->base.player);
    
    if (level->base.player.damage_overlay_time > 0.0f)
        fw64_renderer_util_fullscreen_overlay(renderer, 165, 0, 0, 100);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    ui_draw(&level->base.ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
