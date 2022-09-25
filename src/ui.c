#include "ui.h"

#include "assets/assets.h"

#include <stdio.h>

void ui_init(UI* ui, fw64Engine* engine, fw64Allocator* allocator, Player* player) {
    ui->engine = engine;
    ui->allocator = allocator;
    ui->player = player;

    ui->ammo_font = fw64_font_load(ui->engine->assets, FW64_ASSET_font_weapon_display, ui->allocator);
}

void ui_uninit(UI* ui) {
    fw64_font_delete(ui->engine->assets, ui->ammo_font, ui->allocator);
}

static void ui_draw_player_weapon_crosshair(UI* ui) {
    fw64Texture* crosshair = ui->player->weapon_controller.weapon.crosshair;

    IVec2 screen_size, crosshair_pos;
    fw64_renderer_get_screen_size(ui->engine->renderer, &screen_size);
    crosshair_pos.x = screen_size.x / 2 - fw64_texture_width(crosshair) / 2;
    crosshair_pos.y = screen_size.y / 2 - fw64_texture_height(crosshair) / 2;

    fw64_renderer_draw_sprite(ui->engine->renderer, crosshair, crosshair_pos.x, crosshair_pos.y);
}

static void ui_draw_player_ammo_status(UI* ui) {
    WeaponAmmo* weapon_ammo = weapon_controller_get_current_weapon_ammo(&ui->player->weapon_controller);

    char ammo_text[16];
    sprintf(ammo_text, "%d / %d", weapon_ammo->current_mag_count, weapon_ammo->additional_rounds_count);

    fw64_renderer_draw_text(ui->engine->renderer, ui->ammo_font, 10, 10, &ammo_text[0]);
}

void ui_draw(UI* ui) {
    if (ui->player->weapon_controller.weapon.info->type != WEAPON_TYPE_NONE) {
        ui_draw_player_weapon_crosshair(ui);
        ui_draw_player_ammo_status(ui);
    }

}