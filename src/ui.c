#include "ui.h"

void ui_init(UI* ui, fw64Engine* engine, Player* player) {
    ui->engine = engine;
    ui->player = player;
}

void ui_draw(UI* ui) {
    fw64Renderer* renderer = ui->engine->renderer;

    fw64Texture* crosshair = ui->player->weapon->crosshair_sprite;

    IVec2 screen_size, crosshair_pos;
    fw64_renderer_get_screen_size(renderer, &screen_size);
    crosshair_pos.x = screen_size.x / 2 - fw64_texture_width(crosshair) / 2;
    crosshair_pos.y = screen_size.y / 2 - fw64_texture_height(crosshair) / 2;

    fw64_renderer_draw_sprite(renderer, crosshair, crosshair_pos.x, crosshair_pos.y);
}