#include "church_hill.h"

#include "assets/assets.h"
#include "assets/scene_Church_Hill.h"
#include "assets/sound_bank_sounds.h"

static void setup_sound_trigger(HillLevel* hill_level, fw64Scene* scene, int trigger_box_index, int node_index);
static fw64Scene* setup_level(HillLevel* hill_level);

void hill_level_init(HillLevel* level, fw64Engine* engine, GameData* game_data, fw64Allocator* level_allocator) {
    level_base_init(&level->base, engine, game_data, level_allocator);

    fw64RenderPass* renderpass = level->base.renderpasses[RENDER_PASS_LEVEL];
    fw64_rendererpass_set_clear_color(renderpass, 20, 4, 40);
    fw64_rendererpass_set_fog_color(renderpass, 20, 4, 40);
    fw64_rendererpass_set_fog_positions(renderpass, 0.8, 1.0f);
    fw64_renderpass_set_fog_enabled(renderpass, 1);

    fw64Scene* scene = setup_level(level);
    setup_sound_trigger(level, scene, HILL_LEVEL_TRIGGER_CROW, FW64_scene_Church_Hill_node_CrowTrigger);
    setup_sound_trigger(level, scene, HILL_LEVEL_TRIGGER_HOWL, FW64_scene_Church_Hill_node_HowlTrigger);

    fw64Node* start_node = fw64_scene_get_node(scene, FW64_scene_Church_Hill_node_Player_Start);
    player_set_position(&level->base.player, &start_node->transform.position);
    player_add_ammo(&level->base.player, WEAPON_TYPE_MP5, 320);
    player_set_weapon(&level->base.player, WEAPON_TYPE_MP5);

    zombie_spawner_init(&level->zombie_spawner, engine, &level->base.level, &level->base.player.movement.camera->node->transform, level->base.allocator);
    zombie_spawner_add_node(&level->zombie_spawner,fw64_scene_get_node(scene, FW64_scene_Church_Hill_node_Zombie_Spawn_1));
    zombie_spawner_add_node(&level->zombie_spawner,fw64_scene_get_node(scene, FW64_scene_Church_Hill_node_Zombie_Spawn_2));
}

fw64Scene* setup_level(HillLevel* level) {
    fw64LevelChunkInfo info;
    fw64_level_chunk_info_init(&info);
    info.scene_id = FW64_ASSET_scene_Church_Hill;
    info.allocator = level->base.allocator;
    return fw64_level_load_chunk(&level->base.level, &info)->scene;
}

void hill_level_uninit(HillLevel* level) {
    level_base_uninit(&level->base);
    zombie_spawner_uninit(&level->zombie_spawner);
}

static void play_triggered_audio_sound(TriggerBox* trigger_box, void* arg) {
    HillLevel* level = (HillLevel*)arg;

    if (trigger_box == &level->triggers[HILL_LEVEL_TRIGGER_CROW])
        audio_controller_play(&level->base.audio_controller, AUDIO_CONTROLLER_ENVIRONMENT, sound_bank_sounds_crow);
    else
        audio_controller_play(&level->base.audio_controller, AUDIO_CONTROLLER_ENVIRONMENT, sound_bank_sounds_werewolf_howl);
}

void setup_sound_trigger(HillLevel* level, fw64Scene* scene, int trigger_box_index, int node_index) {
    TriggerBox* trigger_box = &level->triggers[trigger_box_index];
    fw64Node* node = fw64_scene_get_node(scene, node_index);

    trigger_box_init_from_node(trigger_box, node);
    trigger_box_set_target(trigger_box, level->base.player.node, 1);
    trigger_box_set_func(trigger_box, play_triggered_audio_sound, level);
}

void hill_level_update(HillLevel* level) {
    level_base_update(&level->base);
    zombie_spawner_update(&level->zombie_spawner);

    for (int i = 0; i < HILL_LEVEL_TRIGGER_COUNT; i++) {
        trigger_box_update(&level->triggers[i]);
    }
}

void hill_level_draw(HillLevel* level) {
    fw64RenderPass* renderpass = level->base.renderpasses[RENDER_PASS_LEVEL];
    fw64_renderpass_begin(renderpass);
    player_draw(&level->base.player, renderpass);
    zombie_spawner_draw(&level->zombie_spawner, renderpass);
    fw64_renderpass_end(renderpass);

    renderpass = level->base.renderpasses[RENDER_PASS_PLAYER_WEAPON];
    fw64_renderpass_begin(renderpass);
    player_draw_weapon(&level->base.player, renderpass);
    fw64_renderpass_end(renderpass);

    // if (level->base.player.damage_overlay_time > 0.0f){
    //     fw64_renderer_util_fullscreen_overlay(renderer, 165, 0, 0, 100);
    // }
        

    renderpass = level->base.renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    ui_draw(&level->base.ui);
    fw64_renderpass_end(renderpass);
}