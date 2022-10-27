#include "game.h"
#include "game.h"

static void game_transition_state(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game_data_init(&game->game_data);

    game->current_state = GAME_STATE_NONE;

    // Set this to the state you want to begin at
    game->game_data.transition_to_state = GAME_STATE_GAME_OVER;

    // you can optionally set this to the level you want to begin at
    game->game_data.transition_to_level = LEVEL_NONE;

    game_transition_state(game);
}

void game_update(Game* game) {
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
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_draw(&game->states.level_select);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_draw(&game->states.playing);
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
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_uninit(&game->states.level_select);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_uninit(&game->states.playing);
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
        case GAME_STATE_LEVEL_SELECT:
            game_state_level_select_init(&game->states.level_select, game->engine, &game->game_data);
            break;

        case GAME_STATE_PLAYING:
            game_state_playing_init(&game->states.playing, game->engine, &game->game_data);
            break;

        case GAME_STATE_GAME_OVER:
            game_state_game_over_init(&game->states.game_over, game->engine, &game->game_data);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }
}