#include "levels/test.h"

#include "assets/assets.h"
#include "assets/scene_spooky_level.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

void test_level_init(TestLevel* level, fw64Engine* engine) {
    level->engine = engine;

    // TODO: need to figure out what to do here
    level->allocator = fw64_default_allocator();

    fw64_level_init(&level->level, engine);
    fw64_level_load_chunk(&level->level, engine->assets, FW64_ASSET_scene_spooky_level, level->allocator);

    player_init(&level->player, engine, &level->level, level->allocator);
    player_set_weapon(&level->player, WEAPON_TYPE_AR15);

    zombie_spawner_init(&level->zombie_spawner, engine, &level->level, &level->player.movement.camera.transform, level->allocator);

    ui_init(&level->ui, engine, &level->player);

    fw64Renderer* renderer = level->engine->renderer;
    fw64_renderer_set_clear_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(renderer, 0.45, 0.9f);

    level->music = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_music, level->allocator);
    fw64_audio_set_music_bank(engine->audio, level->music);

    level->sound = fw64_sound_bank_load(engine->assets, FW64_ASSET_soundbank_sounds, level->allocator);
    fw64_audio_set_sound_bank(engine->audio, level->sound);
}

void test_level_uninit(TestLevel* level) {
    player_uninit(&level->player);
    zombie_spawner_uninit(&level->zombie_spawner);
    fw64_level_delete(&level->level);
    
    fw64_sound_bank_delete(level->engine->assets, level->sound, level->allocator);
    fw64_music_bank_delete(level->engine->assets, level->music, level->allocator);
    ui_uninit(&level->ui);
}

void test_level_update(TestLevel* level){
    //if (fw64_audio_get_music_status(level->engine->audio) == FW64_AUDIO_STOPPED)
    //    fw64_audio_play_music(level->engine->audio, 0);

    player_update(&level->player);
    zombie_spawner_update(&level->zombie_spawner);
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