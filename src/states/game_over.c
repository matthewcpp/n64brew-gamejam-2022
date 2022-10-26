#include "game_over.h"
#include "framework64/n64/controller_button.h"

#define LEVEL_MEMORY_POOL_SIZE (400 * 1024)

void game_state_game_over_init(GameOver* state, fw64Engine* engine, GameData* game_data) {
    state->engine = engine;
    state->game_data = game_data;
    fw64_bump_allocator_init(&state->bump_allocator, LEVEL_MEMORY_POOL_SIZE);
}

void game_state_game_over_uninit(GameOver* state) {
    fw64_bump_allocator_uninit(&state->bump_allocator);
}

void game_state_game_over_update(GameOver* state){
    if (fw64_input_controller_button_pressed(state->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        state->game_data->transition_to_state = GAME_STATE_LEVEL_SELECT;
    }

}

void game_state_game_over_draw(GameOver* state) {

}