#include "game.h"
#include "debug.h"

static void game_transition_state(Game* game);

void game_init(Game* game, fw64Engine* engine) {

    INIT_LOG(); // needed for N64. sets up logging thread.

    game->engine = engine;
    game_data_init(&game->game_data);

    game->current_state = GAME_STATE_NONE;

    // Set this to the state you want to being at
    game->game_data.transition_to_state = GAME_STATE_LEVEL_SELECT;

    // you can optionally set this to the level you want to begin at
    game->game_data.transition_to_level = LEVEL_NONE;

    game_transition_state(game);
}

void game_update(Game* game) {
    
    UPDATE_LOG(); // not required, but suggested. fetches any usb data on N64.

    if (game->game_data.transition_to_state) {
        game_transition_state(game);
    }

    switch(game->current_state) {
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_update(&game->states.level_select);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_update(&game->states.playing);
            break;

        case GAME_STATE_NONE:
            break;
    }
}

void game_draw(Game* game) {
    switch(game->current_state) {
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_draw(&game->states.level_select);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_draw(&game->states.playing);
            break;

        case GAME_STATE_NONE:
            break;
    }
}

void game_transition_state(Game* game) {
    switch(game->current_state) {
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_uninit(&game->states.level_select);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_uninit(&game->states.playing);
            break;

        case GAME_STATE_NONE:
            break;
    }

    game->current_state = game->game_data.transition_to_state;
    game->game_data.transition_to_state = GAME_STATE_NONE;

    switch(game->current_state) {
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_init(&game->states.level_select, game->engine, &game->game_data);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_init(&game->states.playing, game->engine, &game->game_data);
            break;

        case GAME_STATE_NONE:
            break;
    }
}