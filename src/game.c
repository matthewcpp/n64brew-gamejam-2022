#include "game.h"
#include "assets.h"
#include "scene_spooky_level.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_level_init(&game->level);
    game->level.scene = fw64_scene_load(engine->assets, FW64_ASSET_scene_spooky_level, NULL);

    player_init(&game->player, engine, &game->level);
    player_set_weapon(&game->player, WEAPON_TYPE_AR15);

    zombie_spawner_init(&game->zombie_spawner, engine, &game->level, &game->player.camera.camera.transform);

    ui_init(&game->ui, engine, &game->player);

    boo_init(&game->boo, engine, &game->player, game->level.scene);

    fw64Renderer* renderer = game->engine->renderer;
    fw64_renderer_set_clear_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(renderer, 0.45, 0.9f);

    fw64MusicBank* music = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_music, NULL);
    fw64_audio_set_music_bank(engine->audio, music);

    fw64SoundBank* sound = fw64_sound_bank_load(engine->assets, FW64_ASSET_soundbank_sounds, NULL);
    fw64_audio_set_sound_bank(engine->audio, sound);
}

void game_update(Game* game){
    //if (fw64_audio_get_music_status(game->engine->audio) == FW64_AUDIO_STOPPED)
    //    fw64_audio_play_music(game->engine->audio, 0);

    player_update(&game->player);
    //boo_update(&game->boo);
    zombie_spawner_update(&game->zombie_spawner);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&game->player);
    zombie_spawner_draw(&game->zombie_spawner);
    player_draw_weapon(&game->player);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    ui_draw(&game->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
