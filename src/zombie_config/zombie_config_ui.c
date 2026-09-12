#include "zombie_config_ui.h"

#include "assets/assets.h"
#include "assets/zombie_animation.h"
#include "assets/zombie_image_texture_defs.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>
#include <string.h>

const char* option_names[OPTION_COUNT] = {"Animation", "Top"};

static void change_selected_option(ZombieConfigUi* ui, int direction);
static void change_selected_option_value(ZombieConfigUi* ui, int direction);

void zombie_config_ui_init(ZombieConfigUi* ui, fw64Engine* engine, fw64SkinnedMeshInstance* zombie_instance, ZombieConfig* config, fw64Allocator* allocator) {
    ui->allocator = allocator;
    ui->engine = engine;
    ui->zombie_instance = zombie_instance;
    ui->config = config;
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_zombie_config, allocator);

    ui->selected_option = OPTION_ANIMATION;
    ui->current_animation = zombie_animation_Idle;
    ui->current_face_image = 0;
    ui->shirt_palette = 0;
    ui->pants_palette = 0;

    //initialize the face images and setup the config
    fw64MaterialCollection* material_collection = fw64_mesh_instance_get_material_collection(&zombie_instance->mesh_instance);
    fw64Material* initial_face_material = fw64_material_collection_get_material(material_collection, ZOMBIE_FACE_PRIM_INDEX);
    ui->face_images[0] = fw64_texture_get_image(fw64_material_get_texture(initial_face_material));
    ui->face_images[1] = fw64_assets_load_image(ui->engine->assets, FW64_ASSET_image_Z_Face02, allocator);
    ui->face_images[2] = fw64_assets_load_image(ui->engine->assets, FW64_ASSET_image_Z_Face03, allocator);
    ui->face_images[3] = fw64_assets_load_image(ui->engine->assets, FW64_ASSET_image_Z_Face01_alt, allocator);

    ui->spritebatch = fw64_spritebatch_create(1, allocator);
}

void zombie_config_ui_uninit(ZombieConfigUi* ui) {
    fw64_font_delete(ui->engine->assets, ui->font, ui->allocator);
    fw64_spritebatch_delete(ui->spritebatch);

    for (int i = 1; i <= 3; i++) {
        fw64_image_delete(ui->engine->assets, ui->face_images[i], ui->allocator);
    }
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
        fw64_spritebatch_set_color(ui->spritebatch, 255, 255, 0, 255);
        fw64_spritebatch_draw_string(ui->spritebatch, ui->font, text, x_pos, y_pos);
        fw64_spritebatch_set_color(ui->spritebatch, 255, 255, 255, 255);
    }
    else {
        fw64_spritebatch_draw_string(ui->spritebatch, ui->font, text, x_pos, y_pos);
    }

    return fw64_font_size(ui->font) + 4;
}

void zombie_config_ui_draw(ZombieConfigUi* ui, fw64RenderPass* renderpass) {
    fw64Renderer* renderer = ui->engine->renderer;

    int x_pos = 20;
    int y_pos = 20;
    int font_size = fw64_font_size(ui->font);
    char text_buf[32];

    fw64_spritebatch_begin(ui->spritebatch);

    sprintf(text_buf, "Animation: %d", ui->current_animation);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_ANIMATION);

    sprintf(text_buf, "Face: %d", ui->current_face_image);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_FACE_IMAGE);

    sprintf(text_buf, "Shirt: %d", ui->shirt_palette);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_SHIRT_PALETTE);

    sprintf(text_buf, "Pants: %d", ui->pants_palette);
    y_pos += draw_text(ui, text_buf, x_pos, y_pos, ui->selected_option == OPTION_PANTS_PALETTE);

    fw64_spritebatch_end(ui->spritebatch);

    fw64_renderpass_draw_sprite_batch(renderpass, ui->spritebatch);
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
    fw64AnimationController* animation_controller = &ui->zombie_instance->controller;
    ui->current_animation += direction;

    if (ui->current_animation >= animation_controller->animation_data->animation_count)
        ui->current_animation = 0;
    else if (ui->current_animation < 0)
        ui->current_animation = animation_controller->animation_data->animation_count - 1;

    fw64_animation_controller_set_animation(animation_controller, ui->current_animation);
    fw64_animation_controller_play(animation_controller);
}

static int change_texture_palette(ZombieConfigUi* ui, int direction, int material_index) {
    fw64MaterialCollection* material_collection = fw64_mesh_instance_get_material_collection(&ui->zombie_instance->mesh_instance);
    fw64Material* material = fw64_material_collection_get_material(material_collection, material_index);
    fw64Texture* texture = fw64_material_get_texture(material);
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

static void change_face_image(ZombieConfigUi* ui, int direction) {
    ui->current_face_image += direction;

    if (ui->current_face_image >= ZOMBIE_FACE_IMAGE_COUNT) {
        ui->current_face_image = 0;
    }
    else if (ui->current_face_image < 0) {
        ui->current_face_image = ZOMBIE_FACE_IMAGE_COUNT - 1;
    }

    zombie_config_set_face_image(&ui->zombie_instance->mesh_instance, ui->face_images[ui->current_face_image]);
}

void change_selected_option_value(ZombieConfigUi* ui, int direction) {
    switch(ui->selected_option){
        case OPTION_ANIMATION:
            change_animation(ui, direction);
        break;

        case OPTION_FACE_IMAGE:
            change_face_image(ui, direction);
            break;

        case OPTION_SHIRT_PALETTE: {
            ui->shirt_palette = change_texture_palette(ui, direction, ZOMBIE_SHIRT_PRIM_INDEX);
            zombie_config_set_shirt_palette(&ui->zombie_instance->mesh_instance, ui->shirt_palette);
            break;
        }

        case OPTION_PANTS_PALETTE: {
            ui->pants_palette = change_texture_palette(ui, direction, ZOMBIE_PANTS_PRIM_INDEX);
            zombie_config_set_pants_palette(&ui->zombie_instance->mesh_instance, ui->pants_palette);
            break;
        }
    }
}