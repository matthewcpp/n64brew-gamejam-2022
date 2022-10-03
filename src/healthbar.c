#include "healthbar.h"

#include <stdio.h>

void healthbar_init(Healthbar* healthbar, Player* player, fw64Font* font, fw64Renderer* renderer) {
    healthbar->player = player;
    healthbar->font = font;
    healthbar->renderer = renderer;
}

void healthbar_draw(Healthbar* healthbar) {
    char health_text[8];
    sprintf(&health_text[0], "%d", healthbar->player->current_health);

    int pos_x = 30;
    int pos_y = 180;

    fw64_renderer_draw_text(healthbar->renderer, healthbar->font, pos_x, pos_y, &health_text[0]);
}