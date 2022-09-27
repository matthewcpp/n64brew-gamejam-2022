#include "levels/test.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

static fw64Scene* setup_level(TestLevel* level);

void test_level_init(TestLevel* level, fw64Engine* engine) {
    level->engine = engine;

    // TODO: need to figure out what to do here
    level->allocator = fw64_default_allocator();

    setup_level(level);

    projectile_controller_init(&level->projectile_controller, &level->level);

    player_init(&level->player, engine, &level->level, &level->projectile_controller, level->allocator);

    zombie_spawner_init(&level->zombie_spawner, engine, &level->level, FW64_scene_spooky_level_node_Zombie_Spawn, &level->player.movement.camera.transform, level->allocator);

    ui_init(&level->ui, engine, level->allocator, &level->player);

    fw64Renderer* renderer = level->engine->renderer;
    fw64_renderer_set_clear_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(renderer, 0.45, 0.9f);

    level->music = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_music, level->allocator);
    fw64_audio_set_music_bank(engine->audio, level->music);

    level->sound = fw64_sound_bank_load(engine->assets, FW64_ASSET_soundbank_sounds, level->allocator);
    fw64_audio_set_sound_bank(engine->audio, level->sound);
}

static fw64Scene* setup_level(TestLevel* hill_level) {
    fw64_level_init(&hill_level->level, hill_level->engine);
    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);
    info.scene_id = FW64_ASSET_scene_spooky_level;
    info.allocator = hill_level->allocator;
    uint32_t chunk_handle = fw64_level_load_chunk(&hill_level->level, &info);
    fw64Scene* scene = fw64_level_get_chunk_by_handle(&hill_level->level, chunk_handle);

    fw64Mesh* shotgun_pickup = fw64_mesh_load(hill_level->engine->assets, FW64_ASSET_mesh_shotgun_pickup, hill_level->allocator);
    fw64Mesh* uzi_pickup = fw64_mesh_load(hill_level->engine->assets, FW64_ASSET_mesh_uzi_pickup, hill_level->allocator);
    fw64Mesh* ar15_pickup = fw64_mesh_load(hill_level->engine->assets, FW64_ASSET_mesh_ar15_pickup, hill_level->allocator);

    fw64Node* shotgun_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_shotgun_ammo_spawn);
    fw64_node_set_mesh(shotgun_node, shotgun_pickup);

    fw64Node* uzi_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_uzi_ammo_spawn);
    fw64_node_set_mesh(uzi_node, uzi_pickup);

    fw64Node* ar15_node = fw64_scene_get_node(scene, FW64_scene_spooky_level_node_ar15_ammo_spawn);
    fw64_node_set_mesh(ar15_node, ar15_pickup);

    weapon_pickups_init(&hill_level->weapon_pickups, &hill_level->player);
    weapon_pickups_add(&hill_level->weapon_pickups, WEAPON_TYPE_AR15, 180, ar15_node);
    weapon_pickups_add(&hill_level->weapon_pickups, WEAPON_TYPE_SHOTGUN, 40, shotgun_node);
    weapon_pickups_add(&hill_level->weapon_pickups, WEAPON_TYPE_UZI, 320, uzi_node);
    return scene;
}

void test_level_uninit(TestLevel* level) {
    player_uninit(&level->player);
    zombie_spawner_uninit(&level->zombie_spawner);
    fw64_level_uninit(&level->level);
    
    fw64_sound_bank_delete(level->engine->assets, level->sound, level->allocator);
    fw64_music_bank_delete(level->engine->assets, level->music, level->allocator);
    ui_uninit(&level->ui);
}

void test_level_update(TestLevel* level){
    //if (fw64_audio_get_music_status(level->engine->audio) == FW64_AUDIO_STOPPED)
    //    fw64_audio_play_music(level->engine->audio, 0);

    player_update(&level->player);
    zombie_spawner_update(&level->zombie_spawner);
    weapon_pickups_update(&level->weapon_pickups);
}

void test_level_draw(TestLevel* level) {
    fw64Renderer* renderer = level->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&level->player);
    zombie_spawner_draw(&level->zombie_spawner);
    player_draw_weapon(&level->player);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    ui_draw(&level->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
