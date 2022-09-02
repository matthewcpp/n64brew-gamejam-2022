#include "game.h"
#include "assets.h"
#include "scene_spooky_level.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->scene = fw64_scene_load(engine->assets, FW64_ASSET_scene_spooky_level, NULL);

    player_init(&game->player, engine, game->scene);
    player_set_weapon(&game->player, WEAPON_TYPE_AR15);

    ui_init(&game->ui, engine, &game->player);

    boo_init(&game->boo, engine, &game->player, game->scene);

    fw64Renderer* renderer = game->engine->renderer;
    fw64_renderer_set_clear_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_color(renderer, 20, 4, 40);
    fw64_renderer_set_fog_positions(renderer, 0.45, 0.9f);

    fw64MusicBank* music = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_music, NULL);
    fw64_audio_set_music_bank(engine->audio, music);

    fw64SoundBank* sound = fw64_sound_bank_load(engine->assets, FW64_ASSET_soundbank_sounds, NULL);
    fw64_audio_set_sound_bank(engine->audio, sound);

    fw64AnimationData* animation_data = fw64_animation_data_load(game->engine->assets, FW64_ASSET_animation_data_zombie, NULL);
    fw64Mesh* zombie_mesh = fw64_mesh_load(game->engine->assets, FW64_ASSET_mesh_zombie, NULL);
    fw64Node* zombie_node = fw64_scene_get_node(game->scene, FW64_scene_spooky_level_node_Zombie_Spawn);

    zombie_init(&game->zombie, engine, zombie_node, zombie_mesh, animation_data);
    zombie_set_target(&game->zombie, &game->player.camera.camera.transform);
}

void game_update(Game* game){
    //if (fw64_audio_get_music_status(game->engine->audio) == FW64_AUDIO_STOPPED)
    //    fw64_audio_play_music(game->engine->audio, 0);

    player_update(&game->player);
    boo_update(&game->boo);
    zombie_update(&game->zombie);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    player_draw(&game->player);
    zombie_draw(&game->zombie);
    player_draw_weapon(&game->player);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    ui_draw(&game->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
