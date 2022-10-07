#pragma once

#include "player.h"

typedef struct {
    Player* player;
    fw64Renderer* renderer;
    fw64Font* font;
} Healthbar;

void healthbar_init(Healthbar* healthbar, Player* player, fw64Font* font, fw64Renderer* renderer);
void healthbar_draw(Healthbar* healthbar);