#include "states/level_select.h"

#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

// note font only setup with lowercase letters!
const char* level_names[] = {
    "",
    "test",
    "church hill",
    "tiles test"
};

void game_state_level_select_init(LevelSelect* level_select, fw64Engine* engine, GameData* game_data) {
    level_select->engine = engine;
    level_select->game_data = game_data;

    // todo figure this out
    level_select->allocator = fw64_default_allocator();
    level_select->font = fw64_font_load(engine->assets, FW64_ASSET_font_level_select, level_select->allocator);

    fw64_camera_init(&level_select->camera);
    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    IVec2 screen_size;
    fw64_renderer_get_screen_size(engine->renderer, &screen_size);
    IVec2 dimensions = fw64_font_measure_text(level_select->font, "level select");
    level_select->title_pos_x = (screen_size.x / 2) - (dimensions.x / 2);

    fw64_renderer_set_clear_color(engine->renderer, 0, 0, 0);

    level_select->selected_level = 1;
}

static void level_select_navigate(LevelSelect* level_select, int direction) {
    level_select->selected_level += direction;

    if (level_select->selected_level <= 0) {
        level_select->selected_level = LEVEL_COUNT - 1;
    }

    if (level_select->selected_level >= LEVEL_COUNT) {
        level_select->selected_level = 1;
    }
}

void game_state_level_select_update(LevelSelect* level_select) {
    if (fw64_input_controller_button_pressed(level_select->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        level_select->game_data->transition_to_level = level_select->selected_level;
        level_select->game_data->transition_to_state = GAME_STATE_PLAYING;
    }

    if (fw64_input_controller_button_pressed(level_select->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
        level_select_navigate(level_select, -1);
    }

        if (fw64_input_controller_button_pressed(level_select->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
        level_select_navigate(level_select, 1);
    }
}

void game_state_level_select_draw(LevelSelect* level_select) {
    fw64Renderer* renderer = level_select->engine->renderer;

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &level_select->camera);

    int font_size = fw64_font_size(level_select->font);
    int y_pos = 30;
    fw64_renderer_draw_text(renderer, level_select->font, level_select->title_pos_x, y_pos, "level select");
    y_pos += font_size;

    for (int i = 1; i < LEVEL_COUNT; i++) {
        if (i == level_select->selected_level)
            fw64_renderer_draw_text(renderer, level_select->font, 30, y_pos, ">");

        fw64_renderer_draw_text(renderer, level_select->font, 40, y_pos, level_names[i]);    
        y_pos += font_size;
    }

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void game_state_level_select_uninit(LevelSelect* level_select) {
    fw64_font_delete(level_select->engine->assets, level_select->font, level_select->allocator);
}