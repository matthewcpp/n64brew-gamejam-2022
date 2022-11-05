#include "ui.h"

#include "levels/level_base.h"

#include "assets/assets.h"

#include <stdio.h>
#include <string.h>

void ui_init(UI* ui, fw64Engine* engine, fw64Allocator* allocator, LevelBase* level) {
    ui->engine = engine;
    ui->allocator = allocator;
    ui->level = level;

    ui->hud_font = fw64_font_load(ui->engine->assets, FW64_ASSET_font_ui_hud, ui->allocator);
    healthbar_init(&ui->healthbar, &ui->level->player, ui->hud_font, engine->renderer);

    ui->interaction_font = fw64_font_load(ui->engine->assets, FW64_ASSET_font_ui_interact, ui->allocator);
    fw64Image* button_image = fw64_image_load_with_options(engine->assets, FW64_ASSET_image_n64_buttons, FW64_IMAGE_FLAG_DMA_MODE, ui->allocator);
    ui->interaction_button = fw64_texture_create_from_image(button_image, ui->allocator);
    ui->interaction_text[0] = 0;
    ui->interaction_image_frame = 0;
    ui->interaction_loaded_frame = 0;
}

void ui_uninit(UI* ui) {
    fw64_font_delete(ui->engine->assets, ui->hud_font, ui->allocator);
    fw64_font_delete(ui->engine->assets, ui->interaction_font, ui->allocator);
    fw64_image_delete(ui->engine->assets, fw64_texture_get_image(ui->interaction_button), ui->allocator);
    fw64_texture_delete(ui->interaction_button, ui->allocator);
}

static void ui_draw_player_weapon_crosshair(UI* ui, IVec2* screen_center) {
    fw64Texture* crosshair = ui->level->player.weapon_controller.weapon.crosshair;
    fw64Renderer* renderer = ui->engine->renderer;

    IVec2 crosshair_pos;

    crosshair_pos.x = screen_center->x - fw64_texture_width(crosshair) / 2;
    crosshair_pos.y = screen_center->y - fw64_texture_height(crosshair) / 2;

    fw64_renderer_draw_sprite(renderer, crosshair, crosshair_pos.x, crosshair_pos.y);
}

/** Precondition: ui->level->interaction.interesting_node != NULL */
static void ui_draw_interaction_indicator(UI* ui, IVec2* screen_center) {
    if (ui->interaction_image_frame != ui->interaction_loaded_frame) {
        fw64_image_load_frame(fw64_texture_get_image(ui->interaction_button), ui->interaction_image_frame );
        ui->interaction_loaded_frame = ui->interaction_image_frame;
    }
    IVec2 text_size = fw64_font_measure_text(ui->interaction_font, ui->interaction_text);
    int slice_width = fw64_texture_slice_width(ui->interaction_button);
    int message_width = slice_width + 4 + text_size.x;

    int x_pos = screen_center->x - message_width / 2;
    int y_pos = screen_center->y + 16;

    fw64_renderer_draw_sprite_slice(ui->engine->renderer, ui->interaction_button, ui->interaction_image_frame, x_pos, y_pos);
    x_pos += slice_width + 4;

    y_pos += (fw64_font_size(ui->interaction_font) - text_size.y);

    fw64_renderer_draw_text(ui->engine->renderer, ui->interaction_font, x_pos, y_pos, ui->interaction_text);
}


static void ui_draw_player_ammo_status(UI* ui, WeaponType weapon_type) {
    WeaponAmmo* weapon_ammo = weapon_controller_get_current_weapon_ammo(&ui->level->player.weapon_controller);

    char ammo_text[16];

    if (weapon_type == WEAPON_TYPE_1911)
        sprintf(ammo_text, "%d", weapon_ammo->current_mag_count);
    else
        sprintf(ammo_text, "%d / %d", weapon_ammo->current_mag_count, weapon_ammo->additional_rounds_count);

    fw64_renderer_draw_text(ui->engine->renderer, ui->hud_font, 20, 16, &ammo_text[0]);
}

void ui_draw(UI* ui) {
    fw64_renderer_set_fill_color(ui->engine->renderer, 255, 255, 255, 255);

    IVec2 screen_center;
    fw64_renderer_get_screen_size(ui->engine->renderer, &screen_center);
    screen_center.x /= 2;
    screen_center.y /= 2;

    WeaponType weapon_type = ui->level->player.weapon_controller.weapon.info->type;
    if (weapon_type != WEAPON_TYPE_NONE) {
        ui_draw_player_weapon_crosshair(ui, &screen_center);
        ui_draw_player_ammo_status(ui, weapon_type);
    }

    if (strlen(ui->interaction_text) > 0)
        ui_draw_interaction_indicator(ui, &screen_center);

    healthbar_draw(&ui->healthbar);
}

void ui_set_interaction_text(UI* ui, const char* text, uint32_t icon_index) {
    strcpy(ui->interaction_text, text);
    ui->interaction_image_frame = icon_index;
}

void ui_clear_interaction_text(UI* ui) {
    ui->interaction_text[0] = 0;
}