#include "hill_level.h"

#include "assets/assets.h"
#include "assets/scene_church_hill.h"
#include "assets/sound_bank_sounds.h"

static void setup_sound_trigger(HillLevel* hill_level, fw64Scene* scene, int trigger_box_index, int node_index);

void hill_level_init(HillLevel* hill_level, fw64Engine* engine) {
    hill_level->engine = engine;

    fw64_renderer_set_clear_color(engine->renderer, 20, 4, 40);

    fw64_level_init(&hill_level->level, engine);
    fw64Scene* scene = fw64_level_load_chunk(&hill_level->level, engine->assets, FW64_ASSET_scene_church_hill, NULL);
    player_init(&hill_level->player, engine, &hill_level->level, fw64_default_allocator());
    fw64_level_add_dyanmic_node(&hill_level->level, hill_level->player.node);

    fw64Node* start_node = fw64_scene_get_node(scene, FW64_scene_church_hill_node_Player_Start);
    player_set_position(&hill_level->player, &start_node->transform.position);
    player_set_weapon(&hill_level->player, WEAPON_TYPE_AR15);
    
    ui_init(&hill_level->ui, engine, &hill_level->player);

    fw64SoundBank* sound = fw64_sound_bank_load(engine->assets, FW64_ASSET_soundbank_sounds, NULL);
    fw64_audio_set_sound_bank(engine->audio, sound);

    setup_sound_trigger(hill_level, scene, HILL_LEVEL_TRIGGER_CROW, FW64_scene_church_hill_node_CrowTrigger);
    setup_sound_trigger(hill_level, scene, HILL_LEVEL_TRIGGER_HOWL, FW64_scene_church_hill_node_HowlTrigger);
}

static void play_triggered_audio_sound(TriggerBox* trigger_box, void* arg) {
    HillLevel* hill_level = (HillLevel*)arg;

    if (trigger_box == &hill_level->triggers[HILL_LEVEL_TRIGGER_CROW])
        fw64_audio_play_sound(hill_level->engine->audio, sound_bank_sounds_crow);
    else
        fw64_audio_play_sound(hill_level->engine->audio, sound_bank_sounds_werewolf_howl);
}

void setup_sound_trigger(HillLevel* hill_level, fw64Scene* scene, int trigger_box_index, int node_index) {
    TriggerBox* trigger_box = &hill_level->triggers[trigger_box_index];
    fw64Node* node = fw64_scene_get_node(scene, node_index);

    trigger_box_init_from_node(trigger_box, node);
    trigger_box_set_target(trigger_box, hill_level->player.node, 1);
    trigger_box_set_func(trigger_box, play_triggered_audio_sound, hill_level);
}

void hill_level_update(HillLevel* hill_level) {
    player_update(&hill_level->player);

    for (int i = 0; i < HILL_LEVEL_TRIGGER_COUNT; i++) {
        trigger_box_update(&hill_level->triggers[i]);
    }
}

void hill_level_draw(HillLevel* hill_level) {
    fw64Renderer* renderer = hill_level->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&hill_level->player);
    player_draw_weapon(&hill_level->player);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    ui_draw(&hill_level->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}