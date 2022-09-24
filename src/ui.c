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
    fw64Renderer* renderer = ui->engine->renderer;
    fw64Texture* crosshair = ui->player->weapon_controller.weapon.crosshair;

    IVec2 screen_size, crosshair_pos;
    fw64_renderer_get_screen_size(renderer, &screen_size);
    crosshair_pos.x = screen_size.x / 2 - fw64_texture_width(crosshair) / 2;
    crosshair_pos.y = screen_size.y / 2 - fw64_texture_height(crosshair) / 2;

    fw64_renderer_draw_sprite(renderer, crosshair, crosshair_pos.x, crosshair_pos.y);
}

void ui_draw(UI* ui) {
    if (ui->player->weapon_controller.weapon.type != WEAPON_TYPE_NONE) {
        ui_draw_player_weapon_crosshair(ui);

        char ammo_text[16];
        sprintf(ammo_text, "%d / %d", 0,0);
    }

}