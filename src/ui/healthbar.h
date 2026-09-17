#pragma once

#include "player.h"

typedef struct {
    Player* player;
    fw64Texture* fill_texture;
    fw64Font* font;
} Healthbar;

void healthbar_init(Healthbar* healthbar, Player* player, fw64Font* font, fw64Texture* fill_texture);
void healthbar_draw(Healthbar* healthbar, fw64SpriteBatch* spritebatch);