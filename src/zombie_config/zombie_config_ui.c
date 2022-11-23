#include "zombie_config_ui.h"

#include "assets/assets.h"
#include "assets/zombie_animation.h"
#include "assets/zombie_image_texture_defs.h"

#include "framework64/n64/controller_button.h"

#include <string.h>

const char* option_names[OPTION_COUNT] = {"Animation", "Top"};

static void change_selected_option(ZombieConfigUi* ui, int direction);
static void change_selected_option_value(ZombieConfigUi* ui, int direction);

void zombie_config_ui_init(ZombieConfigUi* ui, fw64Engine* engine, fw64Mesh* mesh, ZombieConfig* config, fw64AnimationController* zombie_controller) {
    ui->engine = engine;
    ui->mesh = mesh;
    ui->config = config;
    ui->animation_controller = zombie_controller;
    ui->font = fw64_font_load(engine->assets, FW64_ASSET_font_zombie_config, fw64_default_allocator());

    ui->selected_option = OPTION_ANIMATION;
    ui->current_animation = zombie_animation_Idle;
    ui->current_face_image = 0;

    //initialize the face images and setup the config
    fw64Texture* texture = fw64_mesh_get_texture(ui->mesh, zombie_image_Z_Face01_texture_0);
    ui->face_images[0] = fw64_texture_get_image(texture);
    ui->face_images[1] = fw64_image_load(ui->engine->assets, FW64_ASSET_image_Z_Face02, fw64_default_allocator());
    ui->face_images[2] = fw64_image_load(ui->engine->assets, FW64_ASSET_image_Z_Face03, fw64_default_allocator());
    ui->face_images[3] = fw64_image_load(ui->engine->assets, FW64_ASSET_image_Z_Face01_alt, fw64_default_allocator());

    zombie_config_init(config, ui->face_images[0], 0, 0);
}

void zombie_config_ui_update(ZombieConfigUi* ui) {
    fw64Input* input = ui->engine->input;

    if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP) || fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
        change_selected_option(ui, -1);
    }
    else if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN) || fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
        change_selected_option(ui, 1);
    }
    else if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT) || fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        change_selected_option_value(ui, 1);
    }
    else if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT) || fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        change_selected_option_value(ui, -1);
    }
}

static int draw_text(ZombieConfigUi* ui, const char* text, int x_pos, int y_pos, int selected) {
    fw64Renderer* renderer = ui->engine->renderer;

    if (selected) {
        fw64_renderer_set_fill_color(ui->engine->renderer, 255, 255, 0, 255);
        fw64_renderer_draw_text(ui->engine->renderer, ui->font, x_pos, y_pos, text);
        fw64_renderer_set_fill_color(ui->engine->renderer, 255, 255, 255, 255);
    }
    else {
        fw64_renderer_draw_text(ui->engine->renderer, ui->font, x_pos, y_pos, text);
    }

    return fw64_font_size(ui->font) + 4;
}

void zombie_config_ui_draw(ZombieConfigUi* ui) {
    fw64Renderer* renderer = ui->engine->renderer;

    int x_pos = 20;
    int y_pos = 20;
    int font_size = fw64_font_size(ui->font);
    char text_buf[32];

    sprintf(text_buf, "Animation: %d", ui->current_animation);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_ANIMATION);

    sprintf(text_buf, "Face: %d", ui->current_face_image);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_FACE_IMAGE);

    sprintf(text_buf, "Shirt: %d", ui->config->shirt_palette);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_SHIRT_PALETTE);

    sprintf(text_buf, "Pants: %d", ui->config->pants_palette);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_PANTS_PALETTE);
}

void change_selected_option(ZombieConfigUi* ui, int direction) {
    ui->selected_option += direction;

    if (ui->selected_option >= OPTION_COUNT) {
        ui->selected_option = 0;
    }
    else if (ui->selected_option < 0) {
        ui->selected_option = OPTION_COUNT - 1;
    }
}

static void change_animation(ZombieConfigUi* ui, int direction) {
    ui->current_animation += direction;

    if (ui->current_animation >= ui->animation_controller->animation_data->animation_count)
        ui->current_animation = 0;
    else if (ui->current_animation < 0)
        ui->current_animation = ui->animation_controller->animation_data->animation_count - 1;

    fw64_animation_controller_set_animation(ui->animation_controller, ui->current_animation);
    fw64_animation_controller_play(ui->animation_controller);
}

static int change_texture_palette(ZombieConfigUi* ui, int direction, int texture_index) {
    fw64Texture* texture = fw64_mesh_get_texture(ui->mesh, texture_index);
    fw64Image* image = fw64_texture_get_image(texture);

    int palette_index = (int)fw64_texture_get_palette_index(texture);
    uint16_t palette_count = fw64_image_get_palette_count(image);

    palette_index += direction;

    if (palette_index >= palette_count)
        palette_index = 0;
    else if (palette_index < 0)
        palette_index = palette_count - 1;

    return palette_index;
}

static int change_face_image(ZombieConfigUi* ui, int direction) {
    ui->current_face_image += direction;

    if (ui->current_face_image >= ZOMBIE_FACE_IMAGE_COUNT) {
        ui->current_face_image = 0;
    }
    else if (ui->current_face_image < 0) {
        ui->current_face_image = ZOMBIE_FACE_IMAGE_COUNT - 1;
    }

    ui->config->face_image = ui->face_images[ui->current_face_image];
}

void change_selected_option_value(ZombieConfigUi* ui, int direction) {
    switch(ui->selected_option){
        case OPTION_ANIMATION:
            change_animation(ui, direction);
        break;

        case OPTION_FACE_IMAGE:
            change_face_image(ui, direction);
            break;

        case OPTION_SHIRT_PALETTE:
            ui->config->shirt_palette = change_texture_palette(ui, direction, zombie_image_Z_TopFront_texture_0);
        break;

        case OPTION_PANTS_PALETTE:
            ui->config->pants_palette = change_texture_palette(ui, direction, zombie_image_Z_JeanTop_texture_0);
        break;
    }
}