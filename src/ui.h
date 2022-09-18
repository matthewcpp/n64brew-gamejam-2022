#pragma once

#include "framework64/engine.h"

#include "player.h"

typedef struct {
    fw64Engine* engine;
    Player* player;
    fw64Allocator* allocator;
    fw64Font* font;
} UI;

void ui_init(UI* ui, fw64Engine* engine, Player* player);
void ui_uninit(UI* ui);
void ui_draw(UI* ui);