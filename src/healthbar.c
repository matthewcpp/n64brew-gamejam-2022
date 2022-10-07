#include "healthbar.h"

#include <stdio.h>

#define HEALTHBAR_ANCHOR_X 200
#define HEALTHBAR_ANCHOR_Y 16

#define HEALTHBAR_WIDTH 100
#define HEALTHBAR_HEIGHT 12
#define HEALTHBAR_BORDER 1

void healthbar_init(Healthbar* healthbar, Player* player, fw64Font* font, fw64Renderer* renderer) {
    healthbar->player = player;
    healthbar->font = font;
    healthbar->renderer = renderer;
}

void healthbar_draw(Healthbar* healthbar) {
    char health_text[8];
    sprintf(&health_text[0], "%d", healthbar->player->current_health);

    int pos_x = HEALTHBAR_ANCHOR_X;
    int pos_y = HEALTHBAR_ANCHOR_Y;
    int width = HEALTHBAR_WIDTH;
    int height = HEALTHBAR_HEIGHT;

    fw64_renderer_draw_text(healthbar->renderer, healthbar->font, pos_x, pos_y, &health_text[0]);
    pos_y += fw64_font_size(healthbar->font) + 4;

    // draw border
    fw64_renderer_set_fill_color(healthbar->renderer, 255, 255, 255, 255);
    fw64_renderer_draw_filled_rect(healthbar->renderer, pos_x, pos_y, width, HEALTHBAR_BORDER); // top
    fw64_renderer_draw_filled_rect(healthbar->renderer, pos_x, pos_y + height - HEALTHBAR_BORDER, width, HEALTHBAR_BORDER); // bottom
    fw64_renderer_draw_filled_rect(healthbar->renderer, pos_x, pos_y, HEALTHBAR_BORDER, height); //left
    fw64_renderer_draw_filled_rect(healthbar->renderer, pos_x + width - HEALTHBAR_BORDER, pos_y, HEALTHBAR_BORDER, height); // right

    // determine size and draw track
    pos_x += HEALTHBAR_BORDER;
    pos_y += HEALTHBAR_BORDER;
    width -= HEALTHBAR_BORDER * 2;
    height -= HEALTHBAR_BORDER * 2;

    fw64_renderer_set_fill_color(healthbar->renderer, 55, 55, 55, 200);
    fw64_renderer_draw_filled_rect(healthbar->renderer, pos_x, pos_y, width, height);

    // determine width / color and draw bar
    float health_pct = (float)healthbar->player->current_health / (float)PLAYER_MAX_HEALTH;
    width = width * health_pct;

    if (health_pct >= 0.66f)
        fw64_renderer_set_fill_color(healthbar->renderer, 0, 255, 0, 255);
    else if (health_pct >= 0.33f)
        fw64_renderer_set_fill_color(healthbar->renderer, 255, 255, 0, 255);
    else
        fw64_renderer_set_fill_color(healthbar->renderer, 255, 0, 0, 255);

    fw64_renderer_draw_filled_rect(healthbar->renderer, pos_x, pos_y, width, height);
}