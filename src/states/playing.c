#include "playing.h"
#include "framework64/random.h"

void game_state_playing_init(Playing* state, fw64Engine* engine, GameData* game_data) {
    state->engine = engine;
    state->game_data = game_data;
    fw64_random_set_seed(*(uint32_t*)&engine->time->total_time); //intentional type punning
    state->return_to_level_select_time = 0.0f;

    state->current_level = game_data->transition_to_level;
    game_data->transition_to_level = LEVEL_NONE;

    switch(state->current_level) {
        case LEVEL_TEST:
            test_level_init(&state->levels.test_level, engine);
            break;

        case LEVEL_CHURCH_HILL:
            hill_level_init(&state->levels.church_hill, engine);
            break;

        case LEVEL_TILES:
            tiles_test_level_init(&state->levels.tiles_test, engine);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            game_data->transition_to_state = GAME_STATE_LEVEL_SELECT;
            break;
    }
}

void game_state_playing_uninit(Playing* state) {
    switch(state->current_level) {
        case LEVEL_TEST:
            test_level_uninit(&state->levels.test_level);
            break;

        case LEVEL_CHURCH_HILL:
            hill_level_uninit(&state->levels.church_hill);
            break;

        case LEVEL_TILES:
            tiles_test_level_uninit(&state->levels.tiles_test);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }
}

void game_state_playing_update(Playing* state) {
    #ifndef NDEBUG
    if (fw64_input_controller_button_down(state->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        state->return_to_level_select_time += state->engine->time->time_delta;
    }
    else {
        state->return_to_level_select_time = 0.0f;
    }

    if (state->return_to_level_select_time > 2.0f) {
        state->game_data->transition_to_state = GAME_STATE_LEVEL_SELECT;
        return;
    }
    #endif

    switch(state->current_level) {
        case LEVEL_TEST:
            test_level_update(&state->levels.test_level);
            break;

        case LEVEL_CHURCH_HILL:
            hill_level_update(&state->levels.church_hill);
            break;

        case LEVEL_TILES:
            tiles_test_level_update(&state->levels.tiles_test);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }
}

void game_state_playing_draw(Playing* state) {
    switch(state->current_level) {
        case LEVEL_TEST:
            test_level_draw(&state->levels.test_level);
            break;

        case LEVEL_CHURCH_HILL:
            hill_level_draw(&state->levels.church_hill);
            break;

        case LEVEL_TILES:
            tiles_test_level_draw(&state->levels.tiles_test);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }
}