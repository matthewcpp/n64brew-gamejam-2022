#include "playing.h"
#include "framework64/random.h"

#define LEVEL_MEMORY_POOL_SIZE (400 * 1024)

static void switch_to_level(Playing* state, Level level);

void game_state_playing_init(Playing* state, fw64Engine* engine, GameData* game_data) {
    fw64_bump_allocator_init(&state->bump_allocator, LEVEL_MEMORY_POOL_SIZE);

    state->engine = engine;
    state->game_data = game_data;
    state->current_level = LEVEL_NONE;

    fw64_random_set_seed(*(uint32_t*)&engine->time->total_time); //intentional type punning
    state->return_to_level_select_time = 0.0f;

    switch_to_level(state, game_data->transition_to_level);
    game_data->transition_to_level = LEVEL_NONE;
}

void game_state_playing_uninit(Playing* state) {
    switch_to_level(state, LEVEL_NONE);
    fw64_bump_allocator_uninit(&state->bump_allocator);
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
        state->game_data->transition_to_state = GAME_STATE_MENU;
        return;
    }
    #endif

    Level transition_to_level = state->game_data->transition_to_level;
    if (transition_to_level) {
        switch_to_level(state, transition_to_level);
        state->game_data->transition_to_level = LEVEL_NONE;
        return;
    }

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

        case LEVEL_INTERIOR:
            interior_level_update(&state->levels.interior);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }

    if (state->levels.current.player.current_health <= 0) {
        state->game_data->transition_to_state = GAME_STATE_GAME_OVER;
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

        case LEVEL_INTERIOR:
            interior_level_draw(&state->levels.interior);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }
}

void switch_to_level(Playing* state, Level level) {
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

        case LEVEL_INTERIOR:
            interior_level_uninit(&state->levels.interior);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }

    state->current_level = level;
    fw64_bump_allocator_reset(&state->bump_allocator);

    switch(state->current_level) {
        case LEVEL_TEST:
            test_level_init(&state->levels.test_level, state->engine, state->game_data, &state->bump_allocator.interface);
            break;

        case LEVEL_CHURCH_HILL:
            hill_level_init(&state->levels.church_hill, state->engine, state->game_data, &state->bump_allocator.interface);
            break;

        case LEVEL_TILES:
            tiles_test_level_init(&state->levels.tiles_test, state->engine, state->game_data, &state->bump_allocator.interface);
            break;

        case LEVEL_INTERIOR:
            interior_level_init(&state->levels.interior, state->engine, state->game_data, &state->bump_allocator.interface);
            break;

        case LEVEL_NONE:
        case LEVEL_COUNT:
            break;
    }
}
