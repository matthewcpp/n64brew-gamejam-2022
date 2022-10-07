#pragma once

#include "framework64/engine.h"

#include "player.h"
#include "healthbar.h"

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    Player* player;
    fw64Font* hud_font;
    Healthbar healthbar;
} UI;

void ui_init(UI* ui, fw64Engine* engine, fw64Allocator* allocator, Player* player);
void ui_uninit(UI* ui);
void ui_draw(UI* ui);