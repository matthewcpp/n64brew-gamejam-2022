#include "game.h"

#include "weapon.h"

#include "assets/assets.h"

static void game_transition_state(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game_data_init(&game->game_data);
    init_weapon_info();

    game->current_state = GAME_STATE_NONE;

    // Set this to the state you want to being at
    game->game_data.transition_to_state = GAME_STATE_SPLASH;

    // you can optionally set this to the level you want to begin at
    game->game_data.transition_to_level = LEVEL_NONE;

    fw64SoundBank* sound_bank = fw64_sound_bank_load(engine->assets, FW64_ASSET_soundbank_sounds, fw64_default_allocator());
    fw64MusicBank* music_bank = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_music, fw64_default_allocator());

    fw64_audio_set_sound_bank(engine->audio, sound_bank);
    fw64_audio_set_music_bank(engine->audio, music_bank);

    game_transition_state(game);
}

void game_update(Game* game) {
    if (game->game_data.transition_to_state) {
        game_transition_state(game);
    }

    switch(game->current_state) {
        case GAME_STATE_MENU:
            game_state_menu_update(&game->states.menu);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_update(&game->states.playing);
            break;

        case GAME_STATE_SPLASH:
            game_state_splash_update(&game->states.splash);
            break;

        case GAME_STATE_GAME_OVER:
            game_state_game_over_update(&game->states.game_over);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }
}

void game_draw(Game* game) {
    switch(game->current_state) {
        case GAME_STATE_MENU:
            game_state_menu_draw(&game->states.menu);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_draw(&game->states.playing);
            break;

        case GAME_STATE_SPLASH:
            game_state_splash_draw(&game->states.splash);
            break;

        case GAME_STATE_GAME_OVER:
            game_state_game_over_draw(&game->states.game_over);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }
}

void game_transition_state(Game* game) {
    switch(game->current_state) {
        case GAME_STATE_MENU:
            game_state_menu_uninit(&game->states.menu);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_uninit(&game->states.playing);
            break;

        case GAME_STATE_SPLASH:
            game_state_splash_uninit(&game->states.splash);
            break;

        case GAME_STATE_GAME_OVER:
            game_state_game_over_uninit(&game->states.game_over);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }

    game->current_state = game->game_data.transition_to_state;
    game->game_data.transition_to_state = GAME_STATE_NONE;

    switch(game->current_state) {
        case GAME_STATE_MENU:
            game_state_menu_init(&game->states.menu, game->engine, &game->game_data);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_init(&game->states.playing, game->engine, &game->game_data);
            break;

        case GAME_STATE_SPLASH:
            game_state_splash_init(&game->states.splash, game->engine, &game->game_data);
            break;

        case GAME_STATE_GAME_OVER:
            game_state_game_over_init(&game->states.game_over, game->engine, &game->game_data);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }
}
