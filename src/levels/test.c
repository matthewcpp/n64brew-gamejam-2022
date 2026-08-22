#include "levels/test.h"

#include "assets/assets.h"
#include "assets/scene_Spooky_Level.h"

#include "framework64/controller_mapping/n64.h"

#define ROTATION_SPEED 90.0f

static fw64Scene* load_scene(TestLevel* level);
static void init_weapon_pickups(TestLevel* level, fw64Scene* scene);

void test_level_init(TestLevel* level, fw64Engine* engine, GameData* game_data, fw64Allocator* state_allocator) {
    level_base_init(&level->base, engine, game_data, state_allocator);
    fw64Scene* scene = load_scene(level);
    pickups_add_from_scene(&level->base.pickups, scene);

    zombie_spawner_init(&level->zombie_spawner, engine, &level->base.level, &level->base.player.movement.camera->node->transform, level->base.allocator);
    zombie_spawner_add_node(&level->zombie_spawner, fw64_scene_get_node(scene, FW64_scene_Spooky_Level_node_Zombie_Spawn));
    fw64Node* player_spawn_node = fw64_scene_get_node(scene, FW64_scene_Spooky_Level_node_Player_Spawn);
    player_set_position(&level->base.player, &player_spawn_node->transform.position);

    player_add_ammo(&level->base.player, WEAPON_TYPE_1911, 90);
    player_set_weapon(&level->base.player, WEAPON_TYPE_1911);

    fw64RenderPass* renderpass = level->base.renderpasses[RENDER_PASS_LEVEL];
    fw64_renderpass_set_clear_color(renderpass, 20, 4, 40);
    fw64_renderpass_set_fog_color(renderpass, 20, 4, 40);
    fw64_renderpass_set_fog_positions(renderpass, 0.8, 1.0f);
    fw64_renderpass_set_fog_enabled(renderpass, 1);
}

static fw64Scene* load_scene(TestLevel* level) {
    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);

    info.scene_id = FW64_ASSET_scene_Spooky_Level;
    info.allocator = level->base.allocator;
    return fw64_level_load_chunk(&level->base.level, &info)->scene;
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

    fw64RenderPass* renderpass = level->base.renderpasses[RENDER_PASS_LEVEL];
    fw64_renderpass_begin(renderpass);
    player_draw(&level->base.player, renderpass);
    pickups_draw(&level->base.pickups, renderpass);
    zombie_spawner_draw(&level->zombie_spawner, renderpass);
    fw64_renderpass_end(renderpass);

    renderpass = level->base.renderpasses[RENDER_PASS_PLAYER_WEAPON];
    fw64_renderpass_begin(renderpass);
    player_draw_weapon(&level->base.player, renderpass);
    player_draw_damage(&level->base.player, renderpass);
    fw64_renderpass_end(renderpass);

    renderpass = level->base.renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    ui_draw(&level->base.ui);
    fw64_renderpass_end(renderpass);
}
