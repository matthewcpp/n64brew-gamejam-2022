#include "game_over.h"
#include "framework64/n64/controller_button.h"
#include "framework64/math.h"

#include "assets/assets.h"
#include "assets/sound_bank_sounds.h"

#define LEVEL_MEMORY_POOL_SIZE (400 * 1024)

#define TEXT_EFFECT_DURATION 3.0f

void game_state_game_over_init(GameOver* state, fw64Engine* engine, GameData* game_data) {
    state->engine = engine;
    state->game_data = game_data;
    fw64_bump_allocator_init(&state->bump_allocator, LEVEL_MEMORY_POOL_SIZE);

    state->text_effect_time = 0.0f;

    fw64Image* you_died_img = fw64_image_load(engine->assets, FW64_ASSET_image_you_died, &state->bump_allocator.interface);
    state->you_died_texture = fw64_texture_create_from_image(you_died_img, &state->bump_allocator.interface);
    state->sound_handle = fw64_audio_play_sound(engine->audio, sound_bank_sounds_you_died);

    fw64_renderer_set_clear_color(engine->renderer, 0, 0, 0);
}

void game_state_game_over_uninit(GameOver* state) {
    if (fw64_audio_get_sound_status(state->engine->audio, state->sound_handle) == FW64_AUDIO_PLAYING) {
        fw64_audio_stop_sound(state->engine->audio, state->sound_handle);
    }
    
    fw64_image_delete(state->engine->assets, fw64_texture_get_image(state->you_died_texture), &state->bump_allocator.interface);
    fw64_texture_delete(state->you_died_texture, &state->bump_allocator.interface);

    fw64_bump_allocator_uninit(&state->bump_allocator);
}

void game_state_game_over_update(GameOver* state){
    fw64AudioStatus sound_status = fw64_audio_get_sound_status(state->engine->audio, state->sound_handle);
    if (sound_status == FW64_AUDIO_STOPPED && fw64_input_controller_button_pressed(state->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        state->game_data->transition_to_state = GAME_STATE_MENU;
    }

    if (state->text_effect_time < TEXT_EFFECT_DURATION) {
        state->text_effect_time = fw64_minf(state->text_effect_time + state->engine->time->time_delta, TEXT_EFFECT_DURATION);
    }
}

#define YOU_DIED_BORDER_OFFSET 30

static void draw_you_died_text(GameOver* state) {
    fw64Renderer* renderer = state->engine->renderer;
    IVec2 screen_size;
    fw64_renderer_get_screen_size(renderer, &screen_size);

    if (state->text_effect_time == TEXT_EFFECT_DURATION) {
        int draw_x = (screen_size.x / 2) - (fw64_texture_width(state->you_died_texture) / 2);
        fw64_renderer_draw_sprite(state->engine->renderer, state->you_died_texture, draw_x, YOU_DIED_BORDER_OFFSET);
        return;
    }
    float tex_width = (float)fw64_texture_width(state->you_died_texture);

    float t_val = fw64_lerp(0.0f, 1.0f, state->text_effect_time / TEXT_EFFECT_DURATION);
    float target_x = (float)(screen_size.x / 2) - tex_width / 2.0f;
    
    float left_pos = YOU_DIED_BORDER_OFFSET + ((target_x - YOU_DIED_BORDER_OFFSET) * t_val);
    float right_ref = (float)screen_size.x - (float)YOU_DIED_BORDER_OFFSET - tex_width;
    float right_pos = right_ref - ((right_ref - target_x) * t_val);

    fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);
    fw64_renderer_draw_sprite(state->engine->renderer, state->you_died_texture, (int)right_pos, YOU_DIED_BORDER_OFFSET);

    fw64_renderer_set_fill_color(renderer, 255, 255, 255, 100);
    fw64_renderer_draw_sprite(state->engine->renderer, state->you_died_texture, (int)left_pos, YOU_DIED_BORDER_OFFSET);

    fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);
}

void game_state_game_over_draw(GameOver* state) {
    fw64Renderer* renderer = state->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    draw_you_died_text(state);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}