#include "healthbar.h"

#include <stdio.h>

#define HEALTHBAR_ANCHOR_X 200
#define HEALTHBAR_ANCHOR_Y 16

#define HEALTHBAR_WIDTH 100
#define HEALTHBAR_HEIGHT 12
#define HEALTHBAR_BORDER 1

void healthbar_init(Healthbar* healthbar, Player* player, fw64Font* font, fw64Texture* fill_texture) {
    healthbar->player = player;
    healthbar->fill_texture = fill_texture;
    healthbar->font = font;
}

void healthbar_draw(Healthbar* healthbar, fw64SpriteBatch* spritebatch) {
    char health_text[8];
    sprintf(&health_text[0], "%d", healthbar->player->current_health);

    int pos_x = HEALTHBAR_ANCHOR_X;
    int pos_y = HEALTHBAR_ANCHOR_Y;
    int width = HEALTHBAR_WIDTH;
    int height = HEALTHBAR_HEIGHT;

    // fw64_spritebatch_draw_string(spritebatch, healthbar->font, &health_text[0], pos_x, pos_y);
    // pos_y += fw64_font_size(healthbar->font) + 4;

    // draw border
    fw64_spritebatch_set_color(spritebatch, 255, 255, 255, 255);
    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, healthbar->fill_texture, 0, pos_x, pos_y, width, HEALTHBAR_BORDER); // top
    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, healthbar->fill_texture, 0, pos_x, pos_y + height - HEALTHBAR_BORDER, width, HEALTHBAR_BORDER); // bottom
    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, healthbar->fill_texture, 0, pos_x, pos_y, HEALTHBAR_BORDER, height); //left
    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, healthbar->fill_texture, 0, pos_x + width - HEALTHBAR_BORDER, pos_y, HEALTHBAR_BORDER, height); // right

    // determine size and draw track
    pos_x += HEALTHBAR_BORDER;
    pos_y += HEALTHBAR_BORDER;
    width -= HEALTHBAR_BORDER * 2;
    height -= HEALTHBAR_BORDER * 2;

    fw64_spritebatch_set_color(spritebatch, 55, 55, 55, 200);
    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, healthbar->fill_texture, 0, pos_x, pos_y, width, height);

    // determine width / color and draw bar
    float health_pct = (float)healthbar->player->current_health / (float)PLAYER_MAX_HEALTH;
    width = width * health_pct;

    if (health_pct >= 0.66f) {
        fw64_spritebatch_set_color(spritebatch, 0, 255, 0, 255);
    }
    else if (health_pct >= 0.33f) {
        fw64_spritebatch_set_color(spritebatch, 255, 255, 0, 255);
    }
    else {
        fw64_spritebatch_set_color(spritebatch, 255, 0, 0, 255);
    }

    fw64_spritebatch_draw_sprite_slice_rect(spritebatch, healthbar->fill_texture, 0, pos_x, pos_y, width, height);
    fw64_spritebatch_set_color(spritebatch, 255, 255, 255, 255);
}