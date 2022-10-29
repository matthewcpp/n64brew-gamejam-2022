#include "dialogue_window.h"

#include "framework64/filesystem.h"
#include "framework64/n64/controller_button.h"

#include "assets/assets.h"

#include <string.h>

static void dialogue_window_reset(DialogueWindow* window);
static void read_next_character(DialogueWindow* window);

#define CURRENT_TEXT_LINE(window_ptr) (&(window_ptr)->lines[(window_ptr)->current_line_index])

void dialogue_window_init(DialogueWindow* window, fw64Engine* engine, int font_asset, int dialogue_asset, IVec2* position, IVec2* size, fw64Allocator* allocator) {
    window->engine = engine;
    window->font = fw64_font_load(engine->assets, font_asset, allocator);

    window->background_texture = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_dialogue_overlay, allocator), allocator);
    window->next_indicator = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_dialogue_next, allocator), allocator);

    dialogue_window_reset(window);

    int handle = fw64_filesystem_open(dialogue_asset);
    window->data_size = fw64_filesystem_size(handle);
    window->dialogue_data = allocator->malloc(allocator, window->data_size);
    fw64_filesystem_read(window->dialogue_data, 1, window->data_size, handle);
    fw64_filesystem_close(handle);

    window->status = DIALOGUE_WINDOW_STATUS_INACTIVE;
    window->character_write_time = SPEECH_DIALOG_CHARACTER_WRITE_TIME;

    window->position = *position;
    window->size = *size;
}

void dialogue_window_uninit(DialogueWindow* window, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();

    fw64Image* image = fw64_texture_get_image(window->background_texture);
    fw64_image_delete(window->engine->assets, image, allocator);
    fw64_texture_delete(window->background_texture, allocator);

    image = fw64_texture_get_image(window->next_indicator);
    fw64_image_delete(window->engine->assets, image, allocator);
    fw64_texture_delete(window->next_indicator, allocator);

    fw64_font_delete(window->engine->assets, window->font, allocator);

    allocator->free(allocator, window->dialogue_data);
}

void dialogue_window_reset(DialogueWindow* window) {
    memset(&window->lines[0], 0, sizeof(DialogueWindowLine) * DIALOGUE_WINDOW_LINE_COUNT);
    window->current_line_index = 0;

    window->current_data_index = 0;
    window->current_character_time = 0.0f;

}

static void dialogue_window_update_waiting(DialogueWindow* window) {
    if (fw64_input_controller_button_pressed(window->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A)) {
        window->current_line_index += 1;

        if (window->current_line_index < DIALOGUE_WINDOW_LINE_COUNT) {
            CURRENT_TEXT_LINE(window)->text = window->dialogue_data + window->current_data_index;
            window->current_character_time = 0;
            window->status = DIALOGUE_WINDOW_STATUS_WRITING;
            read_next_character(window);
        }
        else {
            window->status = DIALOGUE_WINDOW_STATUS_DONE;

            if (window->done_callback)
                window->done_callback(window->callbark_arg);
        }
    }
}

static void dialogue_window_update_writing(DialogueWindow* window) {
    window->current_character_time += window->engine->time->time_delta;
    if (window->current_character_time >= window->character_write_time) {
        window->current_character_time -= window->character_write_time;
        read_next_character(window);
    }
}

void dialogue_window_update(DialogueWindow* window) {
    switch (window->status) {
        case DIALOGUE_WINDOW_STATUS_WRITING:
            dialogue_window_update_writing(window);
            break;

        case DIALOGUE_WINDOW_STATUS_WAITING:
            dialogue_window_update_waiting(window);
            break;

        default:
            break;
    }
}

void dialogue_window_draw(DialogueWindow* window) {
    if (window->status == DIALOGUE_WINDOW_STATUS_INACTIVE)
        return;

    fw64Renderer* renderer = window->engine->renderer;

    float scale_x = (float)window->size.x / (float)fw64_texture_height(window->background_texture);
    float scale_y = (float)window->size.y / (float)fw64_texture_width(window->background_texture);

    fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);

    fw64_renderer_draw_sprite_slice_transform(renderer, window->background_texture, 0, window->position.x, window->position.y, scale_x, scale_y, 0.0f);

    int draw_x = window->position.x + 5;
    int draw_y = window->position.y + 5;

    for (int i  = 0; i < DIALOGUE_WINDOW_LINE_COUNT; i++) {
        if (!window->lines[0].text)
            continue;

        fw64_renderer_draw_text_count(renderer, window->font, draw_x, draw_y, window->lines[i].text, window->lines[i].count);

        draw_y += fw64_font_size(window->font) + 3;
    }

    if (window->status == DIALOGUE_WINDOW_STATUS_WAITING) {
        draw_x = window->position.x + window->size.x - fw64_texture_width(window->next_indicator) - 2;
        draw_y = window->position.y + window->size.y - fw64_texture_height(window->next_indicator) / 2;

        fw64_renderer_draw_sprite(renderer, window->next_indicator, draw_x, draw_y);
    }
}

static void read_next_character(DialogueWindow* window) {
    if (window->current_data_index >= window->data_size) {
        window->status = DIALOGUE_WINDOW_STATUS_WAITING;
        return;
    }
    char ch;
    do {
        ch = window->dialogue_data[window->current_data_index++];
    } while (ch == '\r');
     

    if (ch == '\n') {
        window->status = DIALOGUE_WINDOW_STATUS_WAITING;
    }
    else {
        CURRENT_TEXT_LINE(window)->count += 1;
    }
}

void dialogue_window_start(DialogueWindow* window, DialogueWindowDoneCallback callback, void* arg) {
    dialogue_window_reset(window);
    window->done_callback = callback;
    window->callbark_arg = arg;
    CURRENT_TEXT_LINE(window)->text = window->dialogue_data;
    window->status = DIALOGUE_WINDOW_STATUS_WRITING;
    read_next_character(window);
}